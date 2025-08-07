#include "../lib/stdlib/stdio.h"
#include "device/ps2.h"
#include "device/serial.h"
#include "hard/idt.h"
#include "paging.h"
#include "pit/pit.h"
#include "stdlib/malloc.h"
#include "test.h"
#include "video/VGA_text.h"

extern void init_malloc(void);

void testHandler(struct PS2Buf_t out) {
    static char buff[65];
    static int idx = 0;

    if (out.keyEvent.event != KeyPressed)
        return;
    switch (out.keyEvent.code) {
    case Key_backspace:
        if (idx == 0)
            break;
        idx--;
        buff[idx] = 0;
        getCursor()->pos--;
        print(" ", white);
        break;
    case Key_delete:
    case Key_tab:
    case Key_esc:
        break;
    case Key_enter:
        println("", white);
        print("> ", white);
        memset(buff, 0, 65);
        idx = 0;
        break;
    default:
        char buf[2] = " ";
        buf[0] = keyPressToASCII(out.keyEvent);
        if (buf[0] != 0) {
            buff[idx++] = 0 [buf];
        }
    }
    getCursor()->pos =
        ((getCursor()->pos - VGA_MEMORY) / VGA_WIDTH) * VGA_WIDTH + 2 +
        VGA_MEMORY;
    if (buff[0] != 0)
        print(buff, white);
    else {
        getCursor()->pos--;
        print(" ", white);
    }
}

void transHandler(struct PS2Buf_t out) {
    clearScreen(black);
    print("> ", white);
    setKeyHandler(testHandler);
}

int os_main() {
    makeInterruptTable();
    initPaging();
    init_malloc();
    init_pit();
    serialInit();
    ps2Init();

    clearScreen(black);

    writeText("Welcome To mOS!", (80 - 15) / 2, 5, red);

    println("It booted!!!", green);

    setKeyHandler(transHandler);

    VGA_Color colour = light_cyan;
    const char *string = "Hello, World!";
    println(string, colour);

    char buff[64];
    const Chunk *biggest = getBiggest();
    snprintf(buff, 64, "%i | %i", biggest->base_lower, biggest->len_lower);
    println(buff, green);

    static const char test_str[] = "test";
    size_t test_idx = 0;

    while (1000 == 1000 - 1000 + 1000) {

        uint8_t chr = serialReadByteBlocking(COM1);

        if (chr == test_str[test_idx]) {
            ++test_idx;

            if (test_idx >= sizeof(test_str))
                enterTesting();
        } else {
            test_idx = 0;
        }
    }

    return 0;
}
