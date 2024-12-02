section .bss
	mask: resb 16
section .data
	ascii: db 0x01, 0xFF
	times 13 db 0x00
	one: dq 1

section .text

; int hamming_dist(char str1[MAX_STR], char str2[MAX_STR])
; rdi = str1
; rsi = str2
global hamming_dist
hamming_dist:
	push rbp
	push rbx
	mov rbp, rsp

	call str_len
	mov rbx, rax
	push rdi
	mov rdi, rsi
	call str_len
	cmp rax, rbx
	jl .less
	sub rax, rbx
	mov rbx, rax
.less:
	sub rbx, rax

	pop rdi
.mismatch:
	movdqu xmm0, [rdi]
	pcmpistrm xmm0, [rsi], 0b0011000
	cmovz rdx, [rel one]
	cmovs rdx, [rel one]
	movdqu [rel mask], xmm0
	mov ax, [rel mask]
	call count_ones
	add rbx, rcx

	test rdx, rdx
	jnz .done
	add rsi, 16
	add rdi, 16
	jmp .mismatch

.done:
	mov rax, rbx
	mov rsp, rbp
	pop rbx
	pop rbp
	ret

str_len:
	xor rax, rax
	xor rcx, rcx
	movdqu xmm0, [rel ascii]
.loop:
	add rax, rcx
	pcmpistri xmm0, [rdi + rax], 0b0010100
	jnz .loop
	add rax, rcx
	ret

; counts into rcx the ones in ax
count_ones:
	xor rcx, rcx
.loop:
	test ax, ax
	jz .done
	test ax, 1
	jz .next
	inc rcx
.next:
	shr ax, 1
	jmp .loop
.done:
	ret