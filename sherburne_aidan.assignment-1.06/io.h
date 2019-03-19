#ifndef IO_H
# define IO_H

# include "dims.h"
# include "character.h"
# include "dungeon.h"
typedef struct dungeon dungeon_t;

void io_init_terminal(void);
void io_reset_terminal(void);
void io_display(dungeon_t *d);
void io_display_nofog(dungeon_t *d, pair_t cursorpos);
void io_handle_input(dungeon_t *d);
void io_queue_message(const char *format, ...);

#endif
