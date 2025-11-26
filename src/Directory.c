#include "Directory.h"
#include "Entry.h"
#include "GlobalState.h"

static void s_load_child(GlobalState* const global_state, const char* const entry_name)
{
    const bool is_root = !strcmp(global_state->current_path, "/");
    char buffer_path[PATH_MAX_SIZE];

    memset(buffer_path, 0, PATH_MAX_SIZE);
    snprintf(buffer_path, PATH_MAX_SIZE, is_root ? "%s%s" : "%s/%s", global_state->current_path, entry_name);
    
    if (load_directory(global_state, buffer_path))
    {
        set_global_state(global_state, buffer_path);
    }
}

static int s_open_file(const char* const file)
{
    char command[PATH_MAX_SIZE];

    memset(command, 0, PATH_MAX_SIZE);
    snprintf(command, PATH_MAX_SIZE, "xdg-open '%s'", file);

    return system(command);
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
                GtkListBoxRow* const list_box_row = GTK_LIST_BOX_ROW(gtk_list_box_row_new());
                gtk_list_box_append(global_state->entry_list, GTK_WIDGET(list_box_row));
                g_signal_connect(list_box_row, "activate", G_CALLBACK(load_child_activate), global_state);

                GtkButton* const button = GTK_BUTTON(gtk_button_new_with_label(g_array_index(entries, Entry, i).name));
                gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(list_box_row), GTK_WIDGET(button));
                g_signal_connect(button, "clicked", G_CALLBACK(load_child_clicked), global_state);

                GtkLabel* const label = GTK_LABEL(gtk_button_get_child(button));
                gtk_label_set_xalign(label, 0.0F);
            }

            g_array_free(entries, TRUE);
            entries = NULL;

            return true;
        }
        else
        {
            alert_error(global_state, path);
        }

        closedir(directory);
        directory = NULL;

        return false;
    }
    else
    {
        if (errno != ENOTDIR || s_open_file(path))
        {
            alert_error(global_state, path);
        }

        return false;
    }
}

void load_parent(GlobalState* const global_state)
{
    if (!strcmp(global_state->current_path, "/")) return;

    const char* const last_slash = strrchr(global_state->current_path, '/');
    const char* c = global_state->current_path;
    const bool root_is_parent_dir = strchr(global_state->current_path, '/') == last_slash;
    char buffer_path[PATH_MAX_SIZE];

    memset(buffer_path, 0, PATH_MAX_SIZE);

    if (root_is_parent_dir)
    {
        buffer_path[0] = '/';
    }
    else
    {
        for (size_t i = 0; i < PATH_MAX_SIZE; i++)
        {
            if (c != last_slash)
            {
                buffer_path[i] = global_state->current_path[i];
                c++;
            }
            else break;
        }
    }

    if (load_directory(global_state, buffer_path))
    {
        set_global_state(global_state, buffer_path);
    }
}

void load_child_activate(GtkListBoxRow* const list_box_row, GlobalState* const data)
{
    GtkListBox* const list_box = GTK_LIST_BOX(gtk_widget_get_parent(GTK_WIDGET(list_box_row)));
    const char* const entry_name = gtk_button_get_label(GTK_BUTTON(gtk_list_box_row_get_child(list_box_row)));

    s_load_child(data, entry_name);
}

void load_child_clicked(GtkButton* const button, GlobalState* const data)
{
    GtkListBox* const list_box = GTK_LIST_BOX(gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_LIST_BOX));
    const char* const entry_name = gtk_button_get_label(button);

    s_load_child(data, entry_name);
}