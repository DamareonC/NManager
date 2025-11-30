#include "Directory.h"
#include "Entry.h"
#include "Error.h"
#include "File.h"
#include "Util.h"

static void s_load_child(GlobalState* const global_state, const char* const entry_name)
{
    char buffer_path[PATH_MAX_LENGTH];

    set_buffer_path(buffer_path, global_state->current_path, entry_name);
    load_directory_and_set_state(global_state, buffer_path);
}

static void s_load_child_directory_row(GtkListBoxRow* const list_box_row, GlobalState* const global_state)
{
    const char* const entry_name = gtk_button_get_label(GTK_BUTTON(gtk_list_box_row_get_child(list_box_row)));
    s_load_child(global_state, entry_name);
}

static void s_load_child_directory_button(GtkButton* const button, GlobalState* const global_state)
{
    const char* const entry_name = gtk_button_get_label(button);
    s_load_child(global_state, entry_name);
}

static void s_run_child_file_row(GtkListBoxRow* const list_box_row, GlobalState* const global_state)
{
    const char* const entry_name = gtk_button_get_label(GTK_BUTTON(gtk_list_box_row_get_child(list_box_row)));
    run_child_file(global_state, entry_name);
}

static void s_run_child_file_button(GtkButton* const button, GlobalState* const global_state)
{
    const char* const entry_name = gtk_button_get_label(button);
    run_child_file(global_state, entry_name);
}

void load_directory_and_set_state(GlobalState* const global_state, const char* const path)
{
    if (load_directory(global_state, path))
    {
        set_global_state(global_state, path);
    }
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
                Entry entry = g_array_index(entries, Entry, i);

                if (!global_state->show_hidden && entry.is_hidden) continue;

                GtkListBoxRow* const list_box_row = GTK_LIST_BOX_ROW(gtk_list_box_row_new());
                GtkButton* const button = GTK_BUTTON(gtk_button_new_with_label(entry.name));
                GtkLabel* const label = GTK_LABEL(gtk_button_get_child(button));

                gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(list_box_row), GTK_WIDGET(button));
                gtk_list_box_append(global_state->entry_list, GTK_WIDGET(list_box_row));
                gtk_label_set_xalign(label, 0.0F);

                if (entry.is_directory)
                {
                    g_signal_connect(list_box_row, "activate", G_CALLBACK(s_load_child_directory_row), global_state);
                    g_signal_connect(button, "clicked", G_CALLBACK(s_load_child_directory_button), global_state);
                }
                else
                {
                    g_signal_connect(list_box_row, "activate", G_CALLBACK(s_run_child_file_row), global_state);
                    g_signal_connect(button, "clicked", G_CALLBACK(s_run_child_file_button), global_state);
                }
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
        buffer_path[0] = '/';
    }
    else
    {
        for (size_t i = 0; i < PATH_MAX_LENGTH; i++)
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