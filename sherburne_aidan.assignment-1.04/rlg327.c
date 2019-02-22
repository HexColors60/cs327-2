#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "dungeon.h"
#include "path.h"

void usage(char *name)
{
  fprintf(stderr,
          "Usage: %s [-r|--rand <seed>] [-l|--load [<file>]]\n"
          "          [-s|--save [<file>]] [-i|--image <pgm file>]\n"
	  "[-n|--nummon <num monsters>]\n",
          name);

  exit(-1);
}

void move_mon(dungeon_t *d, monster_t *m){
  pair_t new_pos;
  uint32_t i;
  int x, y;
  int min_val = INT_MAX;
  switch(m->attributes){
  case 0: //plain old boring monster
    //TODO move line of sight
    break;
  
  case 1: //erratic
    //50% chance to move randomly
    if(rand() % 2 == 1){//move erratically
      new_pos[dim_y] = (m->position[dim_y] - 1 + (rand() % 3));
      new_pos[dim_x] = (m->position[dim_x] - 1 + (rand() % 3));
      //find open position
      while(d->hardness[new_pos[dim_y]][new_pos[dim_x]] != 0 ||
	    (new_pos[dim_y] == m->position[dim_y] && new_pos[dim_x] == m->position[dim_x])){
	new_pos[dim_y] = (m->position[dim_y] - 1 + (rand() % 3));
	new_pos[dim_x] = (m->position[dim_x] - 1 + (rand() % 3));
      }
    }/*else{//move line of sight
      new_pos[dim_y] = 0;
      new_pos[dim_x] = 0;
      }*/
    break;
  
  case 2: //tunneling
    //TODO move line of sight tunneling
    break;
  
  case 3: //erratic & tunneling
    if(rand() % 2 == 1){//move erratically
      new_pos[dim_y] = (m->position[dim_y] - 1 + (rand() % 3));
      new_pos[dim_x] = (m->position[dim_x] - 1 + (rand() % 3));
      //find open position
      while(d->map[new_pos[dim_y]][new_pos[dim_x]] != ter_wall_immutable ||
	    (new_pos[dim_y] == m->position[dim_y] && new_pos[dim_x] == m->position[dim_x])){
	new_pos[dim_y] = (m->position[dim_y] - 1 + (rand() % 3));
	new_pos[dim_x] = (m->position[dim_x] - 1 + (rand() % 3));
      }
      //if we need to create a tunnel
      if(d->hardness[new_pos[dim_y]][new_pos[dim_x]] > 85){
	d->hardness[new_pos[dim_y]][new_pos[dim_x]] -= 85;
	new_pos[dim_y] = m->position[dim_y];
	new_pos[dim_x] = m->position[dim_x];
      }
      else if(d->hardness[new_pos[dim_y]][new_pos[dim_x]]  <= 85){
	d->hardness[new_pos[dim_y]][new_pos[dim_x]] = 0;
	d->map[new_pos[dim_y]][new_pos[dim_x]] = ter_floor_hall;
      }
    }/*else{//TODO move line of sight
      new_pos[dim_y] = 0;
      new_pos[dim_x] = 0;
      }*/
    break;
  
  case 4: //telepathic
    //TODO move telepathically
    break;
  
  case 5: //erratic & telepathic
    /*if(rand() % 2 == 1){//move erratically
      new_pos[dim_y] = (m->position[dim_y] - 1 + (rand() % 3));
      new_pos[dim_x] = (m->position[dim_x] - 1 + (rand() % 3));
      //find open position
      while(d->hardness[new_pos[dim_y]][new_pos[dim_x]] != 0 ||
	    (new_pos[dim_y] == m->position[dim_y] && new_pos[dim_x] == m->position[dim_x])){
	new_pos[dim_y] = (m->position[dim_y] - 1 + (rand() % 3));
	new_pos[dim_x] = (m->position[dim_x] - 1 + (rand() % 3));
      }
    }else{//move telepathically
      
    }*/
    break;
  
  case 6: //tunneling & telepathic
    //TODO move telepathically
    break;
  
  case 7: //erratic & tunneling & telepathic
    break;
  
  case 8: //intelligent
    
    break;
  
  case 9: //erratic & intelligent
    break;
  
  case 10://tunneling & intelligent
    break;
  
  case 11://erratic & tunneling & intelligent
    break;
  
  case 12://telepathic & intelligent
    dijkstra(d);
    for(y = -1; y <= 1; y++){
      for(x = -1; x <= 1; x++){
	if(d->pc_distance[m->position[dim_y]+y][m->position[dim_x]+x] < min_val){
	  min_val = d->pc_distance[m->position[dim_y]+y][m->position[dim_x]+x];
	  new_pos[dim_y] = m->position[dim_y]+y;
	  new_pos[dim_x] = m->position[dim_x]+x;
	}
      }
    }
    break;
  
  case 13://erratic & elepathic & intelligent
    break;
  
  case 14://tunneling & telepathic & intelligent
    dijkstra_tunnel(d);
    for(y = -1; y <= 1; y++){
      for(x = -1; x <= 1; x++){
	if(d->pc_tunnel[m->position[dim_y]+y][m->position[dim_x]+x] < min_val){
	  min_val = d->pc_tunnel[m->position[dim_y]+y][m->position[dim_x]+x];
	  new_pos[dim_y] = m->position[dim_y]+y;
	  new_pos[dim_x] = m->position[dim_x]+x;
	}
      }
    }
    //if we need to create a tunnel
    if(d->hardness[new_pos[dim_y]][new_pos[dim_x]] > 85){
      d->hardness[new_pos[dim_y]][new_pos[dim_x]] -= 85;
      new_pos[dim_y] = m->position[dim_y];
      new_pos[dim_x] = m->position[dim_x];
    }
    else if(d->hardness[new_pos[dim_y]][new_pos[dim_x]]  <= 85){
      d->hardness[new_pos[dim_y]][new_pos[dim_x]] = 0;
      d->map[new_pos[dim_y]][new_pos[dim_x]] = ter_floor_hall;
    }
    break;
  
  case 15://erratic & tunneling & telepathic & intelligent
    if(rand() % 2 == 1){//move erratically
      new_pos[dim_y] = (m->position[dim_y] - 1 + (rand() % 3));
      new_pos[dim_x] = (m->position[dim_x] - 1 + (rand() % 3));
      //find open position
      while(d->hardness[new_pos[dim_y]][new_pos[dim_x]] != 0 ||
	    (new_pos[dim_y] == m->position[dim_y] && new_pos[dim_x] == m->position[dim_x])){
	new_pos[dim_y] = (m->position[dim_y] - 1 + (rand() % 3));
	new_pos[dim_x] = (m->position[dim_x] - 1 + (rand() % 3));
      }
    }else{//move tunneling & telepathic & intelligent
      dijkstra_tunnel(d);
      for(y = -1; y <= 1; y++){
	for(x = -1; x <= 1; x++){
	  if(d->pc_tunnel[m->position[dim_y]+y][m->position[dim_x]+x] < min_val){
	    min_val = d->pc_tunnel[m->position[dim_y]+y][m->position[dim_x]+x];
	    new_pos[dim_y] = m->position[dim_y]+y;
	    new_pos[dim_x] = m->position[dim_x]+x;
	  }
	}
      }
      //if we need to create a tunnel
      if(d->hardness[new_pos[dim_y]][new_pos[dim_x]] > 85){
	d->hardness[new_pos[dim_y]][new_pos[dim_x]] -= 85;
	new_pos[dim_y] = m->position[dim_y];
	new_pos[dim_x] = m->position[dim_x];
      }
      else if(d->hardness[new_pos[dim_y]][new_pos[dim_x]]  <= 85){
	d->hardness[new_pos[dim_y]][new_pos[dim_x]] = 0;
	d->map[new_pos[dim_y]][new_pos[dim_x]] = ter_floor_hall;
      }
    }
    break;
    
  default:
    break;
  }
  //if there is a monster in the new pos, kill it.
  for(i = 0; i <= d->num_mon; i++){
    if((new_pos[dim_y] == d->mons[i].position[dim_y] && new_pos[dim_x] == d->mons[i].position[dim_x])){
      if(i==0)d->pc_alive = 0;
      d->mons[i].alive = 0;
      d->mons_alive--;
    }
  }
  m->position[dim_y] = new_pos[dim_y];
  m->position[dim_x] = new_pos[dim_x];
}

