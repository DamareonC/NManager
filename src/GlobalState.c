#include "GlobalState.h"

void set_global_state(GlobalState* const global_state, const char* const path)
{
    memset(global_state->current_path, 0, PATH_MAX_LENGTH);
    strncpy(global_state->current_path, path, PATH_MAX_LENGTH);
    gtk_entry_buffer_set_text(global_state->path_entry_buffer, global_state->current_path, strnlen(global_state->current_path, PATH_MAX_LENGTH));
}