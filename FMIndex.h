#include <sys/mman.h> //for mremap()

void pim(void *arg1, void *arg2, void *arg3, int flag)
{
    mremap(arg1, (size_t)arg2, (size_t)arg3, flag);
}