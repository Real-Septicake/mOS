#ifndef CPU_H
#define CPU_H

    //long rax = 0, rbx = 0, rcx = 0, rdx = 0, rsi = 0, rdi = 0, rsp = 0, rbp = 0;

#define DUMP() do{ \
    short rip = 0;\
    __asm__ volatile ("mov %0, %%ax" : "=r"(rip)); \
    char buff[256];\
    int len = snprintf(buff, sizeof(buff), "rip=%i", rip);\
    while (!serialWriteReady(COM1))\
        ;\
    serialWrite(COM1, (uint8_t *)buff, len);\
}while(0)

#endif
