#include <lclib.h>

int get_device_list(device** out){
    int n = 0;

    // Get list of BUILTIN displays
    DIR* sys_class_backlight = opendir(BUILTIN_PATH);
    struct dirent* dp;
    while (sys_class_backlight) {
        errno = 0;
        if ((dp = readdir(sys_class_backlight)) != NULL) {
            if (strcmp(dp->d_name, ".") != 0 && 
                strcmp(dp->d_name, "..") != 0) {
                // We found a valid backlight
                char* name = calloc(sizeof(char), NAME_MAX + 1);
                strcpy(name, "Built-in: ");
                strncat(name, dp->d_name, NAME_MAX - strlen(name));
                char* id = calloc(sizeof(char), NAME_MAX+1);
                strcpy(id, dp->d_name);
                device* new_device = malloc(sizeof(device));
                new_device->name = name;
                new_device->d_type = BUILTIN;
                new_device->id = id;
                out[n++] = new_device;
            }
        } else {
            if (errno != 0){
                fprintf(stderr, 
                        "Directory listing error: %s\n", 
                        strerror(errno)
                        );
            }
            break;
        }
    }
    free(sys_class_backlight);
    return n;
}

int free_device_list(device** out, int n){
    for(int i = 0; i < n; i++){
        // Free the strings
        free(out[i]->name);
        free(out[i]->id);
        free(out[i]);
    }
    return 0;
}

int get_device_brightness(device* device){

    return 0;
}
int get_device_max_brightness(device* device){

    return 0;
}
int set_device_brightness(device* device, int brightness){

    return 0;
}

