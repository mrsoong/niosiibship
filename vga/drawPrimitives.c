/* A Bresenham's line algorithm based off the pseudocode 
 * on https://en.wikipedia.org/wiki/Bresenham's_line_algorithm
 * is used to implement the drawLine function
 *
 * NOTE: Needs the linker flag -lm to work!!!!!!!!!!!!!!!
 */
#include <stdio.h>
#include <math.h>

/*
 * Draws a line w/ the fillpixel subroutine
 * Note that the x coordinate is the column (i.e 0<=x<320)
 * and the y coordinate is the row (i.e 0<=y<240)
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
		return drawVerticalLine (x0, y0, x1, y1, color);
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

