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

// Global variables
int square[2][8][8];	/* There a two 8x8 grids that will be displayed
						   one belongs to the player and the other belongs
						   to the AI controlled opponent
						   A value of 1 indicates that the square is empty/unoccupied
						   A value of 2 indicates that the square is occupied
						   A value of 3 indicates that the square is empty 
						   and the opponent has fired a round into the square
						   A value of 4 indicates that the square is occupied 
						   and has been hit
						 */
						   
void initializeSquares () {
	int x;
	int y = 0;
	for (x = 0; x < 8; x++) {
		square[1][x][y] = 1;
		square[2][x][y] = 1;
	}
}

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

/* Color the grids */
void colorGrid (int grid1x, int grid1y, int grid2x, int grid2y) {
	int row, column, x, y;
	for (row = 0; row < 8; row++) {
		for (column = 0; column < 8; column++) {
			
		}
	}
}

void fillRectangle (int x0, int y0, int deltax, int deltay, short color) {
	int x = x0;
	int y = y0;
	for (y = y0; y <= (y0 + deltay - 1); y++) {
		drawLine (x, y, (x + deltax - 1), y, color);
	}
}

/* write a single character to the character buffer at x,y
 * x in [0,79], y in [0,59]
 */
void write_char(int x, int y, char c) {
  // VGA character buffer
  volatile char * character_buffer = (char *) (0x09000000 + (y<<7) + x);
  *character_buffer = c;
}

// Prints some prewritten text in a horizontal line on the screen
void printText (int x, int y, int state) {
	// Clear the entire line in the character buffer
	int i;
	char *blank = " ";
	for (i = 0; i < 80; i++) {
		write_char(i, y, *blank);
	}
	
	char *text[12];
	text[1] = "Press Key 1 to begin";
	text[2] = "Please enter the x coordinates for your battleship (1x4)";
	text[3] = "Please enter the y coordinates for your battleship (1x4)";
	text[4] = "Please enter the x coordinates for your submarine (1x3)";
	text[5] = "Please enter the y coordinates for your submarine (1x3)";
	text[6] = "Please enter the x coordinates for your destroyer (1x2)";
	text[7] = "Please enter the y coordinates for your destroyer (1x2)";
	text[8] = "It's your turn to attack! Please enter the x coordinate of a square that you wish to strike";
	text[9] = "It's your turn to attack! Please enter the y coordinate of a square that you wish to strike";
	text[10] = "Victory!";
	text[11] = "You were defeated";
	
    char *current = text[state];
	int currentX = x;
	while (*current) {
		write_char(currentX, y, *current);
		currentX++;
		current++;
	}
}