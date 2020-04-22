 ;
 ; Port of uClibc for TMS320C6000 DSP architecture
 ; Copyright (C) 2004, 2012 Texas Instruments Incorporated
 ; Author of TMS320C6000 port: Aurelien Jacquiot
 ;
 ; This program is free software; you can redistribute it and/or modify it
 ; under the terms of the GNU Library General Public License as published by
 ; the Free Software Foundation; either version 2 of the License, or (at your
 ; option) any later version.
 ;
 ; This program is distributed in the hope that it will be useful, but WITHOUT
 ; ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 ; FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public License
 ; for more details.
 ;
 ; You should have received a copy of the GNU Library General Public License
 ; along with this program; if not, see <http://www.gnu.org/licenses/>.
 ;
#define __ASSEMBLY__

	 ; int clone(int (*fn)(void *arg), void *child_stack, int flags, void *arg,
         ;	     int *parent_tidptr, struct user_desc *newtls, int *child_pidptr)

#include <asm/errno.h>
#include <sys/syscall.h>
#include <sysdep.h>

#ifdef RESET_PID
#include <tcb-offsets.h>
#include <tls.h>
#endif

/* unfortunatly sched.h cannot be included from assembly */
#define CLONE_VM      0x00000100
#define CLONE_THREAD  0x00010000

	.global __clone
	.global	clone

 ;
 ;  syscall prototype is as following:
 ;  int sys_c6x_clone(unsigned long clone_flags, unsigned long newsp,
 ;		      int __user *parent_tidptr, int __user *child_tidptr, int tls_val)

__clone:
	CMPEQ	.L1   0,A4,A2
||	CMPEQ   .L2   0,B4,B2
	OR	.D2X  B2,A2,B2	    ; sanity check arguments, no NULL function or stack pointers
||	MV	.S2   B4,B9
||	MV	.D1   A4,A9	    ; backup fn and child_stack pointers

  [B2]	B	.S2   __syscall_error
||[B2]  MVK	.S1   -EINVAL,A4
	NOP     4

	MVK	.S1   CLONE_VM,A8
	OR	.L1   A6,A8,A6	    ; always set CLONE_VM flag on MMU-less

#ifdef RESET_PID
	MV      .D1   A6,A7         ; save flags
#endif
	MV      .D1   A6,A4	    ; get flags as arg0, arg1 is the new stack
||	AND     .D2   ~7,B4,B4

	LDW	.D2T2 *+B15[1],B7   ; arg
	LDW	.D2T1 *+B15[2],A6   ; parent_tidpr
	LDW	.D2T1 *+B15[3],A8   ; newtls
	LDW	.D2T2 *+B15[4],B6   ; child_tidptr
	NOP     4

	; do the system call
	DO_CALL(clone)
	MV      .D2   B9,B4	    ; restore child stack

||	CMPEQ   .L1   0,A4,A2
||	CMPLT   .L2X  A4,0,B2
   [B2]	B	.S2   __syscall_error	; if syscall < 0, it is an error
	NOP	 5

[!A2]	BNOP	.S2   B3,5		; if we are the parent, return directly

#ifdef RESET_PID
||	SHR	.S1   A7,16,A1	        ; A7 preserved by syscall
	AND	.L1   (CLONE_THREAD>>16),A1,A0
   [A0] BNOP	.S2   __branch_fn,5
||[!A0] MVK	.S1   -1,A5

	STW	.D2T2 B3,*B15--[6]
	STW	.D2T2 B14,*+B15[1]
	STW	.D2T1 A2,*+B15[2]
	STW	.D2T2 B7,*+B15[3]
	STW	.D2T1 A9,*+B15[4]
	STW	.D2T1 A5,*+B15[5]

	LDW	.D2T2 *+B14($DSBT_index(__c6xabi_DSBT_BASE)),B14
        NOP	4
	CALLP	.S2   PLTJMP(C_SYMBOL_NAME(__get_thread_pointer)),B3

	LDW	.D2T1 *+B15[5],A5
	LDW	.D2T1 *+B15[4],A9
	LDW	.D2T2 *+B15[3],B7
	LDW	.D2T1 *+B15[2],A2
	NOP
	STW	.D1T1 A5,*+A4(PID_OFFSET)
	STW	.D1T1 A5,*+A4(TID_OFFSET)
	LDW	.D2T2 *+B15[1],B14
        LDW	.D2T2 *++B15[6],B3
	NOP	4
#endif

__branch_fn:
   [A2] B	.S2X  A9	; branch to function
||	MV	.D1X  B7,A4	; set saved A4 (B7 is preserved by syscall)
  [!A2] B	.S2   B3	; otherwise (syscall result > 0) returns directly
   [A2]	ADDKPC	.S2   __return_thread,B3,4

__return_thread:
	B	.S2   HIDDEN_JUMPTARGET(_exit)
	NOP	5

.set clone, __clone
