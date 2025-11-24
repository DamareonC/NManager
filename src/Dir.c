#include "Dir.h"
#include "Globals.h"

extern Globals globals;

static int s_open_file(const char* const file)
{
    char command[PATH_MAX_SIZE];

    memset(command, 0, PATH_MAX_SIZE);
    sprintf(command, "xdg-open '%s'", file);

    return system(command);
}

static void s_load_child(GtkListBox* const list_box, const char* const entry_name)
{
    const bool is_root = !strcmp(globals.current_path, "/");
    char buffer_path[PATH_MAX_SIZE];

    memset(buffer_path, 0, PATH_MAX_SIZE);
    sprintf(buffer_path, is_root ? "%s%s" : "%s/%s", globals.current_path, entry_name);
    
    if (load_dir(list_box, buffer_path))
    {
        set_globals(buffer_path);
    }
}

bool load_dir(GtkListBox* const list_box, const char* const dir)
{
    DIR* const dir_ptr = opendir(dir);

    if (dir_ptr)
    {
        gtk_list_box_remove_all(list_box);

        const struct dirent* dir_ent_ptr;

        while ((dir_ent_ptr = readdir(dir_ptr)))
        {
            if (!strcmp(dir_ent_ptr->d_name, ".") || !strcmp(dir_ent_ptr->d_name, "..")) continue;

            GtkListBoxRow* const list_box_row = GTK_LIST_BOX_ROW(gtk_list_box_row_new());
            gtk_list_box_append(list_box, GTK_WIDGET(list_box_row));
            g_signal_connect(list_box_row, "activate", G_CALLBACK(load_child_activate), NULL);

            GtkButton* const button = GTK_BUTTON(gtk_button_new_with_label(dir_ent_ptr->d_name));
            gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(list_box_row), GTK_WIDGET(button));
            g_signal_connect(button, "clicked", G_CALLBACK(load_child_clicked), NULL);

            GtkLabel* const label = GTK_LABEL(gtk_button_get_child(button));
            gtk_label_set_xalign(label, 0.0F);
        }

        closedir(dir_ptr);

        return true;
    }
    else
    {
        if (errno != ENOTDIR || s_open_file(dir))
        {
            GtkAlertDialog* alert_dialog = gtk_alert_dialog_new("ERROR: Unable to open %s: %s", dir, strerror(errno));
            GtkWindow* window = GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(list_box), GTK_TYPE_WINDOW));

            gtk_alert_dialog_show(alert_dialog, window);
            g_object_unref(alert_dialog);
        }

        return false;
    }
}

void load_parent(GtkListBox* const list_box)
{
    if (!strcmp(globals.current_path, "/")) return;

    const char* const last_slash = strrchr(globals.current_path, '/');
    const char* c = globals.current_path;
    const bool root_is_parent_dir = strchr(globals.current_path, '/') == last_slash;
    char buffer_path[PATH_MAX_SIZE];

    memset(buffer_path, 0, PATH_MAX_SIZE);

    if (root_is_parent_dir)
    {
        buffer_path[0] = '/';
    }
    else
    {
        for (int i = 0; i < PATH_MAX_SIZE; i++)
        {
            if (c != last_slash)
            {
                buffer_path[i] = globals.current_path[i];
                c++;
            }
            else break;
        }
    }

    if (load_dir(list_box, buffer_path))
    {
        set_globals(buffer_path);
    }
}

void load_child_activate(GtkListBoxRow* const list_box_row)
{
    GtkListBox* const list_box = GTK_LIST_BOX(gtk_widget_get_parent(GTK_WIDGET(list_box_row)));
    const char* const entry_name = gtk_button_get_label(GTK_BUTTON(gtk_list_box_row_get_child(list_box_row)));

    s_load_child(list_box, entry_name);
}

void load_child_clicked(GtkButton* const button)
{
    GtkListBox* const list_box = GTK_LIST_BOX(gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_LIST_BOX));
    const char* const entry_name = gtk_button_get_label(button);

    s_load_child(list_box, entry_name);
}

void set_globals(const char* const dir)
{
    memset(globals.current_path, 0, PATH_MAX_SIZE);
    strcpy(globals.current_path, dir);
    gtk_entry_buffer_set_text(globals.path_entry_buffer, dir, strlen(dir));
}