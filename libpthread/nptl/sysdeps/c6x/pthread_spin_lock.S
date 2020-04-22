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

#include <sysdep.h>

	.text
	.align	4

ENTRY (pthread_spin_lock)
	MVK   .S1   1,A0
1:	DINT
	LDW   .D1T1 *A4,A1
	NOP   4
	STW   .D1T1 A0,*A4
	RINT
 [A1]	BNOP  .S2   1b,5
	MVK   .S1   0,A4
	PSEUDO_RET_NOERRNO
END (pthread_spin_lock)
