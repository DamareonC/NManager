#include "File.h"
#include "Menu.h"
#include "Util.h"

static void s_add_file(GlobalState* const global_state, const bool add_folder)
{
    static AddFileInfo add_file_info;

    add_file_info.global_state = global_state;
    add_file_info.is_folder = add_folder;

    GtkBuilder* const builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "res/ui/add_file_window.ui", NULL);

    GtkWindow* const popup_window = GTK_WINDOW(gtk_builder_get_object(builder, "add_file_window"));
    gtk_window_set_transient_for(popup_window, add_file_info.global_state->main_window);
    gtk_window_present(popup_window);

    GtkWidget* const entry = GTK_WIDGET(gtk_builder_get_object(builder, "add_file_entry"));
    g_signal_connect(entry, "activate", G_CALLBACK(add_file), &add_file_info);

    g_object_unref(builder);
}

static void s_add_file_activate(GSimpleAction* const action, GVariant* const param, const gpointer state)
{
    s_add_file(state, false);
}

static void s_add_folder_activate(GSimpleAction* const action, GVariant* const param, const gpointer state)
{
    s_add_file(state, true);
}

static void s_move_to_trash_activate(GSimpleAction* const action, GVariant* const param, const gpointer state)
{
    GlobalState* const global_state = state;
    GtkListBoxRow* const list_box_row = gtk_list_box_get_selected_row(global_state->file_list_box);

    if (list_box_row)
    {
        static DeleteFileInfo delete_file_info;
        
        delete_file_info.global_state = global_state;
        delete_file_info.entry_name = gtk_label_get_text(GTK_LABEL(gtk_list_box_row_get_child(list_box_row)));

        trash_file(global_state, &delete_file_info);
    }
}

static void s_permanently_delete_activate(GSimpleAction* const action, GVariant* const param, const gpointer state)
{
    GlobalState* const global_state = state;
    GtkListBoxRow* const list_box_row = gtk_list_box_get_selected_row(global_state->file_list_box);

    if (list_box_row)
    {
        static DeleteFileInfo delete_file_info;
        GtkAlertDialog* const alert_dialog = gtk_alert_dialog_new("Permanent Delete");
        const char* const buttons[3] = { "Delete", "Cancel", NULL };
        const char* const entry_name = gtk_label_get_text(GTK_LABEL(gtk_list_box_row_get_child(list_box_row)));
        char message[PATH_MAX_LENGTH];
        
        delete_file_info.global_state = global_state;
        delete_file_info.entry_name = entry_name;

        set_buffer_format(message, "Are you sure you want to permanently delete %s? This action cannot be undone.", entry_name);
        gtk_alert_dialog_set_detail(alert_dialog, message);
        gtk_alert_dialog_set_buttons(alert_dialog, buttons);
        gtk_alert_dialog_set_default_button(alert_dialog, 0);
        gtk_alert_dialog_set_cancel_button(alert_dialog, 1);
        gtk_alert_dialog_choose(alert_dialog, global_state->main_window, NULL, delete_file, &delete_file_info);
        

        g_object_unref(alert_dialog);
    }
}

static gboolean s_filter_by_hidden(GtkListBoxRow* row, gpointer data)
{
    return g_variant_get_boolean(data) || gtk_label_get_text(GTK_LABEL(gtk_list_box_row_get_child(row)))[0] != '.';
}

static void s_show_hidden_files_activate(GSimpleAction* const action, GVariant* const param, const gpointer state)
{
    GlobalState* const global_state = state;

    gtk_list_box_set_filter_func(global_state->file_list_box, s_filter_by_hidden, param, NULL);
    g_simple_action_set_state(action, param);
}

void load_menu(GtkApplication* const app, GlobalState* const global_state)
{
    const GActionEntry action_entries[] = {
        { "add-file", s_add_file_activate, NULL, NULL, NULL },
        { "add-folder", s_add_folder_activate, NULL, NULL, NULL },
        { "move-to-trash", s_move_to_trash_activate, NULL, NULL, NULL },
        { "permanently-delete", s_permanently_delete_activate, NULL, NULL, NULL },
        { "show-hidden-files", NULL, NULL, "true", s_show_hidden_files_activate }
    };

    g_action_map_add_action_entries(G_ACTION_MAP(app), action_entries, G_N_ELEMENTS(action_entries), global_state);
}