/* Assembler macros for C6x.
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

#ifndef _LINUX_C6X_SYSDEP_H
#define _LINUX_C6X_SYSDEP_H 1

#include <common/sysdep.h>

#include <sys/syscall.h>

#ifdef	__ASSEMBLER__

/* Syntactic details of assembler.  */
#define ALIGNARG(log2) log2

/* For ELF we need the `.type' directive to make shared libs work right.  */
#define ASM_TYPE_DIRECTIVE(name,typearg) .type name,%##typearg@
#define ASM_SIZE_DIRECTIVE(name) .size name,.-name@

/* In ELF C symbols are asm symbols.  */
#undef	NO_UNDERSCORES
#define NO_UNDERSCORES

#define PLTJMP(_x)	_x
#define DO_RET(_reg)	BNOP .S2 _reg,5

/* Define an entry point visible from C.  */
#define	ENTRY(name)						\
  ASM_GLOBAL_DIRECTIVE C_SYMBOL_NAME(name)@			\
  ASM_TYPE_DIRECTIVE (C_SYMBOL_NAME(name),function)		\
  .align ALIGNARG(4)@						\
  name##:

#undef	END
#define END(name)						\
  ASM_SIZE_DIRECTIVE(name)

#ifdef	NO_UNDERSCORES
/* Since C identifiers are not normally prefixed with an underscore
   on this system, the asm identifier `syscall_error' intrudes on the
   C name space.  Make sure we use an innocuous name.  */
#define	syscall_error	__syscall_error
#define mcount		_mcount
#endif

/* Linux uses a negative return value to indicate syscall errors,
   unlike most Unices, which use the condition codes' carry flag.

   Since version 2.1 the return value of a system call might be
   negative even if the call succeeded.  E.g., the `lseek' system call
   might return a large offset.  Therefore we must not anymore test
   for < 0, but test for a real error by making sure the value in R0
   is a real error number.  Linus said he will make sure the no syscall
   returns a value in -1 .. -4095 as a valid result so we can safely
   test with -4095.  */

#undef	PSEUDO
#define	PSEUDO(name, syscall_name, args)				\
  .text@								\
  ENTRY (name)@								\
      DO_CALL (syscall_name)@			 		        \
      PSEUDO_RET(SYSCALL_ERROR)@

#define PSEUDO_RET(syscall_error)					\
      MVK    .S1  -4096,A0@                                             \
      CMPGTU .L1  A0,A4,A2@                                             \
[!A2] B      .S2  syscall_error@                                        \
[A2]  B      .S2  B3@			                                \
      NOP         5@

#undef ret
#define ret PSEUDO_RET(SYSCALL_ERROR)

#undef	PSEUDO_END
#define	PSEUDO_END(name)						\
  SYSCALL_ERROR_HANDLER							\
  END (name)

#undef	PSEUDO_NOERRNO
#define	PSEUDO_NOERRNO(name, syscall_name, args)			\
  .text@								\
  ENTRY (name)@				  			        \
      DO_CALL (syscall_name)@

#define PSEUDO_RET_NOERRNO						\
      DO_RET (B3)@

#undef ret_NOERRNO
#define ret_NOERRNO PSEUDO_RET_NOERRNO

#undef	PSEUDO_END_NOERRNO
#define	PSEUDO_END_NOERRNO(name)					\
  END (name)

/* The function has to return the error code.  */
#undef	PSEUDO_ERRVAL
#define	PSEUDO_ERRVAL(name, syscall_name, args)				\
  .text@								\
  ENTRY (name)@								\
      DO_CALL (syscall_name)@						\

#undef	PSEUDO_END_ERRVAL
#define	PSEUDO_END_ERRVAL(name)			                        \
  END (name)

#undef  ret_ERRVAL
#define ret_ERRVAL PSEUDO_RET_NOERRNO

#if defined NOT_IN_libc
# define SYSCALL_ERROR __local_syscall_error
# ifdef RTLD_PRIVATE_ERRNO
#  define SYSCALL_ERROR_HANDLER					\
__local_syscall_error:						\
      NEG  .S1X  A4,A4@	                                        \
      MVKL .S2	 C_SYMBOL_NAME(rtld_errno),B1@			\
      MVKH .S2   C_SYMBOL_NAME(rtld_errno),B1@			\
      BNOP .S2   B3,3@						\
      STW  .D2T2 A4,*B1@                                        \
      MVK  .L1   -1,A4@
# else
#  define SYSCALL_ERROR_HANDLER					\
__local_syscall_error:						\
      NEG .S1   A4,A4@						\
      STW .D2T1 A4,*B15--[4]@				        \
      STW .D2T2 B3,*+B15[1]@                                    \
      STW .D2T2 B14,*+B15[2]@  	                                \
      LDW .D2T2 *+B14($DSBT_index(__c6xabi_DSBT_BASE)),B14@     \
      NOP       4@                                              \
      CALLP .S2 PLTJMP(C_SYMBOL_NAME(__errno_location)),B3@     \
      LDW .D2T2 *+B15[2],B14@					\
      LDW .D2T2 *+B15[1],B3@                                    \
      LDW .D2T1 *++B15[4],A5@                                   \
      NOP       3@                                              \
      BNOP .S2  B3,3@                                           \
      STW .D1T1 A5,*A4@                                         \
      MVK .L1   -1,A4@
# endif
#else
# define SYSCALL_ERROR_HANDLER	/* Nothing here; code in __syscall_error.c is used.  */
# define SYSCALL_ERROR __syscall_error
#endif

#undef	DO_CALL
# ifndef _TMS320C6400_PLUS
# define DO_CALL(syscall_name)			\
      MVC .S2     CSR,B2@			\
      CLR .S2     B2,0,0,B1@			\
      MVC .S2     B1,CSR@			\
      MVC .S2     IFR,B1@			\
      SET .S2     B1,6,6,B1@			\
      MVC .S2     B1,ISR@			\
      MVC .S2     B2,CSR@			\
      NOP
# else  /* _TMS320C6400_PLUS */
# define DO_CALL(syscall_name)			\
      MVK .S2 SYS_ify(syscall_name),B0@		\
      SWE@					\
      NOP
# endif /* _TMS320C6400_PLUS */
#endif	/* __ASSEMBLER__ */

/* Pointer mangling is not yet supported for C6x.  */
#define PTR_MANGLE(var) (void) (var)
#define PTR_DEMANGLE(var) (void) (var)

#endif /* linux/c6x/sysdep.h */
