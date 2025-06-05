	.file	"winning_strategy.c"
	.text
	.globl	opening_book
	.data
	.align 32
	.type	opening_book, @object
	.size	opening_book, 288
opening_book:
	.string	"R......B"
	.string	"........"
	.string	"........"
	.string	"........"
	.string	"........"
	.string	"........"
	.string	"........"
	.string	"B......R"
	.long	82
	.long	0
	.long	0
	.long	1
	.byte	1
	.zero	3
	.long	95
	.string	"R......B"
	.string	"........"
	.string	"........"
	.string	"........"
	.string	"........"
	.string	"........"
	.string	"........"
	.string	"B......R"
	.long	82
	.long	7
	.long	7
	.long	6
	.byte	6
	.zero	3
	.long	90
	.string	"R......B"
	.string	"........"
	.string	"........"
	.string	"........"
	.string	"........"
	.string	"........"
	.string	"........"
	.string	"B......R"
	.long	82
	.long	0
	.long	0
	.long	0
	.byte	2
	.zero	3
	.long	85
	.globl	opening_book_size
	.align 4
	.type	opening_book_size, @object
	.size	opening_book_size, 4
opening_book_size:
	.long	3
	.section	.rodata
	.align 8
.LC0:
	.string	"\354\240\225\354\204\235 \353\215\260\354\235\264\355\204\260\353\262\240\354\235\264\354\212\244 \352\262\200\354\203\211 \354\244\221... (\355\201\254\352\270\260: %d)\n"
.LC1:
	.string	"\355\230\204\354\236\254 \353\263\264\353\223\234:"
.LC2:
	.string	"'%s'\n"
.LC3:
	.string	"\354\240\225\354\204\235 %d:\n"
	.align 8
.LC4:
	.string	"\355\226\211 %d\354\227\220\354\204\234 \353\266\210\354\235\274\354\271\230: '%s' vs '%s'\n"
	.align 8
.LC5:
	.string	"\354\240\225\354\204\235 \354\235\264\353\217\231 \353\260\234\352\262\254! \354\213\240\353\242\260\353\217\204: %d%%, \354\235\264\353\217\231: (%d,%d)->(%d,%d)\n"
	.align 8
.LC6:
	.string	"\354\240\225\354\204\235\354\227\220\354\204\234 \353\247\244\354\271\255\353\220\230\353\212\224 \354\235\264\353\217\231 \354\227\206\354\235\214"
	.text
	.globl	checkOpeningBook
	.type	checkOpeningBook, @function
checkOpeningBook:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$96, %rsp
	movq	%rdi, -72(%rbp)
	movq	%rsi, -80(%rbp)
	movl	%edx, %eax
	movb	%al, -84(%rbp)
	movsbl	-84(%rbp), %eax
	movl	%eax, -32(%rbp)
	movl	$0, -28(%rbp)
	movl	$0, -24(%rbp)
	movl	$0, -20(%rbp)
	movb	$0, -16(%rbp)
	movq	-80(%rbp), %rax
	movq	%rax, %rdi
	call	isOpeningPhase
	testl	%eax, %eax
	jne	.L2
	movq	-72(%rbp), %rcx
	movq	-32(%rbp), %rax
	movq	-24(%rbp), %rdx
	movq	%rax, (%rcx)
	movq	%rdx, 8(%rcx)
	movl	-16(%rbp), %eax
	movl	%eax, 16(%rcx)
	jmp	.L15
.L2:
	movl	opening_book_size(%rip), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, -60(%rbp)
	jmp	.L4
