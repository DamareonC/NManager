#include <string.h>

#include "Button.h"
#include "Dir.h"

#define PATH_MAX_SIZE 1024

extern char current_dir[PATH_MAX_SIZE];

static void up_button_clicked(const GtkButton* const button, const gpointer data)
{
    load_parent_dir(GTK_LIST_BOX((GObject*)data));
}

static void home_button_clicked(const GtkButton* const button, const gpointer data)
{
    memset(current_dir, 0, PATH_MAX_SIZE);
    strcpy(current_dir, g_get_home_dir());

    load_current_dir(GTK_LIST_BOX((GObject*)data));
}

void load_buttons(GtkBuilder* const builder, const GtkListBox* const list_box)
{
    const GObject* const up_button = gtk_builder_get_object(builder, "up_button");
    g_signal_connect(GTK_WIDGET(up_button), "clicked", G_CALLBACK(up_button_clicked), (gpointer)list_box);
    
    const GObject* const home_button = gtk_builder_get_object(builder, "home_button");
    g_signal_connect(GTK_WIDGET(home_button), "clicked", G_CALLBACK(home_button_clicked), (gpointer)list_box);
}