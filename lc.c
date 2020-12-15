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
            printf("[%s]: \n\t%s\n\tCurrent lvl: %d\n\tMax lvl: %d\n", 
                   devices[i]->name,
                   devices[i]->id,
                   get_device_brightness(devices[i]),
                   get_device_max_brightness(devices[i])
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
    argerr(ARG_ERR, argv[0], "not enough arguments", "try running help");
  else if (atoi(argv[2]) == 0 || atoi(argv[2]) > 100)
    argerr(ARG_ERR, argv[0], "invalid brightness [1-100]", argv[2]);

  // TRY TO GET DEVICE
  device* dev = get_device_by_id(argv[1]);
  if (dev == NULL){
    fprintf(stderr, "Device \"%s\" not found\nValid devices:", argv[1]);
    print_devices();
    return 1;
  }

  // READ MAX BRIGHTNESS
  int m_b = get_device_max_brightness(dev);
  // CALCULATE TARGET BRIGHTNESS AS % OF MAX
  // NB: If argv[2] isn't an int, this turns off the screen
  //     Since this is the provided code, it must be as the author intended
  //     Git blame == chregon2001
  //     Yup jeg kalder dig ud b 😘
  int target = atoi(argv[2]) * (m_b / 100);

  set_device_brightness(dev, target);
  free_device(dev);

  return EXIT_SUCCESS;
}
// vim: expandtab:ts=2:sw=2
