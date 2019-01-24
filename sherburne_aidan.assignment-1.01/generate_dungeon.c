#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define ROOM_MIN_X 4
#define ROOM_MIN_Y 3
#define ROOM_MAX_X 10
#define ROOM_MAX_Y 8
#define MIN_DIST 12
#define MIN_ROOMS 6
#define NUM_ROWS 21
#define NUM_COLS 80

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

void print_dungeon(){
  int i,j;
  for (j = 0; j < NUM_ROWS; j++) {
    for (i = 0; i < NUM_COLS; i++) {
      printf("%c",dungeon[i][j]);
    }
    printf("\n");
  }
}

int get_num_rooms(int seed){
  return (seed % MIN_ROOMS) + MIN_ROOMS;
}

int draw_room(int seed, int x, int y){
  srand(seed);

  int w = rand() % ROOM_MAX_X;
  if(w < ROOM_MIN_X) w = w + ROOM_MIN_X;

  int h = rand() % ROOM_MAX_Y;
  if(h < ROOM_MIN_Y) h = h + ROOM_MIN_Y;

}

int get_dist(int x0, int y0, int x1, int y1){
  return (int)sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
}

int generate_rooms(int seed, int num_rooms){
  int r; //rooms generated
  int last_x = 0, last_y = 0;
  srand(seed);
  for(r = 0; r < num_rooms; r++){
    

    int x = 0;
    while(x >= NUM_COLS - 1 || x < 1){
      x = rand() % NUM_COLS;
    }
    int y = 0;
    while(y >= NUM_ROWS - 1 || y < 1){
      y = rand() % NUM_ROWS;
    }
    
    //dungeon[x][y] = 'X';
    if(last_x != 0 && last_y != 0){
      draw_corridor(last_x, last_y, x, y);
    }
    //draw_room(seed, x, y);
    last_x = x;
    last_y = y;
  }
}

int main(int argc, char * argv[]) {
  int seed;
 
  srand(time(NULL));
  if (argc == 2)
    seed = strtol(argv[1], NULL, 10);
  else
    seed = rand();
 
  int num_rooms = get_num_rooms(seed);
  printf("seed: %d, num rooms: %d\n",seed,num_rooms);
  init_dungeon();
  generate_rooms(seed, num_rooms);


  print_dungeon();
  
  return 0;
}
