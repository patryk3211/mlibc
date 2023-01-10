#include <mlibc/all-sysdeps.hpp>
#include <string.h>

#include <mieros/syscall.h>

namespace mlibc {
    void sys_exit(int code) {
        syscall1a(SYSCALL_EXIT, code);
    }

    void sys_libc_log(char const* msg) {
        syscall3a(SYSCALL_WRITE, 2, (sysarg_t)msg, strlen(msg));
    }

    void sys_libc_panic() {
        syscall1a(SYSCALL_EXIT, -1);
    }

    int sys_clock_get(int clock, time_t *secs, long *nanos) {
        /// UNIMPLEMENTED
        return -1;
    }

    int sys_tcb_set(void* ptr) {
        /// UNIMPLEMENTED
        return -1;
    }
}
