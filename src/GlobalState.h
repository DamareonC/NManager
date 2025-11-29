#pragma once

#include <gtk/gtk.h>

#define PATH_MAX_LENGTH 1024

typedef struct
{
    GtkListBox* entry_list;
    GtkEntryBuffer* path_entry_buffer;
    char current_path[PATH_MAX_LENGTH];
    bool show_hidden;
} GlobalState;

void alert_error(const GlobalState* const global_state, const char* const path);
void set_global_state(GlobalState* const global_state, const char* const path);