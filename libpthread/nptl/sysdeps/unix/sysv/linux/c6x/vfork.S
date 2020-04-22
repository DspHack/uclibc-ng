/* Copyright (C) 2012 Free Software Foundation, Inc.
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

#include <tcb-offsets.h>
#include <tls.h>

/* Save the PID value.  */
#define SAVE_PID								  \
	STW   .D2T2 B3,*B15--[2]@         /* save return address */		  \
	STW   .D2T2 B14,*+B15[1]@         /* save DP */	 	                  \
	LDW   .D2T2 *+B14($DSBT_index(__c6xabi_DSBT_BASE)),B14@                   \
        NOP         4@                                                            \
	CALLP .S2   PLTJMP(C_SYMBOL_NAME(__get_thread_pointer)),B3@ /* get TP */  \
	LDW   .D2T2 *+B15[1],B14@                                                 \
        LDW   .D2T2 *++B15[2],B3@         /* restore return address */	          \
	LDW   .D1T2 *+A4(PID_OFFSET),B8@  /* load saved PID */	                  \
 ||     MV    .L1   A4,A8@                /* save TP (A8 preserved by syscall) */ \
	NOP         4@  							  \
	NEG   .S2   B8,B0@	          /* negate PID */     		          \
 [!B0]  MVKL  .S2   0x80000000,B0@        /* use 0x80000000 if it was 0 */        \
 [!B0]  MVKH  .S2   0x80000000,B0@                                                \
	STW   .D1T2 B0,*+A4(PID_OFFSET)@  /* store the temporary PID */

/* Restore the old PID value in the parent.  */
#define RESTORE_PID								  \
	MV    .L1   A4,A0@							  \
 [A0]   STW   .D1T2 B8,*+A8(PID_OFFSET)@  /* restore the parent PID */

#include "../../../../../../../libc/sysdeps/linux/c6x/_vfork.S"
