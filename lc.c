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
#include <math.h>   // for common log, to convert to order of magnitude
#include <stdio.h>  // for printf, etc...
#include <stdlib.h> // for EXIT_FAILURE, EXIT_SUCCESS, etc...
#include <string.h> // for strcat, strcpy

// TODO if we want it, just an example of a --flag
static int verbose_flag;

// if set, instead of setting brighness, changes will be relative.
static int relative_flag;

/* // tiny help, or just usage string */
/* static int tiny_help_flag; */

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

// "self-documenting" (i don't bother right now this took ages) FIXME
int getsize(char *s) {
  char *t;
  for (t = s; *t != '\0'; t++) {
  }
  return t - s;
}
// "self-documenting" (i don't bother right now this took ages) FIXME
// Basically, it's a hack to get proper negative numbers with
// optparse_long... some guy on a perl forum flamed about how
// this was very bad because standards etc... but if POSIX can't even negative
// integers, then i cannot POSIX
int steal_minus(char *optarg, int prefix) { // HACK
  // TODO refactor
  int s = getsize(optarg);
  double res = prefix;
  for (int i = 0; i < getsize(optarg); i++) {
    char t = optarg[i];
    double d = atoi(&t);
    double di = i;
    // shifts numbers to position - (di+1)
    res += d * (1 / pow(10, di + 1));
  }
  // Aligns to origin
  res = res * pow(10, s);
  return floor(res);
}

// For -0..-9 args
int neg_num_arg_prefix = -1000;

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
        {"relative", no_argument, 0, 'r'},
        {"tiny help", no_argument, 0, 'h'},
        {"list devices", no_argument, &list_devices_flag, 'd'},
        // this should be easy, he said
        {"nums", required_argument, 0, '1'},
        {"nums", required_argument, 0, '2'},
        {"nums", required_argument, 0, '3'},
        {"nums", required_argument, 0, '4'},
        {"nums", required_argument, 0, '5'},
        {"nums", required_argument, 0, '6'},
        {"nums", required_argument, 0, '7'},
        {"nums", required_argument, 0, '8'},
        {"nums", required_argument, 0, '9'},
        {"nums", required_argument, 0, '0'},
        /* {"append", no_argument, 0, 'b'}, */
        /* {"delete", required_argument, 0, 'd'}, */
        /* {"create", required_argument, 0, 'c'}, */
        /* {"file", required_argument, 0, 'f'}, */
        //{0, 0, 0, 0}
    };

    /* getopt_long stores the option index here. */
    int option_index = 0;

    // The ":" follows if argument is required
    c = getopt_long(argc, argv, "1:2:3:4:5:6:7:8:9:0:hrd", long_options,
                    &option_index);

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

    case '1':
      neg_num_arg_prefix = steal_minus(optarg, 1);
      /* int s = getsize(optarg); */
      /* for (int i = 0; i < getsize(optarg); i++) { */
      /*   char t = optarg[i]; */
      /*   double d = atoi(&t); */
      /*   double di = i; */
      /*   printf("%f pow: %f\n", pow(10, di), 1 / pow(10, di)); */
      /*   neg_num_arg_prefix += d * (1 / pow(10, di + 1)); */
      /* } */
      /* printf("%f\n", neg_num_arg_prefix * pow(10, s)); */
      break; // NOTE this won't be reached
    case '2':
      neg_num_arg_prefix = steal_minus(optarg, 2);
      break;
    case '3':
      neg_num_arg_prefix = steal_minus(optarg, 3);
      break;
    case '4':
      neg_num_arg_prefix = steal_minus(optarg, 4);
      break;
    case '5':
      neg_num_arg_prefix = steal_minus(optarg, 5);
      break;
    case '6':
      neg_num_arg_prefix = steal_minus(optarg, 6);
      break;
    case '7':
      neg_num_arg_prefix = steal_minus(optarg, 7);
      break;
    case '8':
      neg_num_arg_prefix = steal_minus(optarg, 8);
      break;
    case '9':
      neg_num_arg_prefix = steal_minus(optarg, 9);
      break;
    case '0':
      neg_num_arg_prefix = steal_minus(optarg, 0);
      // we'll be complete but why would one use 0?
      break;
    case 'd':
      print_devices();
      break; // NOTE this won't be reached
    case 'h':
      help();
      break; // NOTE this won't be reached
    case 'r':
      relative_flag = 1;
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
  double brightness = -1;

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

  if (NULL == device) {
    printf("hello");
    help();
  }
  // -1000 is a sentinel value
  if (-1000 == neg_num_arg_prefix || !relative_flag) {
    if (100 < brightness || 0 > brightness) {
      help();
    }
  } else if (100 >= neg_num_arg_prefix || (-100 <= neg_num_arg_prefix)) {
    brightness = -neg_num_arg_prefix;
  } else {
    printf("hello");
    help();
  }

  // TRY TO GET DEVICE
  struct device *dev = get_device_by_id(device);

  if (dev == NULL) {
    fprintf(stderr, "Device \"%s\" not found\nValid devices:", device);
    exit(EXIT_FAILURE);
  }

  // READ MAX BRIGHTNESS
  double m_b = get_device_max_brightness(dev);
  double c_b = get_device_brightness(dev);

  double target;
  // NOTE this was very quick, i hope it works... Didn't feel like getting up
  // and making a sanity check on paper
  if (relative_flag) {
    target = c_b + (m_b / 100) * brightness;
    printf("%.f + %.f = %.f\n", c_b, brightness, target);
  } else
    target = brightness * (m_b / 100);

  printf("%f target\n", target);
  if (1 > target) {
    target = 1;
  } else if (m_b < target) {
    target = m_b;
  }
  printf("%f target\n", target);

  set_device_brightness(dev, target);
  free_device(dev);

  return EXIT_SUCCESS;
}
// vim: expandtab:ts=2;:sw=2
