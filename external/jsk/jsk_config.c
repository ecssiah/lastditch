#include "jsk_config.h"

#include <_string.h>
#include <string.h>
#include <stdio.h>

#include "jsk.h"
#include "jsk_log.h"

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

static b32 parse_line(char *line, JSK_ConfigEntry *out_config_entry)
{
    strip_newline(line);

    if (line[0] == '\0')
    {
        return FALSE;
    }

    char *equal_sign_present = strchr(line, '=');

    if (!equal_sign_present)
    {
        LOG_WARN("Invalid config: %s", line);

        return FALSE;
    }

    *equal_sign_present = '\0';

    const char* key = line;
    const char* value = equal_sign_present + 1;

    out_config_entry->key = strdup(key);
    out_config_entry->value = strdup(value);

    return TRUE;
}

static void extend_capacity(JSK_Config* config)
{
    size_t new_capacity = config->entry_capacity * 2;

    JSK_ConfigEntry *new_config_entry_array = realloc(
        config->config_entry_array,
        new_capacity * sizeof(JSK_ConfigEntry)
    );

    if (!new_config_entry_array)
    {
        // handle allocation failure
        return;
    }

    config->config_entry_array = new_config_entry_array;
    config->entry_capacity = new_capacity;
}

JSK_Config *jsk_load_config(const char* config_path)
{
    JSK_Config *jsk_config = malloc(sizeof(JSK_Config));
    jsk_config->entry_count = 0;
    jsk_config->entry_capacity = 16;
    
    jsk_config->config_entry_array = malloc(sizeof(JSK_ConfigEntry) * jsk_config->entry_capacity);
    
    FILE *file = fopen(config_path, "r");

    if (!file)
    {
        LOG_ERROR("Config not found: %s", config_path);

        return jsk_config;
    }

    char line[512];

    while (fgets(line, sizeof(line), file))
    {
        JSK_ConfigEntry *config_entry = &jsk_config->config_entry_array[jsk_config->entry_count];
		
        b32 success = parse_line(line, config_entry);

        if (success)
        {
            jsk_config->entry_count += 1;

            if (jsk_config->entry_count >= jsk_config->entry_capacity)
            {
                extend_capacity(jsk_config);
            }
        }
    }

    fclose(file);

    return jsk_config;
}

void jsk_destroy_config(JSK_Config *config)
{
    size_t i;
    for (i = 0; i < config->entry_count; ++i)
    {
        free((void*)config->config_entry_array[i].key);
        free((void*)config->config_entry_array[i].value);
    }

    free(config->config_entry_array);

    free(config);
}
