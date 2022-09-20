#ifndef _SYSCALL_H
#define _SYSCALL_H

typedef long long ssysarg_t;
typedef unsigned long long sysarg_t;

#define SYSCALL_EXIT    1
#define SYSCALL_OPEN    2
#define SYSCALL_CLOSE   3
#define SYSCALL_READ    4
#define SYSCALL_WRITE   5
#define SYSCALL_FORK    6
#define SYSCALL_SEEK    7
#define SYSCALL_MMAP    8
#define SYSCALL_MUNMAP  9
#define SYSCALL_EXECVE  10

#ifdef __cplusplus
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

#ifdef __cplusplus
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

template<typename... Args> sysarg_t syscall(sysarg_t call, Args... args) {
    return _syscall(call, (sysarg_t)(args)...);
}

#endif

#endif
