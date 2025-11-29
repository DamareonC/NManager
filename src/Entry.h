#pragma once

#include "GlobalState.h"

#define NAME_MAX_LENGTH 256

typedef struct
{
    char name[NAME_MAX_LENGTH];
    const bool is_directory;
    const bool is_hidden;
} Entry;

void load_entries(const GlobalState* const global_state, DIR* const directory, GArray* const entries, const char* const path);