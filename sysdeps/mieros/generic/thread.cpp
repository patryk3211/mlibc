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

    pid_t sys_getppid() {
        return syscall(SYS_getid, PROCID_PARENT);
    }

    int sys_gettid() {
        return syscall(SYS_getid, PROCID_TID);
    }

    uid_t sys_getuid() {
        return syscall(SYS_getid, PROCID_UID);
    }

    uid_t sys_geteuid() {
        return syscall(SYS_getid, PROCID_EUID);
    }

    gid_t sys_getgid() {
        return syscall(SYS_getid, PROCID_GID);
    }

    gid_t sys_getegid() {
        return syscall(SYS_getid, PROCID_EGID);
    }

    int sys_waitpid(pid_t pid, int* status, int flags, struct rusage*, pid_t* ret_pid) {
        ssysarg_t result = syscall(SYS_waitpid, pid, status, flags);

        if(result < 0)
            return -result;

        *ret_pid = result;
        return 0;
    }
}
