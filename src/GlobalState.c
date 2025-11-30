#include "GlobalState.h"
#include "Util.h"

void set_global_state(GlobalState* const global_state, const char* const path)
{
    set_buffer(global_state->current_path, path);
    gtk_entry_buffer_set_text(global_state->path_entry_buffer, global_state->current_path, strnlen(global_state->current_path, PATH_MAX_LENGTH));
}