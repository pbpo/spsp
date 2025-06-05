#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include "octaflip.h"
#include "json.h"
#include "message_handler.h"
#include <stdbool.h>

int last_pass_player_idx = -1;
#define DEFAULT_PORT 8888
#define MAX_CLIENTS 2
#define BUFFER_SIZE 1024
#define TIMEOUT_SEC 5.0


Move move;


typedef struct {
   int socket;
   char username[64];
   char color;
} Client;


typedef enum {
   SERVER_WAITING_PLAYERS,
   SERVER_GAME_IN_PROGRESS,
   SERVER_GAME_OVER
} ServerState;


typedef struct {
   char buffer[BUFFER_SIZE * 2];
   size_t length;
} ClientBuffer;


ClientBuffer client_buffers[MAX_CLIENTS];
ServerState server_state = SERVER_WAITING_PLAYERS;
Client clients[MAX_CLIENTS];
int client_count = 0;
GameBoard game_board;
int current_player_idx = 0;
struct timeval turn_start_time;
char server_ip[INET_ADDRSTRLEN] = "127.0.0.1";
int server_port = DEFAULT_PORT;


void handle_client_message(int client_idx, char *buffer);
void handle_register_message(int client_idx, JsonValue *json_obj);
void handle_move_message(int client_idx, JsonValue *json_obj);
void handle_client_disconnect(int socket_fd);
void broadcast_game_start();
void send_your_turn(int client_idx);
void check_timeout();
void broadcast_game_over();
void log_game_state(const char *action, int player_idx, Move *move);
int set_socket_nonblocking(int socket_fd);
void cleanup_and_exit(int signal);


void init_client_buffers() {
   int i = 0;
init_loop:
   if (i >= MAX_CLIENTS) goto init_end;
   client_buffers[i].length = 0;
   client_buffers[i].buffer[0] = '\0';
   i++;
   goto init_loop;
init_end:
   return;
}


void print_usage(const char *program_name) {
   printf("Usage: %s [options]\n", program_name);
   printf("Options:\n");
   printf("  -p, --port <port>    서버 포트 번호 (기본값: %d)\n", DEFAULT_PORT);
   printf("  -i, --ip <ip>        서버 IP 주소 (기본값: 0.0.0.0 - 모든 인터페이스)\n");
   printf("  -h, --help           이 도움말 표시\n");
   printf("\n");
   printf("Examples:\n");
   printf("  %s                           # 기본 설정으로 실행 (0.0.0.0:8888)\n", program_name);
   printf("  %s -p 9999                   # 포트 9999로 실행\n", program_name);
   printf("  %s -i 127.0.0.1 -p 8080      # 127.0.0.1:8080으로 실행\n", program_name);
   printf("  %s --ip 192.168.1.100 --port 7777  # 192.168.1.100:7777로 실행\n", program_name);
}


int parse_arguments(int argc, char *argv[]) {
   int i = 1;
arg_loop:
   if (i >= argc) goto parse_end;


   if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--port") == 0) {
       if (i + 1 >= argc) {
           fprintf(stderr, "Error: %s 옵션에 포트 번호가 필요합니다.\n", argv[i]);
           goto parse_error;
       }
       server_port = atoi(argv[i + 1]);
       if (server_port <= 0 || server_port > 65535) {
           fprintf(stderr, "Error: 유효하지 않은 포트 번호: %s (1-65535 범위여야 합니다)\n", argv[i + 1]);
           goto parse_error;
       }
       i += 2;
       goto arg_loop;
   }


   if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--ip") == 0) {
       if (i + 1 >= argc) {
           fprintf(stderr, "Error: %s 옵션에 IP 주소가 필요합니다.\n", argv[i]);
           goto parse_error;
       }
       struct sockaddr_in sa;
       if (inet_pton(AF_INET, argv[i + 1], &(sa.sin_addr)) != 1) {
           fprintf(stderr, "Error: 유효하지 않은 IP 주소: %s\n", argv[i + 1]);
           goto parse_error;
       }
       strncpy(server_ip, argv[i + 1], sizeof(server_ip) - 1);
       server_ip[sizeof(server_ip) - 1] = '\0';
       i += 2;
       goto arg_loop;
   }


   if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
       print_usage(argv[0]);
       exit(0);
   }


   fprintf(stderr, "Error: 알 수 없는 옵션: %s\n", argv[i]);
   print_usage(argv[0]);
   goto parse_error;


