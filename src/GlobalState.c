#include "GlobalState.h"

void set_global_state(GlobalState* const global_state, const char* const dir)
{
    memset(global_state->current_path, 0, PATH_MAX_SIZE);
    strncpy(global_state->current_path, dir, PATH_MAX_SIZE);
    gtk_entry_buffer_set_text(global_state->path_entry_buffer, dir, MIN(strlen(dir), PATH_MAX_SIZE));
}