void gen_monsters(dungeon_t *d){
  uint16_t i, x, y;
  const static char disps[] = "0123456789abcdef";
  srand(time(NULL));

  d->mons_alive = d->num_mon;
  
  //set up our pc 'monster'
  monster_t *pc = malloc(sizeof(monster_t));
  pc->position[dim_y] = d->pc.position[dim_y];
  pc->position[dim_x] = d->pc.position[dim_x];
  pc->speed = 10;
  pc->disp = '@';
  pc->alive = 1;
  pc->attributes = 1; // our pc will only be erratic for now
  d->mons[0] = *pc;
  d->pc_alive = 1;
  for(i = 1; i <= d->num_mon; i++){
    monster_t *mon = malloc(sizeof(monster_t));//Malloc our new monster
    y = rand() % 20;
    x = rand() % 80;
    while((x == d->pc.position[dim_x] && y == d->pc.position[dim_y]) || d->hardness[y][x] != 0){
      y = rand() % 20;
      x = rand() % 80;
    }
    mon->position[dim_y] = y;
    mon->position[dim_x] = x;
    mon->speed = rand() % 16 + 5;
    mon->alive = 1;
    uint8_t at = 0;
    at += (rand() % 2) * 1; // erratic binary bit
    at += (rand() % 2) * 2; // tunneling binary bit
    at += (rand() % 2) * 4; // telepathy binary bit
    at += (rand() % 2) * 8; // intelligent binary bit
    mon->attributes = at;
    mon->disp = disps[at];
    d->mons[i] = *mon;
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
  d.max_mon = MAX_MONSTERS;
  d.num_mon = 10;

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
            uint16_t num_mon = 0;
	    if(sscanf(argv[++i], "%hu", &num_mon) != 1){ // use %hu because num_mon is an unsigned short
	      usage(argv[0]);
	    }
	    if(num_mon >= MAX_MONSTERS){
	      d.num_mon = MAX_MONSTERS-1;
	    }else{
	      d.num_mon = num_mon;
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
    d.pc.position[dim_x] = (d.rooms[0].position[dim_x] +
                            (rand() % d.rooms[0].size[dim_x]));
    d.pc.position[dim_y] = (d.rooms[0].position[dim_y] +
                            (rand() % d.rooms[0].size[dim_y]));
  }

  printf("PC is at (y, x): %d, %d\n",
         d.pc.position[dim_y], d.pc.position[dim_x]);
  gen_monsters(&d);
  render_dungeon(&d);
  uint32_t turn = 0;
  uint32_t j;
  while(d.pc_alive == 1 && d.mons_alive > 1){
    for(j = 0; j <= d.num_mon; j++){
      if(turn % 1000 == floor(1000/d.mons[j].speed)){
	move_mon(&d, &d.mons[j]);
	render_dungeon(&d);
      }
    }
    turn += 10;
  }

  if(d.pc_alive == 0){
    printf("PC has died\n");
  } else if(d.mons_alive == 0){
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
