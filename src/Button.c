#include "Button.h"
#include "Dir.h"
#include "Globals.h"

static void up_button_clicked(const GtkButton* const button, const gpointer data)
{
    load_parent((GtkListBox*)data);
}

static void home_button_clicked(const GtkButton* const button, const gpointer data)
{
    load_dir((GtkListBox*)data, g_get_home_dir());
}

static void path_entry_activate(GtkEntry* const entry, const gpointer data)
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

    if (load_dir((GtkListBox*)data, buffer_path))
    {
        gtk_entry_buffer_set_text(entry_buffer, buffer_path, strlen(buffer_path));
        set_globals(gtk_entry_buffer_get_text(entry_buffer));
    }
}

void load_buttons(GtkBuilder* const builder, const GtkListBox* const list_box)
{
    GtkButton* const up_button = GTK_BUTTON(gtk_builder_get_object(builder, "up_button"));
    g_signal_connect(up_button, "clicked", G_CALLBACK(up_button_clicked), (gpointer)list_box);
    
    GtkButton* const home_button = GTK_BUTTON(gtk_builder_get_object(builder, "home_button"));
    g_signal_connect(home_button, "clicked", G_CALLBACK(home_button_clicked), (gpointer)list_box);

    GtkEntry* const path_entry = GTK_ENTRY(gtk_builder_get_object(builder, "path_entry"));
    GtkEntryBuffer* const path_entry_buffer = gtk_entry_get_buffer(path_entry);

    gtk_entry_buffer_set_text(path_entry_buffer, g_get_home_dir(), strlen(g_get_home_dir()));
    g_signal_connect(path_entry, "activate", G_CALLBACK(path_entry_activate), (gpointer)list_box);
}