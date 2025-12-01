#include "sys/stat.h"

#include "Entry.h"
#include "Error.h"
#include "Util.h"

static gint s_sort_entries_by_name(const gconstpointer left, const gconstpointer right)
{
    const Entry* const left_entry = left;
    const Entry* const right_entry = right;

    return strncmp(left_entry->name, right_entry->name, NAME_MAX_LENGTH);
}

static gint s_sort_entries_by_type(const gconstpointer left, const gconstpointer right)
{
    const Entry* const left_entry = left;
    const Entry* const right_entry = right;

    if (left_entry->is_directory && !right_entry->is_directory) return -1;
    else if (!left_entry->is_directory && right_entry->is_directory) return 1;
    else return 0;
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