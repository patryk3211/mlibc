#ifndef _SYSCALL_H
#define _SYSCALL_H

typedef long long ssysarg_t;
typedef unsigned long long sysarg_t;

#define SYS_exit    1
#define SYS_openat  2
#define SYS_close   3
#define SYS_read    4
#define SYS_write   5
#define SYS_fork    6
#define SYS_seek    7
#define SYS_mmap    8
#define SYS_munmap  9
#define SYS_execve  10

#define SYS_arch_prctl 11
#define ARCH_SET_TCB 0
#define ARCH_GET_TCB 1

#define SYS_init_module 12

#define SYS_symlinkat 13
#define SYS_waitpid 14
#define SYS_ioctl 15
#define SYS_getid 16
#define SYS_dup   17
#define SYS_mount  18
#define SYS_umount 19
#define SYS_pipe   20
#define SYS_sigmask 21
#define SYS_sigaction 22
#define SYS_getcwd 23
#define SYS_kill 24

#if defined(__cplusplus)
extern "C" {
#endif

static inline sysarg_t syscall0a(sysarg_t call) {
    sysarg_t ret;
    asm volatile("int $0x8F" : "=a"(ret) : "a"(call));
    return ret;
}

static inline sysarg_t syscall1a(sysarg_t call, sysarg_t arg0) {
    sysarg_t ret;
    asm volatile("int $0x8F" : "=a"(ret) : "a"(call), "b"(arg0));
    return ret;
}

static inline sysarg_t syscall2a(sysarg_t call, sysarg_t arg0, sysarg_t arg1) {
    sysarg_t ret;
    asm volatile("int $0x8F" : "=a"(ret) : "a"(call), "b"(arg0), "c"(arg1));
    return ret;
}

static inline sysarg_t syscall3a(sysarg_t call, sysarg_t arg0, sysarg_t arg1, sysarg_t arg2) {
    sysarg_t ret;
    asm volatile("int $0x8F" : "=a"(ret) : "a"(call), "b"(arg0), "c"(arg1), "d"(arg2));
    return ret;
}

static inline sysarg_t syscall4a(sysarg_t call, sysarg_t arg0, sysarg_t arg1, sysarg_t arg2, sysarg_t arg3) {
    sysarg_t ret;
    asm volatile("int $0x8F" : "=a"(ret) : "a"(call), "b"(arg0), "c"(arg1), "d"(arg2), "S"(arg3));
    return ret;
}

static inline sysarg_t syscall5a(sysarg_t call, sysarg_t arg0, sysarg_t arg1, sysarg_t arg2, sysarg_t arg3, sysarg_t arg4) {
    sysarg_t ret;
    asm volatile("int $0x8F" : "=a"(ret) : "a"(call), "b"(arg0), "c"(arg1), "d"(arg2), "S"(arg3), "D"(arg4));
    return ret;
}

static inline sysarg_t syscall6a(sysarg_t call, sysarg_t arg0, sysarg_t arg1, sysarg_t arg2, sysarg_t arg3, sysarg_t arg4, sysarg_t arg5) {
    sysarg_t ret;
    register sysarg_t arg5r asm("r8") = arg5;
    asm volatile("int $0x8F" : "=a"(ret) : "a"(call), "b"(arg0), "c"(arg1), "d"(arg2), "S"(arg3), "D"(arg4), "r"(arg5r) : "memory");
    return ret;
}

#define __SYSCALL_N0(call) syscall0a(call)
#define __SYSCALL_N1(call, _1) syscall1a(call, (sysarg_t)_1)
#define __SYSCALL_N2(call, _1, _2) syscall2a(call, (sysarg_t)_1, (sysarg_t)_2)
#define __SYSCALL_N3(call, _1, _2, _3) syscall3a(call, (sysarg_t)_1, (sysarg_t)_2, (sysarg_t)_3)
#define __SYSCALL_N4(call, _1, _2, _3, _4) syscall4a(call, (sysarg_t)_1, (sysarg_t)_2, (sysarg_t)_3, (sysarg_t)_4)
#define __SYSCALL_N5(call, _1, _2, _3, _4, _5) syscall5a(call, (sysarg_t)_1, (sysarg_t)_2, (sysarg_t)_3, (sysarg_t)_4, (sysarg_t)_5)
#define __SYSCALL_N6(call, _1, _2, _3, _4, _5, _6) syscall6a(call, (sysarg_t)_1, (sysarg_t)_2, (sysarg_t)_3, (sysarg_t)_4, (sysarg_t)_5, (sysarg_t)_6)

#define __SYSCALL_NARGS_8ARG(_1, _2, _3, _4, _5, _6, _7, _8, ...) _8
#define __SYSCALL_NARGS(...) __SYSCALL_NARGS_8ARG(dummy, ##__VA_ARGS__, 6, 5, 4, 3, 2, 1, 0)

#define __SYSCALL_CONCAT(a, b) a ## b
#define __SYSCALL_XCONCAT(a, b) __SYSCALL_CONCAT(a, b)

#define __SYSCALLN_(M, ...) M(__VA_ARGS__)
#define SYSCALL(call, ...) __SYSCALLN_(__SYSCALL_XCONCAT(__SYSCALL_N, __SYSCALL_NARGS(__VA_ARGS__)), call, ##__VA_ARGS__)

#if defined(__cplusplus)
}

static inline sysarg_t syscall(sysarg_t call) {
    return syscall0a(call);
}

static inline sysarg_t _syscall(sysarg_t call, sysarg_t arg0) {
    return syscall1a(call, arg0);
}

static inline sysarg_t _syscall(sysarg_t call, sysarg_t arg0, sysarg_t arg1) {
    return syscall2a(call, arg0, arg1);
}

static inline sysarg_t _syscall(sysarg_t call, sysarg_t arg0, sysarg_t arg1, sysarg_t arg2) {
    return syscall3a(call, arg0, arg1, arg2);
}

static inline sysarg_t _syscall(sysarg_t call, sysarg_t arg0, sysarg_t arg1, sysarg_t arg2, sysarg_t arg3) {
    return syscall4a(call, arg0, arg1, arg2, arg3);
}

static inline sysarg_t _syscall(sysarg_t call, sysarg_t arg0, sysarg_t arg1, sysarg_t arg2, sysarg_t arg3, sysarg_t arg4) {
    return syscall5a(call, arg0, arg1, arg2, arg3, arg4);
}

static inline sysarg_t _syscall(sysarg_t call, sysarg_t arg0, sysarg_t arg1, sysarg_t arg2, sysarg_t arg3, sysarg_t arg4, sysarg_t arg5) {
    return syscall6a(call, arg0, arg1, arg2, arg3, arg4, arg5);
}

template<typename... Args> inline sysarg_t syscall(sysarg_t call, Args... args) {
    return _syscall(call, (sysarg_t)(args)...);
}

#endif

#endif
