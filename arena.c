
#include "arena.h"

#include <stdio.h>

/**
 * Allocates a single internal arena.
 */
void _Arena_allocate_internal_arena(Arena *arena) {

    int new_arena_index = arena->number_of_arenas;
    arena->number_of_arenas++;

    // Allocate enough space for another arena.
    arena->arenas = realloc(arena->arenas, sizeof(_Arena) * arena->number_of_arenas);

    // Initialize our new arena.
    _Arena *new_arena = arena->arenas + new_arena_index;
    new_arena->start = malloc(arena->arena_size);
    new_arena->end = new_arena->start;
    new_arena->allocations_left = arena->allocations_per_arena;

}

Arena *Arena_create(size_t allocation_size, int allocations_hint) {

    Arena *arena = malloc(sizeof(Arena));

    // Figure out our allocation sizes.
    arena->allocation_size = allocation_size;
    arena->arena_size = allocation_size * allocations_hint;
    arena->allocations_per_arena = allocations_hint;

    // Zero out our stats.
    arena->stats.number_of_allocations = 0;

    // Allocate a single arena.
    arena->number_of_arenas = 0;
    arena->arenas = NULL;
    _Arena_allocate_internal_arena(arena);

    return arena;

}

void *Arena_delete(Arena *arena) {

    for (int i = 0; i < arena->number_of_arenas; i++) {
        free(arena->arenas[i].start);
    }

    free(arena->arenas);
    free(arena);

}

void *Arena_allocate(Arena *arena) {

    int allocate_from_arena_index = arena->number_of_arenas - 1;
    _Arena *allocate_from = arena->arenas + allocate_from_arena_index;

    // Check if we need to create a new arena.
    if (allocate_from->allocations_left == 0) {

        _Arena_allocate_internal_arena(arena);

        // Point to the new arena instead.
        allocate_from_arena_index++;
        allocate_from = arena->arenas + allocate_from_arena_index;

    }

    void *allocation = allocate_from->end;

    // Update the arena's pointers.
    allocate_from->end += arena->allocation_size;
    allocate_from->allocations_left--;

    arena->stats.number_of_allocations++;
    return allocation;

}

void Arena_print(Arena *arena) {

    printf("Number of allocations: %d, number of resizes: %d\n",
        arena->stats.number_of_allocations,
        arena->number_of_arenas
    );

}