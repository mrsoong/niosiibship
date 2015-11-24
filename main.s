/* Main subroutine for the battleship project
 * Note: Starting address of main should be set to 0x1000,
 * to avoid conflicts with the interrupt
 */
 
/* Constants */
.equ ADDR_TIMER, 0xFF202000 
.equ ADDR_SLIDESWITCHES, 0xFF200040
.equ ADDR_PUSHBUTTONS, 0xFF200050

/* Interrupt Handler */
.section .exceptions, "ax"
IHANDLER:
		rdctl et, ctl4           # check for hardware interrupt
		andi   et,et,0b1         # check if interrupt pending from IRQ0 (timer line)
										# (ctl4:bit0)
		bgt    et,r0,TIMER    # if an interrupt is pending from IRQ0, goto TIMER		
		
TIMER:
		movia r9, ADDR_TIMER
		addi sp, sp, -96
		stw r23, 0(sp)
		stw r22, 4(sp)
		stw r21, 8(sp)
		stw r20, 12(sp)
		stw r19, 16(sp)
		stw r18, 20(sp)
		stw r17, 24(sp)
		stw r16, 28(sp)
		stw r15, 32(sp)
		stw r14, 36(sp)
		stw r13, 40(sp)
		stw r12, 44(sp)
		stw r11, 48(sp)
		stw r10, 52(sp)
		stw r9, 56(sp)
		stw r8, 60(sp)
		stw r7, 64(sp)
		stw r6, 68(sp)
		stw r5, 72(sp)
		stw r4, 76(sp)
		stw ea, 80(sp)
		stw ra, 84(sp)
		stw r3, 88(sp)
		stw r2, 92(sp)
		
		movia r10, 1
		beq r8, r10, MENU
		
MENU:
		call fillBackground
		call drawGrids
		movi r4, 70
		movi r5, 150
		ldw r6, 60(sp)
		call printText
		br CLEANUP
		
4XINPUT:
		call fillBackground
		call drawGrids
		movi r4, 70
		movi r5, 150
		ldw r6, 60(sp)
		call printText
		br CLEANUP
		
4YINPUT:
		call fillBackground
		call drawGrids
		movi r4, 70
		movi r5, 150
		ldw r6, 60(sp)
		call printText
		br CLEANUP
		
3XINPUT:
		call fillBackground
		call drawGrids
		movi r4, 70
		movi r5, 150
		ldw r6, 60(sp)
		call printText
		br CLEANUP
		
3YINPUT:
		call fillBackground
		call drawGrids
		movi r4, 70
		movi r5, 150
		ldw r6, 60(sp)
		call printText
		br CLEANUP

2XINPUT:
		call fillBackground
		call drawGrids
		movi r4, 70
		movi r5, 150
		ldw r6, 60(sp)
		call printText
		br CLEANUP
		
2YINPUT:
		call fillBackground
		call drawGrids
		movi r4, 70
		movi r5, 150
		ldw r6, 60(sp)
		call printText
		br CLEANUP

CLEANUP:
		ldw r23, 0(sp)
		ldw r22, 4(sp)
		ldw r21, 8(sp)
		ldw r20, 12(sp)
		ldw r19, 16(sp)
		ldw r18, 20(sp)
		ldw r17, 24(sp)
		ldw r16, 28(sp)
		ldw r15, 32(sp)
		ldw r14, 36(sp)
		ldw r13, 40(sp)
		ldw r12, 44(sp)
		ldw r11, 48(sp)
		ldw r10, 52(sp)
		ldw r9, 56(sp)
		ldw r8, 60(sp)
		ldw r7, 64(sp)
		ldw r6, 68(sp)
		ldw r5, 72(sp)
		ldw r4, 76(sp)
		ldw ea, 80(sp)
		ldw ra, 84(sp)
		ldw r3, 88(sp)
		ldw r2, 92(sp)
		addi sp, sp, 96
		stwio r0, 0(r9)			#Acknowledge the interrupt

EXIT_IHANDLER:
		subi ea,ea,4    # Replay inst that got interrupted 
		eret

.global main
main:
	movia sp, 0x3B9ACA00	/* Stack starts from the 1000^3th byte (very high up in memory) */
	
	movia r12, ADDR_TIMER
	movia r11, 0xB735
	stwio r11, 8(sp)
	movi r11, 0x000c		/* Set the timeout period to be 1/60 of a second */
	stwio r11, 12(sp)
	movi r11, 0b0111			/* Tell the timer to restart and continue whenever it times out,
									and set the interrupt bit to 1 */
	stw r11, 4(r12)				/* Finally enable interrupts on the timer device */
	
	movi r11, 0b1
	wrctl ctl3, r11				/* Enable the correct IRQ lines in ienable */
	movi r11, 0b1
	wrctl ctl0, r11				/* Enable bit 0 in ctl0 */
	
	movia r8, 1				/* Current state of the game
							   1 - Start screen
							   2 - Waiting for x coordinate of the 1x4 battleship
							   3 - Waiting for y coordinate of the 1x4 battleship
							   4 - Waiting for x coordinate of the 1x3 cruiser
							   5 - Waiting for y coordinate of the 1x3 cruiser
							   6 - Waiting for x coordinate of the 1x2 destroyer
							   7 - Waiting for y coordinate of the 1x2 destroyer
							   8 - Victory screen
							   9 - Game over/Defeat screen
							 */
	movi r9, 0b0			#Current state of push button KEY[1]
	movi r10, 0				#Current state of the (slider) switches
	movi r14, 9				#Highest state # in the game
							 
inputLoop:
	movia r15, ADDR_PUSHBUTTONS
	ldwio r11, 0(r15)		#Read in buttons - active low */
	andi r13, r11, 0b10
	mov r11, r0
	movia r12, 0x989680		#Number of clock cycles that need to pass for 
							#the input to stabilize (used for debouncing later)
	bgt r13, r0, debounce
buttonPressed:
	#Process the switch input
	movia r15, ADDR_SLIDESWITCHES
	ldwio r10, 0(r15)		#Read (slider) switches
	addi r8, r8, 1
	bgt r8, r14, resetState
	br inputLoop
	
debounce:
	#Have to debounce the input (for ~15 msec)
	#simply by including a delay in the program
	addi r11, r11, 1
	blt r11, r12, debounce
	br buttonPressed
	
resetState:
	movi r8, 1
	br inputLoop
	
fillBackground:
	addi sp, sp, -6		# Make the background blue
	movi r4, 0
	movi r5, 0
	movi r6, 320
	movi r7, 240
	movi r15, 0b11111	
	sth r15, 0(sp)
	stw ra, 2(sp)
	call fillRectangle
	ldw ra, 2(sp)
	addi sp, sp, 6
	ret
	
drawGrids:
	addi sp, sp, -4
	stw ra, 0(sp)

	movi r4, 50
	movi r5, 50
	movi r6, 8
	movi r7, 8
	
	call drawGrid	# Draw the first grid
	
	movi r4, 180
	movi r5, 50
	movi r6, 8
	movi r7, 8
	call drawGrid	# Draw the second grid

	ldw ra, 0(sp) 
	addi sp, sp, 4
	ret
	
	
	