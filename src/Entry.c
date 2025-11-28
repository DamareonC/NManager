#include "sys/stat.h"

#include "Entry.h"
#include "GlobalState.h"

static gint s_sort_entries_by_name(const gconstpointer left, const gconstpointer right)
{
    const Entry* const left_entry = left;
    const Entry* const right_entry = right;

    return strncmp(left_entry->name, right_entry->name, PATH_MAX_SIZE);
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
    char full_path[PATH_MAX_SIZE];
    const bool is_root = !strncmp(path, "/", PATH_MAX_SIZE);

    while ((directory_entry = readdir(directory)))
    {
        if (!strncmp(directory_entry->d_name, ".", PATH_MAX_SIZE) || !strncmp(directory_entry->d_name, "..", PATH_MAX_SIZE)) continue;
        
        snprintf(full_path, PATH_MAX_SIZE, is_root ? "%s%s" : "%s/%s", path, directory_entry->d_name);
        
        if (stat(full_path, &entry_stats) != 0)
        {
            alert_error(global_state, full_path);
            continue;
        }

        Entry ent = {
            .name = directory_entry->d_name,
            .is_directory = S_ISDIR(entry_stats.st_mode),
            .is_hidden = directory_entry->d_name[0] == '.'
        };

        g_array_append_val(entries, ent);
    }

    g_array_sort(entries, s_sort_entries_by_name);
    g_array_sort(entries, s_sort_entries_by_type);
}