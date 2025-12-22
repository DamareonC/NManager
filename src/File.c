#define _XOPEN_SOURCE 500

#include <ftw.h>

#include "Directory.h"
#include "Error.h"
#include "File.h"
#include "Util.h"

static void s_launch_file(GObject* const object, GAsyncResult* const result, const gpointer data)
{
    gtk_file_launcher_launch_finish(GTK_FILE_LAUNCHER(object), result, NULL);
}

static int s_recursive_delete(const char* const filepath, const struct stat* const stat, const int type, struct FTW* const ftw)
{
    GFile* const file = g_file_new_for_path(filepath);
    const gboolean result = !g_file_delete(file, NULL, NULL);

    g_object_unref(file);
    return result;
}

bool has_file(const GlobalState* const global_state, const char* const filename)
{
    GDir* const dir = g_dir_open(global_state->current_path, 0U, NULL);

    if (dir)
    {
        const char* file;

        while ((file = g_dir_read_name(dir)))
        {
            if (!g_strcmp0(filename, file)) return true;
        }

        g_dir_close(dir);
    }

    return false;
}

void add_file(GtkEntry* const entry, AddFileInfo* const add_file_info)
{
    const char* const filename = gtk_entry_buffer_get_text(gtk_entry_get_buffer(entry));
    char buffer_path[PATH_MAX_LENGTH];
    set_buffer_path(buffer_path, add_file_info->global_state->current_path, filename);
    GFile* const file = g_file_new_for_path(buffer_path);

    if (has_file(add_file_info->global_state, filename))
    {
        alert_error_format(add_file_info->global_state, "File/Folder Already Exists", "%s already exists. Please choose another name.", filename);
    }
    else
    {
        if (add_file_info->is_folder)
        {
            if(!g_file_make_directory(file, NULL, NULL))
            {
                alert_error_format(add_file_info->global_state, "Error Creating Folder", "%s could not be created.", filename);
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
                alert_error_format(add_file_info->global_state, "Error Creating File", "%s could not be created.", filename);
            }
        }
    }

    reload_directory(add_file_info->global_state);

    gtk_window_close(GTK_WINDOW(gtk_widget_get_parent(GTK_WIDGET(entry))));
    g_object_unref(file);
}

void trash_file(const GlobalState* const global_state, DeleteFileInfo* const delete_file_info)
{
    char buffer_path[PATH_MAX_LENGTH];
    set_buffer_path(buffer_path, global_state->current_path, delete_file_info->filename);
    GFile* file = g_file_new_for_path(buffer_path);
    
    if (g_file_trash(file, NULL, NULL))
    {
        reload_directory(delete_file_info->global_state);
    }
    else
    {
        alert_error_format(delete_file_info->global_state, "Error Trashing File/Folder", "%s could not be sent to trash.", delete_file_info->filename);
    }

    g_object_unref(file);
}

void delete_file(GObject* const object, GAsyncResult* const result, const gpointer data)
{
    DeleteFileInfo* const delete_file_info = data;

    if (!gtk_alert_dialog_choose_finish(GTK_ALERT_DIALOG(object), result, NULL))
    {
        char buffer_path[PATH_MAX_LENGTH];
        set_buffer_path(buffer_path, delete_file_info->global_state->current_path, delete_file_info->filename);
        
        if (!nftw(buffer_path, s_recursive_delete, 64, FTW_DEPTH | FTW_PHYS))
        {
            reload_directory(delete_file_info->global_state);
        }
        else
        {
            alert_error_format(delete_file_info->global_state, "Error Deleting File/Folder", "%s could not be deleted.", delete_file_info->filename);
        }
    }
}

void run_file(GlobalState* const global_state, const char* const path)
{
    GFile* const file = g_file_new_for_path(path);
    GtkFileLauncher* const file_launcher = gtk_file_launcher_new(file);

    gtk_file_launcher_launch(file_launcher, global_state->main_window, NULL, s_launch_file, global_state);

    g_object_unref(file_launcher);
    g_object_unref(file);
}