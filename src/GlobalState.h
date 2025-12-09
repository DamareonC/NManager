#pragma once

#include <gtk/gtk.h>

#define PATH_MAX_LENGTH 1024UL

typedef struct
{
    GtkWindow* main_window;
    GtkListBox* file_list_box;
    GtkEntryBuffer* wd_entry_buffer;
    char current_path[PATH_MAX_LENGTH];
} GlobalState;

void set_global_state(GlobalState* const global_state, const char* const path);