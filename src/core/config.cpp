#include "config.h"

#include <cassert>
#include <fstream>
#include "core/log.h"
#include "core/types.h"

using namespace std;

void
ConfigData::strip_newline(string& line)
{
    while (!line.empty() && (line.back() == '\n' || line.back() == '\r'))
    {
        line.pop_back();
    }
}

b32
ConfigData::parse_line(ConfigEntry& config_entry, string& line)
{
    strip_newline(line);

    if (line.empty())
    {
        return false;
    }

    const size_t equal_sign_index { line.find('=') };
    
    if (equal_sign_index == string::npos)
    {
        LOG_WARN("Invalid config: %s", line.c_str());

        return false;
    }

    config_entry.key = line.substr(0, equal_sign_index);
    config_entry.value = line.substr(equal_sign_index + 1);

    return true;
}

void
ConfigData::load(const string& config_path)
{
    ifstream ifs { config_path };
    
    assert(ifs.is_open());

    string line {};

    while (getline(ifs, line))
    {
        ConfigEntry config_entry {};

        if (parse_line(config_entry, line))
        {
            entry_vector.push_back(config_entry);
        }
    }
}
