#include <gtk/gtk.h>

#include "App.h"

void run(GtkApplication* const app, const gpointer user_data)
{
    GtkBuilder* const builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "res/main.ui", NULL);

    const GObject* const window = gtk_builder_get_object(builder, "window");
    gtk_window_set_application(GTK_WINDOW(window), app);

    const GObject* const list_box = gtk_builder_get_object(builder, "entry_list");
    load_dir_entries(GTK_WIDGET(list_box), g_get_home_dir());
    load_css();

    gtk_widget_set_visible(GTK_WIDGET(window), true);
    g_object_unref(builder);
}