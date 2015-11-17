/* Main subroutine for the battleship project
 * Note: Starting address of main should be set to 0x1000,
 * to avoid conflicts with the interrupt
 */

/* Interrupt Handler */
.section .exceptions, "ax"
IHANDLER:
		rdctl et, ctl4           # check for hardware interrupt
		andi   et,et,0b1         # check if interrupt pending from IRQ0 (timer line)
										# (ctl4:bit0)
		bgt    et,r0,TIMER    # if an interrupt is pending from IRQ0, goto TIMER

EXIT_IHANDLER:
		subi ea,ea,4    # Replay inst that got interrupted 
		eret

.global main
main:
	movia sp, 0x3B9ACA00	/* Stack starts from the 1000^3th byte (very high up in memory) */
	
	addi sp, sp, -2		// Make the background blue
	movi r4, 0
	movi r5, 0
	movi r6, 320
	movi r7, 240
	movi r8, 0b11111	
	sth r8, 2(sp)
	call fillRectangle
	addi sp, sp, 2
	
	movi r4, 50
	movi r5, 50
	movi r6, 8
	movi r7, 8
	call drawGrid	// Draw the first grid
	
	movi r4, 180
	movi r5, 50
	movi r6, 8
	movi r7, 8
	call drawGrid	// Draw the second grid
	
	