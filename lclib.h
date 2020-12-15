#ifndef lclib
#define lclib

#include <stdio.h>  // for printf, etc...
#include <stdlib.h> // for EXIT_FAILURE, EXIT_SUCCESS, etc...
#include <string.h> // for strcat, strcpy
#include <errno.h>  // for errno, strerror
#include <dirent.h> // For directory listing
#include <assert.h> // For assert
#include <linux/limits.h> // For limits
#ifndef NAME_MAX
#define NAME_MAX 255 // For some reason this fails 4 me,
                     // So i hardcode this
#endif

#define BUILTIN_PATH "/sys/class/backlight"
#define MAX_DEVICES 12 // Pretty arbitrary, but the struct is like 6 bytes
// GNU coreutils style, take inspiration from ls
#define OPEN_ERR "%s: cannot open %s: %s\n"
#define WRITE_ERR "%s: cannot write %s: %s\n"
#define ARG_ERR "%s: %s: %s\n"

// This will enable us to use both vcp panels and laptop panels
typedef enum display_type {
    BUILTIN,
    DDCDISPLAY
} display_type;
// Hello
typedef struct device {
    char* name; // Display name
    display_type d_type;
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
device* get_device_by_name(char* name);


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
 * synopsis:	Frees a single device struct
 *
 * Parameters:
 * dev:			The device struct			(device*)
 *
 * return:		Nothing. Avoid double-frees so malloc won't fail ya doink
 */
void free_device(device* dev);

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
 * dev:			The device			                    (device*)
 * brightness:	The brightness as a raw integer			(int)
 *
 * return:		The brightness as a raw integer(not percentage)
 */
int set_device_brightness(device* dev, int brightness);


/*
 * synopsis: generic file error function
 *
 * err:      the error "type"                   (char *)
 * argv0:    the command name                   (char *)
 * fstr:     the path of file being operated on (char *)
 * errstr:   the operation error                (char *)
 *
 * return:   EXIT_FALURE
 *
 * desc: Takes a predefined error string (containing formatting for three
 * strings), and inserts the arguments argv[0] (should be calling command),
 * fstr (path of file), and errstr (the error, likely from errstring(errno)).
 * Then prints this formatted string to stderr, and exits with EXIT_FAILURE.
 */
void ferr(char *err, char *argv0, char *fstr, char *errstr);


#endif
