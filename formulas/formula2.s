section .rodata
	one: dd 1.0, 1.0, 1.0, 1.0

section .text

; float formula2(float *x, float *y, unsigned int length)
; rdi = x
; rsi = y
; rdx = length
global formula2
formula2:
	push rbp
	mov rbp, rsp

	movss xmm0, DWORD [rel one]			; xmm0 = prod
	shufps xmm0, xmm0, 0
	movaps xmm1, xmm0				; xmm1 = 1 constant

	xor rcx, rcx
.prod_loop:
	cmp rcx, rdx
	jge .shuf_prod

	movaps xmm2, [rdi + rcx * 4]	; xmm2 = x_vec
	movaps xmm3, [rsi + rcx * 4]	; xmm3 = y_vec
	
	subps xmm2, xmm3			; x - y
	mulps xmm2, xmm2			; (x - y)^2
	addps xmm2, xmm1			; (x - y)^2 + 1
	mulps xmm0, xmm2			; prod((x - y)^2 + 1)

	add rcx, 4
	jmp .prod_loop

.shuf_prod:
	vshufps xmm2, xmm0, xmm0, 0b01001110
	mulps xmm0, xmm2
	vshufps xmm2, xmm0, xmm0, 0b10110001
	mulps xmm0, xmm2

	pxor xmm4, xmm4				; xmm4 = sum
	xor rcx, rcx
.sum_loop:
	cmp rcx, rdx
	jge .hadd_sum
	movaps xmm1, [rdi + rcx * 4]	; xmm2 = x_vec
	movaps xmm2, [rsi + rcx * 4]	; xmm3 = y_vec

	mulps xmm2, xmm3			; x * y
	divps xmm2, xmm0			; xy / prod
	addps xmm4, xmm2

	add rcx, 4
	jmp .sum_loop

.hadd_sum:
	haddps xmm4, xmm4
	haddps xmm4, xmm4

	movss xmm0, xmm4
	mov rbp, rsp
	pop rbp
	ret