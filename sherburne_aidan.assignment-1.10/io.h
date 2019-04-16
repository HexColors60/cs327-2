#ifndef IO_H
#define IO_H

#include "object.h"

class dungeon;

void io_init_terminal(void);
void io_reset_terminal(void);
void io_display(dungeon *d);
void io_handle_input(dungeon *d);
void io_queue_message(const char *format, ...);
void io_display_inventory(dungeon *d, uint8_t index);
void io_display_equipment(dungeon *d, uint8_t index);
void io_look(dungeon *d);
std::string io_object_info(object *o);
void io_respawn_menu(dungeon *d);
uint32_t io_teleport_pc(dungeon *d);

#endif
