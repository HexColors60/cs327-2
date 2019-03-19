#ifndef PC_H
# define PC_H

# include <stdint.h>

# include "dims.h"
# include "character.h"
# include "dungeon.h"

typedef struct dungeon dungeon_t;

typedef struct pc pc_t;

class pc : character{
  public:
    terrain_type_t fog[DUNGEON_Y][DUNGEON_X];
    uint8_t visible[DUNGEON_Y][DUNGEON_X];
};

void pc_delete(pc_t *pc);
uint32_t pc_is_alive(dungeon_t *d);
void config_pc(dungeon_t *d);
uint32_t pc_next_pos(dungeon_t *d, pair_t dir);
void place_pc(dungeon_t *d);
uint32_t pc_in_room(dungeon_t *d, uint32_t room);

#endif
