/* lc - light command: a brightness setter
 * Copyright (C) 2020 - 2020  Christian Egon Sørensen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define MAXPATHLENGTH 128


#include <assert.h> // for assert
#include <errno.h>  // for errno, strerror
#include <stdio.h>  // for printf, etc...
#include <stdlib.h> // for EXIT_FAILURE, EXIT_SUCCESS, etc...
#include <string.h> // for strcat, strcpy
#include <lclib.h>
#include <linux/limits.h>

void print_devices(){
    device** devices = calloc(sizeof(device*), MAX_DEVICES);
    int n = get_device_list(devices);
    if (n != 0){
        for(int i = 0; i < n; i++){
            printf("[%s]: %s, current lvl: %d\n", 
                   devices[i]->name,
                   devices[i]->id,
                   get_device_brightness(devices[i])
                  );
        }
    } else
        printf("No devices found\n");

    free_device_list(devices, n);
    free(devices);
}

/*
 * synopsis: prints some help to the user
 *
 * TODO consider if the placement of ls is appropriate
 * TODO documentation, waited because it is not finalized
 * */
void help() {
  fprintf(stderr, "Usage: lc <device> <brighness>\n");
  fprintf(stderr, "Where brightness is a integer percentage of max(e.g. 1-100)\n");
  fprintf(stderr, "#DEVICES#\n");
  print_devices();
  exit(EXIT_SUCCESS);
}


/*
 * synopsis:   generic arguments error function
 *
 * err:        the error "type"    (char *),
 * argv0:      the command name    (char *)
 * errmsg:     the error           (char *)
 * more:       further information (char *)
 *
 * return:     EXIT_FALURE
 *
 * desc: Takes a predefined error string (containing formatting for three
 * strings), and inserts the arguments argv[0] (should be calling command),
 * errmsg (the error as human readable string), and more (containing a small
 * tip about the error).Then prints this formatted string to stderr, and exits
 * with EXIT_FAILURE.
 */
void argerr(char *err, char *argv0, char *errmsg, char *more) {
  fprintf(stderr, ARG_ERR, argv0, errmsg, more);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  // CHECK INPUT
  if (argc == 2)
    if ((!(strcmp(argv[1], "--help"))) || (!(strcmp(argv[1], "help"))) ||
        (!(strcmp(argv[1], "-h"))))
      help();
  if (argc != 3)
    argerr(ARG_ERR, argv[0], "not enoguh arguments", "try running help");
  else if (atoi(argv[2]) == 0 || atoi(argv[2]) > 100)
    argerr(ARG_ERR, argv[0], "invalid brightness [1-100]", argv[2]);

  // INITIALIZE PATH VARS
  char *pre = "/sys/class/backlight/";
  char basepath[PATH_MAX] = {0};
  char maxpath[PATH_MAX] = {0};
  char brightpath[PATH_MAX] = {0};
  strcpy(basepath, pre); // This is safe. PRE is of known length

  // VALIDATE USER INPUT LENGTH
  strncat(basepath, argv[1], PATH_MAX - strlen(pre) - 1);
  strncat(maxpath, basepath, PATH_MAX - 1);
  strncat(brightpath, basepath, PATH_MAX - 1);

  // SET PATHS FOR BACKLIGHT FILE I/O
  strcat(maxpath,
         "/max_brightness"); // Not really safe, but not exploitable either.
  strcat(brightpath, "/brightness"); // Will just crash if length exceeds

  // OPEN FILES FOR I/O
  FILE *m_fp = fopen(maxpath, "r");
  if (m_fp == NULL)
    ferr(OPEN_ERR, argv[0], maxpath, strerror(errno));

  FILE *b_fp = fopen(brightpath, "w");
  if (b_fp == NULL)
    ferr(OPEN_ERR, argv[0], brightpath, strerror(errno));

  // READ MAX BRIGHTNESS
  int m_b;
  assert(0 < fscanf(m_fp, "%d", &m_b));

  // CALCULATE TARGET BRIGHTNESS AS % OF MAX
  if (!fprintf(b_fp, "%d", atoi(argv[2]) * (m_b / 100)))
    ferr(WRITE_ERR, argv[0], brightpath, strerror(errno));

  // CLOSE FILES
  assert(0 == fclose(m_fp)); // These shouldn't really fail
  assert(0 == fclose(b_fp)); // but error handling could be improved

  return EXIT_SUCCESS;
}
