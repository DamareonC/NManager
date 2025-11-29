#include "Error.h"

void alert_error(const GlobalState* const global_state, const char* const path)
{
    GtkAlertDialog* alert_dialog = gtk_alert_dialog_new("ERROR: Unable to open %s: %s", path, strerror(errno));
    GtkWindow* const window = GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(global_state->entry_list), GTK_TYPE_WINDOW));

    gtk_alert_dialog_show(alert_dialog, window);
    g_object_unref(alert_dialog);
    
    alert_dialog = NULL;
}