#include "sys/stat.h"

#include "Entry.h"

static gint s_sort_entries_by_name(gconstpointer l, gconstpointer r)
{
    const Entry* const ent_l = l;
    const Entry* const ent_r = r;

    return strcmp(ent_l->name, ent_r->name);
}

static gint s_sort_entries_by_type(gconstpointer l, gconstpointer r)
{
    const Entry* const ent_l = l;
    const Entry* const ent_r = r;

    if (ent_l->is_directory && !ent_r->is_directory) return -1;
    else if (!ent_l->is_directory && ent_r->is_directory) return 1;
    else return 0;
}

void load_entries(GlobalState* const global_state, DIR* const directory, GArray* const entries, const char* const path)
{
    const struct dirent* directory_entry;
    struct stat entry_stats;
    char full_path[PATH_MAX_SIZE];
    const bool is_root = !strcmp(path, "/");

    while ((directory_entry = readdir(directory)))
    {
        if (!strcmp(directory_entry->d_name, ".") || !strcmp(directory_entry->d_name, "..")) continue;
        
        snprintf(full_path, PATH_MAX_SIZE, is_root ? "%s%s" : "%s/%s", path, directory_entry->d_name);
        
        if (stat(full_path, &entry_stats) != 0)
        {
            alert_error(global_state, full_path);
            continue;
        }

        Entry ent = {
            directory_entry->d_name,
            S_ISDIR(entry_stats.st_mode),
            directory_entry->d_name[0] == '.'
        };

        g_array_append_val(entries, ent);
    }

    g_array_sort(entries, s_sort_entries_by_name);
    g_array_sort(entries, s_sort_entries_by_type);
}