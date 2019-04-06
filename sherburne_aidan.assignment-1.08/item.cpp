#include "dungeon.h"
#include "item.h"
#include "dice.h"
#include "utils.h"
#include "descriptions.h"
#include <cstring>
#include <vector>

//char item::get_symbol(){
//  return object_symbol[type];
//}

item::item(object_description &o, pair_t pos) :
  odesc(o),
  name(o.get_name()),
  description(o.get_description()),
  type(o.get_type()),
  color(o.get_color()),
  hit(o.get_hit().roll()),
  damage(o.get_damage()),
  dodge(o.get_dodge().roll()),
  def(o.get_defence().roll()),
  weight(o.get_weight().roll()),
  speed(o.get_speed().roll()),
  attr(o.get_attribute().roll()),
  val(o.get_value().roll())
{
  position[dim_y] = pos[dim_y];
  position[dim_x] = pos[dim_x];
  seen = 0;
}

void gen_items(dungeon_t *d)
{
  uint32_t i, j;
  item *it;
  uint32_t room;
  pair_t p;
  std::vector<object_description> &o = d->object_descriptions;

  memset(d->item_map, 0, sizeof (d->item_map));

  for (i = 0; i < d->max_items; i++) {

    j = rand_range(0, o.size()-1);
    do {
      room = rand_range(1, d->num_rooms - 1);
      p[dim_y] = rand_range(d->rooms[room].position[dim_y],
                            (d->rooms[room].position[dim_y] +
                             d->rooms[room].size[dim_y] - 1));
      p[dim_x] = rand_range(d->rooms[room].position[dim_x],
                            (d->rooms[room].position[dim_x] +
                             d->rooms[room].size[dim_x] - 1));
    } while (d->item_map[p[dim_y]][p[dim_x]]);
    it = new item(o[j], p);
    itempair(p) = it;
  }

  d->num_items = d->max_items;
}

item::~item(){
}

void destroy_items(dungeon_t *d)
{
  uint32_t x, y;
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (itemxy(x,y)) {
        delete itemxy(x,y);
        itemxy(x,y) = 0;
      }
    }
  }
}
