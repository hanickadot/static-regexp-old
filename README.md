static-regexp
=============

Static regular expression using c++11 templates.

Usage
-----

	#include "regexp/regexp2.hpp"
	
	using namespace SRegExp2;
	
	// Equivalent to ^([a-z]+)
	RegularExpression<Begin, OneCatch<1, Plus<CSet<'a','z'>> > > regexp;
	
	if (regexp.match(string))
	{
		std::cout << "matched: " << regexp.part<1>(string) << "\n";
	}
	else std::cout << "string not matched!\n";
 
Example of generated code using clang 3.4
-----------------------------------------

	^[a-z]+://[a-z][a-z0-9]+(\.[a-z][a-z0-9]+)*/

will turn into:

	#include "regexp/regexp2.hpp"
	#include <cstdio>

	using namespace SRegExp2;

	int main (int argc, char const *argv[])
	{
		// you can combine sub-regexp into one
		using Protocol = Plus<CSet<'a','z'>>;
		using Domain = Seq<CSet<'a','z'>, Plus<CSet<'a','z','0','9'>>>;
		RegularExpression<Begin, Protocol, Str<':','/','/'>, Domain, Star<Chr<'.'>, Domain >, Chr<'/'> > regexp;
		if (argc >= 2)
		{
			if (regexp(argv[1]))
			{
				printf("yes\n");
			}
			else
			{
				printf("no\n");
			}
		}
		return 0;
	}

