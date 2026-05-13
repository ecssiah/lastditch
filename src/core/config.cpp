#include "config.h"

#include <cassert>
#include <fstream>

#include "core/log.h"
#include "core/types.h"

static void strip_newline(std::string& line)
{
    while (!line.empty() && (line.back() == '\n' || line.back() == '\r'))
    {
        line.pop_back();
    }
}

static b32 parse_line(ConfigEntry& config_entry, std::string& line)
{
    strip_newline(line);

    if (line.empty())
    {
        return false;
    }

    const size_t equal_sign_index = line.find('=');
    
    if (equal_sign_index == std::string::npos)
    {
        LOG_WARN("Invalid config: %s", line.c_str());

        return false;
    }

    config_entry.key = line.substr(0, equal_sign_index);
    config_entry.value = line.substr(equal_sign_index + 1);

    return true;
}

ConfigData config_load(const std::string& config_path)
{
    ConfigData config_data = {};

    std::ifstream ifs(config_path);
    
    assert(ifs.is_open());

    std::string line;

    while (getline(ifs, line))
    {
        ConfigEntry config_entry = {};

        if (parse_line(config_entry, line))
        {
            config_data.entry_vector.push_back(config_entry);
        }
    }

    return config_data;
}

void config_destroy(ConfigData& config_data)
{

}
