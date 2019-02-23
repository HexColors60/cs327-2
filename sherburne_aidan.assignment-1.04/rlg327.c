#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <unistd.h>

#include "dungeon.h"
#include "path.h"

void usage(char *name)
{
  fprintf(stderr,
          "Usage: %s [-r|--rand <seed>] [-l|--load [<file>]]\n"
          "          [-s|--save [<file>]] [-i|--image <pgm file>]\n"
	  "[-n|--nummon <num monsters(min 1)>]\n",
          name);

  exit(-1);
}

static int32_t event_cmp(const void *key, const void *with){
  int32_t ret = ((int32_t)((character_t *) key)->next_turn - (int32_t)((character_t *) with)->next_turn);
  if(ret == 0){
    ret = ((int32_t)((character_t *) key)->tie_breaker - (int32_t)((character_t *) with)->tie_breaker);
  }
  return ret;
}

//returns positive if c1 can see c2, 0 if not
//modified from http://www.roguebasin.com/index.php?title=Another_version_of_BLA
int line_of_sight(dungeon_t *d, character_t *c1, character_t *c2){
  pair_t p1, p2, delta, move;
  int32_t error;
  int32_t view_dist = 5;

  p1[dim_x] = c1->pos[dim_x];
  p1[dim_y] = c1->pos[dim_y];
  p2[dim_x] = c2->pos[dim_x];
  p2[dim_y] = c2->pos[dim_y];

  //Calculate deltas
  delta[dim_y] = abs(p2[dim_y] - p1[dim_y]) << 1;
  delta[dim_x] = abs(p2[dim_x] - p1[dim_x]) << 1;
  //Calculate signs
  move[dim_y] = p2[dim_y] >= p1[dim_y] ? 1 : -1;
  move[dim_x] = p2[dim_x] >= p1[dim_x] ? 1 : -1;

  /* There is an automatic line of sight, of course, between a
   * location and the same location or directly adjacent
   * locations. */
  if(abs(p2[dim_x] - p1[dim_x]) < 2 && abs(p2[dim_y] - p1[dim_y]) < 2) {
    return 1;
  }

  /* Ensure that the line will not extend too long. */
  if (((p2[dim_x] - p1[dim_x]) * (p2[dim_x] - p1[dim_x])) +
      ((p2[dim_y] - p1[dim_y]) * (p2[dim_y] - p1[dim_y])) >
      view_dist * view_dist) {
    return 0;
  }

  /* "Draw" the line, checking for obstructions. */
  if (delta[dim_x] >= delta[dim_y]) {
    /* Calculate the error factor, which may go below zero. */
    error = delta[dim_y] - (delta[dim_x] >> 1);

    /* Search the line. */
    while (p2[dim_x] != p1[dim_x]) {
      /* Check for an obstruction. If the obstruction can be "moved
       * around", it isn't really an obstruction. */
      if ((hardnesspair(p1) > 0 && (((p1[dim_y] - move[dim_y] >= 1) && (p1[dim_y] - move[dim_y] <= DUNGEON_Y)) && (hardnessxy(p1[dim_x], p1[dim_y] - move[dim_y]) > 0))) || (p1[dim_y] != p2[dim_y] || !(delta[dim_y]))) {
	return 0;
      }

      /* Update values. */
      if (error > 0) {
	if (error || (move[dim_x] > 0)) {
	  p1[dim_y] += move[dim_y];
	  error -= delta[dim_x];
	}
      }
      p1[dim_x] += move[dim_x];
      error += delta[dim_y];
    }
  }
  else {
      /* Calculate the error factor, which may go below zero. */
    error = delta[dim_x] - (delta[dim_y] >> 1);

    /* Search the line. */
    while (p1[dim_y] != p2[dim_y]) {
      /* Check for an obstruction. If the obstruction can be "moved
       * around", it isn't really an obstruction. */
      if (((hardnesspair(p1) > 0) && (((p1[dim_x] - move[dim_x] >= 1) && (p1[dim_x] - move[dim_x] <= DUNGEON_X)) && (hardnessxy(p1[dim_x] - move[dim_x], p1[dim_y]) > 0))) || (p1[dim_x] != p2[dim_x] || !(delta[dim_x]))) {
	return 0;
      }

      /* Update values. */
      if (error > 0) {
	if (error || (move[dim_y] > 0)) {
	  p1[dim_x] += move[dim_x];
	  error -= delta[dim_y];
	}
      }
      p1[dim_y] += move[dim_y];
      error += delta[dim_x];
    }
  }
  return 1;
}

