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
    DIR* directory = opendir(path);

    if (directory)
    {
        gtk_list_box_remove_all(global_state->entry_list);

        GArray* entries = g_array_sized_new(FALSE, TRUE, sizeof(Entry), 100U);

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
            entries = NULL;

            return true;
        }
        else
        {
            alert_error(global_state, "Error Opening Directory", "%s could not be opened.", path);
        }

        closedir(directory);
        directory = NULL;

        return false;
    }
    else
    {
        run_file(path);
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
    if (!strncmp(global_state->current_path, "/", PATH_MAX_LENGTH)) return;

    const char* const last_slash = strrchr(global_state->current_path, '/');
    const char* c = global_state->current_path;
    const bool root_is_parent_dir = strchr(global_state->current_path, '/') == last_slash;
    char buffer_path[PATH_MAX_LENGTH];

    memset(buffer_path, 0, PATH_MAX_LENGTH);

    if (root_is_parent_dir)
    {
        buffer_path[0UL] = '/';
    }
    else
    {
        for (size_t i = 0UL; i < PATH_MAX_LENGTH; i++)
        {
            if (c != last_slash)
            {
                buffer_path[i] = global_state->current_path[i];
                c++;
            }
            else break;
        }
    }

    load_directory_and_set_state(global_state, buffer_path);
}