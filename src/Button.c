#include "Button.h"
#include "Directory.h"

static void s_up_button_clicked(const GtkButton* const button, GlobalState* const global_state)
{
    load_parent_directory(global_state);
}

static void s_home_button_clicked(const GtkButton* const button, GlobalState* const global_state)
{
    load_directory_and_set_state(global_state, g_get_home_dir());
}

static void s_refresh_button_clicked(const GtkButton* const button, GlobalState* const global_state)
{
    reload_directory(global_state);
}

static void s_path_entry_activate(GtkEntry* const entry, GlobalState* const global_state)
{
    GtkEntryBuffer* const entry_buffer = gtk_entry_get_buffer(entry);
    const char* const current_text = gtk_entry_buffer_get_text(entry_buffer);

    if (g_strcmp0(current_text, "") && load_directory(global_state, current_text))
    {
        set_global_state(global_state, gtk_entry_buffer_get_text(entry_buffer));
    }
}

void load_buttons(GtkBuilder* const builder, GlobalState* const global_state)
{
    g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "up_button")), "clicked", G_CALLBACK(s_up_button_clicked), global_state);
    g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "home_button")), "clicked", G_CALLBACK(s_home_button_clicked), global_state);
    g_signal_connect(GTK_BUTTON(gtk_builder_get_object(builder, "refresh_button")), "clicked", G_CALLBACK(s_refresh_button_clicked), global_state);
    g_signal_connect(GTK_ENTRY(gtk_builder_get_object(builder, "path_entry")), "activate", G_CALLBACK(s_path_entry_activate), global_state);
}