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
#include <tcb-offsets.h>
#ifndef __ASSEMBLER__
# include <pthreadP.h>
#endif

#if !defined NOT_IN_libc || defined IS_IN_libpthread || defined IS_IN_librt

# undef PSEUDO
# define PSEUDO(name, syscall_name, args)			        \
  .sect  ".text"@						        \
  .global __##syscall_name##_nocancel@					\
  .type __##syscall_name##_nocancel,%function@				\
  __##syscall_name##_nocancel:@					        \
      cfi_sections(.debug_frame)@					\
      cfi_startproc@							\
      DOARGS_##name(args)@                                              \
      DO_CALL (syscall_name)@			                        \
      PSEUDO_RET(SYSCALL_ERROR)@					\
      cfi_endproc@							\
  .size __##syscall_name##_nocancel,.-__##syscall_name##_nocancel@	\
  ENTRY (name)@							        \
      SINGLE_THREAD_P(args)@						\
[!B1] BNOP .S2 .Lpseudo_cancel,5@					\
      cfi_remember_state@                                               \
      DOARGS_##name(args)@                                              \
      DO_CALL (syscall_name)@						\
      PSEUDO_RET(SYSCALL_ERROR)@					\
      cfi_restore_state@                                                \
  .Lpseudo_cancel:@                                                     \
      DOARGS_##name(args)@                                              \
      SAVESTK@                                                          \
      PUSHARGS_##args@                                                  \
      CENABLE@							        \
      STW .D2T1 A4,*+B15[13]@       /* save canceltype */		\
      cfi_rel_offset (A4, 52)@                                          \
      POPARGS_##args@                                                   \
      POPARGS_FINISH@                                                   \
      DO_CALL (syscall_name)@						\
      PUSHARGS_1@                                                       \
      LDW .D2T1 *+B15[13],A4@       /* restore canceltype */		\
      NOP 4@                                                            \
      cfi_restore(A4)@                                                  \
      CDISABLE@                                                         \
      POPARGS_1@                                                        \
      POPARGS_FINISH@                                                   \
      RESTORESTK@                                                       \
      PSEUDO_RET(SYSCALL_ERROR)@

#ifndef O_CREAT /* defined in fcntl.h but cannot be included there */
#define O_CREAT 0100
#endif

/*
 * Variable argument functions have a different ABI on C6x: arguments are passed
 * on the stack instead of passing them through standard registers.
 */
#define DOARGS_FNCTL_LOAD(args)	 \
      LDW  .D2T2 *+B15[1],B4@    \
      NOP        4@              \
      LDW  .D2T1 *+B15[2],A6@    \
      NOP        4@

#define DOARGS_OPEN_LOAD(args)   \
      LDW  .D2T2 *+B15[1],B0@	 \
      MVKL .S2   O_CREAT,B1@	 \
      NOP        3@              \
      AND  .D2   B1,B0,B1@	 \
 [B1] LDW  .D2T1 *+B15[2],A6@    \
[!B1] MVKL .S1   0,A6@           \
      MV   .L2   B0,B4@          \
      NOP        2@

