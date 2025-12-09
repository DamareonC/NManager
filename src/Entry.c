#define _XOPEN_SOURCE 500

#include <ftw.h>
#include <strings.h>

#include "Directory.h"
#include "Entry.h"
#include "Error.h"
#include "Util.h"

static int s_recursive_remove(const char* const filepath, const struct stat* const stat, const int type, struct FTW* const ftw)
{
    GFile* const file = g_file_new_for_path(filepath);
    const gboolean result = !g_file_delete(file, NULL, NULL);

    g_object_unref(file);
    return result;
}

static gint s_sort_entries_by_name(const gconstpointer left, const gconstpointer right)
{
    return strncasecmp(((Entry*)(left))->name, ((Entry*)(right))->name, NAME_MAX_LENGTH);
}

static gint s_sort_entries_by_type(const gconstpointer left, const gconstpointer right)
{
    const Entry* const left_entry = left;
    const Entry* const right_entry = right;

    if (left_entry->is_directory && !right_entry->is_directory) return -1;
    else if (!left_entry->is_directory && right_entry->is_directory) return 1;
    else return 0;
}

void add_entry(GtkEntry* const entry, AddInfo* const add_info)
{
    const char* const entry_name = gtk_entry_buffer_get_text(gtk_entry_get_buffer(entry));
    char buffer_path[PATH_MAX_LENGTH];
    set_buffer_path(buffer_path, add_info->global_state->current_path, entry_name);
    GFile* const file = g_file_new_for_path(buffer_path);

    if (has_entry(add_info->global_state, entry_name))
    {
        alert_error_format(add_info->global_state, "File/Folder Already Exists", "%s already exists. Please choose another name.", entry_name);
    }
    else
    {
        if (add_info->is_folder)
        {
            if(!g_file_make_directory(file, NULL, NULL))
            {
                alert_error_format(add_info->global_state, "Error Creating Folder", "%s could not be created.", entry_name);
            }
        }
        else
        {
             GFileOutputStream* const file_output_stream = g_file_create(file, G_FILE_CREATE_NONE, NULL, NULL);

            if (file_output_stream)
            {
                g_object_unref(file_output_stream);
            }
            else
            {
                alert_error_format(add_info->global_state, "Error Creating File", "%s could not be created.", entry_name);
            }
        }
    }

    gtk_window_close(GTK_WINDOW(gtk_widget_get_parent(GTK_WIDGET(entry))));
    reload_directory(add_info->global_state);

    g_object_unref(file);
    free(add_info);
}

void delete_entry(GObject* const object, GAsyncResult* const result, const gpointer data)
{
    DeleteInfo* const delete_info = data;

    if (!gtk_alert_dialog_choose_finish(GTK_ALERT_DIALOG(object), result, NULL))
    {
        char buffer_path[PATH_MAX_LENGTH];
        set_buffer_path(buffer_path, delete_info->global_state->current_path, delete_info->entry_name);
        
        if (!nftw(buffer_path, s_recursive_remove, 64, FTW_DEPTH | FTW_PHYS))
        {
            reload_directory(delete_info->global_state);
        }
        else
        {
            alert_error_format(delete_info->global_state, "Error Deleting File/Folder", "%s could not be deleted.", delete_info->entry_name);
        }
    }

    free(delete_info);
}

bool has_entry(const GlobalState* const global_state, const char* const entry_name)
{
    GDir* const dir = g_dir_open(global_state->current_path, 0U, NULL);

    if (dir)
    {
        const char* entry;

        while ((entry = g_dir_read_name(dir)))
        {
            if (!g_strcmp0(entry_name, entry)) return true;
        }

        g_dir_close(dir);
    }

    return false;
}

void load_entries(const GlobalState* const global_state, GFileEnumerator* const file_enumerator, GArray* const entries, const char* const path)
{
    GFileInfo* file_info;
    GError* error = NULL;
    gboolean result;
    const char* filename;

    while (true)
    {
        result = g_file_enumerator_iterate(file_enumerator, &file_info, NULL, NULL, &error);

        if (!file_info) break;

        filename = g_file_info_get_name(file_info);

        if (!result)
        {
            alert_error_format(global_state, "Error Reading File/Folder", "%s could not be read.", filename);
            g_error_free(error);
        }
        else
        {
            Entry entry = {
                .is_directory = g_file_info_get_file_type(file_info) == G_FILE_TYPE_DIRECTORY,
                .is_hidden = g_file_info_get_is_hidden(file_info)
            };

            g_strlcpy(entry.name, filename, NAME_MAX_LENGTH);
            g_array_append_val(entries, entry);
        }
    }

    g_array_sort(entries, s_sort_entries_by_name);
    g_array_sort(entries, s_sort_entries_by_type);
}

void trash_entry(const GlobalState* const global_state, DeleteInfo* const delete_info)
{
    char buffer_path[PATH_MAX_LENGTH];
    set_buffer_path(buffer_path, global_state->current_path, delete_info->entry_name);
    GFile* file = g_file_new_for_path(buffer_path);
    
    if (g_file_trash(file, NULL, NULL))
    {
        reload_directory(delete_info->global_state);
    }
    else
    {
        alert_error_format(delete_info->global_state, "Error Trashing File/Folder", "%s could not be sent to trash.", delete_info->entry_name);
    }

    g_object_unref(file);
    free(delete_info);
}