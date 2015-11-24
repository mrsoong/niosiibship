/* This file includes functions that are used to output primitives,
 * texts and complex structures to the VGA adapter
 * A Bresenham's line algorithm based off the pseudocode 
 * on https://en.wikipedia.org/wiki/Bresenham's_line_algorithm
 * is used to implement the drawLine function
 *
 * NOTE: Needs the linker flag -lm to work!!!!!!!!!!!!!!!
 */
#include <stdio.h>
#include <math.h>

// Prototypes
void drawVerticalLine (int x0, int y0, int x1, int y1, short color);

/*
 * Draws a line w/ the fillpixel subroutine
 * Note that the x coordinate is the column number (i.e 0<=x<=239)
 * and the y coordinate is the row number (i.e 0<=y<=319)
 * Assumes the line segment is not just a point 
 * and that this line that starts at (x0,y0) and 
 * ends at (x1, y1) goes down and/or to the right.
 */
void drawLine (int x0, int y0, int x1, int y1, short color) {
	float deltax = x1-x0;
	float deltay = y1-y0;
	float error = 0;
	if (deltax == 0) {
		// Special case when line is vertical
		drawVerticalLine (x0, y0, x1, y1, color);
		return;
	}
	float deltaerr = abs (deltay/deltax);	// The slope of the line, used to keep track of how far 
											// away the line that is being drawn is from the given line
	int y = y0;
	int x, sign;
	// Possibly redundant, since the line is always
	// assumed to be going down and to the right
	if (y1-y0 > 0)
		sign = 1;
	else
		sign = -1;
	for ( x = x0; x <= x1; x++) {
		fillpixel (x, y, color);
		error += deltaerr;
		
		while (error >= 0.5) {
			fillpixel (x, y, color);
			y += sign;
			error -= 1;
		}
	}
}

/* 
 * A function designed to draw purely vertical lines
 */
void drawVerticalLine (int x0, int y0, int x1, int y1, short color) {
	int y;
	for ( y = y0; y <= y1; y++) {
		fillpixel (x0, y, color);
	}
}

/* Draws a (black) grid with a size specified by the caller.
 * Note that the size of each grid is set to 10 pixels
 */
void drawGrid (int x0, int y0, int rows, int columns) {
	int cRow, cColumn;
	int x = x0;
	int y = y0;
	int cellSize = 10;
	
	// Draw the vertical lines first
	for (cColumn = 0; cColumn <= columns; cColumn++) {
		drawLine(x, y, x, (y + (cellSize * rows)), 0);
		x += 10;
	}
	
	x = x0;
	y = y0;
	// Draw the horizontal lines next
	for (cRow = 0; cRow <= rows; cRow++) {
		drawLine(x, y, (x + (cellSize * columns)), y, 0);
		y += 10;
	}
}

void fillRectangle (int x0, int y0, int deltax, int deltay, short color) {
	int x = x0;
	int y = y0;
	for (y = y0; y <= (y0 + deltay - 1); y++) {
		drawLine (x, y, (x + deltax - 1), y, color);
	}
}

void printText (int x, int y, int state) {
	const char *text[8];
	a[1] = "Press Key 0 to begin";
	a[2] = "Please enter the x coordinates for your battleship (1x4)";
	a[3] = "Please enter the y coordinates for your battleship (1x4)";
	a[4] = "Please enter the x coordinates for your submarine (1x3)";
	a[5] = "Please enter the y coordinates for your submarine (1x3)";
	a[6] = "Please enter the x coordinates for your destroyer (1x2)";
	a[7] = "Please enter the y coordinates for your destroyer (1x2)";
	a[8] = "Victory!";
	a[9] = "You were defeated";
	
	// VGA character buffer
    volatile char * character_buffer = (char *) (0x09000000 + (y<<7) + x);
	*character_buffer = text[state];
}