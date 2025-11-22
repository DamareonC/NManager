#include <glib.h>
#include <gtk/gtk.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATH_MAX_SIZE 1024

extern char current_dir[PATH_MAX_SIZE];

static void load_dir_entries(const GtkWidget* const list_box, const char* const path);

static void load_entry_dir(GtkWidget* const widget, const gpointer data)
{
    if (GTK_IS_LIST_BOX_ROW(widget))
    {
        const GtkWidget* const list_box = gtk_widget_get_parent(widget);
        GtkListBoxRow* const list_box_row = GTK_LIST_BOX_ROW(widget);
        GtkLabel* const label = GTK_LABEL(gtk_list_box_row_get_child(list_box_row));
        const char* const entry_name = gtk_label_get_text(label);
        const bool root = !strcmp(current_dir, "/");

        sprintf(current_dir, root ? "%s%s" : "%s/%s", current_dir, entry_name);
        load_dir_entries(list_box, current_dir);
    }
    else if (GTK_IS_BUTTON(widget))
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
                if (++c > last_slash) current_dir[i] = 0;
            }
        }

        load_dir_entries((GtkWidget*)data, current_dir);
    }
}

static void load_dir_entries(const GtkWidget* const list_box, const char* const path)
{
    DIR* const dir_ptr = opendir(path);

    if (dir_ptr)
    {
        gtk_list_box_remove_all(GTK_LIST_BOX(list_box));

        const struct dirent* dir_ent_ptr;

        while ((dir_ent_ptr = readdir(dir_ptr)))
        {
            if (!strcmp(dir_ent_ptr->d_name, ".") || !strcmp(dir_ent_ptr->d_name, "..")) continue;

            GtkWidget* const list_box_row = gtk_list_box_row_new();
            gtk_list_box_append(GTK_LIST_BOX(list_box), list_box_row);
            g_signal_connect(list_box_row, "activate", G_CALLBACK(load_entry_dir), NULL);

            GtkWidget* const label = gtk_label_new(dir_ent_ptr->d_name);
            gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(list_box_row), label);
            gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
        }

        closedir(dir_ptr);
    }
    else
    {
        char error[1024];

        sprintf(error, "ERROR: Unable to read directory %s", path);
        perror(error);
        exit(EXIT_FAILURE);
    }
}

static void load_css(void)
{
    GtkCssProvider* const css_provider = gtk_css_provider_new();
    GdkDisplay* const display = gdk_display_get_default();
    GFile* const css_file_ptr = g_file_new_for_path("res/main.css");

    gtk_css_provider_load_from_file(css_provider, css_file_ptr);
    gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(css_provider);
}