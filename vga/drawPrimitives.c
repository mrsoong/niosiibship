/* This file includes functions that are used to output primitives,
 * texts and complex structures to the VGA adapter
 * Also includes various other functions used for the game
 *
 * The Bresenham's line algorithm is based off the pseudocode 
 * on https://en.wikipedia.org/wiki/Bresenham's_line_algorithm
 * The functions used to draw triangles are based off of the 
 * code that can be found on this site:
 * http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
 * 3rd party functions were included in the project, after obtaining permission from Prof Anderson to do so
 *
 * NOTE: Needs the linker flag -lm to work!!!!!!!!!!!!!!!
 */
#include <stdio.h>
#include <math.h>
#include "ship.h"

// Structures
struct Vertice {
   float x;
   float y;
};

// Prototypes
void drawVerticalLine (int x0, int y0, int x1, int y1, short color);
void clearText();

// Global variables
volatile char * character_buffer = (char *) 0x09000000;
int square[2][8][8];	/* There a two 8x8 grids that will be displayed
						   the first one belongs to the player and the other belongs
						   to the AI controlled opponent
						   A value of 1 indicates that the square is empty/unoccupied
						   A value of 2 indicates that the square is occupied
						   A value of 3 indicates that the square is empty 
						   and the opponent has fired a round into the square
						   A value of 4 indicates that the square is occupied 
						   and has been hit
						 */
int battleshipPos[3][2];	/* Stores the (x,y) starting coordinates of the "battleships" */
short squareRGB[5]={0b11111, 0b11111, 0, 0xFFFF, 0xF800};
long inputs[13];		// Not all of the inputs are used by the program
int invalidInput = 0;

