/* Low-level functions for atomic operations. C6x version.
   Copyright (C) 2012 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Aurelien Jacquiot <a-jacquiot@ti.com>

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

#ifndef _BITS_ATOMIC_H
#define _BITS_ATOMIC_H	1

void __c6x_link_error (void);

/*
 * The only basic operation needed is compare and exchange.
 * On C6x we have the capability to disable interrupts in user space, so use it.
 */
#define __arch_compare_and_exchange_val_8_acq(mem, newval, oldval) \
  ({ __c6x_link_error (); oldval; })

#define __arch_compare_and_exchange_val_16_acq(mem, newval, oldval) \
  ({ __c6x_link_error (); oldval; })

#define __arch_compare_and_exchange_val_32_acq(mem, newval, oldval) \
    ({  register __typeof (mem)    __memp    = (mem);		    \
	register __typeof (newval) __newval  = (newval);	    \
	register __typeof (newval) __oldval  = (oldval);	    \
	register __typeof (newval) __ret;			    \
	register unsigned int __cond;				    \
	__asm__ __volatile__					    \
	    ("      dint\n"					    \
	     "      ldw .d2t2 *%2,%0\n"				    \
	     "      nop 4\n"					    \
	     "      cmpeq .l2 %0,%3,%1\n"			    \
	     " [%1] stw .d2t2 %4,*%2\n"				    \
	     "      nop\n"					    \
	     "      rint\n"					    \
	     : "=&b"(__ret), "=&B"(__cond)			    \
	     : "b"(__memp), "b"(__oldval), "b"(__newval));	    \
	__ret; })

#define __arch_compare_and_exchange_val_64_acq(mem, newval, oldval) \
  ({ __c6x_link_error (); oldval; })

#endif	/* bits/atomic.h */

