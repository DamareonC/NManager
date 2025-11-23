#include "Button.h"
#include "Dir.h"

static void up_button_click(const GtkButton* const button, const gpointer data)
{
    load_parent_dir(GTK_LIST_BOX((GObject*)data));
}

void load_buttons(GtkBuilder* const builder, const GtkListBox* const list_box)
{
    const GObject* const up_button = gtk_builder_get_object(builder, "up_button");
    g_signal_connect(GTK_WIDGET(up_button), "clicked", G_CALLBACK(up_button_click), (gpointer)list_box);
}