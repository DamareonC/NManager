#include "Directory.h"
#include "Entry.h"
#include "Error.h"
#include "File.h"
#include "Util.h"

void load_child(GlobalState* const global_state, const char* const entry_name)
{
    char buffer_path[PATH_MAX_LENGTH];
    set_buffer_path(buffer_path, global_state->current_path, entry_name);

    load_directory_and_set_state(global_state, buffer_path);
}

bool load_directory(GlobalState* const global_state, const char* const path)
{
    DIR* const directory = opendir(path);

    if (directory)
    {
        gtk_list_box_remove_all(global_state->entry_list);

        GArray* const entries = g_array_sized_new(FALSE, TRUE, sizeof(Entry), 1000U);

        if (entries)
        {
            load_entries(global_state, directory, entries, path);

            for (gint i = 0; i < entries->len; i++)
            {
                const Entry* const entry = &g_array_index(entries, Entry, i);

                if (!global_state->show_hidden && entry->is_hidden) continue;

                GtkListBoxRow* const list_box_row = GTK_LIST_BOX_ROW(gtk_list_box_row_new());
                GtkLabel* label = GTK_LABEL(gtk_label_new(entry->name));

                gtk_list_box_append(global_state->entry_list, GTK_WIDGET(list_box_row));
                gtk_list_box_row_set_child(list_box_row, GTK_WIDGET(label));
                gtk_label_set_xalign(label, 0.0F);
            }

            g_array_free(entries, TRUE);

            return true;
        }
        else
        {
            alert_error(global_state, "Error Loading Directory", "%s could not be loading.", path);
        }

        closedir(directory);

        return false;
    }
    else
    {
        run_file(global_state, path);
        return false;
    }
}

void load_directory_and_set_state(GlobalState* const global_state, const char* const path)
{
    if (load_directory(global_state, path))
    {
        set_global_state(global_state, path);
    }
}

void load_parent_directory(GlobalState* const global_state)
{
    GFile* const current_directory = g_file_new_for_path(global_state->current_path);
    GFile* const parent_directory = g_file_get_parent(current_directory);
    
    if (parent_directory)
    {
        load_directory_and_set_state(global_state, g_file_get_path(parent_directory));
        g_object_unref(parent_directory);
    }
    
    g_object_unref(current_directory);
}

void reload_directory(GlobalState* const global_state)
{
    load_directory(global_state, global_state->current_path);
}