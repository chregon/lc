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

#include "lclib.h"
#include <assert.h> // for assert
#include <errno.h>  // for errno, strerror
#include <getopt.h> // to get command line arguments with getopts
#include <linux/limits.h>
#include <stdio.h>  // for printf, etc...
#include <stdlib.h> // for EXIT_FAILURE, EXIT_SUCCESS, etc...
#include <string.h> // for strcat, strcpy

static int verbose_flag;

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
}

/*
 * synopsis: prints some help to the user
 *
 * TODO consider if the placement of ls is appropriate
 * TODO documentation, waited because it is not finalized
 * */
void help() {
  fprintf(stderr, "Usage: lc <device> <brighness>\n");
  fprintf(stderr,
          "Where brightness is a integer percentage of max(e.g. 1-100)\n");
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
  int c;

  while (1) {
    static struct option long_options[] = {
        /* These options set a flag. */
        {"verbose", no_argument, &verbose_flag, 1},
        {"brief", no_argument, &verbose_flag, 0},
        /* These options don’t set a flag.
           We distinguish them by their indices. */
        {"relative", no_argument, 0, 'r'},
        /* {"append", no_argument, 0, 'b'}, */
        /* {"delete", required_argument, 0, 'd'}, */
        /* {"create", required_argument, 0, 'c'}, */
        /* {"file", required_argument, 0, 'f'}, */
        {0, 0, 0, 0}};

    /* getopt_long stores the option index here. */
    int option_index = 0;

    c = getopt_long(argc, argv, "abc:d:f:", long_options, &option_index);

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

      /* case 'a': */
      /*   puts("option -a\n"); */
      /*   break; */

      /* case 'b': */
      /*   puts("option -b\n"); */
      /*   break; */

      /* case 'c': */
      /*   printf("option -c with value `%s'\n", optarg); */
      /*   break; */

      /* case 'd': */
      /*   printf("option -d with value `%s'\n", optarg); */
      /*   break; */

      /* case 'f': */
      /*   printf("option -f with value `%s'\n", optarg); */
      /*   break; */

    case 'b':
      puts("option -b\n");
      break;

    case '?':
      /* getopt_long already printed an error message. */
      break;

    default:
      abort();
    }
  }

  /* Instead of reporting ‘--verbose’
     and ‘--brief’ as they are encountered,
     we report the final status resulting from them. */
  if (verbose_flag)
    puts("verbose flag is set");

  /* Print any remaining command line arguments (not options). */
  // if (optind < argc) {
  // printf("non-option ARGV-elements: ");
  // while (optind < argc)
  // printf("%s ", argv[optind++]);
  // putchar('\n');
  //}

  // Setting to sentinel values
  char *device = NULL;
  int brightness = -1;

  for (int index = optind; index < argc; index++) {
    if (index == 1)
      device = argv[1];
    if (index == 2)
      brightness = atoi(argv[2]);
  }

  if (device == NULL || brightness > 100 || brightness < 1)
    exit(1);

  /* // GETOPTS LOOP */
  /* int aflag = 0; */
  /* int bflag = 0; */
  /* char *cvalue = NULL; */
  /* int index; */
  /* int c; */

  /* opterr = 0; */
  /* // This is shamelessly stolen from some internet site */
  /* opterr = 0; */

  /* while ((c = getopt(argc, argv, "abc:")) != -1) */
  /*   switch (c) { */
  /*   case 'a': */
  /*     aflag = 1; */
  /*     break; */
  /*   case 'b': */
  /*     bflag = 1; */
  /*     break; */
  /*   case 'c': */
  /*     cvalue = optarg; */
  /*     break; */
  /*   case '?': */
  /*     if (optopt == 'c') */
  /*       fprintf(stderr, "Option -%c requires an argument.\n", optopt); */
  /*     else if (isprint(optopt)) */
  /*       fprintf(stderr, "Unknown option `-%c'.\n", optopt); */
  /*     else */
  /*       fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt); */
  /*     return 1; */
  /*   default: */
  /*     abort(); */
  /*   } */

  /* printf("aflag = %d, bflag = %d, cvalue = %s\n", aflag, bflag, cvalue); */

  /* for (index = optind; index < argc; index++) { */
  /*   if (index == 1) */
  /*     device = argv[1]; */
  /*   if (index == 2) */
  /*     brightness = atoi(argv[2]); */
  /*   /\* printf("Non-option argument %s\n", argv[index]); *\/ */
  /* } */
  /* return 0; */

  // CHECK INPUT
  //
  /* if (argc == 2) */
  /*   if ((!(strcmp(argv[1], "--help"))) || (!(strcmp(argv[1], "help"))) || */
  /*       (!(strcmp(argv[1], "-h")))) */
  /*     help(); */
  /* if (argc != 3) */
  /*   argerr(ARG_ERR, argv[0], "not enough arguments", "try running help"); */
  /* else if (atoi(argv[2]) == 0 || atoi(argv[2]) > 100) */
  /*   argerr(ARG_ERR, argv[0], "invalid brightness [1-100]", argv[2]); */

  // TRY TO GET DEVICE
  struct device *dev = get_device_by_id(device);
  if (dev == NULL) {
    fprintf(stderr, "Device \"%s\" not found\nValid devices:", device);
    print_devices();
    return 1;
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
