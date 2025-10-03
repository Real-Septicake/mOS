#ifndef CPU_H
#define CPU_H

    //
#define DUMP() do{ \
    long eax = 0, ebx = 0, ecx = 0, edx = 0, esi = 0, edi = 0, esp = 0, ebp = 0;\
    long eip = 0;\
    (void)(eax+ebx+ecx+edx+esi+edi+esp+ebp+eip);\
    __asm__ volatile ("mov %%eax, %0\nmov %%ebx, %1\n" \
            "mov %%ecx, %2\nmov %%edx, %3\nmov %%esi, %4" : \
            "=r"(eax), "=r"(ebx), "=r"(ecx), "=r"(edx), "=r"(esi)); \
    char buff[256];\
    (void)snprintf(buff, sizeof(buff), "eax=%i ebx=%i ecx=%i edx=%i esi=%i", eax, ebx, ecx, edx, esi);\
    writeText(buff, 0, 23, white);\
    __asm__ volatile ("mov %%edi, %0\nmov %%esp, %1\nmov %%ebp, %2" \
            : "=r"(edi), "=r"(esp), "=r"(ebp)); \
    (void)snprintf(buff, sizeof(buff), "edi=%i esp=%i epb=%i", edi, esp, ebp);\
    writeText(buff, 0, 24, white);\
}while(0)

#endif
