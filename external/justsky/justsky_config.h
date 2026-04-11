#ifndef JUSTSKY_CONFIG_H
#define JUSTSKY_CONFIG_H

#include <stdlib.h>

#include "justsky.h"

typedef struct JUSTSKY_ConfigEntry JUSTSKY_ConfigEntry;
struct JUSTSKY_ConfigEntry
{
    const char *key;
    const char *value;
};

typedef struct JUSTSKY_Config JUSTSKY_Config;
struct JUSTSKY_Config
{
    u32 entry_count;
    u32 entry_capacity;
    
    JUSTSKY_ConfigEntry *config_entry_array;
};

JUSTSKY_Config *justsky_load_config(const char *config_path);
void justsky_destroy_config(JUSTSKY_Config *config);

#endif