parse_end:
   return 0;
parse_error:
   return -1;
}


void cleanup_and_exit(int signal) {
   printf("\n서버 종료 중...\n");
   int i = 0;
cleanup_loop:
   if (i >= MAX_CLIENTS) goto cleanup_end;
   if (clients[i].socket != -1) close(clients[i].socket);
   i++;
   goto cleanup_loop;
cleanup_end:
   exit(0);
}


int set_socket_nonblocking(int socket_fd) {
   int flags = fcntl(socket_fd, F_GETFL, 0);
   if (flags == -1) goto nonblock_error;
   if (fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) == -1) goto nonblock_error;
   return 0;
nonblock_error:
   perror("fcntl");
   return -1;
}


void process_client_data(int client_idx, char *new_data, size_t data_len) {
   ClientBuffer *cb = &client_buffers[client_idx];
   if (cb->length + data_len >= sizeof(cb->buffer) - 1) {
       printf("[Server] Buffer overflow for client %d, resetting\n", client_idx);
       cb->length = 0;
   }
   memcpy(cb->buffer + cb->length, new_data, data_len);
   cb->length += data_len;
   cb->buffer[cb->length] = '\0';


   char *start = cb->buffer;
process_loop:
   {
       char *newline_pos = strchr(start, '\n');
       if (!newline_pos) goto process_remainder;


       *newline_pos = '\0';
       if (strlen(start) > 0) {
           printf("[Server] Processing complete JSON: %s\n", start);
           handle_client_message(client_idx, start);
       }
       start = newline_pos + 1;
       goto process_loop;
   }
process_remainder:
   {
       size_t remaining = strlen(start);
       if (remaining > 0) {
           memmove(cb->buffer, start, remaining + 1);
           cb->length = remaining;
       } else {
           cb->length = 0;
           cb->buffer[0] = '\0';
       }
   }
}

int find_next_live(int from_idx)
{
    int next = (from_idx + 1) % MAX_CLIENTS;

find_loop:
    if (next == from_idx) goto no_live;          /* 한바퀴 → 아무도 안 살아있음 */
    if (clients[next].socket != -1) goto live_ok;/* 소켓 살아 있음 → 성공 */
    next = (next + 1) % MAX_CLIENTS;             /* 끊긴 슬롯 → skip */
    goto find_loop;

live_ok:
    return next;
no_live:
    return -1;
}
/* ==============================================================
 *  플레이어 연결 끊김 처리 (GOTO 스타일 + 남은 플레이어 턴 유지 패치)
 * ============================================================ */
