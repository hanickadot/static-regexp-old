static-regexp
=============

Very fast static regular expression using c++11 templates.

Usage
-----

	#include "regexp/regexp.hpp"
	
	using namespace SRX;
	
	// Equivalent to ^([a-z]+)
	RegularExpression<Begin, OneCatch<1, Plus<CRange<'a','z'>> > > regexp;
	
	if (regexp.match(string))
	{
		std::cout << "matched: " << regexp.part<1>(string) << "\n";
	}
	else std::cout << "string not matched!\n";
 
Benchmark
---------

Simple pattern using static regular expression (`ABCD`) is quick:

	Str<'A','B','C','D'>

	$ time ./build/native/supergrep /tmp/somebigfile.txt > /dev/null
	
	real	0m20.654s
	user	0m18.842s
	sys	0m1.373s

While the normal libc++'s _<regexp>_ implementation is __19.5-times__ slower:

	$ time ./build/native/normalgrep /tmp/somebigfile.txt > /dev/null
	
	real	6m43.090s
	user	6m41.235s
	sys	0m1.544s

And BSD egrep is __3-times__ slower:

	$ time egrep "ABCD" /tmp/somebigfile.txt > /dev/null
	
	real	1m1.935s
	user	1m0.897s
	sys	0m0.784s

And for compare simple BSD fgrep is at same speed as egrep:

	$ time fgrep "ABCD" /tmp/somebigfile.txt > /dev/null
	
	real	1m1.370s
	user	1m0.410s
	sys	0m0.759s

More complex benchmark
----------------------

For __more complex pattern__ (`ABCDE|DEFGH|EFGHI|AAAA+`) static regular expression maintain same speed:

	Selection<Str<'A','B','C','D','E'>,Str<'D','E','F','G','H'>,Str<'E','F','G','H','I'>,Repeat<4,0,Chr<'A'>>>

	real	0m21.647s
	user	0m20.579s
	sys	0m1.064s

And libc++'s _<regexp>_ is __3650-times__ slower:

	real	63m18.043s
	user	63m9.890s
	sys	0m4.731s

And BSD egrep is __10.6-times__ slower:

	real	3m39.765s
	user	3m37.909s
	sys	0m1.129s
 
Example of generated code using clang 3.4
-----------------------------------------

	^[a-z]+://[a-z][a-z0-9]+(\.[a-z][a-z0-9]+)*/

