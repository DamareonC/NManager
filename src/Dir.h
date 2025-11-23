#pragma once

#include <gtk/gtk.h>

bool load_dir(GtkListBox* const list_box, const char* const dir);
void load_parent(GtkListBox* const list_box);
void load_child_activate(GtkListBoxRow* const list_box);
void load_child_clicked(GtkButton* const button);
void set_globals(const char* const dir);