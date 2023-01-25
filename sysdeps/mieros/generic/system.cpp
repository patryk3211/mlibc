#include <mlibc/all-sysdeps.hpp>
#include <string.h>

#include <mieros/syscall.h>

namespace mlibc {
    void sys_exit(int code) {
        syscall(SYSCALL_EXIT, code);
        while(true);
    }

    void sys_libc_log(char const* msg) {
        syscall(SYSCALL_WRITE, 2, msg, strlen(msg));
    }

    void sys_libc_panic() {
        syscall(SYSCALL_EXIT, -1);
        while(true);
    }

    int sys_clock_get(int clock, time_t *secs, long *nanos) {
        /// UNIMPLEMENTED
        return -1;
    }

    int sys_futex_wait(int *pointer, int expected, const struct timespec *time) {
        /// UNIMPLEMENTED
        return -1;
    }

    int sys_futex_wake(int *pointer) {
        /// UNIMPLEMENTED
        return -1;
    }

    int sys_fork(pid_t *child) {
        int status = syscall(SYSCALL_FORK);
        if(status >= 0) {
            *child = status;
            return 0;
        } else return -status;
    }

    int sys_execve(const char *path, char *const argv[], char *const envp[]) {
        return -syscall(SYSCALL_EXECVE, path, argv, envp);
    }
}