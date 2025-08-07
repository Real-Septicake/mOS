#include "stdlib/malloc.h"
#include "test_helper.h"

typedef struct _Block {
    size_t len;
    char used;
    struct _Block *last,
        *next; // doubly linked list to allow for defragging in both directions
} Block;

Block *getBlock(void *ptr) {
    return (Block *)(ptr - sizeof(Block));
}

void test_block_size(Block *ptr, size_t expected) {
    ASSERT_M(ptr->len == expected,
             "Size of block expected to be %i but was %i instead", expected,
             getBlock(ptr)->len);
}

void test_main() {
    void *ptr1 = malloc(sizeof(Block));
    test_block_size(getBlock(ptr1), sizeof(Block));

    void *ptr2 = malloc(2);
    test_block_size(getBlock(ptr2), 2);

    free(ptr2);
    test_block_size(
        getBlock(ptr1),
        sizeof(Block)); // check that block does not get defragged unnecessarily

    free(ptr1);
    test_block_size(
        getBlock(ptr1),
        (2 * sizeof(Block) + 2)); // check that block is properly defragged

    ptr2 = malloc(4);
    test_block_size(
        getBlock(ptr1),
        (sizeof(Block) - 2)); // check that block is broken down for space

    for (int i = 0; i < 200; i++) { // make sure it doesn't leak unexpectedly
        ptr2 = malloc(4);
        free(ptr2);
    }

    test_block_size(getBlock(ptr1), (2 * sizeof(Block) + 2));
    ASSERT_M(getBlock(ptr1)->next == NULL,
             "Head should not have ended with a `next` value");

    ptr1 = calloc(14, sizeof(Block));
    test_block_size(getBlock(ptr1), 14 * sizeof(Block));

    ptr1 = realloc(ptr1, 10 * sizeof(Block));
    test_block_size(getBlock(ptr1), 14 * sizeof(Block));

    ptr1 = realloc(ptr1, 12 * sizeof(Block));
    test_block_size(getBlock(ptr1), 14 * sizeof(Block));

    ptr1 = calloc(14, sizeof(Block));
    test_block_size(getBlock(ptr1), 14 * sizeof(Block));

    char done[] = "test_malloc done";
    while (!serialWriteReady(COM1))
        ;
    serialWrite(COM1, (uint8_t *)(done), sizeof(done) - 1);
}