void handle_client_disconnect(int socket_fd)
{
    struct sockaddr_in addr; socklen_t len = sizeof(addr);
    if (getpeername(socket_fd,(struct sockaddr*)&addr,&len) == 0)
        printf("연결 종료, IP:%s, 포트:%d\n",
               inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

    /* ─ ① 끊긴 슬롯 탐색 ─ */
    int d = 0;
find_d:
    if (d >= MAX_CLIENTS) goto not_found;
    if (clients[d].socket == socket_fd) goto found;
    d++; goto find_d;

not_found:
    return;

found:
    printf("플레이어 %s 연결 끊김\n", clients[d].username);
    close(socket_fd);
    clients[d].socket = -1;
    client_count--;

    /* ─ ② 게임 중이라면 opponent_left 통보 ─ */
    if (server_state == SERVER_GAME_IN_PROGRESS) {
        int br = 0;
brcast:
        if (br >= MAX_CLIENTS) goto after_br;
        if (clients[br].socket != -1) {
            JsonValue *m = createOpponentLeftMessage(clients[d].username);
            char *s = json_stringify(m);
            send(clients[br].socket, s, strlen(s), 0);
            send(clients[br].socket, "\n", 1, 0);
            free(s); json_free(m);
        }
        br++; goto brcast;
after_br:
        /* 끊긴 사람이 현재 턴이면 턴 전환 */
        if (current_player_idx == d) {
            int nl = find_next_live(d);
            if (nl == -1) {
                printf("[Server] All players disconnected. Shutting down server…\n");
                cleanup_and_exit(0);
            }
            current_player_idx = nl;
            send_your_turn(nl);
        }
    } else if (server_state == SERVER_WAITING_PLAYERS) {
        printf("[Server] Player disconnected while waiting for game start\n");
    }

    /* ─ ③ 남은 인원 체크 ─ */
    int rem = 0, i = 0;
cnt:
    if (i >= MAX_CLIENTS) goto after_cnt;
    if (clients[i].socket != -1) rem++;
    i++; goto cnt;
after_cnt:
    printf("[Server] Remaining connected clients: %d\n", rem);
    if (rem == 0) {
        printf("[Server] All players disconnected. Shutting down server…\n");
        cleanup_and_exit(0);
    }
}

void check_timeout()
{
    if (server_state != SERVER_GAME_IN_PROGRESS) return;

    /* 현재 턴 플레이어가 끊겨 있으면 즉시 턴 넘기기 */
    if (clients[current_player_idx].socket == -1)
        goto switch_after_disc;

    /* 정상적인 타임-체크 */
    struct timeval now;
    gettimeofday(&now, NULL);
    double elap = (now.tv_sec  - turn_start_time.tv_sec) +
                  (now.tv_usec - turn_start_time.tv_usec) / 1e6;
    if (elap <= TIMEOUT_SEC) return;             /* 아직 시간 남음 */

    /* ✅ 수정: username이 비어있지 않은지 확인 */
    const char *player_name = clients[current_player_idx].username;
    if (strlen(player_name) == 0) {
        player_name = "Unknown Player";
    }
    
    printf("[Server] %s timed-out (%.2f s) → pass.\n", player_name, elap);
    game_board.consecutivePasses++;

    /* 패스 메시지 */
    JsonValue *pm = createPassMessage(player_name);
    char *ps = json_stringify(pm);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].socket != -1) {
            send(clients[i].socket, ps, strlen(ps), 0);
            send(clients[i].socket, "\n", 1, 0);
        }
    }
    free(ps); json_free(pm);

    if (game_board.consecutivePasses >= 2 || hasGameEnded(&game_board)) {
        broadcast_game_over();
        return;
    }

switch_after_disc:   /* fall-through: 끊김 or 타임아웃 후 턴 전환 */
    {
        int next_idx = find_next_live(current_player_idx);
        if (next_idx == -1) {
            printf("[Server] All players disconnected. Shutting down server…\n");
            cleanup_and_exit(0);
        }
        current_player_idx = next_idx;
        send_your_turn(next_idx);
    }
}

void log_game_state(const char *action, int player_idx, Move *move) {
   printf("[게임 로그] %s - 플레이어: %s", action,
          (player_idx >= 0) ? clients[player_idx].username : "시스템");


   if (move && (move->sourceRow != 0 || move->sourceCol != 0 ||
                move->targetRow != 0 || move->targetCol != 0)) {
       printf(", 이동: (%d,%d)->(%d,%d)", move->sourceRow,
              move->sourceCol, move->targetRow, move->targetCol);
   }
   printf(", 보드상태: R=%d B=%d Empty=%d\n",
          game_board.redCount, game_board.blueCount, game_board.emptyCount);
}


