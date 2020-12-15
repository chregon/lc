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

/*
 * synopsis:    Gets a list of available devices
 *
 * Parameters:
 *
 * out:			The device list output			(device**)
 *
 * return:      n devices found
 *
 * desc:        Loops through available device types, and adds to the list,
 *              given as parameter. Return the amount of found devices as n
 *
 * note:        This allocates heap memory. Please call =free_device_list=
 *              afterwards
 */
int get_device_list(device** out);
/*
 * synopsis:	Frees the device list
 *
 * Parameters:
 * out:			The list to free			        (device**)
 * n:			How many devices in list			(int)
 *
 * return:		0 on success, -1 on error(if free errors out)
 *
 * desc:		Loop through the available devices and free their respecitve
 *              structs
 */     
int free_device_list(device** out, int n);

/*
 * synopsis:	Gets the current brightness of a device
 *
 * Parameters:
 * dev:			The device			(device*)
 *
 * return:		The brightness as a raw integer(not percentage)
 *
 */
int get_device_brightness(device* dev);
/*
 * synopsis:	Gets the max brightness of a device
 *
 * Parameters:
 * dev:			The device			(device*)
 *
 * return:		The brightness as a raw integer(not percentage)
 *
 */
int get_device_max_brightness(device* dev);

/*
 * synopsis:	Sets the current brightness of a device
 *
 * Parameters:
 * dev:			The device			(device*)
 *
 * return:		The brightness as a raw integer(not percentage)
 */
int set_device_brightness(device* dev, int brightness);

#endif
