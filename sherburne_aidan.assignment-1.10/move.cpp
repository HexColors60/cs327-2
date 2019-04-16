#include "move.h"

#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

#include "character.h"
#include "dungeon.h"
#include "event.h"
#include "heap.h"
#include "io.h"
#include "move.h"
#include "npc.h"
#include "object.h"
#include "path.h"
#include "pc.h"
#include "utils.h"

static int bosses_killed = 0;

bool boss_alive() { return (bosses_killed == 0); }

void do_combat(dungeon *d, character *atk, character *def) {
  int can_see_atk, can_see_def;
  const char *organs[] = {
      "liver",               /*  0 */
      "pancreas",            /*  1 */
      "heart",               /*  2 */
      "eye",                 /*  3 */
      "arm",                 /*  4 */
      "leg",                 /*  5 */
      "intestines",          /*  6 */
      "gall bladder",        /*  7 */
      "lungs",               /*  8 */
      "hand",                /*  9 */
      "foot",                /* 10 */
      "spinal cord",         /* 11 */
      "pituitary gland",     /* 12 */
      "thyroid",             /* 13 */
      "tongue",              /* 14 */
      "bladder",             /* 15 */
      "diaphram",            /* 16 */
      "stomach",             /* 17 */
      "pharynx",             /* 18 */
      "esophagus",           /* 19 */
      "trachea",             /* 20 */
      "urethra",             /* 21 */
      "spleen",              /* 22 */
      "ganglia",             /* 23 */
      "ear",                 /* 24 */
      "subcutaneous tissue", /* 25 */
      "cerebellum",          /* 26 */
      "hippocampus",         /* 27 */
      "frontal lobe",        /* 28 */
      "brain",               /* 29 */
  };
  uint32_t damage, i;

  if (def->alive) {

    can_see_atk =
        can_see(d, character_get_pos(d->PC), character_get_pos(atk), 1, 0);
    can_see_def =
        can_see(d, character_get_pos(d->PC), character_get_pos(def), 1, 0);

    if (def != d->PC && atk != d->PC) { // monster vs monster
      if (can_see_atk && can_see_def) {
        io_queue_message("%s%s bumped into %s%s!", is_unique(atk) ? "" : "the ",
                         atk->name, is_unique(def) ? "" : "the ", def->name);
      }
      return;
    }

    damage = atk->damage->roll(); // get base damage from monster/PC

    if (atk == d->PC) {
      for (i = 0; i < bp_capacity; i++) {
        if (d->PC->bp[i]) {
          damage += d->PC->bp[i]->roll_dice();
        }
      }
      io_queue_message("You attack %s%s for %d damage",
                       is_unique(def) ? "" : "the ", def->name, damage);
    } else {
      io_queue_message("%s%s attacks your %s for %d damage",
                       is_unique(atk) ? "" : "the ", atk->name,
                       organs[rand() % (sizeof(organs) / sizeof(organs[0]))],
                       damage);
    }

    if (damage >= def->hp) { // if we deal more damage than the defender has

      if (atk == d->PC) {
        uint32_t reward;
        io_queue_message("You have slain the beast!");
        if (is_boss(def)) {
          bosses_killed++;
          reward = 250;
        } else {
          reward = rand() % 50;
        }
        d->PC->gold += reward;
        io_queue_message("You receive %d gold for your bravery.", reward);
      } else {
        io_queue_message("You have been slain!");
      }
      io_queue_message("");
      def->alive = 0;
      def->hp = 0;
      atk->kills[kill_direct]++;
      atk->kills[kill_avenged] +=
          (def->kills[kill_direct] + def->kills[kill_avenged]);
      charpair(def->position) = NULL;
    } else {
      def->hp -= damage;
    }
  }
}

void move_character(dungeon *d, character *c, pair_t next) {
  if (charpair(next) && ((next[dim_y] != c->position[dim_y]) ||
                         (next[dim_x] != c->position[dim_x]))) {
    if ((charpair(next) == d->PC) || c == d->PC)
      do_combat(d, c, charpair(next));
    else { // both are monsters
      int i, r;
      pair_t direction;
      pair_t circle[9] = {{-1, -1}, {0, -1}, {1, -1}, {-1, 0},
                          {1, 0},   {-1, 1}, {0, 1},  {1, 1}};
      int stuck = 1;

      for (i = 0, r = rand(); i < 8 && stuck; i++, r++) {
        direction[dim_y] = next[dim_y] + circle[r % 8][dim_y];
        direction[dim_x] = next[dim_x] + circle[r % 8][dim_x];
        if ((!charpair(direction) && (mappair(direction) >= ter_floor)) ||
            (charpair(direction) == c)) {
          stuck = 0;
        }
      }
      if (stuck) {
        return;
      } // swap
      charpair(c->position) = NULL;
      charpair(direction) = charpair(next);
      charpair(next) = c;
      charpair(direction)->position[dim_y] = direction[dim_y];
      charpair(direction)->position[dim_x] = direction[dim_x];
      c->position[dim_y] = next[dim_y];
      c->position[dim_x] = next[dim_x];
    }
  } else {
    /* No character in new position. */

    d->character_map[c->position[dim_y]][c->position[dim_x]] = NULL;
    c->position[dim_y] = next[dim_y];
    c->position[dim_x] = next[dim_x];
    d->character_map[c->position[dim_y]][c->position[dim_x]] = c;
  }

  if (c == d->PC) {
    pc_reset_visibility(d->PC);
    pc_observe_terrain(d->PC, d);
  }
}

