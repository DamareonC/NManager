#include "Dir.h"

#define PATH_MAX_SIZE 1024

extern char current_dir[PATH_MAX_SIZE];

void load_current_dir(GtkListBox* const list_box)
{
    DIR* const dir_ptr = opendir(current_dir);

    if (dir_ptr)
    {
        gtk_list_box_remove_all(list_box);

        const struct dirent* dir_ent_ptr;

        while ((dir_ent_ptr = readdir(dir_ptr)))
        {
            if (!strcmp(dir_ent_ptr->d_name, ".") || !strcmp(dir_ent_ptr->d_name, "..")) continue;

            GtkListBoxRow* const list_box_row = GTK_LIST_BOX_ROW(gtk_list_box_row_new());
            gtk_list_box_append(list_box, GTK_WIDGET(list_box_row));
            g_signal_connect(list_box_row, "activate", G_CALLBACK(load_child_dir), NULL);

            GtkLabel* const label = GTK_LABEL(gtk_label_new(dir_ent_ptr->d_name));
            gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(list_box_row), GTK_WIDGET(label));
            gtk_label_set_xalign(label, 0.0F);
        }

        closedir(dir_ptr);
    }
    else
    {
        char error[1024];

        sprintf(error, "ERROR: Unable to read directory %s", current_dir);
        perror(error);
        exit(EXIT_FAILURE);
    }
}

void load_parent_dir(GtkListBox* const list_box)
{
    if (!strcmp(current_dir, "/")) return;

    const char* const last_slash = strrchr(current_dir, '/');
    const char* c = current_dir;
    const bool root_is_parent_dir = strchr(current_dir, '/') == last_slash;

    for (int i = 0; i < PATH_MAX_SIZE; i++)
    {
        if (root_is_parent_dir)
        {
            memset(current_dir + 1, 0, PATH_MAX_SIZE - 1);
            break;
        }
        else
        {
            if (current_dir[i] == 0) break;
            if (++c > last_slash) current_dir[i] = 0;
        }
    }

    load_current_dir(list_box);
}

void load_child_dir(GtkListBoxRow* const list_box_row)
{
    GtkListBox* const list_box = GTK_LIST_BOX(gtk_widget_get_parent(GTK_WIDGET(list_box_row)));
    GtkLabel* const label = GTK_LABEL(gtk_list_box_row_get_child(list_box_row));
    const char* const entry_name = gtk_label_get_text(label);
    const bool root = !strcmp(current_dir, "/");

    sprintf(current_dir, root ? "%s%s" : "%s/%s", current_dir, entry_name);
    load_current_dir(list_box);
}