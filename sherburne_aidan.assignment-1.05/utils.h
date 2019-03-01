#ifndef UTILS_H
# define UTILS_H

# include <assert.h>
# include <stdlib.h>
#include "dungeon.h"

/* Returns true if random float in [0,1] is less than *
 * numerator/denominator.  Uses only integer math.    */
# define rand_under(numerator, denominator) \
  (rand() < ((RAND_MAX / denominator) * numerator))

/* Returns random integer in [min, max]. */
# define rand_range(min, max) ((rand() % (((max) + 1) - (min))) + (min))

#define malloc(size) ({          \
  void *_tmp;                    \
  assert((_tmp = malloc(size))); \
  _tmp;                          \
})

#define UNUSED(f) ((void) f)

void show_menu(dungeon_t *d, uint32_t start_index);
int makedirectory(char *dir);

#endif
