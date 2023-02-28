#include "mlibc/posix-sysdeps.hpp"
#include <mlibc/all-sysdeps.hpp>

#include <mieros/syscall.h>
#include <sys/ioctl.h>
#include <errno.h>

namespace mlibc {
    int sys_open(const char *pathname, int flags, mode_t mode, int *fd) {
        ssysarg_t ret = syscall(SYS_openat, pathname, flags, mode, AT_FDCWD);

        if(ret < 0)
            return -ret;

        *fd = ret;
        return 0;
    }

    int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
        ssysarg_t ret = syscall(SYS_read, fd, buf, count);

        if(ret < 0)
            return -ret;

        *bytes_read = ret;
        return 0;
    }

    int sys_write(int fd, void const *buf, size_t count, ssize_t *bytes_written) {
        ssysarg_t ret = syscall(SYS_write, fd, buf, count);

        if(ret < 0)
            return -ret;

        *bytes_written = ret;
        return 0;
    }

    int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
        ssysarg_t ret = syscall(SYS_seek, fd, offset, whence);

        if(ret < 0)
            return -ret;

        *new_offset = ret;
        return 0;
    }

    int sys_close(int fd) {
        return -syscall(SYS_close, fd);
    }

    int sys_ioctl(int fd, unsigned long request, void *arg, int *result) {
        ssysarg_t ret = syscall(SYS_ioctl, fd, request, arg);

        if(ret < 0)
            return -ret;

        *result = ret;
        return 0;
    }

    int sys_isatty(int fd) {
        // TCGETS returns 0 on success indicating that this fd is in fact a terminal.
        int result;
        int error = sys_ioctl(fd, TCGETS, 0, &result);

        // This fd is a tty
        if(!error)
            return 0;
        if(error == EBADF || error == ENOSYS)
            return error;
        
        return ENOTTY;
    }

    int sys_dup(int fd, int flags, int *newfd) {
        ssysarg_t ret = syscall(SYS_dup, fd, -1, flags);
        if(ret < 0)
            return -ret;

        *newfd = ret;
        return 0;
    }

    int sys_dup2(int fd, int flags, int newfd) {
        ssysarg_t ret = syscall(SYS_dup, fd, newfd, flags);
        if(ret < 0)
            return -ret;

        return 0;
    }

    int sys_mount(const char *source, const char *target, const char *fstype, unsigned long flags, const void *data) {
        return -syscall(SYS_mount, source, target, fstype, flags, data);
    }

    int sys_umount2(const char *target, int flags) {
        return -syscall(SYS_umount, target, flags);
    }

    int sys_symlinkat(const char *target_path, int dirfd, const char *link_path) {
        return -syscall(SYS_symlinkat, target_path, dirfd, link_path);
    }

    int sys_symlink(const char *target_path, const char *link_path) {
        return -syscall(SYS_symlinkat, target_path, AT_FDCWD, link_path);
    }

    int sys_open_dir(const char *path, int *handle) {
        ssysarg_t ret = syscall(SYS_openat, path, O_DIRECTORY | O_CLOEXEC, 0, AT_FDCWD);

        if(ret < 0)
            return -ret;

        *handle = ret;
        return 0;
    }

    int sys_read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read) {
        // We can use a simple read syscall here since we opened the file as
        // a directory stream, it automatically outputs standard directory entries
        // and we don't have to use any special syscalls.
        ssysarg_t ret = syscall(SYS_read, handle, buffer, max_size);

        if(ret < 0)
            return -ret;

        *bytes_read = ret;
        return 0;
    }

    int sys_pipe(int *fds, int flags) {
        return -syscall(SYS_pipe, fds, flags);
    }

    int sys_fcntl(int fd, int request, va_list args, int *result) {
        ssysarg_t ret = -ENOTSUP;

        switch(request) {
            case F_DUPFD:
                ret = syscall(SYS_dup, fd, va_arg(args, int), DUP_SOFT);
                break;
            case F_DUPFD_CLOEXEC:
                ret = syscall(SYS_dup, fd, va_arg(args, int), DUP_SOFT | O_CLOEXEC);
                break;
            case F_SETFD:
                ret = syscall(SYS_fdflags, fd, va_arg(args, int) | FDF_SETD);
                break;
            case F_GETFD:
                ret = syscall(SYS_fdflags, fd, FDF_GETD);
                break;
            case F_SETFL:
                ret = syscall(SYS_fdflags, fd, va_arg(args, int) | FDF_SETS);
                break;
            case F_GETFL:
                ret = syscall(SYS_fdflags, fd, FDF_GETS);
                break;
        }

        if(ret < 0)
            return -ret;

        *result = ret;
        return 0;
    }
}
