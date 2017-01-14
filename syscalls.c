/*
 * syscalls.c
 *
 *  Created on: 7.1.2017
 *  Based on work of: freddiechopin.info
 *      Author: korgeaux
 */

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SYSCALLS_HAVE_SBRK_R        1
#define SYSCALLS_HAVE_CLOSE_R       0
#define SYSCALLS_HAVE_FSTAT_R       0
#define SYSCALLS_HAVE_ISATTY_R      0
#define SYSCALLS_HAVE_LSEEK_R       0
#define SYSCALLS_HAVE_READ_R        0
#define SYSCALLS_HAVE_WRITE_R       0

#undef errno
extern int errno;

#if SYSCALLS_HAVE_SBRK_R == 1
caddr_t _sbrk_r(struct _reent *r, int size) {
    extern char __heap_start;
    extern char __heap_end;
    static char *current_heap_end = &__heap_start;
    char *previous_heap_end;

    previous_heap_end = current_heap_end;

    if (current_heap_end + size > &__heap_end) {
        errno = ENOMEM;
        return (caddr_t) -1;
    }

    current_heap_end += size;

    return (caddr_t) previous_heap_end;
}
#endif

#if SYSCALLS_HAVE_CLOSE_R == 1
int _close_r(struct _reent *r, int file) {
    return -1;
}
#endif

#if SYSCALLS_HAVE_FSTAT_R == 1
int _fstat_r(struct _reent *r, int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}
#endif

#if SYSCALLS_HAVE_ISATTY_R == 1
int _isatty_r(struct _reent *r, int file) {
    return 1;
}
#endif

#if SYSCALLS_HAVE_LSEEK_R == 1
off_t _lseek_r(struct _reent *r, int file, off_t offset, int whence) {
    return 0;
}
#endif

#if SYSCALLS_HAVE_READ_R == 1
ssize_t _read_r(struct _reent *r, int file, void *buf, size_t nbyte) {
    return 0;
}
#endif

#if SYSCALLS_HAVE_WRITE_R == 1
ssize_t _write_r(struct _reent *r, int file, const void *buf, size_t nbyte) {
    return 0;
}
#endif
