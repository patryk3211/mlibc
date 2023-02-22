#include <mlibc/all-sysdeps.hpp>

#include <mieros/syscall.h>

namespace mlibc {
int sys_sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict oldSet) {
    return -syscall(SYS_sigmask, how, set, oldSet);
}

int sys_sigaction(int signum, const struct sigaction *__restrict action, struct sigaction *__restrict oldAction) {
    return -syscall(SYS_sigaction, signum, action, oldAction);
}
}

