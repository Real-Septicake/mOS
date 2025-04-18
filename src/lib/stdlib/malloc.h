#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>
#include <stdint.h>

// RAM addr of structure describing useable memory
#define MEM_STRUCT_ADDR 0x8000

// for the purposes of our os, which is 32-bit, we
// will rely soley on the lower 32-bits
typedef struct {
    uint32_t base_lower;
    uint32_t base_upper;
    uint32_t len_lower;
    uint32_t len_upper;
    uint32_t type;
    uint32_t attrs; // ACPI v3.0 Extended Attributes bitfield
} Chunk;

void *sbrk(intptr_t);
void init_malloc(void);
void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void *calloc(size_t nmem, size_t size);
void free(void *ptr);

const Chunk *getBiggest();

#endif