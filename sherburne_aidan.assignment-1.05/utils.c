#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <ncurses.h>

#include "utils.h"
#include "dungeon.h"

void show_menu(dungeon_t *d, uint32_t start_index){
  clear();
  char new_line = '\n';
  pair_t p;
  uint32_t num = 0;
  uint32_t count = 0;
  addch(new_line);
  for (p[dim_y] = 0; p[dim_y] < DUNGEON_Y; p[dim_y]++) {
    for (p[dim_x] = 0; p[dim_x] < DUNGEON_X; p[dim_x]++) {
      if (charpair(p) && count < 21) {
        int8_t xdif = p[dim_x] - d->pc.position[dim_x];
        int8_t ydif = p[dim_y] - d->pc.position[dim_y];
        char str[40];
        if(xdif < 0 && ydif < 0 && num >= start_index && charpair(p)->alive){
          sprintf(str, "%c, %d west and %d north\n", charpair(p)->symbol, (-1 * xdif), (-1 * ydif));
          addstr(str);
          count++;
        }else if(xdif == 0 && ydif < 0 && num >= start_index && charpair(p)->alive){
          sprintf(str, "%c, %d north\n", charpair(p)->symbol, (-1 * ydif));
          addstr(str);
          count++;
        }else if(xdif < 0 && ydif == 0 && num >= start_index && charpair(p)->alive){
          sprintf(str, "%c, %d west\n", charpair(p)->symbol, (-1 * xdif));
          addstr(str);
          count++;
        }else if(xdif > 0 && ydif > 0 && num >= start_index && charpair(p)->alive){
          sprintf(str, "%c, %d east and %d south\n", charpair(p)->symbol, xdif, ydif);
          addstr(str);
          count++;
        }else if(xdif == 0 && ydif > 0 && num >= start_index && charpair(p)->alive){
          sprintf(str, "%c, %d south\n", charpair(p)->symbol, ydif);
          addstr(str);
          count++;
        }else if(xdif > 0 && ydif == 0 && num >= start_index && charpair(p)->alive){
          sprintf(str, "%c, %d east\n", charpair(p)->symbol, xdif);
          addstr(str);
          count++;
        }
        num++;
      }
    }
  }
  addch(new_line);
  addch(new_line);
  refresh();
}

int makedirectory(char *dir)
{
  char *slash;

  for (slash = dir + strlen(dir); slash > dir && *slash != '/'; slash--)
    ;

  if (slash == dir) {
    return 0;
  }

  if (mkdir(dir, 0700)) {
    if (errno != ENOENT && errno != EEXIST) {
      fprintf(stderr, "mkdir(%s): %s\n", dir, strerror(errno));
      return 1;
    }
    if (*slash != '/') {
      return 1;
    }
    *slash = '\0';
    if (makedirectory(dir)) {
      *slash = '/';
      return 1;
    }

    *slash = '/';
    if (mkdir(dir, 0700) && errno != EEXIST) {
      fprintf(stderr, "mkdir(%s): %s\n", dir, strerror(errno));
      return 1;
    }
  }

  return 0;
}
