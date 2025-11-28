#include "Button.h"
#include "Directory.h"
#include "GlobalState.h"

static void s_up_button_clicked(const GtkButton* const button, GlobalState* const global_state)
{
    load_parent(global_state);
}

static void s_home_button_clicked(const GtkButton* const button, GlobalState* const global_state)
{
    load_directory_and_set_state(global_state, g_get_home_dir());
}

static void s_refresh_button_clicked(const GtkButton* const button, GlobalState* const global_state)
{
    load_directory(global_state, global_state->current_path);
}

static void s_path_entry_activate(GtkEntry* const entry, GlobalState* const global_state)
{
    GtkEntryBuffer* const entry_buffer = gtk_entry_get_buffer(entry);
    const char* const current_text = gtk_entry_buffer_get_text(entry_buffer);
    long last_index = strnlen(current_text, PATH_MAX_SIZE) - 1;
    char buffer_path[PATH_MAX_SIZE];

    memset(buffer_path, 0, PATH_MAX_SIZE);
    strncpy(buffer_path, current_text, PATH_MAX_SIZE);

    while (current_text[last_index] == '/' && last_index > 0)
    {
        buffer_path[last_index] = 0;
        last_index--;
    }

    if (strncmp(buffer_path, "", PATH_MAX_SIZE) && load_directory(global_state, buffer_path))
    {
        gtk_entry_buffer_set_text(entry_buffer, buffer_path, strnlen(buffer_path, PATH_MAX_SIZE));
        set_global_state(global_state, gtk_entry_buffer_get_text(entry_buffer));
    }
}

void load_buttons(GtkBuilder* const builder, GlobalState* const global_state)
{
    GtkButton* const up_button = GTK_BUTTON(gtk_builder_get_object(builder, "up_button"));
    g_signal_connect(up_button, "clicked", G_CALLBACK(s_up_button_clicked), global_state);
    
    GtkButton* const home_button = GTK_BUTTON(gtk_builder_get_object(builder, "home_button"));
    g_signal_connect(home_button, "clicked", G_CALLBACK(s_home_button_clicked), global_state);

    GtkButton* const refresh_button = GTK_BUTTON(gtk_builder_get_object(builder, "refresh_button"));
    g_signal_connect(refresh_button, "clicked", G_CALLBACK(s_refresh_button_clicked), global_state);

    GtkEntry* const path_entry = GTK_ENTRY(gtk_builder_get_object(builder, "path_entry"));
    g_signal_connect(path_entry, "activate", G_CALLBACK(s_path_entry_activate), global_state);
}