void move_character(dungeon_t *d, character_t *c){
  pair_t new_pos;
  new_pos[dim_y] = c->pos[dim_y];
  new_pos[dim_x] = c->pos[dim_x];

  //uint32_t i;
  //int x, y;
  //int min_val = INT_MAX;

  // switch(c->at){//switch based on the attributes
  // case 0: //plain old boring monster
  //   if(line_of_sight(d, c, &d->pc) == 1){
  //     //TODO move in straight line
  //     break;
  //   }
  //   break;
  //
  // case 1: //erratic
  //   //50% chance to move randomly
  //   if(rand() % 2 == 1){//move erratically
  //     new_pos[dim_y] = (c->pos[dim_y] - 1 + (rand() % 3));
  //     new_pos[dim_x] = (c->pos[dim_x] - 1 + (rand() % 3));
  //     //find open position
  //     while(hardnesspair(new_pos) != 0 ||
	//     (new_pos[dim_y] == c->pos[dim_y] && new_pos[dim_x] == c->pos[dim_x])){
	//        new_pos[dim_y] = (c->pos[dim_y] - 1 + (rand() % 3));
	//        new_pos[dim_x] = (c->pos[dim_x] - 1 + (rand() % 3));
  //     }
  //   }/*else{//move line of sight
  //      new_pos[dim_y] = 0;
  //      new_pos[dim_x] = 0;
  //      }*/
  //   break;
  //
  // case 2: //tunneling
  //   //TODO move line of sight tunneling
  //   break;
  //
  // case 3: //erratic & tunneling
  //   if(rand() % 2 == 1){//move erratically
  //     new_pos[dim_y] = (c->pos[dim_y] - 1 + (rand() % 3));
  //     new_pos[dim_x] = (c->pos[dim_x] - 1 + (rand() % 3));
  //     //find open position
  //     while(mappair(new_pos) != ter_wall_immutable ||
	//     (new_pos[dim_y] == c->pos[dim_y] && new_pos[dim_x] == c->pos[dim_x])){
	// new_pos[dim_y] = (c->pos[dim_y] - 1 + (rand() % 3));
	// new_pos[dim_x] = (c->pos[dim_x] - 1 + (rand() % 3));
  //     }
  //     //if we need to create a tunnel
  //     if(hardnesspair(new_pos) > 85){
	// hardnesspair(new_pos) -= 85;
	// new_pos[dim_y] = c->pos[dim_y];
	// new_pos[dim_x] = c->pos[dim_x];
  //     }
  //     else if(hardnesspair(new_pos) <= 85){
	// hardnesspair(new_pos) = 0;
	// mappair(new_pos) = ter_floor_hall;
  //     }
  //   }/*else{//TODO move line of sight
  //      new_pos[dim_y] = 0;
  //      new_pos[dim_x] = 0;
  //      }*/
  //   break;
  //
  // case 4: //telepathic
  //   //TODO move telepathically
  //   break;
  //
  // case 5: //erratic & telepathic
  //   /*if(rand() % 2 == 1){//move erratically
  //     new_pos[dim_y] = (c->pos[dim_y] - 1 + (rand() % 3));
  //     new_pos[dim_x] = (c->pos[dim_x] - 1 + (rand() % 3));
  //     //find open position
  //     while(hardnesspair(new_pos) != 0 ||
  //     (new_pos[dim_y] == c->pos[dim_y] && new_pos[dim_x] == c->pos[dim_x])){
  //     new_pos[dim_y] = (c->pos[dim_y] - 1 + (rand() % 3));
  //     new_pos[dim_x] = (c->pos[dim_x] - 1 + (rand() % 3));
  //     }
  //     }else{//move telepathically
  //
  //     }*/
  //   break;
  //
  // case 6: //tunneling & telepathic
  //   //TODO move telepathically
  //   break;
  //
  // case 7: //erratic & tunneling & telepathic
  //   break;
  //
  // case 8: //intelligent
  //
  //   break;
  //
  // case 9: //erratic & intelligent
  //   break;
  //
  // case 10://tunneling & intelligent
  //   break;
  //
  // case 11://erratic & tunneling & intelligent
  //   break;
  //
  // case 12://telepathic & intelligent
  //   dijkstra(d);
  //   for(y = -1; y <= 1; y++){
  //     for(x = -1; x <= 1; x++){
	// if(d->pc_distance[c->pos[dim_y]+y][c->pos[dim_x]+x] < min_val){
	//   min_val = d->pc_distance[c->pos[dim_y]+y][c->pos[dim_x]+x];
	//   new_pos[dim_y] = c->pos[dim_y]+y;
	//   new_pos[dim_x] = c->pos[dim_x]+x;
	// }
  //     }
  //   }
  //   break;
  //
  // case 13://erratic & elepathic & intelligent
  //   break;
  //
  // case 14://tunneling & telepathic & intelligent
  //   dijkstra_tunnel(d);
  //   for(y = -1; y <= 1; y++){
  //     for(x = -1; x <= 1; x++){
	// if(d->pc_tunnel[c->pos[dim_y]+y][c->pos[dim_x]+x] < min_val){
	//   min_val = d->pc_tunnel[c->pos[dim_y]+y][c->pos[dim_x]+x];
	//   new_pos[dim_y] = c->pos[dim_y]+y;
	//   new_pos[dim_x] = c->pos[dim_x]+x;
	// }
  //     }
  //   }
  //   //if we need to create a tunnel
  //   if(hardnesspair(new_pos) > 85){
  //     hardnesspair(new_pos) -= 85;
  //     new_pos[dim_y] = c->pos[dim_y];
  //     new_pos[dim_x] = c->pos[dim_x];
  //   }
  //   else if(hardnesspair(new_pos) <= 85){
  //     hardnesspair(new_pos) = 0;
  //     mappair(new_pos) = ter_floor_hall;
  //   }
  //   break;
  //
  // case 15://erratic & tunneling & telepathic & intelligent
  //   if(rand() % 2 == 1){//move erratically
  //     new_pos[dim_y] = (c->pos[dim_y] - 1 + (rand() % 3));
  //     new_pos[dim_x] = (c->pos[dim_x] - 1 + (rand() % 3));
  //     //find open position
  //     while(d->hardness[new_pos[dim_y]][new_pos[dim_x]] != 0 ||
	//     (new_pos[dim_y] == c->pos[dim_y] && new_pos[dim_x] == c->pos[dim_x])){
	// new_pos[dim_y] = (c->pos[dim_y] - 1 + (rand() % 3));
	// new_pos[dim_x] = (c->pos[dim_x] - 1 + (rand() % 3));
  //     }
  //   }else{//move tunneling & telepathic & intelligent
  //     dijkstra_tunnel(d);
  //     for(y = -1; y <= 1; y++){
	// for(x = -1; x <= 1; x++){
	//   if(d->pc_tunnel[c->pos[dim_y]+y][c->pos[dim_x]+x] < min_val){
	//     min_val = d->pc_tunnel[c->pos[dim_y]+y][c->pos[dim_x]+x];
	//     new_pos[dim_y] = c->pos[dim_y]+y;
	//     new_pos[dim_x] = c->pos[dim_x]+x;
	//   }
	// }
  //     }
  //     //if we need to create a tunnel
  //     if(hardnesspair(new_pos) > 85){
	// hardnesspair(new_pos) -= 85;
	// new_pos[dim_y] = c->pos[dim_y];
	// new_pos[dim_x] = c->pos[dim_x];
  //     }
  //     else if(hardnesspair(new_pos) <= 85){
	// hardnesspair(new_pos) = 0;
	// mappair(new_pos) = ter_floor_hall;
  //     }
  //   }
  //   break;
  //
  // default:
  //   break;
  // }
  if(c->pos[dim_y]+1 < DUNGEON_Y-1 && c->pos[dim_x]+1 < DUNGEON_X-1){
    new_pos[dim_y] = c->pos[dim_y]+1;
    new_pos[dim_x] = c->pos[dim_x]+1;
  }
  printf("trying %c from %d,%d to %d,%d\n",c->disp,c->pos[dim_y],c->pos[dim_x],new_pos[dim_y],new_pos[dim_x]);

  //if we need to move and there is a different monster in the new pos, kill it
  if(cpair(new_pos) != NULL && cpair(new_pos)->tie_breaker != c->tie_breaker){
      printf("***%c was killed by %c***\n", cpair(new_pos)->disp, c->disp);
      printf("monsters alive: %d\n",d->alive_monsters);
      cpair(new_pos)->alive = 0;
      d->alive_monsters--;
    }
  cpair(c->pos) = NULL;
  c->pos[dim_y] = new_pos[dim_y];
  c->pos[dim_x] = new_pos[dim_x];
  cpair(c->pos) = c;
  //printf("moved %c to %d,%d\n",c->disp,c->pos[dim_y],c->pos[dim_x]);
}

