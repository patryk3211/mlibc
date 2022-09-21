#include <mlibc/internal-sysdeps.hpp>

#include <mieros/syscall.h>

int mlibc::sys_anon_allocate(size_t size, void **pointer) {
    return sys_vm_map(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, 0, 0, pointer);
}

int mlibc::sys_anon_free(void *pointer, size_t size) {
    return sys_vm_unmap(pointer, size);
}

int mlibc::sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window) {
    ssysarg_t ret = syscall(SYSCALL_MMAP, hint, size, prot, flags, fd, offset);

    if(ret < 0)
        return -ret;

    *window = (void*)ret;
    return 0;
}

int mlibc::sys_vm_unmap(void *pointer, size_t size) {
    return -syscall(SYSCALL_MUNMAP, pointer, size);
}
