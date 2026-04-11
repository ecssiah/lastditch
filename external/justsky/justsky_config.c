#include "justsky_config.h"

#include <_string.h>
#include <string.h>
#include <stdio.h>

#include "justsky.h"
#include "justsky_log.h"

static void strip_newline(char *str)
{
    char *p = str;
    
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

static b32 parse_line(char *line, JUSTSKY_ConfigEntry *out_config_entry)
{
    strip_newline(line);

    if (line[0] == '\0')
    {
        return false;
    }

    char *equal_sign_present = strchr(line, '=');

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

static void extend_capacity(JUSTSKY_Config* config)
{
    size_t new_capacity = config->entry_capacity * 2;

    JUSTSKY_ConfigEntry *new_config_entry_array = realloc(
        config->config_entry_array,
        new_capacity * sizeof(JUSTSKY_ConfigEntry)
    );

    if (!new_config_entry_array)
    {
        // handle allocation failure
        return;
    }

    config->config_entry_array = new_config_entry_array;
    config->entry_capacity = new_capacity;
}

JUSTSKY_Config *justsky_load_config(const char* config_path)
{
    JUSTSKY_Config *justsky_config = malloc(sizeof(JUSTSKY_Config));
    justsky_config->entry_count = 0;
    justsky_config->entry_capacity = 16;
    
    justsky_config->config_entry_array = malloc(sizeof(JUSTSKY_ConfigEntry) * justsky_config->entry_capacity);
    
    FILE *file = fopen(config_path, "r");

    if (!file)
    {
        LOG_ERROR("Config not found: %s", config_path);

        return justsky_config;
    }

    char line[512];

    while (fgets(line, sizeof(line), file))
    {
        JUSTSKY_ConfigEntry *config_entry = &justsky_config->config_entry_array[justsky_config->entry_count];
		
        b32 success = parse_line(line, config_entry);

        if (success)
        {
            justsky_config->entry_count += 1;

            if (justsky_config->entry_count >= justsky_config->entry_capacity)
            {
                extend_capacity(justsky_config);
            }
        }
    }

    fclose(file);

    return justsky_config;
}

void justsky_destroy_config(JUSTSKY_Config *config)
{
    for (u32 index = 0; index < config->entry_count; ++index)
    {
        free((void*)config->config_entry_array[index].key);
        free((void*)config->config_entry_array[index].value);
    }

    free(config->config_entry_array);

    free(config);
}
