#ifndef PC_H
#define PC_H

#include <stdint.h>

#include "character.h"
#include "dims.h"
#include "dungeon.h"

typedef enum backpack {
  bp_weapon,
  bp_offhand,
  bp_ranged,
  bp_armor,
  bp_helmet,
  bp_cloak,
  bp_gloves,
  bp_boots,
  bp_amulet,
  bp_light,
  bp_lring,
  bp_rring,
  bp_capacity
} backpack_t;

class pc : public character {
private:
  int32_t get_inv_slot();
  void update_speed();

public:
  pc();
  ~pc();
  terrain_type known_terrain[DUNGEON_Y][DUNGEON_X];
  uint8_t visible[DUNGEON_Y][DUNGEON_X];
  object *inv[INVENTORY_SLOTS];
  object *bp[bp_capacity];
  uint32_t gold;
  uint32_t experience;
  uint32_t respawn_cost;
  uint32_t level;
  uint8_t grab_item(dungeon *d);
  uint8_t drop_item(dungeon *d, uint8_t sel, uint8_t slot);
  uint8_t destroy_item(uint8_t sel, uint8_t slot);
  uint8_t equip_item(uint8_t slot);
  uint8_t remove_item(uint8_t slot);
  uint8_t sell_item(uint8_t sel, uint8_t slot);
  void respawn(dungeon *d);
  uint8_t level_up();
};

void pc_delete(pc *pc);
uint32_t pc_is_alive(dungeon *d);
void config_pc(dungeon *d);
uint32_t pc_next_pos(dungeon *d, pair_t dir);
void place_pc(dungeon *d);
uint32_t pc_in_room(dungeon *d, uint32_t room);
void pc_learn_terrain(pc *p, pair_t pos, terrain_type ter);
terrain_type pc_learned_terrain(pc *p, int16_t y, int16_t x);
void pc_init_known_terrain(pc *p);
void pc_observe_terrain(pc *p, dungeon *d);
int32_t is_illuminated(pc *p, int16_t y, int16_t x);
void pc_reset_visibility(pc *p);
void pc_offer_respawn(dungeon *d);

#endif