void turn_handler(dungeon_t *d){
  character_t *c = heap_remove_min(d->h);
  if(c->alive == 1){ //only need to move if the character was alive
    move_character(d, c); //perform our move action
    c->next_turn += (1000/c->speed); //update this character's next turns
    c->hn = heap_insert(d->h, c); //reinsert our character into the heap
  }
  if(c->is_pc == 1){ //redraw the dungeon after each pc move
    render_dungeon(d);
    usleep(250000); //pause o that an observer can see the updates
  }
}

void gen_monsters(dungeon_t *d){
  uint16_t i, x, y;
  const static char disps[] = "0123456789abcdef";
  srand(time(NULL));

  d->alive_monsters = 0;

  //set up our pc character
  d->pc.speed = 10;
  d->pc.disp = '@';
  d->pc.alive = 1;
  d->pc.next_turn = 0;
  d->pc.tie_breaker = 0;
  d->pc.is_pc = 1;
  d->pc.at = 1; // our pc will only be erratic for now
  d->pc.hn = heap_insert(d->h, &(d->pc));
  cpair(d->pc.pos) = &(d->pc);
  for(i = 0; i < d->num_monsters; i++){
    character_t *mon = malloc(sizeof(character_t));//Malloc our new monster
    y = rand() % 20;
    x = rand() % 80;
    while(cxy(x, y) || d->hardness[y][x] != 0){
      y = rand() % 20;
      x = rand() % 80;
    }
    mon->pos[dim_y] = y;
    mon->pos[dim_x] = x;
    mon->speed = rand() % 16 + 5;
    mon->alive = 1;
    uint8_t at = 0;
    at += (rand() % 2) * 1; // erratic binary bit
    at += (rand() % 2) * 2; // tunneling binary bit
    at += (rand() % 2) * 4; // telepathy binary bit
    at += (rand() % 2) * 8; // intelligent binary bit
    mon->at = at;
    mon->disp = disps[at];
    mon->next_turn = 0;
    mon->tie_breaker = i+1;
    mon->is_pc = 0;
    mon->hn = heap_insert(d->h, mon) ;
    cpair(mon->pos) = mon;
    d->alive_monsters++;
  }
}

