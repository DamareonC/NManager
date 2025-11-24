#pragma once

#include <gtk/gtk.h>

#define PATH_MAX_SIZE 1024

typedef struct
{
    GtkListBox* entry_list;
    GtkEntryBuffer* path_entry_buffer;
    char current_path[PATH_MAX_SIZE];
} GlobalState;

void set_global_state(GlobalState* const global_state, const char* const dir);