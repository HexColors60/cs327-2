#include <stdlib.h>
#include <ncurses.h>

#include "string.h"

#include "dungeon.h"
#include "pc.h"
#include "npc.h"
#include "utils.h"
#include "move.h"
#include "path.h"

void pc_delete(pc_t *pc)
{
  if (pc) {
    free(pc);
  }
}

uint32_t pc_is_alive(dungeon_t *d)
{
  return d->pc.alive;
}

void place_pc(dungeon_t *d)
{
  d->pc.position[dim_y] = rand_range(d->rooms->position[dim_y],
                                     (d->rooms->position[dim_y] +
                                      d->rooms->size[dim_y] - 1));
  d->pc.position[dim_x] = rand_range(d->rooms->position[dim_x],
                                     (d->rooms->position[dim_x] +
                                      d->rooms->size[dim_x] - 1));
}

void config_pc(dungeon_t *d)
{
  memset(&d->pc, 0, sizeof (d->pc));
  d->pc.symbol = '@';

  place_pc(d);

  d->pc.speed = PC_SPEED;
  d->pc.alive = 1;
  d->pc.sequence_number = 0;
  d->pc.pc = calloc(1, sizeof (*d->pc.pc));
  d->pc.npc = NULL;
  d->pc.kills[kill_direct] = d->pc.kills[kill_avenged] = 0;

  d->character[d->pc.position[dim_y]][d->pc.position[dim_x]] = &d->pc;

  dijkstra(d);
  dijkstra_tunnel(d);
}

uint32_t pc_next_pos(dungeon_t *d, pair_t dir)
{
  static int target_room = -1;
  static int target_is_valid = 0;

  if (target_is_valid &&
      (d->pc.position[dim_x] == d->rooms[target_room].position[dim_x]) &&
      (d->pc.position[dim_y] == d->rooms[target_room].position[dim_y])) {
    target_is_valid = 0;
  }

  dir[dim_y] = dir[dim_x] = 0;
  pair_t next_pos;

  switch(getch()){
    //move up-left
    case '7':
    case 'y':
      dir[dim_y] = -1;
      dir[dim_x] = -1;
      break;
    //move up
    case '8':
    case 'k':
      dir[dim_y] = -1;
      dir[dim_x] = 0;
      break;
    //move up-right
    case '9':
    case 'u':
      dir[dim_y] = -1;
      dir[dim_x] = 1;
      break;
    //move right
    case '6':
    case 'l':
      dir[dim_y] = 0;
      dir[dim_x] = 1;
      break;
    //move down-right
    case '3':
    case 'n':
      dir[dim_y] = 1;
      dir[dim_x] = 1;
      break;
    //move down
    case '2':
    case 'j':
      dir[dim_y] = 1;
      dir[dim_x] = 0;
      break;
    //move down-left
    case '1':
    case 'b':
      dir[dim_y] = 1;
      dir[dim_x] = -1;
      break;
    //move left
    case '4':
    case 'h':
      dir[dim_y] = 0;
      dir[dim_x] = -1;
      break;
    //Rest
    case '5':
    case ' ':
    case '.':
      dir[dim_y] = 0;
      dir[dim_x] = 0;
      break;
    //go down stairs
    case '>':
      if(mappair(d->pc.position) == ter_stairs_down){
        pc_delete(d->pc.pc);
        delete_dungeon(d);
        init_dungeon(d);
	      gen_dungeon(d);
	      config_pc(d);
	      gen_monsters(d);
        clear();
        render_dungeon(d);
	      refresh();
      }
      break;
    //go up stairs
    case '<':
      if(mappair(d->pc.position) == ter_stairs_up){
        pc_delete(d->pc.pc);
        delete_dungeon(d);
        init_dungeon(d);
	      gen_dungeon(d);
	      config_pc(d);
	      gen_monsters(d);
        clear();
        render_dungeon(d);
	      refresh();
      }
      break;
    case 'q':
      pc_delete(d->pc.pc);
      delete_dungeon(d);
      endwin();
      exit(0);
      break;

    default:
      break;
  }
  next_pos[dim_y] = d->pc.position[dim_y] + dir[dim_y];
  next_pos[dim_x] = d->pc.position[dim_x] + dir[dim_x];
  if(hardnesspair(next_pos) != 0){
    dir[dim_y] = 0;
    dir[dim_x] = 0;
  }

  return 0;
}

uint32_t pc_in_room(dungeon_t *d, uint32_t room)
{
  if ((room < d->num_rooms)                                     &&
      (d->pc.position[dim_x] >= d->rooms[room].position[dim_x]) &&
      (d->pc.position[dim_x] < (d->rooms[room].position[dim_x] +
                                d->rooms[room].size[dim_x]))    &&
      (d->pc.position[dim_y] >= d->rooms[room].position[dim_y]) &&
      (d->pc.position[dim_y] < (d->rooms[room].position[dim_y] +
                                d->rooms[room].size[dim_y]))) {
    return 1;
  }

  return 0;
}
