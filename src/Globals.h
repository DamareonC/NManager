#pragma once

#include <gtk/gtk.h>

#define PATH_MAX_SIZE 1024

typedef struct
{
    char current_path[PATH_MAX_SIZE];
    GtkEntryBuffer* path_entry_buffer;
} Globals;