#ifndef MONSTERDEF_H
# define MONSTERDEF_H

# include <stdint.h>
#include <string>

# include "dims.h"

enum colors {BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE, NUM_COLORS};
enum abilities {SMART, TELE, TUNNEL, ERRATIC, PASS, PICKUP, UNIQ, BOSS, DESTROY, NUM_ABILITIES};

void parse_monsters(std::string filename);

class monsterdef{
  public:
    std::string name;
    char symb;
    uint32_t color[NUM_COLORS];
    std::string desc;
    uint32_t speed_base, speed_dice, speed_sides;
    uint32_t dam_base, dam_dice, dam_sides;
    uint32_t hp_base, hp_dice, hp_sides;
    uint32_t abil[NUM_ABILITIES];
    uint32_t rrty;
};

#endif
