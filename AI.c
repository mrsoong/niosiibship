#include <stdlib.h>

#define GRID_X 		0	//Index 0 of "previous" array
#define GRID_Y 		1	//Index 1 of "previous" array
#define GRID_SIZE 	8
#define MAX_IDX 	7
#define MIN_IDX 	0
#define HIT 		1
#define NOT_HIT 	0

//[x][y][hit (1 or 0 bool)]
int grid[GRID_SIZE][GRID_SIZE];
int previous[2];
int hit_prev = NOT_HIT;

//Initiallizes the map grid the AI uses.
void init_grid(){
	int i;
	for (i = 0; i < GRID_SIZE; i++) {
		int j;
		for (j = 0; j < GRID_SIZE; j++) {
			grid[i][j] = NOT_HIT;
		}
	}
}

//Checks if ship exists on the given coordinate.
int check_hit(int x, int y) {
	if (square[0][x][y] == 2) {
		square[0][x][y] = 4;
		return HIT;
	}
	return NOT_HIT;
}

//Checks if the AI already tried shooting at a coordinate.
int check_coord(int x, int y) {
	if (grid[x][y] == HIT) {
		return 0;
	}
	return 1;
}

int getidx() {
	time_t t;
    srand((unsigned) time(&t));

	return rand() % 8;
}

//Chose random square and return its coordinates if its valid.
int *chose_rand_square() {
	int valid = 0;
	int x;
	int y;
	static int result[2];

	while(valid != 0) {
			x = getidx();
			y = getidx();
			valid = check_coord(x, y);
		}

	result[GRID_X] = x;
	result[GRID_Y] = y;

	return result;
}

int *chose_square() {
	int x;
	int y;
	int valid = 0;
	static int result[2];
	int *temp_result;

	if (hit_prev) {
		while (valid != 0) {
			time_t t;
	   		srand((unsigned) time(&t));

			switch(rand() % 4) {
				//Above the previous square.
				case 0:
					x = previous[GRID_X];
					y = previous[GRID_Y] - 1;
					if ((previous[GRID_Y] != MIN_IDX) & check_coord(x, y)) {
						break;
					}
				//Left of the previous square.
				case 1:
					x = previous[GRID_X] - 1;
					y = previous[GRID_Y];
					if ((previous[GRID_X] != MIN_IDX) & check_coord(x, y)) {
						break;
					}
				//Below the previous square.
				case 2:
					x = previous[GRID_X];
					y = previous[GRID_Y] + 1;
					if ((previous[GRID_Y] != MAX_IDX) & check_coord(x, y)) {
						
						break;
					}
				//Right of the previous square.
				case 3:
					x = previous[GRID_X] + 1;
					y = previous[GRID_Y];
					if ((previous[GRID_X] != MAX_IDX) & check_coord(x, y)) {
						break;
					}
				//If it's never valid.
				default:
					temp_result = chose_rand_square();
					x = temp_result[GRID_X];
					y = temp_result[GRID_Y];
			}
			valid = check_coord(x, y);
		}
	}
	else {
		temp_result = chose_rand_square();
		x = temp_result[GRID_X];
		y = temp_result[GRID_Y];
	}

	hit_prev = check_hit(x, y);

	grid[x][y] = HIT;
	result[GRID_X] = x;
	result[GRID_Y] = y;

	return result;
}