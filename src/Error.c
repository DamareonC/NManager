#include "Error.h"
#include "Util.h"

void alert_error(const GlobalState* const global_state, const char* const title, const char* const message, const char* const path)
{
    GtkAlertDialog* alert_dialog = gtk_alert_dialog_new("%s", title);
    char message_format[PATH_MAX_LENGTH];

    set_buffer_format(message_format, message, path);
    gtk_alert_dialog_set_message(alert_dialog, message_format);
    gtk_alert_dialog_show(alert_dialog, global_state->main_window);

    g_object_unref(alert_dialog);
    alert_dialog = NULL;
}