#define DOARGS___libc_accept(args)
#define DOARGS___libc_clone(args)
#define DOARGS___libc_close(args)
#define DOARGS___libc_connect(args)
#define DOARGS___libc_fcntl(args)       DOARGS_FNCTL_LOAD(args)
#define DOARGS___libc_fork(args)
#define DOARGS___libc_fsync(args)
#define DOARGS___libc_gettimeofday(args)
#define DOARGS___libc_initfini(args)
#define DOARGS___libc_llseek(args)
#define DOARGS___libc_longjmp(args)
#define DOARGS___libc_lseek(args)
#define DOARGS___libc_msgrcv(args)
#define DOARGS___libc_msgsnd(args)
#define DOARGS___libc_msync(args)
#define DOARGS___libc_nanosleep(args)
#define DOARGS___libc_open(args)        DOARGS_OPEN_LOAD(args)
#define DOARGS___libc_open64(args)      DOARGS_OPEN_LOAD(args)
#define DOARGS___libc_pause(args)
#define DOARGS___libc_pread_pwrite(args)
#define DOARGS___libc_raise(args)
#define DOARGS___libc_read(args)
#define DOARGS___libc_recv(args)
#define DOARGS___libc_recvfrom(args)
#define DOARGS___libc_recvmsg(args)
#define DOARGS___libc_send(args)
#define DOARGS___libc_sendmsg(args)
#define DOARGS___libc_sendto(args)
#define DOARGS___libc_sigwait(args)
#define DOARGS___libc_sleep(args)
#define DOARGS___libc_tcdrain(args)
#define DOARGS___libc_vfork(args)
#define DOARGS___libc_wait(args)
#define DOARGS___libc_waitpid(args)
#define DOARGS___libc_write(args)
#define DOARGS___mq_timedreceive(args)
#define DOARGS___mq_timedsend(args)

/* Save/restore args */
# define PUSHARGS_0	/* nothing to do */
# define PUSHARGS_1	PUSHARGS_0 STW .D2T1 A4,*+B15[1]@ cfi_rel_offset (A4, 4)@
# define PUSHARGS_2	PUSHARGS_1 STW .D2T2 B4,*+B15[2]@ cfi_rel_offset (B4, 8)@
# define PUSHARGS_3	PUSHARGS_2 STW .D2T1 A6,*+B15[3]@ cfi_rel_offset (A6, 12)@
# define PUSHARGS_4	PUSHARGS_3 STW .D2T2 B6,*+B15[4]@ cfi_rel_offset (B6, 16)@
# define PUSHARGS_5	PUSHARGS_4 STW .D2T1 A8,*+B15[5]@ cfi_rel_offset (A8, 20)@
# define PUSHARGS_6	PUSHARGS_5 STW .D2T2 B8,*+B15[6]@ cfi_rel_offset (B8, 24)@
# define PUSHARGS_7	PUSHARGS_6 STW .D2T1 A10,*+B15[7]@ cfi_rel_offset (A10, 28)@
# define PUSHARGS_8	PUSHARGS_7 STW .D2T2 B10,*+B15[8]@ cfi_rel_offset (B10, 32)@
# define PUSHARGS_9	PUSHARGS_8 STW .D2T1 A12,*+B15[9]@ cfi_rel_offset (A12, 36)@
# define PUSHARGS_10	PUSHARGS_9 STW .D2T2 B12,*+B15[10]@ cfi_rel_offset (B12, 40)@

# define POPARGS_0	/* nothing to do */
# define POPARGS_1	POPARGS_0 LDW .D2T1 *+B15[1],A4@ cfi_restore(A4)@
# define POPARGS_2	POPARGS_1 LDW .D2T2 *+B15[2],B4@ cfi_restore(B4)@
# define POPARGS_3	POPARGS_2 LDW .D2T1 *+B15[3],A6@ cfi_restore(A6)@
# define POPARGS_4	POPARGS_3 LDW .D2T2 *+B15[4],B6@ cfi_restore(B6)@
# define POPARGS_5	POPARGS_4 LDW .D2T1 *+B15[5],A8@ cfi_restore(A8)@
# define POPARGS_6	POPARGS_5 LDW .D2T2 *+B15[6],B8@ cfi_restore(B8)@
# define POPARGS_7	POPARGS_6 LDW .D2T1 *+B15[7],A10@ cfi_restore(A10)@
# define POPARGS_8	POPARGS_5 LDW .D2T2 *+B15[8],B10@ cfi_restore(B10)@
# define POPARGS_9	POPARGS_6 LDW .D2T1 *+B15[9],A12@ cfi_restore(A11)@
# define POPARGS_10	POPARGS_5 LDW .D2T2 *+B15[10],B12@ cfi_restore(B11)@
# define POPARGS_FINISH NOP 4@