will turn into:

	#include "regexp/regexp.hpp"
	#include <cstdio>

	using namespace SRX;

	int main (int argc, char const *argv[])
	{
		// you can combine sub-regexp into one
		using Protocol = Plus<CRange<'a','z'>>;
		using Domain = Seq<CRange<'a','z'>, Plus<CRange<'a','z','0','9'>>>;
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
	0d00  pushq   %rbp
	0d01  movq    %rsp, %rbp
	0d04  pushq   %r15
	0d06  pushq   %r14
	0d08  pushq   %r13
	0d0a  pushq   %r12
	0d0c  pushq   %rbx
	0d0d  subq    $0x38, %rsp
	0d11  cmpl    $0x2, %edi
	0d14  jl      0f56
	0d1a  movq    0x8(%rsi), %rax
	0d1e  cmpb    $0x0, (%rax)
	0d21  je      0f4a
	0d27  leaq    0x7(%rax), %rcx
	0d2b  movq    %rcx, -0x60(%rbp)
	0d2f  xorl    %edx, %edx
	0d31  movq    %rax, %rcx
	0d34  nopw    %cs:(%rax,%rax)
	0d40  movq    %rdx, -0x38(%rbp)
	0d44  cmpq    %rax, %rcx
	0d47  movq    -0x60(%rbp), %rdi
	0d4b  movq    $-0x1, %r8
	0d52  movl    $0x0, %ebx
	0d57  jne     0f36
	0d5d  jmp     0d66
	0d5f  nop    
	0d60  incq    %rbx
	0d63  incq    %rdi
	0d66  movzbl  (%rax,%rbx), %esi
	0d6a  cmpl    $0x3a, %esi
	0d6d  jne     0f20
	0d73  movzbl  0x1(%rbx,%rax), %ecx
	0d78  cmpl    $0x2f, %ecx
	0d7b  jne     0f31
	0d81  movzbl  0x2(%rbx,%rax), %ecx
	0d86  cmpl    $0x2f, %ecx
	0d89  jne     0f20
	0d8f  movb    0x3(%rbx,%rax), %cl
	0d93  addb    $-0x61, %cl
	0d96  movzbl  %cl, %ecx
	0d99  cmpl    $0x19, %ecx
	0d9c  ja      0f20
	0da2  movl    %esi, -0x54(%rbp)
	0da5  movq    %r8, -0x40(%rbp)
	0da9  leaq    0x4(%rbx), %rcx
	0dad  movq    %rcx, -0x30(%rbp)
	0db1  movq    %rbx, -0x50(%rbp)
	0db5  movq    $-0x1, %r14
	0dbc  movq    %rdi, %r12
	0dbf  movq    %rdi, -0x48(%rbp)
	0dc3  xorl    %edi, %edi
	0dc5  jmp     0dd6
	0dc7  nopw    (%rax,%rax)
	0dd0  incq    %rdi
	0dd3  incq    %r12
	0dd6  movq    -0x30(%rbp), %rcx
	0dda  leaq    (%rdi,%rcx), %r8
	0dde  movzbl  (%rax,%r8), %r13d
	0de3  cmpl    $0x2f, %r13d
	0de7  movq    $-0x1, %rdx
	0dee  movl    $0x1, %ecx
	0df3  cmoveq  %rcx, %rdx
	0df7  cmpl    $0x2e, %r13d
	0dfb  movl    $0x1, %esi
	0e00  movl    $0x0, %r9d
	0e06  jne     0eb0
	0e0c  nopl    (%rax)
	0e10  addq    %r8, %rsi
	0e13  movb    (%rax,%rsi), %cl
	0e16  addb    $-0x61, %cl
	0e19  movzbl  %cl, %ecx
	0e1c  cmpl    $0x19, %ecx
	0e1f  ja      0eb0
	0e25  leaq    0x2(%r9,%r8), %rcx
	0e2a  movb    (%rax,%rcx), %bl
	0e2d  movb    %bl, %cl
	0e2f  addb    $-0x61, %cl
	0e32  movzbl  %cl, %ecx
	0e35  cmpl    $0x1a, %ecx
	0e38  jb      0e45
	0e3a  addb    $-0x30, %bl
	0e3d  movzbl  %bl, %ecx
	0e40  cmpl    $0x9, %ecx
	0e43  ja      0eb0
	0e45  leaq    (%r12,%r9), %r11
	0e49  addq    $0x2, %r9
	0e4d  movq    $-0x1, %r10
	0e54  xorl    %esi, %esi
	0e56  nopw    %cs:(%rax,%rax)
	0e60  cmpl    $-0x1, %esi
	0e63  movb    (%r11,%rsi), %r15b
	0e67  leaq    0x1(%rsi), %rsi
	0e6b  cmovneq %rsi, %r10
	0e6f  movb    %r15b, %cl
	0e72  addb    $-0x61, %cl
	0e75  movzbl  %cl, %ecx
	0e78  addb    $-0x30, %r15b
	0e7c  cmpl    $0x1a, %ecx
	0e7f  jb      0e60
	0e81  movzbl  %r15b, %ecx
	0e85  cmpl    $0xa, %ecx
	0e88  jb      0e60
	0e8a  testq   %r10, %r10
	0e8d  js      0eb0
	0e8f  leaq    0x1(%r10,%r9), %rsi
	0e94  leaq    (%r10,%r9), %r9
	0e98  leaq    (%r9,%r8), %rcx
	0e9c  movzbl  (%rax,%rcx), %ecx
	0ea0  cmpl    $0x2f, %ecx
	0ea3  cmoveq  %rsi, %rdx
	0ea7  cmpl    $0x2e, %ecx
	0eaa  je      0e10
	0eb0  testq   %rdx, %rdx
	0eb3  js      0ebe
	0eb5  addq    %rdi, %rdx
	0eb8  testl   %edi, %edi
	0eba  cmovneq %rdx, %r14
	0ebe  movb    %r13b, %cl
	0ec1  addb    $-0x61, %cl
	0ec4  movzbl  %cl, %ecx
	0ec7  cmpl    $0x1a, %ecx
	0eca  jb      0dd0
	0ed0  addb    $-0x30, %r13b
	0ed4  movzbl  %r13b, %ecx
	0ed8  cmpl    $0x9, %ecx
	0edb  jbe     0dd0
	0ee1  testq   %r14, %r14
	0ee4  js      0f02
	0ee6  addq    -0x30(%rbp), %r14
	0eea  movq    -0x50(%rbp), %rbx
	0eee  testl   %ebx, %ebx
	0ef0  movq    -0x40(%rbp), %r8
	0ef4  cmovneq %r14, %r8
	0ef8  movq    -0x38(%rbp), %rdx
	0efc  movq    -0x48(%rbp), %rdi
	0f00  jmp     0f12
	0f02  movq    -0x38(%rbp), %rdx
	0f06  movq    -0x40(%rbp), %r8
	0f0a  movq    -0x48(%rbp), %rdi
	0f0e  movq    -0x50(%rbp), %rbx
	0f12  movl    -0x54(%rbp), %esi
	0f15  nopw    %cs:(%rax,%rax)
	0f20  addb    $-0x61, %sil
	0f24  movzbl  %sil, %ecx
	0f28  cmpl    $0x19, %ecx
	0f2b  jbe     0d60
	0f31  testq   %r8, %r8
	0f34  jns     0f67
	0f36  leaq    0x1(%rax,%rdx), %rcx
	0f3b  cmpb    $0x0, 0x1(%rax,%rdx)
	0f40  leaq    0x1(%rdx), %rdx
	0f44  jne     0d40
	0f4a  leaq    0x41(%rip), %rdi  ## literal pool for: "no"
	0f51  callq   0f70              ## symbol stub for: _puts
	0f56  xorl    %eax, %eax
	0f58  addq    $0x38, %rsp
	0f5c  popq    %rbx
	0f5d  popq    %r12
	0f5f  popq    %r13
	0f61  popq    %r14
	0f63  popq    %r15
	0f65  popq    %rbp
	0f66  retq
	0f67  leaq    0x27(%rip), %rdi  ## literal pool for: "yes"
	0f6e  jmp     0f51