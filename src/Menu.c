#include "Directory.h"
#include "Error.h"
#include "Menu.h"
#include "Util.h"

typedef struct 
{
    GlobalState* global_state;
    const char* entry_name;
} EntryData;

static void s_permanently_delete(GObject* const warning_alert_dialog, GAsyncResult* const result, const gpointer data)
{
    EntryData* entry_data = data;
    const int response = gtk_alert_dialog_choose_finish(GTK_ALERT_DIALOG(warning_alert_dialog), result, NULL);

    if (!response)
    {
        char buffer_path[PATH_MAX_LENGTH];
        char command[PATH_MAX_LENGTH];

        set_buffer_path(buffer_path, entry_data->global_state->current_path, entry_data->entry_name);
        snprintf(command, PATH_MAX_LENGTH, "rm -rf %s", buffer_path);
        
        if (!system(command))
        {
            load_directory(entry_data->global_state, entry_data->global_state->current_path);
        }
        else
        {
            alert_error(entry_data->global_state, "Error Deleting File", "%s could not be deleted.", buffer_path);
        }
    }

    free(entry_data);
    entry_data = NULL;
}

static void s_permanently_delete_activate(GSimpleAction* const action, GVariant* const param, const gpointer state)
{
    GlobalState* const global_state = state;
    GtkListBoxRow* const list_box_row = gtk_list_box_get_selected_row(global_state->entry_list);

    if (list_box_row)
    {
        const char* const entry_name = gtk_button_get_label(GTK_BUTTON(gtk_list_box_row_get_child(list_box_row)));
        GtkAlertDialog* alert_dialog = gtk_alert_dialog_new("Permanent Delete");
        GtkWindow* const window = GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(global_state->entry_list), GTK_TYPE_WINDOW));
        const char* const buttons[3] = { "Delete", "Cancel", NULL };
        char message[PATH_MAX_LENGTH];

        EntryData* const data = malloc(sizeof(EntryData));
        
        if (!data)
        {
            exit(EXIT_FAILURE);
        }
        else
        {
            data->global_state = global_state;
            data->entry_name = entry_name;
        }

        set_buffer_format(message, "Are you sure you want to permanently delete %s? This action cannot be undone.", entry_name);
        gtk_alert_dialog_set_detail(alert_dialog, message);
        gtk_alert_dialog_set_buttons(alert_dialog, buttons);
        gtk_alert_dialog_set_default_button(alert_dialog, 0);
        gtk_alert_dialog_set_cancel_button(alert_dialog, 1);
        gtk_alert_dialog_choose(alert_dialog, window, NULL, s_permanently_delete, data);

        g_object_unref(alert_dialog);
        alert_dialog = NULL;
    }
}

static void s_show_hidden_activate(GSimpleAction* const action, GVariant* const param, const gpointer state)
{
    GlobalState* const global_state = state;
    global_state->show_hidden = g_variant_get_boolean(param);

    load_directory(global_state, global_state->current_path);
    g_simple_action_set_state(action, param);
}

void load_menu(GtkApplication* const app, GlobalState* const global_state)
{
    const GActionEntry action_entries[] = {
        { "permanently-delete", s_permanently_delete_activate, NULL, NULL, NULL },
        { "show-hidden", NULL, NULL, global_state->show_hidden ? "true" : "false", s_show_hidden_activate }
    };

    g_action_map_add_action_entries(G_ACTION_MAP(app), action_entries, G_N_ELEMENTS(action_entries), global_state);
}