# define STKSPACE	14
# define SAVESTK 	SUBAW .D2 B15,STKSPACE,B15@ cfi_adjust_cfa_offset(STKSPACE*4)
# define RESTORESTK 	ADDAW .D2 B15,STKSPACE,B15@ cfi_adjust_cfa_offset(-STKSPACE*4)

/* PLT calls */
#define PLTJMP_PROLOGUE STW .D2T2 B14,*+B15[11]@                                   \
	                STW .D2T2 B3,*+B15[12]@                                    \
                        cfi_rel_offset (B14, 44)@                                  \
                        cfi_rel_offset (B3, 48)@                                   \
	                LDW .D2T2 *+B14($DSBT_index(__c6xabi_DSBT_BASE)),B14@      \
                        NOP       4
#define PLTJMP_EPILOGUE LDW .D2T2 *+B15[12],B3@	                                   \
                        LDW .D2T2 *+B15[11],B14@                                   \
                        cfi_restore (B3)@                                          \
                        cfi_restore (B14)@                                         \
	                NOP       4
#define DO_PLTJMP(func) PLTJMP_PROLOGUE@				           \
                        CALLP .S2 PLTJMP(C_SYMBOL_NAME((func))),B3@		   \
	                PLTJMP_EPILOGUE

# ifdef IS_IN_libpthread
#  define CENABLE	DO_PLTJMP(__pthread_enable_asynccancel)
#  define CDISABLE	DO_PLTJMP((__pthread_disable_asynccancel)
#  define __local_multiple_threads __pthread_multiple_threads
# elif !defined NOT_IN_libc
#  define CENABLE	DO_PLTJMP(__libc_enable_asynccancel)
#  define CDISABLE	DO_PLTJMP(__libc_disable_asynccancel)
#  define __local_multiple_threads __libc_multiple_threads
# elif defined IS_IN_librt
#  define CENABLE	DO_PLTJMP(__librt_enable_asynccancel)
#  define CDISABLE	DO_PLTJMP(__librt_disable_asynccancel)
# else
#  error Unsupported library
# endif

# if defined IS_IN_libpthread || !defined NOT_IN_libc
#  ifndef __ASSEMBLER__
extern int __local_multiple_threads attribute_hidden;
#   define SINGLE_THREAD_P __builtin_expect (__local_multiple_threads == 0, 1)
#  else
#   define SINGLE_THREAD_P(args)					\
    MVKL	.S2	__local_multiple_threads,B1@			\
    MVKH	.S2	__local_multiple_threads,B1@			\
    LDW         .D2T2   *B1,B1@					        \
    NOP                 4@                                              \
    CMPEQ       .L2     0,B1,B1@
#  endif
# else
/*  There is no __local_multiple_threads for librt, so use the TCB.  */
#  ifndef __ASSEMBLER__
#   define SINGLE_THREAD_P					        \
  __builtin_expect (THREAD_GETMEM (THREAD_SELF,				\
				   header.multiple_threads) == 0, 1)
#  else
#   define SINGLE_THREAD_P(args)		                        \
    SAVESTK@                                                            \
    PUSHARGS_##args@                                                    \
    DO_PLTJMP(__get_thread_pointer)@                                    \
    LDW         .D1T2   *+A4(MULTIPLE_THREADS_OFFSET),B1@               \
    POPARGS_##args@                                                     \
    POPARGS_FINISH@                                                     \
    RESTORESTK@                                                         \
    CMPEQ       .L2     0,B1,B1@
#  endif
# endif

#elif !defined __ASSEMBLER__

/* For rtld, et cetera.  */
# define SINGLE_THREAD_P 1
# define NO_CANCELLATION 1

#endif

#ifndef __ASSEMBLER__
# define RTLD_SINGLE_THREAD_P \
  __builtin_expect (THREAD_GETMEM (THREAD_SELF, \
				   header.multiple_threads) == 0, 1)
#endif
