#include "Directory.h"
#include "Error.h"
#include "File.h"
#include "Util.h"

static int s_sort_by_name(GtkListBoxRow* const left, GtkListBoxRow* const right, const gpointer data)
{
    return strncasecmp(gtk_label_get_text(GTK_LABEL(gtk_list_box_row_get_child(left))), gtk_label_get_text(GTK_LABEL(gtk_list_box_row_get_child(right))), PATH_MAX_LENGTH);
}

static int s_sort_by_type(GtkListBoxRow* const left, GtkListBoxRow* const right, const gpointer data)
{
    const bool left_is_directory = gtk_widget_get_name(GTK_WIDGET(left))[0U] == 'd';
    const bool right_is_directory = gtk_widget_get_name(GTK_WIDGET(right))[0U] == 'd';

    if (left_is_directory && !right_is_directory) return -1;
    else if (!left_is_directory && right_is_directory) return 1;
    else return 0;
}

bool load_directory(GlobalState* const global_state, const char* const path)
{
    GFile* file = g_file_new_for_path(path);
    GError* error = NULL;
    GFileEnumerator* file_enumerator = g_file_enumerate_children(file, "standard::*", G_FILE_QUERY_INFO_NONE, NULL, &error);

    if (file_enumerator)
    {
        gtk_list_box_remove_all(global_state->file_list_box);

        GFileInfo* file_info;
        gboolean result;
        const char* filename;
        bool is_hidden;

        while (true)
        {
            result = g_file_enumerator_iterate(file_enumerator, &file_info, NULL, NULL, NULL);

            if (!file_info) break;

            filename = g_file_info_get_name(file_info);
            is_hidden = g_file_info_get_is_hidden(file_info);

            if (!result)
            {
                alert_error_format(global_state, "Error Reading File/Folder", "%s could not be read.", filename);
            }
            else
            {
                GtkWidget* const list_box_row = gtk_list_box_row_new();
                GtkWidget* const label = gtk_label_new(filename);

                if (is_hidden)
                {
                    gtk_widget_set_name(list_box_row, g_file_info_get_file_type(file_info) == G_FILE_TYPE_DIRECTORY ? "dh" : "fh");
                }
                else
                {
                    gtk_widget_set_name(list_box_row, g_file_info_get_file_type(file_info) == G_FILE_TYPE_DIRECTORY ? "dv" : "fv");
                }

                gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
                gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(list_box_row), label);
                gtk_list_box_append(global_state->file_list_box, list_box_row);
            }
        }

        gtk_list_box_set_sort_func(global_state->file_list_box, s_sort_by_name, NULL, NULL);
        gtk_list_box_set_sort_func(global_state->file_list_box, s_sort_by_type, NULL, NULL);

        g_object_unref(file_enumerator);
        return true;
    }
    else if (error != NULL)
    {
        switch (error->code)
        {
            case G_IO_ERROR_NOT_DIRECTORY:
                run_file(global_state, path);
                break;
            case G_IO_ERROR_NOT_FOUND:
                alert_error_format(global_state, "File/Folder Not Found", "%s could not be found.", path);
                break;
            default:
                alert_error_format(global_state, "Error Opening File/Folder", "%s could not be opened.", path);
        }
        
        g_error_free(error);
    }

    g_object_unref(file);
    return false;
}

void reload_directory(GlobalState* const global_state)
{
    load_directory(global_state, global_state->current_path);
}

void load_directory_and_set_global_state(GlobalState* const global_state, const char* const path)
{
    if (load_directory(global_state, path))
    {
        set_global_state(global_state, path);
    }
}

void load_child(GlobalState* const global_state, const char* const filename)
{
    char buffer_path[PATH_MAX_LENGTH];
    set_buffer_path(buffer_path, global_state->current_path, filename);

    load_directory_and_set_global_state(global_state, buffer_path);
}

void load_parent_directory(GlobalState* const global_state)
{
    GFile* const current_directory = g_file_new_for_path(global_state->current_path);
    GFile* const parent_directory = g_file_get_parent(current_directory);
    
    if (parent_directory)
    {
        load_directory_and_set_global_state(global_state, g_file_get_path(parent_directory));
        g_object_unref(parent_directory);
    }
    
    g_object_unref(current_directory);
}