void handle_register_message(int client_idx, JsonValue *json_obj)
{
    JsonValue *uj = json_object_get(json_obj, "username");
    if (!uj || uj->type != JSON_STRING) return;
    const char *username = json_string_value(uj);

    /* ── 동일 이름 검색 ───────────────────────────── */
    int dup_idx = -1;
    for (int i = 0; i < MAX_CLIENTS; ++i)
        if (strlen(clients[i].username) &&
            strcmp(clients[i].username, username) == 0)
        { dup_idx = i; break; }

    /* ── (1) 중복 ID + 소켓 살아있음 → 거부 ───────── */
    if (dup_idx != -1 && clients[dup_idx].socket != -1 && dup_idx != client_idx)
    {
        JsonValue *nack = createRegisterNackMessage("duplicate username");
        char *s = json_stringify(nack);
        send(clients[client_idx].socket, s, strlen(s), 0);
        send(clients[client_idx].socket, "\n", 1, 0);
        free(s); json_free(nack);
        close(clients[client_idx].socket);
        clients[client_idx].socket = -1;
        return;
    }

    /* ── (2) 같은 슬롯 재접속(dup_idx == client_idx) ── */

if (dup_idx == client_idx)
{
    printf("[Server] %s reconnected on same slot %d\n",
           username, client_idx);

    /* ① 끊겼을 때 남은 JSON 조각 제거 */
    client_buffers[client_idx].length   = 0;
    client_buffers[client_idx].buffer[0] = '\0';

    /* ② 중복 감소했던 client_count 복구 */
            /* handle_client_disconnect 에서 -- 됐음 */

    /* ③ 간단 ACK */
    JsonValue *ack = createRegisterAckMessage();
    char *as = json_stringify(ack);
    send(clients[client_idx].socket, as, strlen(as), 0);
    send(clients[client_idx].socket, "\n", 1, 0);
    free(as); json_free(ack);

    /* ✅ 수정: 게임 진행 중이면 상태 복구 메시지 전송 */
    if (server_state == SERVER_GAME_IN_PROGRESS) {
        const char *pl[2] = { clients[0].username, clients[1].username };
        
        // game_restart 메시지 전송
        JsonValue *rst = createGameRestartMessage(pl,
                          clients[0].username,
                          &game_board,
                          clients[current_player_idx].username);
        char *rs = json_stringify(rst);
        send(clients[client_idx].socket, rs, strlen(rs), 0);
        send(clients[client_idx].socket, "\n", 1, 0);
        free(rs); json_free(rst);

        /* ④ 본인이 현재 턴이면 즉시 your_turn */
        if (current_player_idx == client_idx) {
            send_your_turn(client_idx);
        }
    }
    return;
}
    /* ── (3) 다른 슬롯 재접속(dup_idx != client_idx) ───────── */
    if (dup_idx != -1 && clients[dup_idx].socket == -1)
    {
        printf("[Server] %s re-connected (tmp %d → slot %d)\n",
               username, client_idx, dup_idx);

        clients[dup_idx].socket  = clients[client_idx].socket;
        client_buffers[dup_idx]  = client_buffers[client_idx];
        clients[client_idx].socket = -1;

        const char *pl[2] = { clients[0].username, clients[1].username };
        JsonValue *rst = createGameRestartMessage(pl,
                          clients[0].username,
                          &game_board,
                          clients[current_player_idx].username);
        char *rs = json_stringify(rst);
        send(clients[dup_idx].socket, rs, strlen(rs), 0);
        send(clients[dup_idx].socket, "\n", 1, 0);
        free(rs); json_free(rst);

        if (current_player_idx == dup_idx) send_your_turn(dup_idx);
        return;
    }

    /* ── (4) 완전 신규 등록 ───────────────────────── */
    clients[client_idx].color = (client_idx == 0) ? RED_PLAYER : BLUE_PLAYER;
    strncpy(clients[client_idx].username, username,
            sizeof(clients[client_idx].username)-1);
    clients[client_idx].username[sizeof(clients[client_idx].username)-1] = '\0';

    printf("[DEBUG] Registered idx=%d user=%s color=%c\n",
           client_idx, clients[client_idx].username, clients[client_idx].color);

    JsonValue *ack = createRegisterAckMessage();
    char *as = json_stringify(ack);
    send(clients[client_idx].socket, as, strlen(as), 0);
    send(clients[client_idx].socket, "\n", 1, 0);
    free(as); json_free(ack);

    if (client_count == MAX_CLIENTS &&
        clients[0].username[0] && clients[1].username[0])
        broadcast_game_start();
}


