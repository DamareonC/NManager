#include <glib.h>
#include <gtk/gtk.h>

#include <stdio.h>

#include "App.h"

char current_dir[1024];

void run(GtkApplication* const app, const gpointer user_data)
{
    sprintf(current_dir, "%s", g_get_home_dir());

    GtkBuilder* const builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "res/main.ui", NULL);

    const GObject* const window = gtk_builder_get_object(builder, "window");
    gtk_window_set_application(GTK_WINDOW(window), app);

    const GObject* const list_box = gtk_builder_get_object(builder, "entry_list");
    load_dir_entries(GTK_WIDGET(list_box), current_dir);
    load_css();

    const GObject* const up_button = gtk_builder_get_object(builder, "up_button");
    g_signal_connect(GTK_WIDGET(up_button), "clicked", G_CALLBACK(load_entry_dir), (gpointer)list_box);

    gtk_widget_set_visible(GTK_WIDGET(window), true);
    g_object_unref(builder);
}