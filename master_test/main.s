	.file	"main.c"
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__SREG__ = 0x3f
__RAMPZ__ = 0x3b
__tmp_reg__ = 0
__zero_reg__ = 1
	.section	.rodata
.LC0:
	.byte	1
	.byte	2
	.byte	3
	.byte	4
	.byte	5
	.byte	6
	.byte	6
	.byte	7
	.byte	8
	.section	.text.startup,"ax",@progbits
.global	main
	.type	main, @function
main:
	push r16
	push r17
	push r28
	push r29
	in r28,__SP_L__
	in r29,__SP_H__
	sbiw r28,9
	in __tmp_reg__,__SREG__
	cli
	out __SP_H__,r29
	out __SREG__,__tmp_reg__
	out __SP_L__,r28
/* prologue: function */
/* frame size = 9 */
/* stack size = 13 */
.L__stack_usage = 13
	ldi r18,lo8(9)
	ldi r30,lo8(.LC0)
	ldi r31,hi8(.LC0)
	movw r24,r28
	adiw r24,1
	movw r26,r24
	0:
	ld r0,Z+
	st X+,r0
	dec r18
	brne 0b
	out 0x2,__zero_reg__
	ldi r18,lo8(-1)
	out 0x1,r18
	ldi r30,lo8(9)
	ldi r31,0
	movw r26,r24
/* #APP */
 ;  13 "main.c" 1
	RJMP Main_Loop
Txd_bit_high:
	NOP
	NOP
	NOP
	NOP
	NOP
	CBI 2, 0
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	RJMP Bit_Loop_after
	Txd_bit_low:
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	CBI 2, 0
	NOP
	NOP
	NOP
	NOP
	NOP
	RJMP Bit_Loop_after
	Bit_Loop:
	SBI 2, 0
	LSL r16
	BRCC Txd_bit_high
	RJMP Txd_bit_low
	Bit_Loop_after:
	DEC r17
	BREQ Main_Loop
	RJMP Bit_Loop
	Main_Loop:
	LD	r16, X+
	LDI r17,8
	SBIW r30,1
	BRNE Main_Loop
	
 ;  0 "" 2
/* epilogue start */
/* #NOAPP */
	adiw r28,9
	in __tmp_reg__,__SREG__
	cli
	out __SP_H__,r29
	out __SREG__,__tmp_reg__
	out __SP_L__,r28
	pop r29
	pop r28
	pop r17
	pop r16
	ret
	.size	main, .-main
	.ident	"GCC: (GNU) 4.8.1"
.global __do_copy_data