.L14:
	movl	-60(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$5, %rax
	leaq	opening_book(%rip), %rdx
	addq	%rdx, %rax
	movq	%rax, -40(%rbp)
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	movl	$0, -56(%rbp)
	jmp	.L5
.L6:
	movl	-56(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	movq	-80(%rbp), %rdx
	addq	%rdx, %rax
	movq	%rax, %rsi
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	addl	$1, -56(%rbp)
.L5:
	cmpl	$7, -56(%rbp)
	jle	.L6
	movl	-60(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, -52(%rbp)
	jmp	.L7
.L8:
	movl	-52(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	movq	-40(%rbp), %rdx
	addq	%rdx, %rax
	movq	%rax, %rsi
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	addl	$1, -52(%rbp)
.L7:
	cmpl	$7, -52(%rbp)
	jle	.L8
	movl	$1, -48(%rbp)
	movl	$0, -44(%rbp)
	jmp	.L9
.L12:
	movl	-44(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	movq	-40(%rbp), %rdx
	leaq	(%rax,%rdx), %rcx
	movl	-44(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	movq	-80(%rbp), %rdx
	addq	%rdx, %rax
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	je	.L10
	movl	$0, -48(%rbp)
	movl	-44(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	movq	-40(%rbp), %rdx
	leaq	(%rax,%rdx), %rcx
	movl	-44(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	movq	-80(%rbp), %rdx
	addq	%rax, %rdx
	movl	-44(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC4(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
.L10:
	addl	$1, -44(%rbp)
.L9:
	cmpl	$7, -44(%rbp)
	jg	.L11
	cmpl	$0, -48(%rbp)
	jne	.L12
.L11:
	cmpl	$0, -48(%rbp)
	je	.L13
	movq	-40(%rbp), %rax
	movzbl	88(%rax), %eax
	cmpb	%al, -84(%rbp)
	jne	.L13
	movq	-40(%rbp), %rax
	movl	84(%rax), %edi
	movq	-40(%rbp), %rax
	movl	80(%rax), %esi
	movq	-40(%rbp), %rax
	movl	76(%rax), %ecx
	movq	-40(%rbp), %rax
	movl	72(%rax), %edx
	movq	-40(%rbp), %rax
	movl	92(%rax), %eax
	movl	%edi, %r9d
	movl	%esi, %r8d
	movl	%eax, %esi
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	-72(%rbp), %rcx
	movq	-40(%rbp), %rsi
	movq	72(%rsi), %rax
	movq	80(%rsi), %rdx
	movq	%rax, (%rcx)
	movq	%rdx, 8(%rcx)
	movl	88(%rsi), %eax
	movl	%eax, 16(%rcx)
	jmp	.L15
.L13:
	addl	$1, -60(%rbp)
.L4:
	movl	opening_book_size(%rip), %eax
	cmpl	%eax, -60(%rbp)
	jl	.L14
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	movq	-72(%rbp), %rcx
	movq	-32(%rbp), %rax
	movq	-24(%rbp), %rdx
	movq	%rax, (%rcx)
	movq	%rdx, 8(%rcx)
	movl	-16(%rbp), %eax
	movl	%eax, 16(%rcx)
.L15:
	movq	-72(%rbp), %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	checkOpeningBook, .-checkOpeningBook
	.globl	isOpeningPhase
	.type	isOpeningPhase, @function
isOpeningPhase:
.LFB7:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	84(%rax), %eax
	cmpl	$49, %eax
	setg	%al
	movzbl	%al, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	isOpeningPhase, .-isOpeningPhase
	.globl	isEndgamePhase
	.type	isEndgamePhase, @function
isEndgamePhase:
.LFB8:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	84(%rax), %eax
	cmpl	$8, %eax
	setle	%al
	movzbl	%al, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	isEndgamePhase, .-isEndgamePhase
	.section	.rodata
	.align 8
.LC7:
	.string	"\354\242\205\353\260\230 \354\231\204\354\240\204 \352\263\204\354\202\260 \354\213\234\354\236\221 (\353\271\210 \354\271\270: %d)\n"
	.align 8
.LC8:
	.string	"\354\235\264\353\217\231 (%d,%d)->(%d,%d): \354\240\220\354\210\230 %d\n"
	.align 8
.LC9:
	.string	"\354\242\205\353\260\230 \354\265\234\354\240\201 \354\235\264\353\217\231: (%d,%d)->(%d,%d), \354\240\220\354\210\230: %d\n"
	.text
	.globl	solveEndgame
	.type	solveEndgame, @function
solveEndgame:
.LFB9:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$4096, %rsp
	orq	$0, (%rsp)
	subq	$1232, %rsp
	movq	%rdi, -5304(%rbp)
	movq	%rsi, -5312(%rbp)
	movl	%edx, %eax
	movb	%al, -5316(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movq	-5312(%rbp), %rax
	movl	84(%rax), %eax
	movl	%eax, %esi
	leaq	.LC7(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, %eax
	call	createAIEngine@PLT
	movq	%rax, -5272(%rbp)
	cmpq	$0, -5272(%rbp)
	jne	.L21
	movq	-5304(%rbp), %rax
	movq	-5312(%rbp), %rdx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	generateMove@PLT
	jmp	.L29
.L21:
	call	clock@PLT
	movq	-5272(%rbp), %rdx
	movq	%rax, 16(%rdx)
	movq	-5272(%rbp), %rax
	movl	$0, 24(%rax)
	movzbl	-5316(%rbp), %eax
	movb	%al, -5248(%rbp)
	movl	$0, -5252(%rbp)
	movl	-5252(%rbp), %eax
	movl	%eax, -5256(%rbp)
	movl	-5256(%rbp), %eax
	movl	%eax, -5260(%rbp)
	movl	-5260(%rbp), %eax
	movl	%eax, -5264(%rbp)
	movl	$-1000000, -5284(%rbp)
	movsbl	-5316(%rbp), %esi
	leaq	-5288(%rbp), %rcx
	leaq	-5136(%rbp), %rdx
	movq	-5312(%rbp), %rax
	movq	%rax, %rdi
	call	getAllValidMoves@PLT
	movl	-5288(%rbp), %eax
	testl	%eax, %eax
	jne	.L23
	movq	-5272(%rbp), %rax
	movq	%rax, %rdi
	call	destroyAIEngine@PLT
	movq	-5304(%rbp), %rcx
	movq	-5264(%rbp), %rax
	movq	-5256(%rbp), %rdx
	movq	%rax, (%rcx)
	movq	%rdx, 8(%rcx)
	movl	-5248(%rbp), %eax
	movl	%eax, 16(%rcx)
	jmp	.L29
.L23:
	movl	$0, -5280(%rbp)
	jmp	.L24
.L28:
	movq	-5312(%rbp), %rcx
	leaq	-5232(%rbp), %rax
	movl	$92, %edx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	memcpy@PLT
	leaq	-5136(%rbp), %rcx
	movl	-5280(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	leaq	(%rcx,%rax), %rdx
	leaq	-5232(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	applyMove@PLT
	cmpb	$82, -5316(%rbp)
	jne	.L25
	movl	$66, %eax
	jmp	.L26
.L25:
	movl	$82, %eax
.L26:
	movb	%al, -5289(%rbp)
	movsbl	-5316(%rbp), %ecx
	movsbl	-5289(%rbp), %edi
	movq	-5312(%rbp), %rax
	movl	84(%rax), %edx
	leaq	-5232(%rbp), %rsi
	movq	-5272(%rbp), %rax
	subq	$8, %rsp
	pushq	%rcx
	movl	%edi, %r9d
	movl	$1000000, %r8d
	movl	$-1000000, %ecx
	movq	%rax, %rdi
	call	minimax@PLT
	addq	$16, %rsp
	movl	%eax, -5276(%rbp)
	movl	-5276(%rbp), %eax
	cmpl	-5284(%rbp), %eax
	jle	.L27
	movl	-5276(%rbp), %eax
	movl	%eax, -5284(%rbp)
	movl	-5280(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	addq	%rbp, %rax
	leaq	-5136(%rax), %rcx
	movq	(%rcx), %rax
	movq	8(%rcx), %rdx
	movq	%rax, -5264(%rbp)
	movq	%rdx, -5256(%rbp)
	movl	16(%rcx), %eax
	movl	%eax, -5248(%rbp)
.L27:
	movl	-5280(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	addq	%rbp, %rax
	subq	$5124, %rax
	movl	(%rax), %edi
	movl	-5280(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	addq	%rbp, %rax
	subq	$5128, %rax
	movl	(%rax), %ecx
	movl	-5280(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	addq	%rbp, %rax
	subq	$5132, %rax
	movl	(%rax), %esi
	movl	-5280(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	addq	%rbp, %rax
	subq	$5136, %rax
	movl	(%rax), %eax
	movl	-5276(%rbp), %edx
	movl	%edx, %r9d
	movl	%edi, %r8d
	movl	%esi, %edx
	movl	%eax, %esi
	leaq	.LC8(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	addl	$1, -5280(%rbp)
.L24:
	movl	-5288(%rbp), %eax
	cmpl	%eax, -5280(%rbp)
	jl	.L28
	movl	-5252(%rbp), %esi
	movl	-5256(%rbp), %ecx
	movl	-5260(%rbp), %edx
	movl	-5264(%rbp), %eax
	movl	-5284(%rbp), %edi
	movl	%edi, %r9d
	movl	%esi, %r8d
	movl	%eax, %esi
	leaq	.LC9(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	-5272(%rbp), %rax
	movq	%rax, %rdi
	call	destroyAIEngine@PLT
	movq	-5304(%rbp), %rcx
	movq	-5264(%rbp), %rax
	movq	-5256(%rbp), %rdx
	movq	%rax, (%rcx)
	movq	%rdx, 8(%rcx)
	movl	-5248(%rbp), %eax
	movl	%eax, 16(%rcx)
.L29:
	movq	-8(%rbp), %rax
	subq	%fs:40, %rax
	je	.L30
	call	__stack_chk_fail@PLT
.L30:
	movq	-5304(%rbp), %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	solveEndgame, .-solveEndgame
	.globl	calculateMaterial
	.type	calculateMaterial, @function
calculateMaterial:
.LFB10:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)
	movl	%esi, %eax
	movb	%al, -28(%rbp)
	cmpb	$82, -28(%rbp)
	jne	.L32
	movq	-24(%rbp), %rax
	movl	76(%rax), %eax
	jmp	.L33
.L32:
	movq	-24(%rbp), %rax
	movl	80(%rax), %eax
.L33:
	movl	%eax, -8(%rbp)
	cmpb	$82, -28(%rbp)
	jne	.L34
	movq	-24(%rbp), %rax
	movl	80(%rax), %eax
	jmp	.L35
.L34:
	movq	-24(%rbp), %rax
	movl	76(%rax), %eax
.L35:
	movl	%eax, -4(%rbp)
	movl	-8(%rbp), %eax
	subl	-4(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10:
	.size	calculateMaterial, .-calculateMaterial
	.globl	calculateThreatLevel
	.type	calculateThreatLevel, @function
calculateThreatLevel:
.LFB11:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$4096, %rsp
	orq	$0, (%rsp)
	subq	$1184, %rsp
	movq	%rdi, -5272(%rbp)
	movl	%esi, %eax
	movb	%al, -5276(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	cmpb	$82, -5276(%rbp)
	jne	.L38
	movl	$66, %eax
	jmp	.L39
.L38:
	movl	$82, %eax
.L39:
	movb	%al, -5257(%rbp)
	movl	$0, -5252(%rbp)
	movsbl	-5257(%rbp), %esi
	leaq	-5256(%rbp), %rcx
	leaq	-5136(%rbp), %rdx
	movq	-5272(%rbp), %rax
	movq	%rax, %rdi
	call	getAllValidMoves@PLT
	movl	$0, -5248(%rbp)
	jmp	.L40
.L45:
	movq	-5272(%rbp), %rcx
	leaq	-5232(%rbp), %rax
	movl	$92, %edx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	memcpy@PLT
	leaq	-5136(%rbp), %rcx
	movl	-5248(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	leaq	(%rcx,%rax), %rdx
	leaq	-5232(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	applyMove@PLT
	cmpb	$82, -5276(%rbp)
	jne	.L41
	movl	-5156(%rbp), %eax
	jmp	.L42
.L41:
	movl	-5152(%rbp), %eax
.L42:
	movl	%eax, -5244(%rbp)
	cmpb	$82, -5276(%rbp)
	jne	.L43
	movq	-5272(%rbp), %rax
	movl	76(%rax), %eax
	jmp	.L44
.L43:
	movq	-5272(%rbp), %rax
	movl	80(%rax), %eax
.L44:
	movl	%eax, -5240(%rbp)
	movl	-5240(%rbp), %eax
	subl	-5244(%rbp), %eax
	movl	%eax, -5236(%rbp)
	movl	-5236(%rbp), %eax
	addl	%eax, -5252(%rbp)
	addl	$1, -5248(%rbp)
.L40:
	movl	-5256(%rbp), %eax
	cmpl	%eax, -5248(%rbp)
	jl	.L45
	movl	-5252(%rbp), %eax
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L47
	call	__stack_chk_fail@PLT
.L47:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE11:
	.size	calculateThreatLevel, .-calculateThreatLevel
	.section	.rodata
	.align 8
.LC10:
	.string	"\354\212\271\353\266\200\354\210\230 \353\260\234\352\262\254! \355\224\274\355\225\264\353\237\211: %d\n"
	.text
	.globl	findKillerMove
	.type	findKillerMove, @function
findKillerMove:
.LFB12:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$4096, %rsp
	orq	$0, (%rsp)
	subq	$1232, %rsp
	movq	%rdi, -5304(%rbp)
	movq	%rsi, -5312(%rbp)
	movl	%edx, %eax
	movb	%al, -5316(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movsbl	-5316(%rbp), %eax
	movl	%eax, -5264(%rbp)
	movl	$0, -5260(%rbp)
	movl	$0, -5256(%rbp)
	movl	$0, -5252(%rbp)
	movb	$0, -5248(%rbp)
	movl	$0, -5284(%rbp)
	movsbl	-5316(%rbp), %esi
	leaq	-5288(%rbp), %rcx
	leaq	-5136(%rbp), %rdx
	movq	-5312(%rbp), %rax
	movq	%rax, %rdi
	call	getAllValidMoves@PLT
	movl	$0, -5280(%rbp)
	jmp	.L49
.L58:
	movq	-5312(%rbp), %rcx
	leaq	-5232(%rbp), %rax
	movl	$92, %edx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	memcpy@PLT
	leaq	-5136(%rbp), %rcx
	movl	-5280(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	leaq	(%rcx,%rax), %rdx
	leaq	-5232(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	applyMove@PLT
	cmpb	$82, -5316(%rbp)
	jne	.L50
	movl	$66, %eax
	jmp	.L51
.L50:
	movl	$82, %eax
.L51:
	movb	%al, -5289(%rbp)
	cmpb	$82, -5289(%rbp)
	jne	.L52
	movq	-5312(%rbp), %rax
	movl	76(%rax), %eax
	jmp	.L53
.L52:
	movq	-5312(%rbp), %rax
	movl	80(%rax), %eax
.L53:
	movl	%eax, -5272(%rbp)
	cmpb	$82, -5289(%rbp)
	jne	.L54
	movl	-5156(%rbp), %eax
	jmp	.L55
.L54:
	movl	-5152(%rbp), %eax
.L55:
	movl	%eax, -5268(%rbp)
	movl	-5272(%rbp), %eax
	subl	-5268(%rbp), %eax
	movl	%eax, -5276(%rbp)
	movl	-5280(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	addq	%rbp, %rax
	subq	$5124, %rax
	movl	(%rax), %ecx
	movl	-5280(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	addq	%rbp, %rax
	subq	$5128, %rax
	movl	(%rax), %eax
	movl	%ecx, %esi
	movl	%eax, %edi
	call	isCorner@PLT
	testb	%al, %al
	je	.L56
	addl	$50, -5276(%rbp)
.L56:
	movl	-5276(%rbp), %eax
	cmpl	-5284(%rbp), %eax
	jle	.L57
	movl	-5276(%rbp), %eax
	movl	%eax, -5284(%rbp)
	movl	-5280(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	addq	%rbp, %rax
	leaq	-5136(%rax), %rcx
	movq	(%rcx), %rax
	movq	8(%rcx), %rdx
	movq	%rax, -5264(%rbp)
	movq	%rdx, -5256(%rbp)
	movl	16(%rcx), %eax
	movl	%eax, -5248(%rbp)
.L57:
	addl	$1, -5280(%rbp)
.L49:
	movl	-5288(%rbp), %eax
	cmpl	%eax, -5280(%rbp)
	jl	.L58
	cmpl	$2, -5284(%rbp)
	jle	.L59
	movl	-5284(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC10(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	-5304(%rbp), %rcx
	movq	-5264(%rbp), %rax
	movq	-5256(%rbp), %rdx
	movq	%rax, (%rcx)
	movq	%rdx, 8(%rcx)
	movl	-5248(%rbp), %eax
	movl	%eax, 16(%rcx)
	jmp	.L61
.L59:
	movsbl	-5316(%rbp), %edx
	movq	-5304(%rbp), %rax
	movl	%edx, (%rax)
	movq	-5304(%rbp), %rax
	movl	$0, 4(%rax)
	movq	-5304(%rbp), %rax
	movl	$0, 8(%rax)
	movq	-5304(%rbp), %rax
	movl	$0, 12(%rax)
	movq	-5304(%rbp), %rax
	movb	$0, 16(%rax)
.L61:
	movq	-8(%rbp), %rax
	subq	%fs:40, %rax
	je	.L62
	call	__stack_chk_fail@PLT
.L62:
	movq	-5304(%rbp), %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE12:
	.size	findKillerMove, .-findKillerMove
	.globl	isWinningPosition
	.type	isWinningPosition, @function
isWinningPosition:
.LFB13:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movl	%esi, %eax
	movb	%al, -28(%rbp)
	cmpb	$82, -28(%rbp)
	jne	.L64
	movl	$66, %eax
	jmp	.L65
.L64:
	movl	$82, %eax
.L65:
	movb	%al, -13(%rbp)
	cmpb	$82, -13(%rbp)
	jne	.L66
	movq	-24(%rbp), %rax
	movl	76(%rax), %eax
	jmp	.L67
.L66:
	movq	-24(%rbp), %rax
	movl	80(%rax), %eax
.L67:
	movl	%eax, -12(%rbp)
	cmpl	$0, -12(%rbp)
	jne	.L68
	movl	$1, %eax
	jmp	.L69
.L68:
	movsbl	-13(%rbp), %edx
	movq	-24(%rbp), %rax
	movl	%edx, %esi
	movq	%rax, %rdi
	call	hasValidMove@PLT
	testl	%eax, %eax
	jne	.L70
	cmpb	$82, -28(%rbp)
	jne	.L71
	movq	-24(%rbp), %rax
	movl	76(%rax), %eax
	jmp	.L72
.L71:
	movq	-24(%rbp), %rax
	movl	80(%rax), %eax
.L72:
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	cmpl	-12(%rbp), %eax
	setg	%al
	movzbl	%al, %eax
	jmp	.L69
.L70:
	movq	-24(%rbp), %rax
	movl	84(%rax), %eax
	testl	%eax, %eax
	jne	.L73
	cmpb	$82, -28(%rbp)
	jne	.L74
	movq	-24(%rbp), %rax
	movl	76(%rax), %eax
	jmp	.L75
.L74:
	movq	-24(%rbp), %rax
	movl	80(%rax), %eax
.L75:
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	cmpl	-12(%rbp), %eax
	setg	%al
	movzbl	%al, %eax
	jmp	.L69
.L73:
	movl	$0, %eax
.L69:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE13:
	.size	isWinningPosition, .-isWinningPosition
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