void handle_move_message(int client_idx, JsonValue *json_obj)
{
    /* ─ 0. 기본 검사 ─ */
    if (server_state != SERVER_GAME_IN_PROGRESS) {
        printf("[Server] Received move but game not in progress.\n");
        return;
    }
    if (clients[client_idx].socket == -1) {
        printf("[Server] Move from already-disconnected client ignored.\n");
        return;
    }
    
    /* ✅ 수정: 턴이 아니면 그냥 무시 (메시지도 보내지 않음) */
    if (client_idx != current_player_idx) {
        printf("[Server] %s tried to move out of turn. Ignoring.\n", 
               clients[client_idx].username);
        return;
    }

    /* ─ 1. JSON 파싱 ─ */
    char *username = NULL;
    if (!parseMoveMessage(json_obj, &username, &move))
        goto parse_fail;

    /* ✅ 수정: move.player 설정 */
    move.player = clients[client_idx].color;

    /* 로그용 */
    Move org = move;
    printf("[Server] Move from %s: (%d,%d)->(%d,%d)\n",
           clients[client_idx].username,
           org.sourceRow, org.sourceCol, org.targetRow, org.targetCol);

    /* ─ 2. PASS (0,0,0,0) 처리 ─ */
    if (org.sourceRow == 0 && org.sourceCol == 0 &&
        org.targetRow == 0 && org.targetCol == 0)
        goto handle_pass;

    /* ─ 3. 좌표 보정 ─ */
    Move adj = {
        .player    = move.player,
        .sourceRow = org.sourceRow - 1,
        .sourceCol = org.sourceCol - 1,
        .targetRow = org.targetRow - 1,
        .targetCol = org.targetCol - 1
    };

    /* ─ 4. 유효성 검사 ─ */
    if (!isValidMove(&game_board, &adj))
        goto invalid_move;

    /* ─ 5. 정상 이동 처리 ─ */
valid_move:
    printf("[Server] Valid → applying…\n");
    applyMove(&game_board, &adj);
    last_pass_player_idx = -1;

    /* OK + 다음플레이어 안내 */
    int nxt = find_next_live(current_player_idx);
    bool ended = hasGameEnded(&game_board);

    JsonValue *okj = createMoveOkMessage(&game_board,
                       ended ? NULL : clients[nxt].username);
    char *oks  = json_stringify(okj);
    if (clients[current_player_idx].socket != -1) {
        send(clients[current_player_idx].socket, oks, strlen(oks), 0);
        send(clients[current_player_idx].socket, "\n", 1, 0);
    }
    free(oks); json_free(okj);

    if (ended) {
        broadcast_game_over();
        free(username); return;
    }

    current_player_idx = nxt;
    send_your_turn(nxt);
    free(username); return;

    /* ────────── 분기들 ────────── */

parse_fail:
    {
        /* ✅ 수정: 파싱 실패 시에도 단순히 무시 */
        printf("[Server] Failed to parse move from %s. Ignoring.\n", 
               clients[client_idx].username);
    }
    if (username) {
    free(username);
}
return;

handle_pass:
    if (hasValidMove(&game_board, move.player)) {
        /* ✅ 수정: 잘못된 패스도 단순히 무시 */
        printf("[Server] %s tried to pass but has valid moves. Ignoring.\n", 
               clients[client_idx].username);
        free(username); return;
    }

    printf("[Server] %s passes.\n", clients[client_idx].username);

    /* 패스 메시지 전송 */
    {
        JsonValue *pm = createPassMessage(clients[current_player_idx].username);
        char *ps = json_stringify(pm);
        for (int i = 0; i < MAX_CLIENTS; ++i)
            if (clients[i].socket != -1) {
                send(clients[i].socket, ps, strlen(ps), 0);
                send(clients[i].socket, "\n", 1, 0);
            }
        free(ps); json_free(pm);
    }

    if (last_pass_player_idx == ((current_player_idx + 1) % MAX_CLIENTS)) {
        broadcast_game_over();
        free(username); return;
    }
    last_pass_player_idx = current_player_idx;

    current_player_idx = find_next_live(current_player_idx);
    send_your_turn(current_player_idx);
    free(username); return;

invalid_move:
    {
        /* ✅ 수정: 잘못된 이동도 단순히 무시 */
        printf("[Server] Invalid move from %s: (%d,%d)->(%d,%d). Ignoring.\n",
               clients[client_idx].username,
               org.sourceRow, org.sourceCol, org.targetRow, org.targetCol);
    }
    free(username); return;
}

