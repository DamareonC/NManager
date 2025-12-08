#pragma once

#include "GlobalState.h"

#define NAME_MAX_LENGTH 256

typedef struct
{
    char name[NAME_MAX_LENGTH];
    const bool is_directory;
    const bool is_hidden;
} Entry;

typedef struct
{
    GlobalState* global_state;
    bool is_folder;
} AddInfo;

typedef struct 
{
    GlobalState* global_state;
    const char* entry_name;
} DeleteInfo;

void add_entry(GtkEntry* const entry, AddInfo* const add_info);
void delete_entry(GObject* const object, GAsyncResult* const result, const gpointer data);
bool has_entry(const GlobalState* const global_state, const char* const entry_name);
void load_entries(const GlobalState* const global_state, DIR* const directory, GArray* const entries, const char* const path);
void trash_entry(const GlobalState* const global_state, DeleteInfo* const delete_info);