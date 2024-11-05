/**
 *
 * This file describes a non-freeing memory arena.
 *
 * These arenas hold only one type of data within them.
 * An arena is initialized with the type of data it expects
 * to hold and a hint as to how many allocations may be
 * needed.
 *
 * When the number of allocations is met, the arena will
 * allocate another space of equal size to continue allocating
 * from.
 *
 * Memory is freed only from destroying the arena.
 *
 */


#include <stdlib.h>

typedef struct {

    // The start of this arena's memory.
    void *start;

    // The current end of this arena's memory.
    // This is where the next allocation will be placed.
    void *end;

    // How many more allocations fit in this arena.
    int allocations_left;

} _Arena;

typedef struct {

    // The size of individual allocations within the arena
    size_t allocation_size;

    // The size of each arena.
    size_t arena_size;

    // How many allocations fit in each arena.
    int allocations_per_arena;

    // The number of arenas allocated.
    int number_of_arenas;

    // The array of actual memory arenas.
    _Arena *arenas;

    struct {

        int number_of_allocations;

    } stats;

} Arena;

Arena *Arena_create(size_t allocation_size, int allocations_hint);
void *Arena_delete(Arena *arena);

/**
 * Allocates a single new object in this arena.
 */
void *Arena_allocate(Arena *arena);

/**
 * Prints the statistics of this arena.
 */
void Arena_print(Arena *arena);
