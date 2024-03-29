#ifndef FB_MMAP_H
#define FB_MMAP_H

#include "fbbs/fileio.h"

/** Memory mapped file information. */
typedef struct {
	int fd;       ///< file descriptor.
	int oflag;    ///< open flags.
	file_lock_e lock; ///< lock status.
	int prot;     ///< memory protection of the mapping.
	int mflag;    ///< MAP_SHARED or MAP_PRIVATE.
	void *ptr;    ///< starting address of the mapping.
	size_t msize; ///< mmaped size.
	size_t size;  ///< real file size.
} mmap_t;

int mmap_open_fd(mmap_t *m);
int mmap_open(const char *file, mmap_t *m);
void mmap_unmap(mmap_t *m);
int mmap_close(mmap_t *m);
int mmap_truncate(mmap_t *m, size_t size);
int mmap_shrink(mmap_t *m, size_t size);
int mmap_lock(mmap_t *m, file_lock_e lock);

#endif // FB_MMAP_H

