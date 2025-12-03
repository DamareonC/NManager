#include "sys/stat.h"

#include "Directory.h"
#include "Entry.h"
#include "Error.h"
#include "Util.h"

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

void add_entry(GtkEntry* const entry, AddInfo* add_info)
{
    GtkEntryBuffer* const entry_buffer = gtk_entry_get_buffer(entry);
    const char* const entry_name = gtk_entry_buffer_get_text(entry_buffer);
    char buffer_path[PATH_MAX_LENGTH];

    set_buffer_path(buffer_path, add_info->global_state->current_path, gtk_entry_buffer_get_text(entry_buffer));

    if (has_entry(add_info->global_state, entry_name))
    {
        alert_error(add_info->global_state, "File/Folder Already Exists", "%s already exists. Please choose another name.", buffer_path);
    }
    else
    {
        if (add_info->add_folder)
        {
            if (mkdir(buffer_path, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH))
            {
                alert_error(add_info->global_state, "Error Creating Folder", "%s could not be created.", entry_name);
            }
        }
        else
        {
            FILE* file = fopen(buffer_path, "a");

            if (file)
            {
                fclose(file);
                file = NULL;
            }
            else
            {
                alert_error(add_info->global_state, "Error Creating File", "%s could not be created.", entry_name);
            }
        }
    }

    gtk_window_close(GTK_WINDOW(gtk_widget_get_parent(GTK_WIDGET(entry))));
    reload_directory(add_info->global_state);

    free(add_info);
    add_info = NULL;
}

void delete_entry(GObject* const warning_alert_dialog, GAsyncResult* const result, const gpointer data)
{
    DeleteInfo* entry_data = data;

    if (!gtk_alert_dialog_choose_finish(GTK_ALERT_DIALOG(warning_alert_dialog), result, NULL))
    {
        char buffer_path[PATH_MAX_LENGTH];
        char command[PATH_MAX_LENGTH];

        set_buffer_path(buffer_path, entry_data->global_state->current_path, entry_data->entry_name);
        snprintf(command, PATH_MAX_LENGTH, "rm -rf %s", buffer_path);
        
        if (!system(command))
        {
            reload_directory(entry_data->global_state);
        }
        else
        {
            alert_error(entry_data->global_state, "Error Deleting File/Folder", "%s could not be deleted.", buffer_path);
        }
    }

    free(entry_data);
    entry_data = NULL;
}

bool has_entry(const GlobalState* const global_state, const char* const entry_name)
{
    DIR* directory = opendir(global_state->current_path);

    if (directory)
    {
        const struct dirent* directory_entry;

        while ((directory_entry = readdir(directory)))
        {
            if (!strncmp(entry_name, directory_entry->d_name, PATH_MAX_LENGTH)) return true;
        }

        closedir(directory);
        directory = NULL;

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
            alert_error(global_state, "Error Reading Entry", "%s could not be read.", full_path);
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