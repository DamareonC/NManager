#include <gtk/gtk.h>

#include <stdlib.h>
#include <string.h>

static void load_dir_entries(GtkWidget* list_box, const char* path)
{
    DIR* dir_ptr = opendir(path);

    if (dir_ptr)
    {
        struct dirent* dir_ent_ptr;

        while ((dir_ent_ptr = readdir(dir_ptr)) != NULL)
        {
            if (!strcmp(dir_ent_ptr->d_name, ".") || !strcmp(dir_ent_ptr->d_name, "..")) continue;

            GtkWidget* const list_box_row = gtk_list_box_row_new();
            gtk_list_box_append(GTK_LIST_BOX(list_box), list_box_row);

            GtkWidget* const label = gtk_label_new(dir_ent_ptr->d_name);
            gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(list_box_row), label);
            gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
        }

        closedir(dir_ptr);
    }
    else
    {
        perror("ERROR: Unable to read directory. Terminating program.");
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