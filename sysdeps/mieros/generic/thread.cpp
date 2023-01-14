#include <mlibc/all-sysdeps.hpp>

#include <mieros/syscall.h>

namespace mlibc {
    int sys_futex_wait(int *pointer, int expected, const struct timespec *time) {
        /// UNIMPLEMENTED
        return -1;
    }

    int sys_futex_wake(int *pointer) {
        /// UNIMPLEMENTED
        return -1;
    }

    int sys_tcb_set(void* ptr) {
        /// UNIMPLEMENTED
        return SYSCALL(SYSCALL_ARCH_PRCTL, ARCH_SET_TCB, (uintptr_t*)&ptr);
    }
}
