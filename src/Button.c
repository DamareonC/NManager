#include "Button.h"
#include "Dir.h"
#include "GlobalState.h"

static void s_up_button_clicked(const GtkButton* const button, const gpointer data)
{
    load_parent((GlobalState*)data);
}

static void s_home_button_clicked(const GtkButton* const button, const gpointer data)
{
    load_dir((GlobalState*)data, g_get_home_dir());
    set_global_state((GlobalState*)data, g_get_home_dir());
}

static void s_path_entry_activate(GtkEntry* const entry, const gpointer data)
{
    GtkEntryBuffer* const entry_buffer = gtk_entry_get_buffer(entry);
    const char* const current_text = gtk_entry_buffer_get_text(entry_buffer);
    long last_index = strlen(current_text) - 1;
    char buffer_path[PATH_MAX_SIZE];

    memset(buffer_path, 0, PATH_MAX_SIZE);
    strcpy(buffer_path, current_text);

    while (current_text[last_index] == '/')
    {
        buffer_path[last_index] = 0;
        last_index--;
    }

    if (load_dir((GlobalState*)data, buffer_path))
    {
        gtk_entry_buffer_set_text(entry_buffer, buffer_path, strlen(buffer_path));
        set_global_state((GlobalState*)data, gtk_entry_buffer_get_text(entry_buffer));
    }
}

void load_buttons(GtkBuilder* const builder, GlobalState* const global_state)
{
    GtkButton* const up_button = GTK_BUTTON(gtk_builder_get_object(builder, "up_button"));
    g_signal_connect(up_button, "clicked", G_CALLBACK(s_up_button_clicked), (gpointer)global_state);
    
    GtkButton* const home_button = GTK_BUTTON(gtk_builder_get_object(builder, "home_button"));
    g_signal_connect(home_button, "clicked", G_CALLBACK(s_home_button_clicked), (gpointer)global_state);

    GtkEntry* const path_entry = GTK_ENTRY(gtk_builder_get_object(builder, "path_entry"));
    g_signal_connect(path_entry, "activate", G_CALLBACK(s_path_entry_activate), (gpointer)global_state);
}