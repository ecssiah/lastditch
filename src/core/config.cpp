#include "config.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>

#include "core/log.h"

static void strip_newline(char* str)
{
    char* p = str;

    while (*p)
    {
        if (*p == '\n' || *p == '\r')
        {
            *p = '\0';

            return;
        }

        p++;
    }
}

static b32 parse_line(char* line, ConfigEntry* out_config_entry)
{
    strip_newline(line);

    if (line[0] == '\0')
    {
        return false;
    }

    char* equal_sign_present = strchr(line, '=');

    if (!equal_sign_present)
    {
        LOG_WARN("Invalid config: %s", line);

        return false;
    }

    *equal_sign_present = '\0';

    const char* key = line;
    const char* value = equal_sign_present + 1;

    out_config_entry->key = strdup(key);
    out_config_entry->value = strdup(value);

    return true;
}

static void extend_capacity(ConfigData* config)
{
    size_t new_capacity = config->entry_capacity * 2;

    auto new_config_entry_array = static_cast<ConfigEntry*>(realloc(
        config->config_entry_array,
        new_capacity * sizeof(ConfigEntry)
    ));

    if (!new_config_entry_array)
    {
        // handle allocation failure
        return;
    }

    config->config_entry_array = new_config_entry_array;
    config->entry_capacity = static_cast<u32>(new_capacity);
}

ConfigData* config_load(const char* config_path)
{
    ConfigData* config_data = static_cast<ConfigData*>(malloc(sizeof(ConfigData)));
    config_data->entry_count = 0;
    config_data->entry_capacity = 16;

    config_data->config_entry_array = static_cast<ConfigEntry*>(malloc(
        sizeof(ConfigEntry) * config_data->entry_capacity));

    FILE* file = fopen(config_path, "r");

    if (!file)
    {
        LOG_ERROR("Config not found: %s", config_path);

        return config_data;
    }

    char line[512];

    while (fgets(line, sizeof(line), file))
    {
        ConfigEntry* config_entry = &config_data->config_entry_array[config_data->entry_count];

        b32 success = parse_line(line, config_entry);

        if (success)
        {
            config_data->entry_count += 1;

            if (config_data->entry_count >= config_data->entry_capacity)
            {
                extend_capacity(config_data);
            }
        }
    }

    fclose(file);

    return config_data;
}

void config_destroy(ConfigData* config)
{
    for (i32 index = 0; index < config->entry_count; ++index)
    {
        free((void*)config->config_entry_array[index].key);
        free((void*)config->config_entry_array[index].value);
    }

    free(config->config_entry_array);

    free(config);
}
