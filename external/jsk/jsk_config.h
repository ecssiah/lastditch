#ifndef JSK_CONFIG_H
#define JSK_CONFIG_H

#include <stdlib.h>

#include "jsk.h"

typedef struct JSK_ConfigEntry JSK_ConfigEntry;
struct JSK_ConfigEntry
{
    const char *key;
    const char *value;
};

typedef struct JSK_Config JSK_Config;
struct JSK_Config
{
    i32 entry_count;
    i32 entry_capacity;
    
    JSK_ConfigEntry *config_entry_array;
};

JSK_Config *jsk_load_config(const char *config_path);
void jsk_destroy_config(JSK_Config *config);

#endif
