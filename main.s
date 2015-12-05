/* Main subroutine for a simplified battleship game
 * The player is able to place ships (horizontally) on an 8x8 grid displayed
 * on the screen, and battle it out against an AI controlled opponent. 
 * 
 *
 * Note: Starting address of main should be set to 0x1000,
 * to avoid conflicts with the interrupt subroutine
 *
 * Grids: A black square in your grid indicates that a ship is currently occuping that square
 *        A red square indicates that a part of a ship thats currently occupying
 *        that square has been hit
 *        A white square indicates that a shell has been fired at that square, without 
 *		  hitting any ships
 */
 
.data

HIT_SOUND: 
	.include "explosion.s"
HIT_SOUND_END:
SPLASH_SOUND: 
	.include "miss.s"
SPLASH_SOUND_END:

.text
/* Constants */
.equ ADDR_TIMER, 0xFF202000 
.equ ADDR_SLIDESWITCHES, 0xFF200040
.equ ADDR_PUSHBUTTONS, 0xFF200050
.equ ADDR_AUDIODACFIFO, 0xFF203040	/* Base address of the audio core */

/* Interrupt Handler */
.section .exceptions, "ax"
IHANDLER:
		rdctl et, ctl4           # check for hardware interrupt
		andi   et,et,0b1         # check if interrupt pending from IRQ0 (timer line)
										# (ctl4:bit0)
		bgt    et,r0,TIMER    # if an interrupt is pending from IRQ0, goto TIMER	
		rdctl et, ctl4           # check for hardware interrupt
		andi   et,et,0b1000000         # check if interrupt pending from IRQ6 (audio line)
										# (ctl4:bit6)
		bgt et, r0, AUDIO
		
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
		
		movi r10, 1
		beq r8, r10, MENU
		
		movi r10, 2
		bge r8, r10, CHECKINPUT2
		br CHECKINPUTDONE
CHECKINPUT2: 
		movi r10, 7
		ble r8, r10, PLACESHIPS
CHECKINPUTDONE:
		movi r10, 8
		beq r8, r10, ATTACK
		movi r10, 9
		beq r8, r10, ATTACK
		movi r10, 11
		beq r8, r10, VICTORYSCREEN
		movi r10, 12
		beq r8, r10, DEFEATSCREEN
		br MENU
		
MENU:
		call drawGrids
		movi r4, 15
		movi r5, 40
		ldw r6, 60(sp)
		call printText
		br CLEANUP
		
PLACESHIPS:
		call drawGrids
		movi r4, 50
		movi r5, 50
		movi r6, 180
		movi r7, 50
		call colorGrid
		movi r4, 15
		movi r5, 40
		ldw r6, 60(sp)
		call printText
		br CLEANUP		
		
ATTACK:
		call drawGrids
		movi r4, 50
		movi r5, 50
		movi r6, 180
		movi r7, 50
		call colorGrid
		movi r4, 15
		movi r5, 40
		ldw r6, 60(sp)
		call printText
		br CLEANUP			

VICTORYSCREEN:
		call drawGrids
		movi r4, 15
		movi r5, 40
		ldw r6, 60(sp)
		call printText
		br CLEANUP
		
DEFEATSCREEN:
		call drawGrids
		movi r4, 15
		movi r5, 40
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
		br EXIT_IHANDLER

AUDIO:
	addi sp, sp, -4
	stw r15, 0(sp)
	movi r20, 0			#Used to store audio samples for the hit sound
	movi r21, 0			#Used to store audio samples for the splash sound
	
LEFT_SAMPLE:
	movia r22, HIT_SOUND
	sub r15, r16, r18
	bge r16, r22, HIT_SOUND_PLAYING
	br LEFT_SAMPLE_CONTINUED
HIT_SOUND_PLAYING:
	blt r15, r22, retrieveHitLeftSample
LEFT_SAMPLE_CONTINUED:
	movia r22, SPLASH_SOUND
	sub r15, r17, r19
	bge r17, r22, SPLASH_SOUND_PLAYING
	br ADD_LEFT_SAMPLES
SPLASH_SOUND_PLAYING:
	blt r15, r22, retrieveSplashLeftSample
	br disableAudioInterrupt
ADD_LEFT_SAMPLES:
	movia r22, ADDR_AUDIODACFIFO
	add r15, r20, r21
	#sthio r15, 8(r22)
	sthio r21, 8(r22)
RIGHT_SAMPLE:
	movia r22, HIT_SOUND
	sub r15, r16, r18
	br retrieveHitRightSample
RIGHT_SAMPLE_CONTINUED:
	movia r22, SPLASH_SOUND
	sub r15, r17, r19
	br retrieveSplashRightSample
ADD_RIGHT_SAMPLES:
	movia r22, ADDR_AUDIODACFIFO
	add r15, r20, r21
	#sthio r15, 12(r22)
	sthio r21, 12(r22)
	
	ldw r15, 0(sp)
	addi sp, sp, 4
	br EXIT_IHANDLER

disableAudioInterrupt:					
	movia r16, ADDR_AUDIODACFIFO
	stw r0, 0(r16)
	ldw r15, 0(sp)
	addi sp, sp, 4
	
	movia r16, ADDR_TIMER
	movi r15, 0b0111			/* Tell the timer to restart and continue whenever it times out,
									and set the interrupt bit to 1 */
	stw r15, 4(r16)				/* Finally enable interrupts on the timer device */
		
EXIT_IHANDLER:
		subi ea,ea,4    # Replay inst that got interrupted 
		eret

