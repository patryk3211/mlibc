#include <mlibc/all-sysdeps.hpp>
#include <string.h>

#include <mieros/syscall.h>

namespace mlibc {
    void sys_exit(int code) {
        SYSCALL(SYSCALL_EXIT, code);
        while(true);
    }

    void sys_libc_log(char const* msg) {
        SYSCALL(SYSCALL_WRITE, 2, msg, strlen(msg));
    }

    void sys_libc_panic() {
        SYSCALL(SYSCALL_EXIT, -1);
        while(true);
    }

    int sys_clock_get(int clock, time_t *secs, long *nanos) {
        /// UNIMPLEMENTED
        return -1;
    }
}