void broadcast_game_start() {
   
   server_state = SERVER_GAME_IN_PROGRESS;
   initializeBoard(&game_board);
 last_pass_player_idx = -1;

   const char *usernames[MAX_CLIENTS];
   int i = 0;
collect_usernames:
   if (i >= MAX_CLIENTS) goto after_collect;
   usernames[i] = clients[i].username;
   i++;
   goto collect_usernames;
after_collect:


   JsonValue *start_msg = createGameStartMessage(usernames, clients[0].username);
   char *start_str = json_stringify(start_msg);


   int j = 0;
send_start:
   if (j >= MAX_CLIENTS) goto after_send_start;
   if (clients[j].socket != -1) {
       send(clients[j].socket, start_str, strlen(start_str), 0);
       send(clients[j].socket, "\n", 1, 0);
   }
   j++;
   goto send_start;
after_send_start:
   free(start_str);
   json_free(start_msg);


   current_player_idx = 0;
   move.player = clients[current_player_idx].color;
   move.sourceRow = 0;
   move.sourceCol = 0;
   move.targetRow = 0;
   move.targetCol = 0;
   send_your_turn(current_player_idx);
}


void send_your_turn(int client_idx) {
   if (client_idx < 0 || client_idx >= MAX_CLIENTS) return;
   if (clients[client_idx].socket == -1) return;


   move.player = clients[client_idx].color;
   JsonValue *turn_msg = createYourTurnMessage(&game_board, TIMEOUT_SEC);
   char *turn_str = json_stringify(turn_msg);
   send(clients[client_idx].socket, turn_str, strlen(turn_str), 0);
   send(clients[client_idx].socket, "\n", 1, 0);
   free(turn_str);
   json_free(turn_msg);


   gettimeofday(&turn_start_time, NULL);
}


void broadcast_game_over() {
   server_state = SERVER_GAME_OVER;
   countPieces(&game_board);


   int manual_red = 0, manual_blue = 0, manual_empty = 0;
   int i = 0;
count_loop:
   if (i >= BOARD_SIZE) goto after_count_loop_rows;
   int j = 0;
count_loop_cols:
   if (j >= BOARD_SIZE) goto next_row;
   char cell = game_board.cells[i][j];
   if (cell == RED_PLAYER) manual_red++;
   if (cell == BLUE_PLAYER) manual_blue++;
   if (cell == EMPTY_CELL) manual_empty++;
   j++;
   goto count_loop_cols;
next_row:
   i++;
   goto count_loop;
after_count_loop_rows:


   printf("[DEBUG] countPieces result: R=%d, B=%d, Empty=%d\n",
          game_board.redCount, game_board.blueCount, game_board.emptyCount);
   printf("[DEBUG] Manual count: R=%d, B=%d, Empty=%d, Total=%d\n",
          manual_red, manual_blue, manual_empty,
          manual_red + manual_blue + manual_empty);


   const char *players[2] = {clients[0].username, clients[1].username};
   int scores[2];
   if (manual_red + manual_blue + manual_empty == 64) {
       scores[0] = manual_red;
       scores[1] = manual_blue;
   } else {
       scores[0] = game_board.redCount;
       scores[1] = game_board.blueCount;
   }


   JsonValue *game_over_msg = createGameOverMessage(players, scores);
   char *json_str = json_stringify(game_over_msg);


   int k = 0;
send_over:
   if (k >= MAX_CLIENTS) goto after_send_over;
   if (clients[k].socket != -1) {
       send(clients[k].socket, json_str, strlen(json_str), 0);
       send(clients[k].socket, "\n", 1, 0);
   }
   k++;
   goto send_over;
after_send_over:
   free(json_str);
   json_free(game_over_msg);


   if (scores[0] > scores[1]) {
       printf("[Server] Game over: %s wins! (R=%d, B=%d)\n",
              clients[0].username, scores[0], scores[1]);
   } else if (scores[1] > scores[0]) {
       printf("[Server] Game over: %s wins! (R=%d, B=%d)\n",
              clients[1].username, scores[0], scores[1]);
   } else {
       printf("[Server] Game over: Draw! (R=%d, B=%d)\n", scores[0], scores[1]);
   }


   printf("[Server] Game ended normally. Waiting for final messages...\n");
   sleep(2);
   printf("[Server] Game completed. Shutting down server...\n");
   cleanup_and_exit(0);
}