int processInput (int input, int state) {
	/* Various states in the game (can also be found in main.s)
							   1 - Start screen
							   2 - Waiting for x coordinate of the 1x4 battleship/cruiser
							   3 - Waiting for y coordinate of the 1x4 battleship/cruiser
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
	int x, y, i;
	double temp;
	int active_switches = 0;
	invalidInput = 0;
	if (state > 1) {
		// Store the slider input that was given by the user
		// If the user flipped only one switch, then the number of active bits should be equal to just 1
		inputs[state] = 0;
		for (i = 0; i < 10; i++) {
			if (input & (1 << i)) {
				inputs [state] += i;
				active_switches++;
			}
		}
		if (((active_switches == 0) || (active_switches > 1)) && (state < 10)) {
			invalidInput = 1;
			return -1;
		}
		
		switch (state) {
			case 3:
				// Place the battleship onto the grid/screen
				
				if ((inputs[2] + 4) > 7)
					invalidInput = 1;
				if (invalidInput == 1) {
					return -1;
				}
				
				// Input is valid, so store the positions for the ship
				battleshipPos[0][0] = inputs[2];
				battleshipPos[0][1] = inputs[3];
				y = inputs[3];
				for (x = inputs[2]; x < inputs[2] + 4; x++) {
					square[0][x][y] = 2;
				}
				invalidInput = 0;
				break;
			case 5:
				// Place the battleship onto the grid/screen
				
				// Check to see if the ship that the user wants to place
				// overlaps with an existing ship
				for (x = inputs[4]; x < inputs[4] + 3; x++) {
					if (square[0][x][inputs[5]] == 2) {
						// The cruiser overlaps with another ship
						invalidInput = 1;
					}
				}
				if ((inputs[4] + 3) > 7)
					invalidInput = 1;
				if (invalidInput == 1) {
					return -1;
				}
				
				// Input is valid, so store the positions for the ship
				battleshipPos[1][0] = inputs[4];
				battleshipPos[1][1] = inputs[5];
				y = inputs[5];
				for (x = inputs[4]; x < inputs[4] + 3; x++) {
					square[0][x][y] = 2;
				}
				invalidInput = 0;
				break;
			case 7:
				// Place the battleship onto the grid/screen
				
				// Check to see if the ship that the user wants to place
				// overlaps with an existing ship
				for (x = inputs[6]; x < inputs[6] + 2; x++) {
					if (square[0][x][inputs[7]] == 2) {
						// The cruiser overlaps with another ship
						invalidInput = 1;
					}
				}
				if ((inputs[6] + 2) > 7)
					invalidInput = 1;
				if (invalidInput == 1) {
					return -1;
				}
				
				// Input is valid, so store the positions for the ship
				battleshipPos[2][0] = inputs[6];
				battleshipPos[2][1] = inputs[7];
				y = inputs[7];
				for (x = inputs[6]; x < inputs[6] + 2; x++) {
					square[0][x][y] = 2;
				}
				invalidInput = 0;
				break;
		}
	}
	return 0;
}

/* 
 * Checks to see if the player or the opponent has just won
 * the game. 
 * Expect this to be called whenever the player or the AI
 * opponent launches an attack
 * Returns 0 iff neither the player or the AI has won
 * Returns 1 iff the player has won
 * Returns 2 iff the AI opponent has won
 */
int checkVictoryConditions () {
	int squaresToBeHit = 9;	// 9 squares in the 8x8 need to
							// to be hit in order for either 
							// the AI or the player to win the game
	int hitCounter[2] = {0, 0};
	int i, x, y;					
	for (i = 0; i < 2; i++) {
		for (x = 0; x < 8; x++) {
			for (y = 0; y < 8; y++) {
				if (square[i][x][y] == 4) {
					printf("x: %d y: %d hit! \n", inputs[8], inputs[9]);
					hitCounter[i] += 1;
				}
			}
		}
	}
	if (hitCounter[0] >= squaresToBeHit) {
		// All of the player's ships have been hit, 
		return 2;					
	}
	if (hitCounter[1] >= squaresToBeHit) {
		// All of the AI's ships have been hit
		return 1;
	}
	return 0;
}

int registerHits(int state) {
	// If the square that the user attacked is occupied (and has not been hit yet), then mark it as destroyed/hit
	if (square[1][inputs[8]][inputs[9]] == 2) {
		square[1][inputs[8]][inputs[9]] = 4;
		return 1;
	}
	else if (square[1][inputs[8]][inputs[9]] == 1) {
		square[1][inputs[8]][inputs[9]] = 3;
		return 0;
	}
	else if ((square[1][inputs[8]][inputs[9]] == 3) || (square[1][inputs[8]][inputs[9]] == 4)) {
		return 0;
	}
	else
		return -1;
}
						   
void initializeSquares () {
	int x, y;
	for (x = 0; x < 8; x++) {
		for (y = 0; y < 8; y++) {
			square[0][x][y] = 1;
			square[1][x][y] = 1;
			// square[1][x][y] = 4;
		}
	}
	for (y = 0; y < 12; y++) {
		inputs[y] = 0;
	}
	for (x = 0; x < 3; x++) {
		battleshipPos[x][0] = -1;
		battleshipPos[x][1] = -1;
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
			// Draw vertical lines to fill each of the squares in the displayed grids
			y = grid1y + (10 * row) + 1;
			for (x = grid1x + (10 * column) + 1; x < (grid1x + (10 * column)) + 10; x++) {
				if (square[0][column][row] != 2)
					drawLine(x, y, x, y + 8, squareRGB[square[0][column][row]]);
			}
			y = grid2y + (10 * row) + 1;
			for (x = grid2x + (10 * column) + 1; x < (grid2x + (10 * column) + 10); x++) {
				if (squareRGB[square[1][column][row]] == 0)
					drawLine(x, y, x, y + 8, 0b11111);	// Don't show the opponent's ships until they have been hit
				else {
					drawLine(x, y, x, y + 8, squareRGB[square[1][column][row]]);
				}
			}
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

void fillBottomFlatTriangle(struct Vertice v1, struct Vertice v2, struct Vertice v3)
{
  float invslope1 = (v2.x - v1.x) / (v2.y - v1.y);
  float invslope2 = (v3.x - v1.x) / (v3.y - v1.y);

  float curx1 = v1.x;
  float curx2 = v1.x;
  
  int scanlineY;

  for (scanlineY = v1.y; scanlineY <= v2.y; scanlineY++)
  {
    drawLine((int)curx1, scanlineY, (int)curx2, scanlineY, 0xC618);
    curx1 += invslope1;
    curx2 += invslope2;
  }
}

void fillTopFlatTriangle(struct Vertice v1, struct Vertice v2, struct Vertice v3)
{
  float invslope1 = (v3.x - v1.x) / (v3.y - v1.y);
  float invslope2 = (v3.x - v2.x) / (v3.y - v2.y);

  float curx1 = v3.x;
  float curx2 = v3.x;
  
  int scanlineY;

  for (scanlineY = v3.y; scanlineY > v1.y; scanlineY--)
  {
    curx1 -= invslope1;
    curx2 -= invslope2;
    drawLine((int)curx1, scanlineY, (int)curx2, scanlineY, 0xC618);
  }
}

/* Algorithm used to draw a triangle 
   Note: Only works if v1.y <= v2.y <= v3.y  */
void drawTriangle(struct Vertice v1, struct Vertice v2, struct Vertice v3)
{
  /* here we know that v1.y <= v2.y <= v3.y */
  /* check for trivial case of bottom-flat triangle */
  if (v2.y == v3.y)
  {
    fillBottomFlatTriangle(v1, v2, v3);
  }
  /* check for trivial case of top-flat triangle */
  else if (v1.y == v2.y)
  {
    fillTopFlatTriangle(v1, v2, v3);
  }
  else
  {
    /* general case - split the triangle in a topflat and bottom-flat one */
	struct Vertice v4;
	v4.x = (int)(v1.x + ((float)(v2.y - v1.y) / (float)(v3.y - v1.y)) * (v3.x - v1.x));
	v4.y = v2.y;
    fillBottomFlatTriangle(v1, v2, v4);
    fillTopFlatTriangle(v2, v4, v3);
  }
}

void drawShips() {
	int i;
	int firstSquareX, firstSquareY;
	struct Vertice v1;
	struct Vertice v2;
	struct Vertice v3;
	
	for (i = 0; i < 3; i++) {
		if ((battleshipPos[i][0] != -1) && (battleshipPos[i][1] != -1)) {
			switch (i) {
				case 0:
					firstSquareX = (battleshipPos[0][0] * 10) + 50;
					firstSquareY = (battleshipPos[0][1] * 10) + 50;
					v1.x = firstSquareX + 10;
					v1.y = firstSquareY + 2;
					v2.x = firstSquareX + 5;
					v2.y = firstSquareY + 5;
					v3.x = firstSquareX + 10;
					v3.y = firstSquareY + 7;
					drawTriangle(v1, v2, v3);
					fillRectangle(firstSquareX + 10, firstSquareY + 2, 10, 5, 0xC618);
					fillRectangle(firstSquareX + 20, firstSquareY + 2, 10, 5, 0xC618);
					fillRectangle(firstSquareX + 30, firstSquareY + 2, 10, 5, 0xC618);
					break;
				case 1:
					firstSquareX = (battleshipPos[1][0] * 10) + 50;
					firstSquareY = (battleshipPos[1][1] * 10) + 50;
					fillRectangle(firstSquareX + 10, firstSquareY + 2, 10, 5, 0xC618);
					fillRectangle(firstSquareX + 20, firstSquareY + 2, 10, 5, 0xC618);
					fillRectangle(firstSquareX + 30, firstSquareY + 2, 10, 5, 0xC618);
					break;
				case 2:
					firstSquareX = (battleshipPos[2][0] * 10) + 50;
					firstSquareY = (battleshipPos[2][1] * 10) + 50;
					v1.x = firstSquareX + 5;
					v1.y = firstSquareY + 2;
					v2.x = firstSquareX + 1;
					v2.y = firstSquareY + 5;
					v3.x = firstSquareX + 5;
					v3.y = firstSquareY + 7;
					drawTriangle(v1, v2, v3);
					fillRectangle(firstSquareX + 5, firstSquareY + 2, 10, 5, 0xC618);
					break;
			}
		}
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
	/*for (i = 0; i < 80; i++) {
		write_char(i, y, *blank);
		write_char(i, y + 1, *blank);
	}*/
	clearText();
	
	char *text[13];
	text[1] = "Press Key 1 to begin";
	text[2] = "Please enter the x coordinates for your battleship (1x4)";
	text[3] = "Please enter the y coordinates for your battleship (1x4)";
	text[4] = "Please enter the x coordinates for your submarine (1x3)";
	text[5] = "Please enter the y coordinates for your submarine (1x3)";
	text[6] = "Please enter the x coordinates for your destroyer (1x2)";
	text[7] = "Please enter the y coordinates for your destroyer (1x2)";
	text[8] = "Enter a x coordinate to attack!";
	text[9] = "Enter a y coordinate to attack!";
	text[10] = "Awaiting AI response";
	text[11] = "Victory!";
	text[12] = "You were defeated";
	
	char *invalidInputMessage = "The previous (x,y) coordinates inputed were invalid. Please reenter some valid coordinates";
	
    char *current = text[state];
	int currentX = x;
	while (*current) {
		write_char(currentX, y, *current);
		currentX++;
		current++;
	}
	if (invalidInput == 1) {
		current = invalidInputMessage;
		currentX = x;
		while (*current) {
			write_char(currentX, (y + 1), *current);
			currentX++;
			current++;
		}
	}
}

void clearText() {
	int x, y;
	char *blank = " ";
	for (x = 0; x < 80; x++) {
		for (y = 0; y < 60; y++) {
			write_char(x, y, *blank);
		}
	}
}