#ifndef ITEM_H
# define ITEM_H

# include <string>

# include "descriptions.h"
# include "dims.h"

class item {
 private:
  object_description &odesc;
  std::string &name;
  std::string &description;
  object_type_t type;
  uint32_t color;
  uint32_t hit;
  dice &damage;
  uint32_t dodge;
  uint32_t def;
  uint32_t weight;
  uint32_t speed;
  uint32_t attr;
  uint32_t val;
  uint32_t art;
  uint32_t rrty;
  pair_t position;
 public:
  char symbol();
};

void gen_items(dungeon_t *d);
void destroy_items(dungeon_t *d);

#endif