void handle_client_message(int client_idx, char *buffer) {
   JsonValue *json_obj = json_parse(buffer);
   if (!json_obj) return;


   MessageType msg_type = parseMessageType(json_obj);
   if (msg_type == MSG_REGISTER) goto handle_register;
   if (msg_type == MSG_MOVE) goto handle_move;
   goto after_handle;


handle_register:
   handle_register_message(client_idx, json_obj);
   goto after_handle;


handle_move:
   handle_move_message(client_idx, json_obj);
after_handle:
   json_free(json_obj);
}


int main(int argc, char *argv[]) {
   if (parse_arguments(argc, argv) != 0) return EXIT_FAILURE;

   signal(SIGINT, cleanup_and_exit);
   signal(SIGTERM, cleanup_and_exit);

   int i = 0;
init_clients:
   if (i >= MAX_CLIENTS) goto after_init_clients;
   clients[i].socket = -1;
   clients[i].username[0] = '\0';
   i++;
   goto init_clients;
after_init_clients:

   init_client_buffers();
   initializeBoard(&game_board);

   int server_fd = socket(AF_INET, SOCK_STREAM, 0);
   if (server_fd < 0) goto socket_error;

   int opt = 1;
   if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) goto sockopt_error;

   struct sockaddr_in address;
   address.sin_family = AF_INET;
   if (strcmp(server_ip, "0.0.0.0") == 0) {
       address.sin_addr.s_addr = INADDR_ANY;
   } else {
       if (inet_pton(AF_INET, server_ip, &address.sin_addr) != 1) goto inetpton_error;
   }
   address.sin_port = htons(server_port);

   if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) goto bind_error;
   if (listen(server_fd, 3) < 0) goto listen_error;

   printf("OctaFlip 서버가 %s:%d에서 시작되었습니다.\n",
          strcmp(server_ip, "0.0.0.0") == 0 ? "127.0.0.1" : server_ip,
          server_port);

   char buffer[BUFFER_SIZE];
   struct pollfd fds[MAX_CLIENTS + 1];
   int nfds = 1;

   fds[0].fd = server_fd;
   fds[0].events = POLLIN;

   int j = 1;
init_fds:
   if (j > MAX_CLIENTS) goto after_init_fds;
   fds[j].fd = -1;
   fds[j].events = POLLIN;
   j++;
   goto init_fds;
after_init_fds:

accept_loop:
   check_timeout();
   int poll_result = poll(fds, nfds, 500);  /* ✅ 수정: 100ms → 500ms */
   if (poll_result < 0 && errno != EINTR) goto accept_loop;

   if (fds[0].revents & POLLIN) {
       socklen_t addrlen = sizeof(address);
       int new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
       if (new_socket >= 0) {
           set_socket_nonblocking(new_socket);
           printf("새 연결, 소켓 fd: %d, IP: %s, 포트: %d\n",
                  new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

           int slot_found = 0;
           int cidx = 0;
find_slot:
           if (cidx >= MAX_CLIENTS) goto after_find_slot;
           if (clients[cidx].socket == -1) {
               clients[cidx].socket = new_socket;
               
               /* ✅ 수정: 클라이언트 상태 초기화 */
               clients[cidx].username[0] = '\0';
               client_buffers[cidx].length = 0;
               client_buffers[cidx].buffer[0] = '\0';
               
               int k = 1;
find_fds:
               if (k > MAX_CLIENTS) goto after_find_fds;
               if (fds[k].fd == -1) {
                   fds[k].fd = new_socket;
                   if (k >= nfds) nfds = k + 1;
                   goto after_find_fds;
               }
               k++;
               goto find_fds;
after_find_fds:
               client_count++;
               slot_found = 1;
               printf("[Server] Client assigned to slot %d, total clients: %d\n", cidx, client_count);
               goto after_find_slot;
           }
           cidx++;
           goto find_slot;
after_find_slot:
           if (!slot_found) {
               close(new_socket);
               printf("최대 클라이언트 수에 도달. 연결 거부.\n");
           }
       }
   }

accept_cont:
   {
       int idx = 1;
client_loop:
       if (idx > MAX_CLIENTS || idx >= nfds) goto after_client_loop;
       if (fds[idx].fd != -1 && (fds[idx].revents & POLLIN)) {
           char temp_buffer[BUFFER_SIZE];
           int valread = read(fds[idx].fd, temp_buffer, BUFFER_SIZE - 1);
           
           if (valread > 0) {
               temp_buffer[valread] = '\0';
               int client_idx = 0;
find_client_idx:
               if (client_idx >= MAX_CLIENTS) goto skip_process;
               if (clients[client_idx].socket == fds[idx].fd) goto got_client_idx;
               client_idx++;
               goto find_client_idx;
got_client_idx:
               process_client_data(client_idx, temp_buffer, valread);
               goto skip_process;
skip_process:
               ;
           } else if (valread == 0) {
               /* ✅ 수정: 연결이 정상적으로 종료됨 */
               printf("[Server] Client gracefully disconnected (fd: %d)\n", fds[idx].fd);
               handle_client_disconnect(fds[idx].fd);

               int disconnected_idx = 0;
find_disc_idx:
               if (disconnected_idx >= MAX_CLIENTS) goto after_disc_idx;
               if (clients[disconnected_idx].socket == fds[idx].fd) goto found_disc_idx;
               disconnected_idx++;
               goto find_disc_idx;
found_disc_idx:
after_disc_idx:
               memset(&client_buffers[disconnected_idx], 0, sizeof(ClientBuffer));
               fds[idx].fd = -1;
               while (nfds > 1 && fds[nfds - 1].fd == -1) {
                   nfds--;
               }
           } else {
               /* ✅ 수정: 에러 처리 개선 */
               if (errno == EAGAIN || errno == EWOULDBLOCK) {
                   /* 비블로킹 소켓에서 데이터 없음 - 정상 상황 */
                   goto skip_read_error;
               } else {
                   /* 실제 에러 발생 */
                   printf("[Server] Read error on fd %d: %s\n", fds[idx].fd, strerror(errno));
                   handle_client_disconnect(fds[idx].fd);

                   int disconnected_idx = 0;
find_disc_idx2:
                   if (disconnected_idx >= MAX_CLIENTS) goto after_disc_idx2;
                   if (clients[disconnected_idx].socket == fds[idx].fd) goto found_disc_idx2;
                   disconnected_idx++;
                   goto find_disc_idx2;
found_disc_idx2:
after_disc_idx2:
                   memset(&client_buffers[disconnected_idx], 0, sizeof(ClientBuffer));
                   fds[idx].fd = -1;
                   while (nfds > 1 && fds[nfds - 1].fd == -1) {
                       nfds--;
                   }
               }
           }
skip_read_error:
           ;
       }
       idx++;
       goto client_loop;
after_client_loop:
       ;
   }

   goto accept_loop;

socket_error:
   perror("socket failed");
   exit(EXIT_FAILURE);
sockopt_error:
   perror("setsockopt");
   exit(EXIT_FAILURE);
inetpton_error:
   fprintf(stderr, "Error: IP 주소 변환 실패: %s\n", server_ip);
   exit(EXIT_FAILURE);
bind_error:
   perror("bind failed");
   printf("Failed to bind to %s:%d\n", server_ip, server_port);
   exit(EXIT_FAILURE);
listen_error:
   perror("listen");
   exit(EXIT_FAILURE);

   return 0;
}