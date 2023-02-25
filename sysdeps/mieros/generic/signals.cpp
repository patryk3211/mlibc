#include <mlibc/posix-sysdeps.hpp>
#include <mlibc/all-sysdeps.hpp>

#include <mieros/syscall.h>

extern "C" void __mlibc_signal_restore(void);

namespace mlibc {
    int sys_sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict oldSet) {
        return -syscall(SYS_sigmask, how, set, oldSet);
    }

    int sys_sigaction(int signum, const struct sigaction *__restrict action, struct sigaction *__restrict oldAction) {
        if(action) {
            struct sigaction actionCopy = *action;
            actionCopy.sa_restorer = &__mlibc_signal_restore;
            return -syscall(SYS_sigaction, signum, &actionCopy, oldAction);
        } else {
            return -syscall(SYS_sigaction, signum, 0, oldAction);
        }
    }

    int sys_kill(int pid, int signum) {
        return -syscall(SYS_kill, pid, signum);
    }

	// BUG: These symbols are only linked in if there is a global and used symbol here as well.
	// This only happens for static builds.
}

