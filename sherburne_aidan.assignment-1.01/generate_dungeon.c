#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define ROOM_MIN_X 4
#define ROOM_MIN_Y 3
#define ROOM_MAX_X 12
#define ROOM_MAX_Y 10

#define MIN_DIST 12
#define MIN_ROOMS 6
#define NUM_ROWS 21
#define NUM_COLS 80

#define ROOM_SIZE_X 0
#define ROOM_SIZE_Y 1
#define ROOM_POS_X 2
#define ROOM_POS_Y 3

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
        dungeon[i][j] = ROCK;
    }
  }
}

void draw_corridor(int x0, int y0, int x1, int y1) {

  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = (dx > dy ? dx : -dy) / 2, e2;

  for (;;) {
    
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
    if(dungeon[x0][y0] == ROCK)
     dungeon[x0][y0] = CORRIDOR;
  }

  int i,j;
  for (j = 0; j < NUM_ROWS; j++) {
    for (i = 0; i < NUM_COLS; i++) {
      if(dungeon[i][j] == CORRIDOR){
	if(dungeon[i+1][j+1] == CORRIDOR && !(dungeon[i+1][j] == CORRIDOR || dungeon[i][j+1] == CORRIDOR || dungeon[i-1][j] == FLOOR || dungeon[i][j-1] == FLOOR)){
	  if(dungeon[i+1][j] != FLOOR)
	    dungeon[i+1][j] = CORRIDOR;
	  else if(dungeon[i+1][j] != FLOOR)
	    dungeon[i][j+1] = CORRIDOR;
	}
	if(dungeon[i-1][j-1] == CORRIDOR && !(dungeon[i-1][j] == CORRIDOR || dungeon[i][j-1] == CORRIDOR || dungeon[i-1][j] == FLOOR || dungeon[i][j-1] == FLOOR)){
	  if(dungeon[i-1][j] != FLOOR)
	    dungeon[i-1][j] = CORRIDOR;
	  else if(dungeon[i-1][j] != FLOOR)
	    dungeon[i][j-1] = CORRIDOR;
	}
	if(dungeon[i+1][j-1] == CORRIDOR && !(dungeon[i+1][j] == CORRIDOR || dungeon[i][j-1] == CORRIDOR || dungeon[i+1][j] == FLOOR || dungeon[i][j-1] == FLOOR)){
	  if(dungeon[i+1][j] != FLOOR)
	    dungeon[i+1][j] = CORRIDOR;
	  else if(dungeon[i+1][j] != FLOOR)
	    dungeon[i][j+1] = CORRIDOR;
	}
	if(dungeon[i-1][j+1] == CORRIDOR && !(dungeon[i-1][j] == CORRIDOR || dungeon[i][j+1] == CORRIDOR || dungeon[i-1][j] == FLOOR || dungeon[i][j+1] == FLOOR)){
	  if(dungeon[i-1][j] != FLOOR)
	    dungeon[i-1][j] = CORRIDOR;
	  else if(dungeon[i-1][j] != FLOOR)
	    dungeon[i][j-1] = CORRIDOR;
	}
      }
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
  srand(seed);
  return (rand() % (MIN_ROOMS/2)) + MIN_ROOMS;
}


int draw_room(int x, int y, int w, int h){
  int i,j;
  if(x+w >= NUM_COLS || y+h >= NUM_ROWS)return -1;
  int ret = 0;
  for(i = x; i < x+w; i++){
    for(j = y; j < y+h; j++){
      if(dungeon[i][j] != ROCK){
	return -1;
      }
      if((i==x || j==y) && x > 1 && y > 1){
	if(dungeon[i-1][j] != ROCK || dungeon[i][j-1] != ROCK)
	  return -1;
      }
      if((i==x+w-1 || j==y+h-1) && i<NUM_COLS-1 && j<NUM_ROWS-1){
	if(dungeon[i+1][j] != ROCK || dungeon[i][j+1] != ROCK)
	  return -1;
      }
    }
  }
  for(i = x; i < x+w; i++){
    for(j = y; j < y+h; j++){
      dungeon[i][j] = FLOOR;
    }
  }
  return ret;
}

int get_dist(int x0, int y0, int x1, int y1){
  return (int)sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
}

void generate_rooms(int seed, int num_rooms){
  int rooms[num_rooms][4];
  srand(seed);
  int r;
  for(r = 0; r < num_rooms; r++){
    rooms[r][ROOM_POS_X] = rand() % (NUM_COLS-2) + 1;
    rooms[r][ROOM_POS_Y] = rand() % (NUM_ROWS-2) + 1;
    rooms[r][ROOM_SIZE_X] = rand() % ROOM_MAX_X;
    if(rooms[r][ROOM_SIZE_X] < ROOM_MIN_X) rooms[r][ROOM_SIZE_X] += ROOM_MIN_X;
    rooms[r][ROOM_SIZE_Y] = rand() % ROOM_MAX_Y;
    if(rooms[r][ROOM_SIZE_Y] < ROOM_MIN_Y) rooms[r][ROOM_SIZE_Y] += ROOM_MIN_Y;
    int valid = draw_room(rooms[r][ROOM_POS_X],rooms[r][ROOM_POS_Y],rooms[r][ROOM_SIZE_X],rooms[r][ROOM_SIZE_Y]);
    if(valid != 0) r--;
  }
  for(r=0; r < num_rooms-1; r++){
    int start_x = rooms[r][ROOM_POS_X] + (rooms[r][ROOM_SIZE_X]/2);
    int start_y = rooms[r][ROOM_POS_Y] + (rooms[r][ROOM_SIZE_Y]/2);
    int target_x = rooms[r+1][ROOM_POS_X] + (rooms[r+1][ROOM_SIZE_X]/2);
    int target_y = rooms[r+1][ROOM_POS_Y] + (rooms[r+1][ROOM_SIZE_Y]/2);
    draw_corridor(start_x, start_y, target_x, target_y);
  }
  draw_corridor(rooms[num_rooms-1][ROOM_POS_X],rooms[num_rooms-1][ROOM_POS_Y],rooms[0][ROOM_POS_X],rooms[0][ROOM_POS_Y]);
}

void generate_stairs(){
  int i,j;
  for (j = 0; j < NUM_ROWS; j++) {
    for (i = 0; i < NUM_COLS; i++) {
      
    }
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
  printf("seed: %d, num_rooms: %d\n",seed,num_rooms);
  init_dungeon();
  generate_rooms(seed, num_rooms);  
  generate_stairs();
  print_dungeon();
  
  return 0;
}
