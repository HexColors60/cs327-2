#ifndef DESCRIPTIONS_H
#define DESCRIPTIONS_H

#include <stdint.h>
#undef swap
#include "dice.h"
#include <string>
#include <vector>
//# include "npc.h"
#define NPC_UNIQ 0x00000080

typedef struct dungeon dungeon_t;

uint32_t parse_descriptions(dungeon_t *d);
uint32_t print_descriptions(dungeon_t *d);
uint32_t destroy_descriptions(dungeon_t *d);

typedef enum object_type {
  objtype_no_type,
  objtype_WEAPON,
  objtype_OFFHAND,
  objtype_RANGED,
  objtype_LIGHT,
  objtype_ARMOR,
  objtype_HELMET,
  objtype_CLOAK,
  objtype_GLOVES,
  objtype_BOOTS,
  objtype_AMULET,
  objtype_RING,
  objtype_SCROLL,
  objtype_BOOK,
  objtype_FLASK,
  objtype_GOLD,
  objtype_AMMUNITION,
  objtype_FOOD,
  objtype_WAND,
  objtype_CONTAINER
} object_type_t;

extern const char object_symbol[];

class monster_description {
private:
  std::string name, description;
  char symbol;
  std::vector<uint32_t> color;
  uint32_t abilities;
  dice speed, hitpoints, damage;
  uint32_t rarity;
  uint32_t spawned;

public:
  monster_description()
      : name(), description(), symbol(0), color(0), abilities(0), speed(),
        hitpoints(), damage(), rarity(0), spawned(0){}
  void set(const std::string &name, const std::string &description,
           const char symbol, const std::vector<uint32_t> &color,
           const dice &speed, const uint32_t abilities, const dice &hitpoints,
           const dice &damage, const uint32_t rarity);
  std::ostream &print(std::ostream &o);
  char get_symbol() { return symbol; }
  std::vector<uint32_t> get_color() { return color; }
  std::string get_description() { return description; }
  std::string get_name() { return name; }
  dice get_hit() { return hitpoints; }
  dice get_speed() { return speed; }
  bool can_spawn() {
    if ((((abilities & NPC_UNIQ) && spawned == 0) || !(abilities & NPC_UNIQ)) &&
        rarity > (uint8_t)(rand() % 100))
      return true;
    else
      return false;
  }
  void spawn() { spawned++; }
};

class object_description {
private:
  std::string name, description;
  object_type_t type;
  uint32_t color;
  dice hit, damage, dodge, defence, weight, speed, attribute, value;
  bool artifact;
  uint32_t rarity;
  uint32_t spawned;

public:
  object_description()
      : name(), description(), type(objtype_no_type), color(0), hit(), damage(),
        dodge(), defence(), weight(), speed(), attribute(), value(),
        artifact(false), rarity(0), spawned(0) {}
  void set(const std::string &name, const std::string &description,
           const object_type_t type, const uint32_t color, const dice &hit,
           const dice &damage, const dice &dodge, const dice &defence,
           const dice &weight, const dice &speed, const dice &attrubute,
           const dice &value, const bool artifact, const uint32_t rarity);
  std::ostream &print(std::ostream &o);
  /* Need all these accessors because otherwise there is a *
   * circular dependancy that is difficult to get around.  */
  inline const std::string &get_name() const { return name; }
  inline const std::string &get_description() const { return description; }
  inline const object_type_t get_type() const { return type; }
  inline const uint32_t get_color() const { return color; }
  inline const dice &get_hit() const { return hit; }
  inline const dice &get_damage() const { return damage; }
  inline const dice &get_dodge() const { return dodge; }
  inline const dice &get_defence() const { return defence; }
  inline const dice &get_weight() const { return weight; }
  inline const dice &get_speed() const { return speed; }
  inline const dice &get_attribute() const { return attribute; }
  inline const dice &get_value() const { return value; }
  bool can_spawn() {
    if ((((artifact) && spawned == 0) || !artifact) &&
        rarity > (uint8_t)(rand() % 100))
      return true;
    else
      return false;
  }
  void spawn() { spawned++; }
};

std::ostream &operator<<(std::ostream &o, monster_description &m);
std::ostream &operator<<(std::ostream &o, object_description &od);

#endif
