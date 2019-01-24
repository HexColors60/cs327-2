#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define ROOM_MIN_X 4
#define ROOM_MIN_Y 3
#define ROOM_MAX_X 12
#define ROOM_MAX_Y 12
#define MIN_ROOMS 6
#define MAX_ROOMS 10
#define NUM_ROWS 21
#define NUM_COLS 90

char ROCK = ' ';
char FLOOR = '.';
char CORRIDOR = '#';
char PLAYER = '%';
char STAIR_UP = '<';
char STAIR_DN = '>';

char dungeon[NUM_COLS][NUM_ROWS];

void init_dungeon() {
  int i, j;
  for (i = 0; i < NUM_COLS; i++) {
    for (j = 0; j < NUM_ROWS; j++) {
      if (j == 0 || j == NUM_ROWS - 1) {
        dungeon[i][j] = '-';
      } else if (i == 0 || i == NUM_COLS - 1) {
        dungeon[i][j] = '|';
      } else
        dungeon[i][j] = ROCK;
    }
  }
}

void draw_corridor(int x0, int y0, int x1, int y1) {

  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = (dx > dy ? dx : -dy) / 2, e2;

  for (;;) {
    dungeon[x0][y0] = CORRIDOR;
    if (x0 == x1 && y0 == y1) break;
    e2 = err;
    if (e2 > -dx) {
      err -= dy;
      x0 += sx;
    }
    if (e2 < dy) {
      err += dx;
      y0 += sy;
    }
  }
}

int main(int argc, char * argv[]) {
  int seed;
  if (argc == 2)
    srand(strtol(argv[1], NULL, 10));
  else
    srand(time(NULL));

  return 0;
}