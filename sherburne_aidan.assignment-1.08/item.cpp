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
  uint32_t i;
  item *it;
  uint32_t room;
  pair_t p;

  std::vector<object_description> &o = d->object_descriptions;

  memset(d->item_map, 0, sizeof (d->item_map));

  for (i = 0; i < d->max_items; i++) {
    it = new item;
    memset(it, 0, sizeof (*it));
    do {
      room = rand_range(1, d->num_rooms - 1);
      p[dim_y] = rand_range(d->rooms[room].position[dim_y],
                            (d->rooms[room].position[dim_y] +
                             d->rooms[room].size[dim_y] - 1));
      p[dim_x] = rand_range(d->rooms[room].position[dim_x],
                            (d->rooms[room].position[dim_x] +
                             d->rooms[room].size[dim_x] - 1));
    } while (d->item_map[p[dim_y]][p[dim_x]]);
    it->position[dim_y] = p[dim_y];
    it->position[dim_x] = p[dim_x];
    it->odesc = o;
    it->name = o.get_name();
    it->description = o.get_description();
    it->type = o.get_type();
    it->color = o.get_color();
    it->hit = o.get_hit().roll();
    it->damage = o.get_damage();
    it->dodge = o.get_dodge().roll();
    it->def = o.get_defence().roll();
    it->weight = o.get_weight().roll();
    it->speed = o.get_speed().roll();
    it->attr = o.get_attribute().roll();
    it->val = o.get_value().roll();
  }

  d->num_objects = d->max_items;
}


void destroy_items(dungeon_t *d)
{
  uint32_t x, y;
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (d->item_map[y][x]) {
        delete d->item_map[y][x];
        d->item_map[y][x] = 0;
      }
    }
  }
}
