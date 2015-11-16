# Writes a color value into a pixel address
# as specified by the arguments
# Parameters:
# r4=>The pixel's x-coordinate
# r5=>The pixel's y-coordinate
# r6=>The RGB values of the pixel
# Return value (in r2):
# 1 if succeeded, -1 if invalid coordinate(s) is/are provided
# Note: Both the x and y coordinates are expected
# 		to be smaller than or equal to 7

.equ PIXEL_BUFFER_BASE 0x08000000

.text

.global fillpixel
fillpixel: 
	addi sp, sp, -32
	stw r23, 0(sp)
	stw r22, 4(sp)
	stw r21, 8(sp)
	stw r20, 12(sp)
	stw r19, 16(sp)
	stw r18, 20(sp)
	stw r17, 24(sp)
	stw r16, 28(sp)			#Store the callee saved registers
	
	movi r9, 7				#Check to see if the (x,y) coordinates given
							#are invalid
	bgt r4, r9, invalid
	bgt r5, r9, invalid
	blt r4, r0, invalid
	blt r5, r0, invalid
	
	slli r4, 1
	slli r5, 10
	movia r8, PIXEL_BUFFER_BASE		#The pixel buffer's address will initially
									#be set to the base address
	add r8, r8, r4					#Add the x-coordinate to the address
	add r8, r8, r5					#Add the y-coordinate to the address
	stw r6, 0(r8)				#Write color values into the pixel address
	br succeeded	
	
invalid:
	ldw r23, 0(sp)
	ldw r22, 4(sp)
	ldw r21, 8(sp)
	ldw r20, 12(sp)
	ldw r19, 16(sp)
	ldw r18, 20(sp)
	ldw r17, 24(sp)
	ldw r16, 28(sp)			
	addi sp, sp, 32		#Restore the callee saved registers

	movi r2, -1
	ret

succeeded:
	ldw r23, 0(sp)
	ldw r22, 4(sp)
	ldw r21, 8(sp)
	ldw r20, 12(sp)
	ldw r19, 16(sp)
	ldw r18, 20(sp)
	ldw r17, 24(sp)
	ldw r16, 28(sp)			
	addi sp, sp, 32		#Restore the callee saved registers

	movi r2, 1
	ret