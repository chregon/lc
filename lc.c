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
#define VERSION "0.3-rc.2"

#include "lclib.h"
#include <assert.h> // for assert
#include <errno.h>  // for errno, strerror
#include <getopt.h> // to get command line arguments with getopts
#include <linux/limits.h>
#include <stdio.h>  // for printf, etc...
#include <stdlib.h> // for EXIT_FAILURE, EXIT_SUCCESS, etc...
#include <string.h> // for strcat, strcpy

// TODO if we want it, just an example of a --flag
static int verbose_flag;

// if set, instead of setting brighness, changes will be relative.
static int relative_flag;

// tiny help, or just usage string
static int tiny_help_flag;

// full help flag
static int help_flag;

// list devices
static int list_devices_flag;

void print_devices() {
  device **devices = calloc(sizeof(device *), MAX_DEVICES);
  int n = get_device_list(devices);
  if (n != 0) {
    for (int i = 0; i < n; i++) {
      printf("[%s]: \n\t%s\n\tCurrent lvl: %d\n\tMax lvl: %d\n",
             devices[i]->name, devices[i]->id,
             get_device_brightness(devices[i]),
             get_device_max_brightness(devices[i]));
    }
  } else
    printf("No devices found\n");

  free_device_list(devices, n);
  free(devices);
  exit(EXIT_SUCCESS);
}

/*
 * synopsis: prints some help to the user
 *
 * TODO consider if the placement of ls is appropriate
 * TODO documentation, waited because it is not finalized
 * */
void help() {
  fprintf(stderr, "Usage: lc [-dh] device brighness [-r]\n");
  exit(EXIT_SUCCESS);
}

void license_notice() {
  printf("lc " VERSION " -- Copyright (C) 2020 -- Christian Egon Sørensen\n");
  printf("This program comes with ABSOLUTELY NO WARRANTY; \n");
  printf("This is free software, and you are welcome to redistribute it\n");
  printf("under certain conditions; \n");
}

void full_help() {
  fprintf(stderr, "Usage: lc [-dh] DEVICE BRIGHTNESS [-r]\n");
  fprintf(stderr, "Sets brightness (as percentage) for device file in "
                  "/sys/class/backlight\n\n");
  system("cat HELP.txt"); // FIXME this is obviously stupid, should be fixed
  fprintf(stderr, "\n");

  license_notice();
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
  // GET ARGS
  // Taken from getopts info pages, getopts_long examples
  int c;

  while (1) {
    static struct option long_options[] = {
        /* These options set a flag. */
        {"verbose", no_argument, &verbose_flag, 1},
        {"brief", no_argument, &verbose_flag, 0},
        {"list-devices", no_argument, &list_devices_flag, 0},
        {"help", no_argument, &help_flag, 1},
        /* These options don’t set a flag.
           We distinguish them by their indices. */
        {"relative", no_argument, &relative_flag, 'r'},
        {"tiny help", no_argument, &tiny_help_flag, 'h'},
        {"list devices", no_argument, &list_devices_flag, 'd'},
        /* {"append", no_argument, 0, 'b'}, */
        /* {"delete", required_argument, 0, 'd'}, */
        /* {"create", required_argument, 0, 'c'}, */
        /* {"file", required_argument, 0, 'f'}, */
        //{0, 0, 0, 0}
    };

    /* getopt_long stores the option index here. */
    int option_index = 0;

    // The ":" follows if argument is required
    c = getopt_long(argc, argv, "hr", long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1)
      break;

    switch (c) {
    case 0:
      /* If this option set a flag, do nothing else now. */
      if (long_options[option_index].flag != 0)
        break;
      printf("option %s", long_options[option_index].name);
      if (optarg)
        printf(" with arg %s", optarg);
      printf("\n");
      break;

    case 'd':
      print_devices();
      break; // NOTE this won't be reached
    case 'h':
      help();
      break; // NOTE this won't be reached
    case 'r':
      break;
    case 'a':
      printf("option -a with value `%s'\n", optarg);
      break;

    case '?':
      /* getopt_long already printed an error message. */
      break;

    default:
      help();
    }
  }

  /* Instead of reporting ‘--verbose’
     and ‘--brief’ as they are encountered,
     we report the final status resulting from them. */
  if (verbose_flag)
    puts("verbose flag is set");
  if (help_flag)
    full_help();

  // Setting to sentinel values
  char *device = NULL;
  int brightness = -1;

  // Here i is the i'th argv for the non-option arguments i.e. mandatory args
  // j is used to reference the j'th iteration, so we can
  // ensure arguments are always at the same j. (the mandatory arguments are
  // only defined by position)
  for (int i = optind, j = 0; i < argc; i++, j++) {
    if (j == 0)
      device = argv[i];
    if (j == 1)
      brightness = atoi(argv[i]);
  }

  if (NULL == device || 100 < brightness || 1 > brightness)
    help();

  // TRY TO GET DEVICE
  struct device *dev = get_device_by_id(device);

  if (dev == NULL) {
    fprintf(stderr, "Device \"%s\" not found\nValid devices:", device);
    exit(EXIT_FAILURE);
  }

  // READ MAX BRIGHTNESS
  int m_b = get_device_max_brightness(dev);
  // CALCULATE TARGET BRIGHTNESS AS % OF MAX
  // NB: If brightness isn't an int, this turns off the screen
  //     Since this is the provided code, it must be as the author intended
  //     Git blame == chregon2001
  //     Yup jeg kalder dig ud b 😘
  int target = brightness * (m_b / 100);

  set_device_brightness(dev, target);
  free_device(dev);

  return EXIT_SUCCESS;
}
// vim: expandtab:ts=2:sw=2
