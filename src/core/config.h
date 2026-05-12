#pragma once

#include "core/types.h"

struct ConfigEntry
{
    const char* key;
    const char* value;
};

struct JUSTSKY_Config
{
    u32 entry_count;
    u32 entry_capacity;

    ConfigEntry* config_entry_array;
};

JUSTSKY_Config* justsky_load_config(const char* config_path);
void justsky_destroy_config(JUSTSKY_Config* config);
