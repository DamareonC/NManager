#include "Button.h"
#include "Directory.h"

static void s_up_button_clicked(const GtkButton* const button, GlobalState* const global_state)
{
    load_parent_directory(global_state);
}

static void s_home_button_clicked(const GtkButton* const button, GlobalState* const global_state)
{
    load_directory_and_set_global_state(global_state, g_get_home_dir());
}

static void s_refresh_button_clicked(const GtkButton* const button, GlobalState* const global_state)
{
    reload_directory(global_state);
}

void load_buttons(GtkBuilder* const builder, GlobalState* const global_state)
{
    g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "up_button")), "clicked", G_CALLBACK(s_up_button_clicked), global_state);
    g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "home_button")), "clicked", G_CALLBACK(s_home_button_clicked), global_state);
    g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "refresh_button")), "clicked", G_CALLBACK(s_refresh_button_clicked), global_state);
}