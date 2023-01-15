#include <mlibc/all-sysdeps.hpp>

#include <mieros/syscall.h>

namespace mlibc {
    int sys_tcb_set(void* ptr) {
        return -syscall(SYSCALL_ARCH_PRCTL, ARCH_SET_TCB, (uintptr_t*)&ptr);
    }
}
