#include "malloc.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include <stdint.h>

#define MAX_CHUNKS 100

int compar(const void *ina, const void *inb);

// The chunk in which allocations will live
Chunk biggest;
uint32_t base_addr;
uint32_t top;

typedef struct _Block {
    size_t len;
    char used;
    struct _Block *last,
        *next; // doubly linked list to allow for defragging in both directions
} Block;

Block *head = NULL;

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
    base_addr = biggest.base_lower;
    top = base_addr;
}

Chunk *getBiggest() {
    return &biggest;
}

void *sbrk(intptr_t inc) {
    // no modifications to top until check passes
    if (top + inc - base_addr > biggest.len_lower)
        return (void *)-1;
    uint32_t ret = top;
    top += inc;
    return (void *)ret;
}

void *malloc(size_t size) {
    size_t full_size = size + sizeof(Block);
    if (head) {
        Block *last = NULL;
        Block *curr = head;
        // the loop breaks if it hits the end of the list, the current node can
        // hold a new node on top of the requested memory, or the node fits the
        // requested memory exactly. Any other break conditions run the risk of
        // slowly losing memory
        while (curr && (curr->len < full_size || curr->len != size)) {
            last = curr;
            curr = curr->next;
        }

        if (curr) {
            if (curr->len == size) {
                curr->used = 1;
                return curr + 1; // return the address right after the block
            } else {
                void *ptr = curr + 1;
                curr->len -= full_size;
                ptr += curr->len;
                Block *header = (Block *)ptr;
                header->len = size;
                header->used = 1;

                header->next = curr->next;
                if (curr->next)
                    curr->next->last = header;
                header->last = curr;
                curr->next = header;
            }
        } else {
            void *ptr = sbrk(full_size);
            if (ptr == (void*)-1) {
                return NULL;
            }
            Block *header = (Block *)ptr;
            header->last = last;
            last->next = header;
            header->next = NULL;
            header->used = 1;
            header->len = size;
            head = header;
            return ptr + sizeof(Block);
        }
    } else {
        void *ptr = sbrk(full_size);
        if (ptr == (void*)-1) {
            return NULL;
        }
        Block *header = (Block *)ptr;
        header->last = NULL;
        header->next = NULL;
        header->used = 1;
        header->len = size;
        head = header;
        return ptr + sizeof(Block);
    }
    return NULL;
}

void free(void *ptr) {
    Block *header = ptr - sizeof(Block);
    // probably needs a better check, but it's nearly midnight
    if (header->used != 1)
        return;

    header->used = 0;
    if (header->next && !header->next->used) {
        header->len += header->next->len + sizeof(Block);
        header->next = header->next->next;
    }
    if (header->last && !header->last->used) {
        header->last->len += header->len + sizeof(Block);
        header->last->next = header->next;
    }
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