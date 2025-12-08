#include "Util.h"

void set_buffer(char* const buffer, const char* const string)
{
    memset(buffer, 0, PATH_MAX_LENGTH);
    g_strlcpy(buffer, string, PATH_MAX_LENGTH);
}

void set_buffer_format(char* const buffer, const char* const format, const char* const arg)
{
    memset(buffer, 0, PATH_MAX_LENGTH);
    g_snprintf(buffer, PATH_MAX_LENGTH, format, arg);
}

void set_buffer_path(char* const buffer, const char* const path, const char* const entry)
{
    memset(buffer, 0, PATH_MAX_LENGTH);
    g_snprintf(buffer, PATH_MAX_LENGTH, !g_strcmp0(path, "/") ? "%s%s" : "%s/%s", path, entry);
}