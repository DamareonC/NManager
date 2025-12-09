#include "Directory.h"
#include "Entry.h"

static void s_wd_entry_activate(GtkEntry* const entry, GlobalState* const global_state)
{
    GtkEntryBuffer* const entry_buffer = gtk_entry_get_buffer(entry);
    const char* const current_text = gtk_entry_buffer_get_text(entry_buffer);

    if (g_strcmp0(current_text, "") && load_directory(global_state, current_text))
    {
        set_global_state(global_state, gtk_entry_buffer_get_text(entry_buffer));
    }
}

void load_wd_entry(GtkBuilder* const builder, GlobalState* const global_state)
{
    g_signal_connect(GTK_ENTRY(gtk_builder_get_object(builder, "wd_entry")), "activate", G_CALLBACK(s_wd_entry_activate), global_state);
}