After compiled minitest.cpp it will turn into this native code:

	(__TEXT,__text) section
	_main:
	0c30	pushq	%rbp
	0c31	movq	%rsp, %rbp
	0c34	pushq	%r15
	0c36	pushq	%r14
	0c38	pushq	%r13
	0c3a	pushq	%r12
	0c3c	pushq	%rbx
	0c3d	subq	$0x28, %rsp
	0c41	cmpl	$0x2, %edi
	0c44	jl	0x100000df1
	0c4a	movq	0x8(%rsi), %r15
	0c4e	cmpb	$0x0, (%r15)
	0c52	jne	0x100000ced
	0c58	movq	$-0x1, %r12
	0c5f	xorl	%r13d, %r13d
	0c62	leaq	-0x30(%rbp), %r14
	0c66	xorl	%eax, %eax
	0c68	xorl	%ebx, %ebx
	0c6a	jmp	0x100000c80
	0c6c	nopl	(%rax)
	0c70	cmpb	$0x3a, 0x1(%r15,%rbx)
	0c76	leaq	0x1(%rbx), %rbx
	0c7a	sete	%al
	0c7d	decq	%r13
	0c80	testb	$0x1, %al
	0c82	je	0x100000cd0
	0c84	cmpb	$0x2f, 0x1(%rbx,%r15)
	0c8a	jne	0x100000cd0
	0c8c	cmpb	$0x2f, 0x2(%rbx,%r15)
	0c92	jne	0x100000cd0
	0c94	movb	0x3(%rbx,%r15), %al
	0c99	addb	$-0x61, %al
	0c9b	cmpb	$0x19, %al
	0c9d	ja	0x100000cd0
	0c9f	movq	$0x0, -0x30(%rbp)
	0ca7	leaq	0x4(%r15,%rbx), %rdi
	0cac	movq	%r14, %rsi
	0caf	callq	__CYCLE_FOR_DOMAIN
	0cb4	testb	%al, %al
	0cb6	je	0x100000cd0
	0cb8	leaq	0x4(%rbx), %rax
	0cbc	addq	-0x30(%rbp), %rax
	0cc0	testq	%r13, %r13
	0cc3	cmovneq	%rax, %r12
	0cc7	cmovneq	%rax, %rbx
	0ccb	nopl	(%rax,%rax)
	0cd0	movb	(%r15,%rbx), %al
	0cd4	addb	$-0x61, %al
	0cd6	cmpb	$0x19, %al
	0cd8	jbe	0x100000c70
	0cda	testq	%r12, %r12
	0cdd	jns	0x100000de5
	0ce3	cmpb	$0x0, (%r15)
	0ce7	je	0x100000ddc
	0ced	xorl	%ebx, %ebx
	0cef	movq	%r15, %rax
	0cf2	nopw	%cs:(%rax,%rax)
	0d00	leaq	0x1(%rbx), %r12
	0d04	cmpq	%r15, %rax
	0d07	jne	0x100000dca
	0d0d	leaq	0x2(%rbx), %rcx
	0d11	movq	%rcx, -0x48(%rbp)
	0d15	leaq	0x3(%rbx), %rdx
	0d19	movq	%rdx, -0x50(%rbp)
	0d1d	movq	$-0x1, -0x40(%rbp)
	0d25	xorl	%r14d, %r14d
	0d28	xorl	%r13d, %r13d
	0d2b	jmp	0x100000d36
	0d2d	nopl	(%rax)
	0d30	incq	%r13
	0d33	decq	%r14
	0d36	leaq	(%r13,%rbx), %rax
	0d3b	cmpb	$0x3a, (%r15,%rax)
	0d40	jne	0x100000db0
	0d42	leaq	(%r12,%r13), %rax
	0d46	cmpb	$0x2f, (%r15,%rax)
	0d4b	jne	0x100000db0
	0d4d	leaq	(%rcx,%r13), %rax
	0d51	cmpb	$0x2f, (%r15,%rax)
	0d56	jne	0x100000db0
	0d58	leaq	(%rdx,%r13), %rax
	0d5c	movb	(%r15,%rax), %al
	0d60	addb	$-0x61, %al
	0d62	cmpb	$0x19, %al
	0d64	ja	0x100000db0
	0d66	movq	$0x0, -0x38(%rbp)
	0d6e	leaq	0x4(%r13,%rbx), %rdi
	0d73	addq	%r15, %rdi
	0d76	leaq	-0x38(%rbp), %rsi
	0d7a	callq	__CYCLE_FOR_DOMAIN
	0d7f	testb	%al, %al
	0d81	je	0x100000d9e
	0d83	leaq	0x4(%r13), %rax
	0d87	addq	-0x38(%rbp), %rax
	0d8b	testq	%r14, %r14
	0d8e	movq	-0x40(%rbp), %rcx
	0d92	cmovneq	%rax, %rcx
	0d96	movq	%rcx, -0x40(%rbp)
	0d9a	cmovneq	%rax, %r13
	0d9e	movq	-0x48(%rbp), %rcx
	0da2	movq	-0x50(%rbp), %rdx
	0da6	nopw	%cs:(%rax,%rax)
	0db0	leaq	(%r13,%rbx), %rax
	0db5	movb	(%r15,%rax), %al
	0db9	addb	$-0x61, %al
	0dbb	cmpb	$0x19, %al
	0dbd	jbe	0x100000d30
	0dc3	cmpq	$0x0, -0x40(%rbp)
	0dc8	jns	0x100000de5
	0dca	leaq	(%r15,%r12), %rax
	0dce	cmpb	$0x0, (%r15,%r12)
	0dd3	movq	%r12, %rbx
	0dd6	jne	0x100000d00
	0ddc	leaq	0x1af(%rip), %rdi ## literal pool for: "no"
	0de3	jmp	0x100000dec
	0de5	leaq	0x1a9(%rip), %rdi ## literal pool for: "yes"
	0dec	callq	0x100000f70 ## symbol stub for: _puts
	0df1	xorl	%eax, %eax
	0df3	addq	$0x28, %rsp
	0df7	popq	%rbx
	0df8	popq	%r12
	0dfa	popq	%r13
	0dfc	popq	%r14
	0dfe	popq	%r15
	0e00	popq	%rbp
	0e01	ret
	0e02	nopw	%cs:(%rax,%rax)
	__CYCLE_FOR_DOMAIN:
	0e10	pushq	%rbp
	0e11	movq	%rsp, %rbp
	0e14	pushq	%r15
	0e16	pushq	%r14
	0e18	pushq	%r13
	0e1a	pushq	%r12
	0e1c	pushq	%rbx
	0e1d	movq	%rsi, -0x38(%rbp)
	0e21	leaq	0x3(%rdi), %rax
	0e25	movq	%rax, -0x30(%rbp)
	0e29	movq	$-0x1, %r10
	0e30	xorl	%r11d, %r11d
	0e33	xorl	%r13d, %r13d
	0e36	jmp	0x100000e46
	0e38	nopl	(%rax,%rax)
	0e40	incq	%r13
	0e43	incq	%r11
	0e46	movb	(%rdi,%r13), %r14b
	0e4a	cmpb	$0x2f, %r14b
	0e4e	sete	%al
	0e51	movq	$-0x1, %rdx
	0e58	movl	$0x1, %ecx
	0e5d	cmoveq	%rcx, %rdx
	0e61	movzbl	%al, %eax
	0e64	leaq	(%rax,%r13), %rcx
	0e68	cmpb	$0x2e, (%rdi,%rcx)
	0e6c	jne	0x100000f20
	0e72	leaq	0x1(%r13), %r12
	0e76	movq	-0x30(%rbp), %rcx
	0e7a	leaq	(%rcx,%r13), %r15
	0e7e	nop
	0e80	leaq	(%r12,%rax), %rcx
	0e84	movb	(%rdi,%rcx), %cl
	0e87	addb	$-0x61, %cl
	0e8a	cmpb	$0x19, %cl
	0e8d	ja	0x100000f20
	0e93	leaq	0x2(%rax,%r13), %rcx
	0e98	movb	(%rdi,%rcx), %bl
	0e9b	movb	%bl, %cl
	0e9d	addb	$-0x61, %cl
	0ea0	cmpb	$0x1a, %cl
	0ea3	jae	0x100000f14
	0ea5	leaq	(%r15,%rax), %rcx
	0ea9	addq	$0x2, %rax
	0ead	movq	$-0x1, %rbx
	0eb4	movl	$0x1, %r8d
	0eba	nopw	(%rax,%rax)
	0ec0	testq	%r8, %r8
	0ec3	cmovneq	%r8, %rbx
	0ec7	movb	-0x1(%rcx,%r8), %r9b
	0ecc	movb	%r9b, %sil
	0ecf	addb	$-0x61, %sil
	0ed3	addb	$-0x30, %r9b
	0ed7	incq	%r8
	0eda	cmpb	$0x1a, %sil
	0ede	jb	0x100000ec0
	0ee0	cmpb	$0xa, %r9b
	0ee4	jb	0x100000ec0
	0ee6	testq	%rbx, %rbx
	0ee9	js	0x100000f20
	0eeb	leaq	0x1(%rbx,%rax), %rcx
	0ef0	leaq	(%rbx,%rax), %rax
	0ef4	leaq	(%rax,%r13), %rsi
	0ef8	cmpb	$0x2f, (%rdi,%rsi)
	0efc	cmoveq	%rcx, %rdx
	0f00	cmoveq	%rcx, %rax
	0f04	leaq	(%rax,%r13), %rcx
	0f08	cmpb	$0x2e, (%rdi,%rcx)
	0f0c	je	0x100000e80
	0f12	jmp	0x100000f20
	0f14	addb	$-0x30, %bl
	0f17	cmpb	$0x9, %bl
	0f1a	jbe	0x100000ea5
	0f1c	nopl	(%rax)
	0f20	testq	%rdx, %rdx
	0f23	js	0x100000f37
	0f25	addq	%r13, %rdx
	0f28	testq	%r11, %r11
	0f2b	cmovneq	%rdx, %r10
	0f2f	cmovneq	%rdx, %r13
	0f33	movb	(%rdi,%r13), %r14b
	0f37	movb	%r14b, %al
	0f3a	addb	$-0x61, %al
	0f3c	cmpb	$0x1a, %al
	0f3e	jb	0x100000e40
	0f44	addb	$-0x30, %r14b
	0f48	cmpb	$0x9, %r14b
	0f4c	jbe	0x100000e40
	0f52	testq	%r10, %r10
	0f55	js	0x100000f62
	0f57	movq	-0x38(%rbp), %rax
	0f5b	addq	%r10, (%rax)
	0f5e	movb	$0x1, %al
	0f60	jmp	0x100000f64
	0f62	xorl	%eax, %eax
	0f64	popq	%rbx
	0f65	popq	%r12
	0f67	popq	%r13
	0f69	popq	%r14
	0f6b	popq	%r15
	0f6d	popq	%rbp
	0f6e	ret
