#ifndef ITEM_H
# define ITEM_H

# include "descriptions.h"
# include "dims.h"
# include <string>

class item {
 private:
  object_description &odesc;
  const std::string &name;
  const std::string &description;
  object_type_t type;
  uint32_t color;
  uint32_t hit;
  const dice &damage;
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
  item(object_description &o, pair_t pos);
  ~item();
};

void gen_items(dungeon_t *d);
void destroy_items(dungeon_t *d);

#endif
