#include <cstdlib>
#include <cstring>
#include <ncurses.h>

#include "dungeon.h"
#include "io.h"
#include "move.h"
#include "object.h"
#include "path.h"
#include "pc.h"
#include "utils.h"

pc::pc() {
  uint8_t i;
  hp = 5000;
  for (i = 0; i < bp_capacity; i++) {
    bp[i] = 0; // initialize empty backpack
  }
  for (i = 0; i < INVENTORY_SLOTS; i++) {
    inv[i] = 0; // initialize empty inventory
  }
}

pc::~pc() {
  uint8_t i;
  for (i = 0; i < bp_capacity; i++) {
    if (bp[i]) {
      delete bp[i];
      bp[i] = NULL;
    }
  }
  for (i = 0; i < INVENTORY_SLOTS; i++) {
    if (inv[i]) {
      delete inv[i];
      inv[i] = NULL;
    }
  }
}

void pc::update_speed() {
  int8_t i;
  int32_t new_speed = 0;
  for (i = 0; i < bp_capacity; i++) {
    if (bp[i]) {
      new_speed += bp[i]->get_speed();
    }
  }
  if (new_speed < 1) {
    new_speed = 1;
  }
  speed = new_speed;
}

int32_t pc::get_inv_slot() {
  int8_t i;
  for (i = 0; i < INVENTORY_SLOTS; i++) {
    if (!inv[i]) {
      return i;
    }
  }
  return -1;
}

uint8_t pc::grab_item(dungeon *d) {
  // object *o;
  int8_t i = get_inv_slot();
  if (i == -1 && objpair(position)) {
    io_queue_message("You have no room in your inventory.");
    return 1;
  } else {
    if (objpair(position)) {
      inv[i] = objpair(position);
      objpair(position) = NULL;
      io_queue_message("Grabbed: %s.", inv[i]->get_name());
    }
  }
  return 0;
}

uint8_t pc::drop_item(dungeon *d, uint8_t sel, uint8_t slot) {
  if (sel == 0) { // drop from backpack
    if (!bp[slot]) {
      mvprintw(15, 0, "You have nothing equipped there, stupid!");
      return 1;
    }
    if (objpair(position)) {
      mvprintw(15, 0, "There is already an item there!");
      return 1;
    } else {
      mvprintw(15, 0, "Dropped: %s.", bp[slot]->get_name());
      objpair(position) = bp[slot];
      bp[slot] = NULL;
    }
  } else if (sel == 1) { // drop from inventory
    if (!inv[slot]) {
      mvprintw(15, 0, "That inventory slot is empty, stupid!");
      return 1;
    }
    if (objpair(position)) {
      mvprintw(15, 0, "There is already an item there!");
      return 1;
    } else {
      mvprintw(15, 0, "Dropped: %s.", inv[slot]->get_name());
      objpair(position) = inv[slot];
      inv[slot] = NULL;
    }
  }
  refresh();
  return 0;
}

uint8_t pc::destroy_item(uint8_t sel, uint8_t slot) {
  if (sel == 0) { // destroy from backpack
    if (!bp[slot]) {
      mvprintw(15, 0, "You have nothing equipped there, stupid!");
      return 1;
    }
    mvprintw(15, 0, "Destroyed: %s.", bp[slot]->get_name());
    delete bp[slot];
    bp[slot] = NULL;
  } else if (sel == 1) { // destroy from inventory
    if (!inv[slot]) {
      mvprintw(15, 0, "That inventory slot is empty, stupid!");
      return 1;
    }
    mvprintw(15, 0, "Destroyed: %s.", inv[slot]->get_name());
    delete inv[slot];
    inv[slot] = NULL;
  }
  return 0;
}

uint8_t pc::equip_item(uint8_t slot) {
  object *o;
  uint8_t i;
  if (!inv[slot]) {
    mvprintw(15, 0, "That inventory slot is empty, stupid!");
    return 1;
  }
  i = inv[slot]->get_bp_slot();
  // handle two ring slots
  if (inv[slot]->get_type() == objtype_RING && bp[i]) {
    if (bp[i + 1]) {
      mvprintw(15, 0, "Required inventory slot is full.");
      return 1;
    } else
      i++;
  }
  // swap the item in the inventory with the equipped item
  o = inv[slot];
  inv[slot] = bp[i];
  bp[i] = o;
  mvprintw(15, 0, "Equipped: %s.", bp[i]->get_name());
  update_speed();
  return 0;
}

uint8_t pc::remove_item(uint8_t slot) {
  object *o;
  if (!bp[slot]) {
    mvprintw(15, 0, "You have nothing equipped there, stupid!");
    return 1;
  }
  int8_t i = get_inv_slot();
  if (i == -1) {
    mvprintw(15, 0, "You have no open inventory slots.");
    return 1;
  }
  // swap the item in the inventory with the equipped item
  o = bp[slot];
  bp[slot] = inv[i];
  inv[i] = o;
  mvprintw(15, 0, "Removed: %s.", inv[i]->get_name());
  update_speed();
  return 0;
}

