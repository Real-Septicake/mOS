#include "malloc.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include <stdint.h>

#define MAX_CHUNKS 100

int compar(const void *ina, const void *inb);

// The chunk in which allocations will live
Chunk biggest;

void init_malloc() {
    // last entry is repeat of first
    size_t len = *(uint32_t *)MEM_STRUCT_ADDR - 1;

    if (len > MAX_CHUNKS)
        len = MAX_CHUNKS;

    Chunk *chunks = (Chunk *)(MEM_STRUCT_ADDR + sizeof(uint32_t));

    isort(chunks, len, sizeof(Chunk), compar);
    biggest = chunks[0];
    for (int i = 0; i < len; i++) {
        if (chunks[i].type != 1)
            continue;
        if (chunks[i].len_lower > biggest.len_lower)
            biggest = chunks[i];
    }
}

Chunk *getBiggest() {
    return &biggest;
}

// sorts first by type, then by base address
//
// Types:
// - type 1: usable memory
// - type > 1: unusable (some types are reclaimable, but is left unimplemented
// for now)
int compar(const void *ina, const void *inb) {
    Chunk a = *(Chunk *)ina;
    Chunk b = *(Chunk *)inb;

    if (a.type < b.type)
        return -1;
    else if (a.type > b.type)
        return 1;
    else if (a.base_lower < b.base_lower)
        return -1;
    else if (a.base_lower == b.base_lower) {
        ((Chunk *)inb)->type =
            2; // a bit janky for sure: changes duplicate to unusable type
        return -1;
    }
    return 1;
}