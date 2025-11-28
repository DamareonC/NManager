#pragma once

#include "GlobalState.h"

typedef struct
{
    const char* const name;
    const bool is_directory;
    const bool is_hidden;
} Entry;

void load_entries(const GlobalState* const global_state, DIR* const directory, GArray* const entries, const char* const path);