uint32_t pc_is_alive(dungeon *d) { return d->PC->alive; }

void place_pc(dungeon *d) {
  d->PC->position[dim_y] =
      rand_range(d->rooms->position[dim_y],
                 (d->rooms->position[dim_y] + d->rooms->size[dim_y] - 1));
  d->PC->position[dim_x] =
      rand_range(d->rooms->position[dim_x],
                 (d->rooms->position[dim_x] + d->rooms->size[dim_x] - 1));

  pc_init_known_terrain(d->PC);
  pc_observe_terrain(d->PC, d);
}

void config_pc(dungeon *d) {
  static dice pc_dice(1, 1, 4);

  d->PC = new pc;

  d->PC->symbol = '@';

  place_pc(d);
  io_display(d);

  d->PC->speed = PC_SPEED;
  d->PC->alive = 1;
  d->PC->sequence_number = 0;
  d->PC->kills[kill_direct] = d->PC->kills[kill_avenged] = 0;
  d->PC->color.push_back(COLOR_WHITE);
  d->PC->damage = &pc_dice;
  d->PC->name = "Isabella Garcia-Shapiro";

  d->character_map[d->PC->position[dim_y]][d->PC->position[dim_x]] = d->PC;

  dijkstra(d);
  dijkstra_tunnel(d);
}

uint32_t pc_next_pos(dungeon *d, pair_t dir) {
  static uint32_t have_seen_corner = 0;
  static uint32_t count = 0;

  dir[dim_y] = dir[dim_x] = 0;

  if (in_corner(d, d->PC)) {
    if (!count) {
      count = 1;
    }
    have_seen_corner = 1;
  }

  /* First, eat anybody standing next to us. */
  if (charxy(d->PC->position[dim_x] - 1, d->PC->position[dim_y] - 1)) {
    dir[dim_y] = -1;
    dir[dim_x] = -1;
  } else if (charxy(d->PC->position[dim_x], d->PC->position[dim_y] - 1)) {
    dir[dim_y] = -1;
  } else if (charxy(d->PC->position[dim_x] + 1, d->PC->position[dim_y] - 1)) {
    dir[dim_y] = -1;
    dir[dim_x] = 1;
  } else if (charxy(d->PC->position[dim_x] - 1, d->PC->position[dim_y])) {
    dir[dim_x] = -1;
  } else if (charxy(d->PC->position[dim_x] + 1, d->PC->position[dim_y])) {
    dir[dim_x] = 1;
  } else if (charxy(d->PC->position[dim_x] - 1, d->PC->position[dim_y] + 1)) {
    dir[dim_y] = 1;
    dir[dim_x] = -1;
  } else if (charxy(d->PC->position[dim_x], d->PC->position[dim_y] + 1)) {
    dir[dim_y] = 1;
  } else if (charxy(d->PC->position[dim_x] + 1, d->PC->position[dim_y] + 1)) {
    dir[dim_y] = 1;
    dir[dim_x] = 1;
  } else if (!have_seen_corner || count < 250) {
    /* Head to a corner and let most of the NPCs kill each other off */
    if (count) {
      count++;
    }
    if (!against_wall(d, d->PC) && ((rand() & 0x111) == 0x111)) {
      dir[dim_x] = (rand() % 3) - 1;
      dir[dim_y] = (rand() % 3) - 1;
    } else {
      dir_nearest_wall(d, d->PC, dir);
    }
  } else {
    /* And after we've been there, let's head toward the center of the map. */
    if (!against_wall(d, d->PC) && ((rand() & 0x111) == 0x111)) {
      dir[dim_x] = (rand() % 3) - 1;
      dir[dim_y] = (rand() % 3) - 1;
    } else {
      dir[dim_x] = ((d->PC->position[dim_x] > DUNGEON_X / 2) ? -1 : 1);
      dir[dim_y] = ((d->PC->position[dim_y] > DUNGEON_Y / 2) ? -1 : 1);
    }
  }

  /* Don't move to an unoccupied location if that places us next to a monster */
  if (!charxy(d->PC->position[dim_x] + dir[dim_x],
              d->PC->position[dim_y] + dir[dim_y]) &&
      ((charxy(d->PC->position[dim_x] + dir[dim_x] - 1,
               d->PC->position[dim_y] + dir[dim_y] - 1) &&
        (charxy(d->PC->position[dim_x] + dir[dim_x] - 1,
                d->PC->position[dim_y] + dir[dim_y] - 1) != d->PC)) ||
       (charxy(d->PC->position[dim_x] + dir[dim_x] - 1,
               d->PC->position[dim_y] + dir[dim_y]) &&
        (charxy(d->PC->position[dim_x] + dir[dim_x] - 1,
                d->PC->position[dim_y] + dir[dim_y]) != d->PC)) ||
       (charxy(d->PC->position[dim_x] + dir[dim_x] - 1,
               d->PC->position[dim_y] + dir[dim_y] + 1) &&
        (charxy(d->PC->position[dim_x] + dir[dim_x] - 1,
                d->PC->position[dim_y] + dir[dim_y] + 1) != d->PC)) ||
       (charxy(d->PC->position[dim_x] + dir[dim_x],
               d->PC->position[dim_y] + dir[dim_y] - 1) &&
        (charxy(d->PC->position[dim_x] + dir[dim_x],
                d->PC->position[dim_y] + dir[dim_y] - 1) != d->PC)) ||
       (charxy(d->PC->position[dim_x] + dir[dim_x],
               d->PC->position[dim_y] + dir[dim_y] + 1) &&
        (charxy(d->PC->position[dim_x] + dir[dim_x],
                d->PC->position[dim_y] + dir[dim_y] + 1) != d->PC)) ||
       (charxy(d->PC->position[dim_x] + dir[dim_x] + 1,
               d->PC->position[dim_y] + dir[dim_y] - 1) &&
        (charxy(d->PC->position[dim_x] + dir[dim_x] + 1,
                d->PC->position[dim_y] + dir[dim_y] - 1) != d->PC)) ||
       (charxy(d->PC->position[dim_x] + dir[dim_x] + 1,
               d->PC->position[dim_y] + dir[dim_y]) &&
        (charxy(d->PC->position[dim_x] + dir[dim_x] + 1,
                d->PC->position[dim_y] + dir[dim_y]) != d->PC)) ||
       (charxy(d->PC->position[dim_x] + dir[dim_x] + 1,
               d->PC->position[dim_y] + dir[dim_y] + 1) &&
        (charxy(d->PC->position[dim_x] + dir[dim_x] + 1,
                d->PC->position[dim_y] + dir[dim_y] + 1) != d->PC)))) {
    dir[dim_x] = dir[dim_y] = 0;
  }

  return 0;
}

