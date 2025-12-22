#pragma once

#include "GlobalState.h"

typedef struct
{
    GlobalState* global_state;
    bool is_folder;
} AddFileInfo;

typedef struct 
{
    GlobalState* global_state;
    const char* filename;
} DeleteFileInfo;

bool has_file(const GlobalState* const global_state, const char* const filename);
void add_file(GtkEntry* const entry, AddFileInfo* const add_info);
void trash_file(const GlobalState* const global_state, DeleteFileInfo* const delete_info);
void delete_file(GObject* const object, GAsyncResult* const result, const gpointer data);
void run_file(GlobalState* const global_state, const char* const path);