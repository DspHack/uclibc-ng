/* Thread-local storage handling in the ELF dynamic linker. C6x version.
   Copyright (C) 2012 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Aurelien Jacquiot <a-jacquiot@ti.com>, 2012

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <sysdeps/generic/libc-tls.c>
#include <dl-tls.h>

#if defined(USE_TLS) && USE_TLS
#include <tls.h>
/*
 * Since we do not have a fixed user space address for user helper on
 * every C6x devices, we have to get and store in a global variable the user helper
 * address in which we can get the TLS value.
 */

static unsigned long *__user_helper_addr = 0;

unsigned long
__get_thread_pointer(void)
{
	if (unlikely(__user_helper_addr == 0)) {
		unsigned long res;
		INTERNAL_SYSCALL_DECL (err);
		res = INTERNAL_SYSCALL (get_user_helper, err, 0);
		if (unlikely(INTERNAL_SYSCALL_ERROR_P (res, err))) {
			return 0;
		}
		__user_helper_addr = (unsigned long *) res;
	}
	return *__user_helper_addr;
}

char *
__init_thread_pointer(unsigned long tp)
{
	long result_var;

	INTERNAL_SYSCALL_DECL (err);

	if (unlikely(__user_helper_addr == 0)) {
		unsigned long res;
		INTERNAL_SYSCALL_DECL (err);
		res = INTERNAL_SYSCALL (get_user_helper, err, 0);
		if (unlikely(INTERNAL_SYSCALL_ERROR_P (res, err))) {
			return "cannot get user helper";
		}
		__user_helper_addr = (unsigned long *) res;
	}

	result_var = INTERNAL_SYSCALL (set_tls, err, 1, tp);
	return (INTERNAL_SYSCALL_ERROR_P (result_var, err) ? "unknown error" : NULL);
}
#endif
