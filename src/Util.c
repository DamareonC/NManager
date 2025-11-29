#include "Util.h"

void set_buffer_path(char *const buffer, const char *const path, const char *const entry)
{
    const bool is_root = !strncmp(path, "/", PATH_MAX_LENGTH);

    memset(buffer, 0, PATH_MAX_LENGTH);
    snprintf(buffer, PATH_MAX_LENGTH, is_root ? "%s%s" : "%s/%s", path, entry);
}