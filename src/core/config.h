#pragma once

#include <vector>
#include "types.h"

class ConfigEntry
{
public:
    std::string key {};
    std::string value {};
};

class ConfigData
{
public:
    void load(const std::string& config_path);

    static void strip_newline(std::string& line);
    static b32 parse_line(ConfigEntry& config_entry, std::string& line);

    std::vector<ConfigEntry> entry_vector {};
};