int main(int argc, char *argv[])
{
  dungeon_t d;
  time_t seed;
  struct timeval tv;
  uint32_t i;
  uint32_t do_load, do_save, do_seed, do_image, do_save_seed, do_save_image;
  uint32_t long_arg;
  char *save_file;
  char *load_file;
  char *pgm_file;

  /* Quiet a false positive from valgrind. */
  memset(&d, 0, sizeof (d));

  /* Default behavior: Seed with the time, generate a new dungeon, *
   * and don't write to disk.                                      */
  do_load = do_save = do_image = do_save_seed = do_save_image = 0;
  do_seed = 1;
  save_file = load_file = NULL;
  d.max_monsters = MAX_MONSTERS;
  d.num_monsters = 10;

  /* The project spec requires '--load' and '--save'.  It's common  *
   * to have short and long forms of most switches (assuming you    *
   * don't run out of letters).  For now, we've got plenty.  Long   *
   * forms use whole words and take two dashes.  Short forms use an *
    * abbreviation after a single dash.  We'll add '--rand' (to     *
   * specify a random seed), which will take an argument of it's    *
   * own, and we'll add short forms for all three commands, '-l',   *
   * '-s', and '-r', respectively.  We're also going to allow an    *
   * optional argument to load to allow us to load non-default save *
   * files.  No means to save to non-default locations, however.    *
   * And the final switch, '--image', allows me to create a dungeon *
   * from a PGM image, so that I was able to create those more      *
   * interesting test dungeons for you.                             */

 if (argc > 1) {
    for (i = 1, long_arg = 0; i < argc; i++, long_arg = 0) {
      if (argv[i][0] == '-') { /* All switches start with a dash */
        if (argv[i][1] == '-') {
          argv[i]++;    /* Make the argument have a single dash so we can */
          long_arg = 1; /* handle long and short args at the same place.  */
        }
        switch (argv[i][1]) {
        case 'r':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-rand")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%lu", &seed) /* Argument is not an integer */) {
            usage(argv[0]);
          }
          do_seed = 0;
          break;
        case 'l':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-load"))) {
            usage(argv[0]);
          }
          do_load = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            load_file = argv[++i];
          }
          break;
        case 's':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-save"))) {
            usage(argv[0]);
          }
          do_save = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll save to it.  If it is "seed", we'll save to    *
	     * <the current seed>.rlg327.  If it is "image", we'll  *
	     * save to <the current image>.rlg327.                  */
	    if (!strcmp(argv[++i], "seed")) {
	      do_save_seed = 1;
	      do_save_image = 0;
	    } else if (!strcmp(argv[i], "image")) {
	      do_save_image = 1;
	      do_save_seed = 0;
	    } else {
	      save_file = argv[i];
	    }
          }
          break;
        case 'i':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-image"))) {
            usage(argv[0]);
          }
          do_image = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            pgm_file = argv[++i];
          }
          break;
	case 'n':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-nummon"))) {
            usage(argv[0]);
          }
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as the number of monsters to generate.    */
            uint16_t num_monsters = 0;
	    if(sscanf(argv[++i], "%hu", &num_monsters) != 1){ // use %hu because num_mon is an unsigned short
	      usage(argv[0]);
	    }
	    if(num_monsters >= MAX_MONSTERS){
	      d.num_monsters = MAX_MONSTERS-1;
	    }else if(num_monsters == 0){
	      usage(argv[0]);
	    }else{
	      d.num_monsters = num_monsters;
	    }
          }
          break;

        default:
          usage(argv[0]);
        }
      } else { /* No dash */
        usage(argv[0]);
      }
    }
  }

  if (do_seed) {
    /* Allows me to generate more than one dungeon *
     * per second, as opposed to time().           */
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }

  printf("Seed is %ld.\n", seed);
  srand(seed);

  init_dungeon(&d);

  if (do_load) {
    read_dungeon(&d, load_file);
  } else if (do_image) {
    read_pgm(&d, pgm_file);
  } else {
    gen_dungeon(&d);
  }

  if (!do_load) {
    /* Set a valid position for the PC */
    d.pc.pos[dim_x] = (d.rooms[0].position[dim_x] +
                            (rand() % d.rooms[0].size[dim_x]));
    d.pc.pos[dim_y] = (d.rooms[0].position[dim_y] +
                            (rand() % d.rooms[0].size[dim_y]));
  }

  printf("PC is at (y, x): %d, %d\n",
         d.pc.pos[dim_y], d.pc.pos[dim_x]);

  d.h = malloc(sizeof(heap_t));
  heap_init(d.h, event_cmp, NULL);
  gen_monsters(&d);

  /*uint32_t p,q;
  for(p=0;p<DUNGEON_Y;p++){
    for(q=0;q<DUNGEON_X;q++){
      if(d.character[p][q]){
        printf("%c starts at %d,%d\n",d.character[p][q]->disp,p,q);
      }
    }
  }*/

  render_dungeon(&d);
  while(d.pc.alive == 1 && d.alive_monsters > 1){
    turn_handler(&d);
  }

  if(d.pc.alive == 0){
    printf("PC has died\n");
  } else if(d.alive_monsters == 0){
    printf("PC has killed all other monsters\n");
  }
  //dijkstra(&d);
  //dijkstra_tunnel(&d);
  //render_distance_map(&d);
  //render_tunnel_distance_map(&d);
  //render_hardness_map(&d);
  //render_movement_cost_map(&d);

  if (do_save) {
    if (do_save_seed) {
       /* 10 bytes for number, plus dot, extention and null terminator. */
      save_file = malloc(18);
      sprintf(save_file, "%ld.rlg327", seed);
    }
    if (do_save_image) {
      if (!pgm_file) {
	fprintf(stderr, "No image file was loaded.  Using default.\n");
	do_save_image = 0;
      } else {
	/* Extension of 3 characters longer than image extension + null. */
	save_file = malloc(strlen(pgm_file) + 4);
	strcpy(save_file, pgm_file);
	strcpy(strchr(save_file, '.') + 1, "rlg327");
      }
    }
    write_dungeon(&d, save_file);

    if (do_save_seed || do_save_image) {
      free(save_file);
    }
  }

  delete_dungeon(&d);

  return 0;
}