void do_moves(dungeon *d) {
  pair_t next;
  character *c;
  event *e;

  /* Remove the PC when it is PC turn.  Replace on next call.  This allows *
   * use to completely uninit the heap when generating a new level without *
   * worrying about deleting the PC.                                       */

  if (pc_is_alive(d)) {
    /* The PC always goes first one a tie, so we don't use new_event().  *
     * We generate one manually so that we can set the PC sequence       *
     * number to zero.                                                   */
    e = (event *)malloc(sizeof(*e));
    e->type = event_character_turn;
    /* Hack: New dungeons are marked.  Unmark and ensure PC goes at d->time, *
     * otherwise, monsters get a turn before the PC. */
    if (d->is_new) {
      d->is_new = 0;
      e->time = d->time;
    } else {
      e->time = d->time + (1000 / d->PC->speed);
    }
    e->sequence = 0;
    e->c = d->PC;
    heap_insert(&d->events, e);
  }

  while (pc_is_alive(d) && (e = (event *)heap_remove_min(&d->events)) &&
         ((e->type != event_character_turn) || (e->c != d->PC))) {
    d->time = e->time;
    if (e->type == event_character_turn) {
      c = e->c;
    }
    if (!c->alive) {
      if (d->character_map[c->position[dim_y]][c->position[dim_x]] == c) {
        d->character_map[c->position[dim_y]][c->position[dim_x]] = NULL;
      }
      if (c != d->PC) {
        event_delete(e);
      }
      continue;
    }

    npc_next_pos(d, (npc *)c, next);
    move_character(d, (npc *)c, next);

    heap_insert(&d->events, update_event(d, e, 1000 / c->speed));
  }

  io_display(d);
  if (pc_is_alive(d) && e->c == d->PC) {
    c = e->c;
    d->time = e->time;
    /* Kind of kludgey, but because the PC is never in the queue when   *
     * we are outside of this function, the PC event has to get deleted *
     * and recreated every time we leave and re-enter this function.    */
    e->c = NULL;
    event_delete(e);
    io_handle_input(d);
  }
}

void dir_nearest_wall(dungeon *d, character *c, pair_t dir) {
  dir[dim_x] = dir[dim_y] = 0;

  if (c->position[dim_x] != 1 && c->position[dim_x] != DUNGEON_X - 2) {
    dir[dim_x] = (c->position[dim_x] > DUNGEON_X - c->position[dim_x] ? 1 : -1);
  }
  if (c->position[dim_y] != 1 && c->position[dim_y] != DUNGEON_Y - 2) {
    dir[dim_y] = (c->position[dim_y] > DUNGEON_Y - c->position[dim_y] ? 1 : -1);
  }
}

uint32_t against_wall(dungeon *d, character *c) {
  return ((mapxy(c->position[dim_x] - 1, c->position[dim_y]) ==
           ter_wall_immutable) ||
          (mapxy(c->position[dim_x] + 1, c->position[dim_y]) ==
           ter_wall_immutable) ||
          (mapxy(c->position[dim_x], c->position[dim_y] - 1) ==
           ter_wall_immutable) ||
          (mapxy(c->position[dim_x], c->position[dim_y] + 1) ==
           ter_wall_immutable));
}

uint32_t in_corner(dungeon *d, character *c) {
  uint32_t num_immutable;

  num_immutable = 0;

  num_immutable +=
      (mapxy(c->position[dim_x] - 1, c->position[dim_y]) == ter_wall_immutable);
  num_immutable +=
      (mapxy(c->position[dim_x] + 1, c->position[dim_y]) == ter_wall_immutable);
  num_immutable +=
      (mapxy(c->position[dim_x], c->position[dim_y] - 1) == ter_wall_immutable);
  num_immutable +=
      (mapxy(c->position[dim_x], c->position[dim_y] + 1) == ter_wall_immutable);

  return num_immutable > 1;
}

static void new_dungeon_level(dungeon *d, uint32_t dir) {
  /* Eventually up and down will be independantly meaningful. *
   * For now, simply generate a new dungeon.                  */

  switch (dir) {
  case '<':
  case '>':
    new_dungeon(d);
    break;
  default:
    break;
  }
}

uint32_t move_pc(dungeon *d, uint32_t dir) {
  pair_t next;
  uint32_t was_stairs = 0;
  const char *wallmsg[] = {"There's a wall in the way.",
                           "BUMP!",
                           "Ouch!",
                           "You stub your toe.",
                           "You can't go that way.",
                           "You admire the engravings.",
                           "Are you drunk?"};

  next[dim_y] = d->PC->position[dim_y];
  next[dim_x] = d->PC->position[dim_x];

  switch (dir) {
  case 1:
  case 2:
  case 3:
    next[dim_y]++;
    break;
  case 4:
  case 5:
  case 6:
    break;
  case 7:
  case 8:
  case 9:
    next[dim_y]--;
    break;
  }
  switch (dir) {
  case 1:
  case 4:
  case 7:
    next[dim_x]--;
    break;
  case 2:
  case 5:
  case 8:
    break;
  case 3:
  case 6:
  case 9:
    next[dim_x]++;
    break;
  case '<':
    if (mappair(d->PC->position) == ter_stairs_up) {
      was_stairs = 1;
      new_dungeon_level(d, '<');
    }
    break;
  case '>':
    if (mappair(d->PC->position) == ter_stairs_down) {
      was_stairs = 1;
      new_dungeon_level(d, '>');
    }
    break;
  }

  if (was_stairs) {
    return 0;
  }

  if ((dir != '>') && (dir != '<') && (mappair(next) >= ter_floor)) {
    move_character(d, d->PC, next);
    dijkstra(d);
    dijkstra_tunnel(d);
    d->PC->grab_item(d);

    return 0;
  } else if (mappair(next) < ter_floor) {
    io_queue_message(wallmsg[rand() % (sizeof(wallmsg) / sizeof(wallmsg[0]))]);
    io_display(d);
  }

  return 1;
}
