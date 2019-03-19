#ifndef PC_H
# define PC_H

# include <stdint.h>

# include "dims.h"
# include "character.h"
# include "dungeon.h"

#define fogpair(pair) (p->fog[pair[dim_y]][pair[dim_x]])
#define fogxy(x, y) (p->fog[y][x])
#define vispair(pair) (p->visible[pair[dim_y]][pair[dim_x]])
#define visxy(x, y) (p->visible[y][x])

typedef struct dungeon dungeon_t;

typedef struct pc pc_t;

class pc : public character{
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
void pc_init_fog(pc *p);
void pc_reset_fog(pc *p);
void pc_refresh_fog(character_t *c, dungeon_t *d);
void pc_remember(pc *p, pair_t pos, terrain_type_t t);
int8_t is_visible(pc *p, int32_t x, int32_t y);

#endif
