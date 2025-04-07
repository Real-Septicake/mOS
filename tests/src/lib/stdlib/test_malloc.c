#include "stdlib/malloc.h"
#include "test_helper.h"

Block *getBlock(void *ptr) {
    return (Block *)(ptr - sizeof(Block));
}

void test_main() {
    init_malloc();
    sbrk(0x100000); // stupid issue :(

    void *ptr1 = malloc(sizeof(Block));
    ASSERT_M(getBlock(ptr1)->len == sizeof(Block),
             "Size of block expected to be %i but was %i instead",
             sizeof(Block), getBlock(ptr1)->len);

    void *ptr2 = malloc(2);
    ASSERT_M(getBlock(ptr2)->len == 2,
             "Size of block expected to be 2 but was %i instead",
             getBlock(ptr2)->len);

    free(ptr2);

    ASSERT_M(getBlock(ptr1)->len == sizeof(Block),
             "Size of block expected to be %i but was %i instead",
             sizeof(Block), getBlock(ptr1)->len);

    free(ptr1);
    ASSERT_M(getBlock(ptr1)->len == (2 * sizeof(Block) + 2),
             "Size of block expected to be %i but was %i instead",
             (2 * sizeof(Block) + 2), getBlock(ptr1)->len);

    char done[] = "test_malloc done";   
    while (!serialWriteReady(COM1))
        ;
    serialWrite(COM1, (uint8_t *)(done), sizeof(done) - 1);
}