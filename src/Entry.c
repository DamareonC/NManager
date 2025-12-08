#define _XOPEN_SOURCE 500

#include <ftw.h>
#include <sys/stat.h>

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
    return strncmp(((Entry*)(left))->name, ((Entry*)(right))->name, NAME_MAX_LENGTH);
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
        alert_error(add_info->global_state, "File/Folder Already Exists", "%s already exists. Please choose another name.", entry_name);
    }
    else
    {
        if (add_info->is_folder)
        {
            if(!g_file_make_directory(file, NULL, NULL))
            {
                alert_error(add_info->global_state, "Error Creating Folder", "%s could not be created.", entry_name);
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
                alert_error(add_info->global_state, "Error Creating File", "%s could not be created.", entry_name);
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
            alert_error(delete_info->global_state, "Error Deleting File/Folder", "%s could not be deleted.", delete_info->entry_name);
        }
    }

    free(delete_info);
}

bool has_entry(const GlobalState* const global_state, const char* const entry_name)
{
    DIR* const directory = opendir(global_state->current_path);

    if (directory)
    {
        const struct dirent* directory_entry;

        while ((directory_entry = readdir(directory)))
        {
            if (!strncmp(entry_name, directory_entry->d_name, PATH_MAX_LENGTH)) return true;
        }

        closedir(directory);
        return false;
    }
    else return false;
}

void load_entries(const GlobalState* const global_state, DIR* const directory, GArray* const entries, const char* const path)
{
    const struct dirent* directory_entry;
    struct stat entry_stats;
    char full_path[PATH_MAX_LENGTH];

    while ((directory_entry = readdir(directory)))
    {
        if (!strncmp(directory_entry->d_name, ".", PATH_MAX_LENGTH) || !strncmp(directory_entry->d_name, "..", PATH_MAX_LENGTH)) continue;

        set_buffer_path(full_path, path, directory_entry->d_name);
        
        if (stat(full_path, &entry_stats) != 0)
        {
            alert_error(global_state, "Error Reading Entry", "%s could not be read.", directory_entry->d_name);
            continue;
        }

        Entry entry = {
            .is_directory = S_ISDIR(entry_stats.st_mode),
            .is_hidden = directory_entry->d_name[0] == '.'
        };

        strncpy(entry.name, directory_entry->d_name, NAME_MAX_LENGTH);
        g_array_append_val(entries, entry);
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
        alert_error(delete_info->global_state, "Error Trashing File/Folder", "%s could not be sent to trash.", delete_info->entry_name);
    }

    g_object_unref(file);
    free(delete_info);
}