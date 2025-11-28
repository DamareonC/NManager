#include "GlobalState.h"

void alert_error(const GlobalState* const global_state, const char* const path)
{
    GtkAlertDialog* alert_dialog = gtk_alert_dialog_new("ERROR: Unable to open %s: %s", path, strerror(errno));
    GtkWindow* const window = GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(global_state->entry_list), GTK_TYPE_WINDOW));

    gtk_alert_dialog_show(alert_dialog, window);
    g_object_unref(alert_dialog);
    
    alert_dialog = NULL;
}

void set_global_state(GlobalState* const global_state, const char* const path)
{
    memset(global_state->current_path, 0, PATH_MAX_SIZE);
    strncpy(global_state->current_path, path, PATH_MAX_SIZE);
    gtk_entry_buffer_set_text(global_state->path_entry_buffer, global_state->current_path, strnlen(global_state->current_path, PATH_MAX_SIZE));
}