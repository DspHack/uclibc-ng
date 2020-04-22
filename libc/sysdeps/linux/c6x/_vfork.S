/*
 * Port of uClibc for TMS320C6000 DSP architecture
 * Copyright (C) 2004, 2011, 2012 Texas Instruments Incorporated
 * Author of TMS320C6000 port: Aurelien Jacquiot
 *
 *   Use clone syscall: Mark Salter <msalter@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.LIB.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#define __ASSEMBLY__

#include <asm/errno.h>
#include <sys/syscall.h>
#include <linux/sched.h>
#include <asm/signal.h>
#include <sysdep.h>

#define CLONE_FLAGS (CLONE_VFORK | CLONE_VM | SIGCHLD)

#ifndef SAVE_PID
#define SAVE_PID
#endif

#ifndef RESTORE_PID
#define RESTORE_PID
#endif
	
	.global __vfork
__vfork:
	SAVE_PID
	MVKL	.S1	CLONE_FLAGS,A4
	MVKH	.S1	CLONE_FLAGS,A4
 ||	MVK	.L2	0,B4
	DO_CALL (clone)
	RESTORE_PID
	PSEUDO_RET(SYSCALL_ERROR)
	PSEUDO_END(__vfork)

weak_alias(__vfork,vfork)
libc_hidden_weak(vfork)
