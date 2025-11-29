#include "File.h"
#include "Util.h"

int run_file(const char* const file)
{
    char command[PATH_MAX_LENGTH];

    memset(command, 0, PATH_MAX_LENGTH);
    snprintf(command, PATH_MAX_LENGTH, "xdg-open '%s'", file);

    return system(command);
}

void run_child_file(const GlobalState* const global_state, const char* const entry_name)
{
    char buffer_path[PATH_MAX_LENGTH];

    set_buffer_path(buffer_path, global_state->current_path, entry_name);
    run_file(buffer_path);
}