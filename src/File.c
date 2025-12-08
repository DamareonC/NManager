#include "File.h"

static void s_launch_file(GObject* const object, GAsyncResult* const result, const gpointer data)
{
    const GlobalState* const global_state = data;
    GtkFileLauncher* const file_launcher = GTK_FILE_LAUNCHER(object);

    gtk_file_launcher_launch_finish(file_launcher, result, NULL);
}

void run_file(GlobalState* const global_state, const char* const path)
{
    GFile* const file = g_file_new_for_path(path);
    GtkFileLauncher* const file_launcher = gtk_file_launcher_new(file);

    gtk_file_launcher_launch(file_launcher, global_state->main_window, NULL, s_launch_file, global_state);

    g_object_unref(file);
    g_object_unref(file_launcher);
}