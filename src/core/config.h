#pragma once

#include "core/types.h"

struct ConfigEntry
{
    const char* key;
    const char* value;
};

struct ConfigData
{
    u32 entry_count;
    u32 entry_capacity;

    ConfigEntry* config_entry_array;
};

ConfigData* config_load(const char* config_path);
void config_destroy(ConfigData* config);
