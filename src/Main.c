#include "Main.h"

int main(int argc, char* argv[])
{
    GtkApplication* const app = gtk_application_new("app.damareonc.nmanager", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(init_app), NULL);
    const int status = g_application_run(G_APPLICATION(app), argc, argv);
    
    g_object_unref(app);
    return status;
}