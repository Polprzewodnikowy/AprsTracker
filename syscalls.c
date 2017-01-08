/*
 * syscalls.c
 *
 *  Created on: 7.1.2017
 *  Based on work of: freddiechopin.info
 *      Author: korgeaux
 */

#include <errno.h>
#include <sys/types.h>

#define SYSCALLS_HAVE_SBRK_R		1
#define SYSCALLS_HAVE_WRITE_R		0

#undef errno
extern int errno;

#if SYSCALLS_HAVE_SBRK_R == 1
caddr_t _sbrk_r(struct _reent *r, int size)
{
	extern char __heap_start;
	extern char __heap_end;
	static char *current_heap_end = &__heap_start;
	char *previous_heap_end;

	previous_heap_end = current_heap_end;

	if (current_heap_end + size > &__heap_end) {
		errno = ENOMEM;
		return (caddr_t)-1;
	}

	current_heap_end += size;

	return (caddr_t)previous_heap_end;
}
#endif

#if SYSCALLS_HAVE_WRITE_R == 1
ssize_t _write_r(struct _reent *r, int file, const void *buf, size_t nbyte)
{
	return 0;
}
#endif
