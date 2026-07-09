#pragma once

#include <vector>

struct ConfigEntry
{
    std::string key {};
    std::string value {};
};

struct ConfigData
{
    std::vector<ConfigEntry> entry_vector {};
};

ConfigData config_load(const std::string& config_path);
void config_destroy(ConfigData& config);
