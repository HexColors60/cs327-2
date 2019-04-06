#include "dungeon.h"
#include "item.h"
#include "dice.h"
#include "utils.h"
#include "descriptions.h"
#include vector

char item::symbol(){
  return object_symbol[type];
}

void gen_items(dungeon_t *d)
{
}

void destroy_items(dungeon_t *d)
{
}
