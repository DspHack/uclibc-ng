/* Copyright (C) 2010 Texas Instruments Incorporated
 * Contributed by Mark Salter <msalter@redhat.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#ifdef __NR_cacheflush
static __always_inline _syscall3 (int, cacheflush, void *, start, const int, nbytes, const int, flags);
#endif

#ifdef __NR_dsbt_idx_alloc
static __always_inline _syscall4 (int, dsbt_idx_alloc, const char *, name, unsigned long, start, unsigned long, end, unsigned long, op);
#endif

