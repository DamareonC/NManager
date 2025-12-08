#include "Directory.h"
#include "Entry.h"
#include "Menu.h"
#include "Error.h"
#include "Util.h"

static void s_add_entry_activate(GlobalState* const global_state, const bool add_folder)
{
    AddInfo* const add_info = malloc(sizeof(AddInfo));

    if (!add_info)
    {
        alert_error(global_state, "Error Allocating Memory", "Memory could not be allocated.", "");
    }
    else
    {
        add_info->global_state = global_state;
        add_info->is_folder = add_folder;

        GtkBuilder* const builder = gtk_builder_new();
        gtk_builder_add_from_file(builder, "res/ui/add_entry.ui", NULL);

        GtkWindow* const popup_window = GTK_WINDOW(gtk_builder_get_object(builder, "add_entry_window"));
        gtk_window_set_transient_for(popup_window, add_info->global_state->main_window);
        gtk_window_present(popup_window);

        GtkWidget* const entry = GTK_WIDGET(gtk_builder_get_object(builder, "add_entry_entry"));
        g_signal_connect(entry, "activate", G_CALLBACK(add_entry), add_info);

        g_object_unref(builder);
    }
}

static void s_add_file_activate(GSimpleAction* const action, GVariant* const param, const gpointer state)
{
    s_add_entry_activate(state, false);
}

static void s_add_folder_activate(GSimpleAction* const action, GVariant* const param, const gpointer state)
{
    s_add_entry_activate(state, true);
}

static void s_move_to_trash_activate(GSimpleAction* const action, GVariant* const param, const gpointer state)
{
    GlobalState* const global_state = state;
    GtkListBoxRow* const list_box_row = gtk_list_box_get_selected_row(global_state->entry_list);

    if (list_box_row)
    {
        const char* const entry_name = gtk_label_get_text(GTK_LABEL(gtk_list_box_row_get_child(list_box_row)));
        DeleteInfo* const delete_info = malloc(sizeof(DeleteInfo));
        
        if (!delete_info)
        {
            alert_error(global_state, "Error Allocating Memory", "Memory could not be allocated.", "");
        }
        else
        {
            delete_info->global_state = global_state;
            delete_info->entry_name = entry_name;

            trash_entry(global_state, delete_info);
        }
    }
}

static void s_permanently_delete_activate(GSimpleAction* const action, GVariant* const param, const gpointer state)
{
    GlobalState* const global_state = state;
    GtkListBoxRow* const list_box_row = gtk_list_box_get_selected_row(global_state->entry_list);

    if (list_box_row)
    {
        DeleteInfo* const delete_info = malloc(sizeof(DeleteInfo));
        GtkAlertDialog* const alert_dialog = gtk_alert_dialog_new("Permanent Delete");
        const char* const entry_name = gtk_label_get_text(GTK_LABEL(gtk_list_box_row_get_child(list_box_row)));
        const char* const buttons[3] = { "Delete", "Cancel", NULL };
        char message[PATH_MAX_LENGTH];
        
        if (!delete_info)
        {
            alert_error(global_state, "Error Allocating Memory", "Memory could not be allocated.", "");
            g_object_unref(alert_dialog);
        }
        else
        {
            delete_info->global_state = global_state;
            delete_info->entry_name = entry_name;

            set_buffer_format(message, "Are you sure you want to permanently delete %s? This action cannot be undone.", entry_name);
            gtk_alert_dialog_set_detail(alert_dialog, message);
            gtk_alert_dialog_set_buttons(alert_dialog, buttons);
            gtk_alert_dialog_set_default_button(alert_dialog, 0);
            gtk_alert_dialog_set_cancel_button(alert_dialog, 1);
            gtk_alert_dialog_choose(alert_dialog, global_state->main_window, NULL, delete_entry, delete_info);

            g_object_unref(alert_dialog);
        }
    }
}

static void s_show_hidden_activate(GSimpleAction* const action, GVariant* const param, const gpointer state)
{
    GlobalState* const global_state = state;
    global_state->show_hidden = g_variant_get_boolean(param);

    reload_directory(global_state);
    g_simple_action_set_state(action, param);
}

void load_menu(GtkApplication* const app, GlobalState* const global_state)
{
    const GActionEntry action_entries[] = {
        { "add-file", s_add_file_activate, NULL, NULL, NULL },
        { "add-folder", s_add_folder_activate, NULL, NULL, NULL },
        { "move-to-trash", s_move_to_trash_activate, NULL, NULL, NULL },
        { "permanently-delete", s_permanently_delete_activate, NULL, NULL, NULL },
        { "show-hidden", NULL, NULL, global_state->show_hidden ? "true" : "false", s_show_hidden_activate }
    };

    g_action_map_add_action_entries(G_ACTION_MAP(app), action_entries, G_N_ELEMENTS(action_entries), global_state);
}