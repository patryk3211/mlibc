#include <mlibc/all-sysdeps.hpp>

#include <mieros/syscall.h>

namespace mlibc {
    int sys_open(const char *pathname, int flags, mode_t mode, int *fd) {
        ssysarg_t ret = syscall(SYSCALL_OPEN, pathname, flags, mode);

        if(ret < 0)
            return -ret;

        *fd = ret;
        return 0;
    }

    int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
        ssysarg_t ret = syscall(SYSCALL_READ, fd, buf, count);

        if(ret < 0)
            return -ret;

        *bytes_read = ret;
        return 0;
    }

    int sys_write(int fd, void *buf, size_t count, ssize_t *bytes_written) {
        ssysarg_t ret = syscall(SYSCALL_WRITE, fd, buf, count);

        if(ret < 0)
            return -ret;

        *bytes_written = ret;
        return 0;
    }

    int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
        ssysarg_t ret = syscall(SYSCALL_SEEK, fd, offset, whence);

        if(ret < 0)
            return -ret;

        *new_offset = ret;
        return 0;
    }

    int sys_close(int fd) {
        return -syscall(SYSCALL_CLOSE, fd);
    }
}