.global main
main:
	call initializeSquares
	call fillBackground
	call init_grid
	call place_ships

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
							   8 - Waiting for the user to input the x coordinate of a cell to attack
							   9 - Waiting for the user to input the y coordinate of a cell to attack
							   10 - Waiting for the AI opponent to make a decision
							   11 - Victory screen
							   12 - Game over/Defeat screen
							 */
	movi r9, 0b0			#Current state of push button KEY[1]
	movi r10, 0				#Current state of the (slider) switches
	movi r14, 12			#Highest state # in the game
							 
inputLoop: 
	movia r15, ADDR_PUSHBUTTONS
	ldwio r11, 0(r15)		#Read in buttons - active high */
	andi r13, r11, 0b10
	mov r11, r0
	movia r12, 0x989680		#Number of clock cycles that need to pass for 
							#the input to stabilize (used for debouncing later)
	bgt r13, r0, debounce
	br inputLoop
buttonPressed:
	movia r15, ADDR_SLIDESWITCHES
	ldwio r10, 0(r15)		#Read (slider) switches
	mov r4, r10
	mov r5, r8
	movi r15, 1
	bgt r8, r15, analyzeInput 	#Process the switch input, in case the user has finished inputing coordinates for one of his/her ships
inputLoopContinued:
	movi r15, 11
	bge r8, r15, resetState
	movi r15, 9
	bne r8, r15, incrementState	
checkForPlayerVictory:
	addi sp, sp, -12
	stw r14, 0(sp)
	stw r12, 4(sp)
	stw r8, 8(sp)
	mov r4, r8
	call registerHits
	beq r2, r0, playSplashSound
	movi r15, 1
	beq r2, r15, playHitSound
playSound:
	movia r15, ADDR_AUDIODACFIFO
	ldw r23, 0(r15)
	bne r23, r0, playSound		#Wait for the sound to finish playing before preceding
victoryConditionCheck:
	# Check to see if the player has destroyed all of the opponent's ships
	call checkVictoryConditions
	ldw r14, 0(sp)
	ldw r12, 4(sp)
	ldw r8, 8(sp)
	addi sp, sp, 12
	movi r15, 1
	beq r2, r15, victory	#The player has won
	movi r15, 2
	beq r2, r15, defeat	#The AI has won
AIAttack:
	#call a C function to determine the AI's actions
	addi sp, sp, -12
	stw r14, 0(sp)
	stw r12, 4(sp)
	stw r8, 8(sp)
	call choose_square
	# Check to see if the AI has destroyed all of the player's ships
	call checkVictoryConditions
	ldw r14, 0(sp)
	ldw r12, 4(sp)
	ldw r8, 8(sp)
	addi sp, sp, 12
	movi r15, 1
	beq r2, r15, victory	#The player has won
	movi r15, 2
	beq r2, r15, defeat		#The AI has won
	movi r8, 8				#If the game is still ongoing, then continue retrieving coordinates to attack 
	br inputLoop
	
incrementState:
	addi r8, r8, 1
	bgt r8, r14, resetState
	br inputLoop
	
debounce:
	#Have to debounce the input (for ~15 msec)
	#simply by including a delay in the program
	addi r11, r11, 1
	blt r11, r12, debounce
	br buttonPressed
	
analyzeInput:
	addi sp, sp, -12
	stw r14, 0(sp)
	stw r12, 4(sp)
	stw r8, 8(sp)
	call processInput
	ldw r14, 0(sp)
	ldw r12, 4(sp)
	ldw r8, 8(sp)
	addi sp, sp, 12
	blt r2, r0, invalidInput
	br inputLoopContinued
invalidInput:
	movi r15, 3
	beq r8, r15, decrementState
	movi r15, 5
	beq r8, r15, decrementState
	movi r15, 7
	beq r8, r15, decrementState
	br inputLoop
	
decrementState:
	subi r8, r8, 1
	br inputLoop
	
playHitSound:
	movia r16, HIT_SOUND /* The audio core should play a splash sound */
	movia r18, 168939		/* The splash sound contains this many samples */
	muli r18, r18, 2		/* Each sample takes up 4 bytes in memory */
	
	movia r23, ADDR_TIMER
	movi r15, 0b1000			/* Stop the timer */
	stw r15, 4(r23)

	movi r15, 0b10				
	movia r23, ADDR_AUDIODACFIFO
	stw r15, 0(r23)				/* Enable interrupts on the audio codec device*/
	movi r15, 0b1000001
	wrctl ctl3, r15				/* Enable the correct IRQ lines in ienable */
	br playSound
	
playSplashSound:
	movia r17, SPLASH_SOUND /* The audio core should play a splash sound */
	movia r19, 106335		/* The hit sound contains this many samples */
	muli r19, r19, 2		/* Each sample takes up 4 bytes in memory */

	movia r23, ADDR_TIMER
	movi r15, 0b1000			/* Stop the timer */
	stw r15, 4(r23)

	movi r15, 0b10				
	movia r23, ADDR_AUDIODACFIFO
	stw r15, 0(r23)				/* Enable interrupts on the audio codec device*/
	movi r15, 0b1000001
	wrctl ctl3, r15				/* Enable the correct IRQ lines in ienable */
	br playSound
	
retrieveHitLeftSample:
	ldh r20, 0(r16)
	addi r16, r16, 2
	br LEFT_SAMPLE_CONTINUED
	
retrieveSplashLeftSample:
	ldh r21, 0(r17)
	addi r17, r17, 2
	br ADD_LEFT_SAMPLES
	
retrieveHitRightSample:
	ldh r20, 0(r16)
	addi r16, r16, 2
	br RIGHT_SAMPLE_CONTINUED
	
retrieveSplashRightSample:
	ldh r21, 0(r17)
	addi r17, r17, 2
	br ADD_RIGHT_SAMPLES
	
resetState:
	movi r8, 1
	br main
	
victory:
	movi r8, 11
	br inputLoop
	
defeat:
	movi r8, 12
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
	
	
	