uint32_t pc_in_room(dungeon *d, uint32_t room) {
  if ((room < d->num_rooms) &&
      (d->PC->position[dim_x] >= d->rooms[room].position[dim_x]) &&
      (d->PC->position[dim_x] <
       (d->rooms[room].position[dim_x] + d->rooms[room].size[dim_x])) &&
      (d->PC->position[dim_y] >= d->rooms[room].position[dim_y]) &&
      (d->PC->position[dim_y] <
       (d->rooms[room].position[dim_y] + d->rooms[room].size[dim_y]))) {
    return 1;
  }

  return 0;
}

void pc_learn_terrain(pc *p, pair_t pos, terrain_type ter) {
  p->known_terrain[pos[dim_y]][pos[dim_x]] = ter;
  p->visible[pos[dim_y]][pos[dim_x]] = 1;
}

void pc_reset_visibility(pc *p) {
  uint32_t y, x;

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      p->visible[y][x] = 0;
    }
  }
}

terrain_type pc_learned_terrain(pc *p, int16_t y, int16_t x) {
  if (y < 0 || y >= DUNGEON_Y || x < 0 || x >= DUNGEON_X) {
    io_queue_message("Invalid value to %s: %d, %d", __FUNCTION__, y, x);
  }

  return p->known_terrain[y][x];
}

void pc_init_known_terrain(pc *p) {
  uint32_t y, x;

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      p->known_terrain[y][x] = ter_unknown;
      p->visible[y][x] = 0;
    }
  }
}

void pc_observe_terrain(pc *p, dungeon *d) {
  pair_t where;
  int16_t y_min, y_max, x_min, x_max;

  y_min = p->position[dim_y] - PC_VISUAL_RANGE;
  if (y_min < 0) {
    y_min = 0;
  }
  y_max = p->position[dim_y] + PC_VISUAL_RANGE;
  if (y_max > DUNGEON_Y - 1) {
    y_max = DUNGEON_Y - 1;
  }
  x_min = p->position[dim_x] - PC_VISUAL_RANGE;
  if (x_min < 0) {
    x_min = 0;
  }
  x_max = p->position[dim_x] + PC_VISUAL_RANGE;
  if (x_max > DUNGEON_X - 1) {
    x_max = DUNGEON_X - 1;
  }

  for (where[dim_y] = y_min; where[dim_y] <= y_max; where[dim_y]++) {
    where[dim_x] = x_min;
    can_see(d, p->position, where, 1, 1);
    where[dim_x] = x_max;
    can_see(d, p->position, where, 1, 1);
  }
  /* Take one off the x range because we alreay hit the corners above. */
  for (where[dim_x] = x_min - 1; where[dim_x] <= x_max - 1; where[dim_x]++) {
    where[dim_y] = y_min;
    can_see(d, p->position, where, 1, 1);
    where[dim_y] = y_max;
    can_see(d, p->position, where, 1, 1);
  }
}

int32_t is_illuminated(pc *p, int16_t y, int16_t x) { return p->visible[y][x]; }

void pc_see_object(character *the_pc, object *o) {
  if (o) {
    o->has_been_seen();
  }
}
