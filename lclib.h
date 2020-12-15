#ifndef lclib
#define lclib

#include <stdio.h>  // for printf, etc...
#include <stdlib.h> // for EXIT_FAILURE, EXIT_SUCCESS, etc...
#include <string.h> // for strcat, strcpy
#include <errno.h>  // for errno, strerror

// This will enable us to use both vcp panels and laptop panels
typedef enum display_type {
    LAPTOP,
    DDCDISPLAY
} display_type;
// Hello
typedef struct device {
    char* name; // Display name
    display_type isLaptop;
    char* id; // Display ID, used for writing
} device;

int get_device_list(device** out);
int free_device_list(device** out);

int get_device_brightness(device* device);
int get_device_max_brightness(device* device);
int set_device_brightness(device* device, int brightness);

#endif
