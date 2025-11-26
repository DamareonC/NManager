#pragma once

#include "GlobalState.h"

typedef struct
{
    const char* const name;
    bool is_directory;
    bool is_hidden;
} Entry;

void load_entries(GlobalState* const global_state, DIR* const directory, GArray* const entries, const char* const path);