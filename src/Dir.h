#pragma once

#include <gtk/gtk.h>

void load_current_dir(GtkListBox* const list_box);
void load_parent_dir(GtkListBox* const list_box);
void load_child_dir(GtkListBoxRow* const list_box);