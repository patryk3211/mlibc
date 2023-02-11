#include <mlibc/all-sysdeps.hpp>

#include <mieros/syscall.h>
#include <asm/procid.h>

namespace mlibc {
    int sys_tcb_set(void* ptr) {
        return -syscall(SYS_arch_prctl, ARCH_SET_TCB, (uintptr_t*)&ptr);
    }

    int sys_getpid() {
        return syscall(SYS_getid, PROCID_PID);
    }

    int sys_gettid() {
        return syscall(SYS_getid, PROCID_TID);
    }
}
