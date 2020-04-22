/* libc-internal interface for thread-specific data.  Stub or TLS version.
   Copyright (C) 2002, 2003, 2012 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

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

#include <stdlib.h>
#include <atomic.h>
#include "pthreadP.h"
#include "libc-tsd.h"

void* internal_function
__libc_tsd_address_internal(unsigned int key)
{
  struct pthread_key_data *data;

  /* Special case access to the first 2nd-level block.  This is the
     usual case.  */
  if (__builtin_expect (key < PTHREAD_KEY_2NDLEVEL_SIZE, 1))
    data = &THREAD_SELF->specific_1stblock[key];
  else
    {
      /* Verify the key is sane.  */
      if (key >= PTHREAD_KEYS_MAX)
	/* Not valid.  */
	return NULL;

      unsigned int idx1st = key / PTHREAD_KEY_2NDLEVEL_SIZE;
      unsigned int idx2nd = key % PTHREAD_KEY_2NDLEVEL_SIZE;

      /* If the sequence number doesn't match or the key cannot be defined
	 for this thread since the second level array is not allocated
	 return NULL, too.  */
      struct pthread_key_data *level2 = THREAD_GETMEM_NC (THREAD_SELF,
							  specific, idx1st);
      if (level2 == NULL)
	/* Not allocated, therefore no data.  */
	return NULL;

      /* There is data.  */
      data = &level2[idx2nd];
    }

  return &data->data;
}

void* internal_function
__libc_tsd_get_internal(unsigned int key)
{
  void **data_addr;

  data_addr = __libc_tsd_address_internal(key);

  if (data_addr)
    return *data_addr;

  return NULL;
}

int internal_function
__libc_tsd_set_internal(unsigned int key, void *value)
{
  struct pthread *self;
  unsigned int idx1st;
  unsigned int idx2nd;
  struct pthread_key_data *level2;

  self = THREAD_SELF;

  /* Special case access to the first 2nd-level block.  This is the
     usual case.  */
  if (__builtin_expect (key < PTHREAD_KEY_2NDLEVEL_SIZE, 1))
    {
      level2 = &self->specific_1stblock[key];

      /* Remember that we stored at least one set of data.  */
      THREAD_SETMEM (self, specific_used, true);
    }
  else
    {
      if (key >= PTHREAD_KEYS_MAX)
	/* Not valid.  */
	return EINVAL;

      idx1st = key / PTHREAD_KEY_2NDLEVEL_SIZE;
      idx2nd = key % PTHREAD_KEY_2NDLEVEL_SIZE;

      /* This is the second level array.  Allocate it if necessary.  */
      level2 = THREAD_GETMEM_NC (self, specific, idx1st);
      if (level2 == NULL)
	{
	  if (value == NULL)
	    /* We don't have to do anything.  The value would in any case
	       be NULL.  We can save the memory allocation.  */
	    return 0;

	  level2
	    = (struct pthread_key_data *) calloc (PTHREAD_KEY_2NDLEVEL_SIZE,
						  sizeof (*level2));
	  if (level2 == NULL)
	    return ENOMEM;

	  THREAD_SETMEM_NC (self, specific, idx1st, level2);
	}

      /* Pointer to the right array element.  */
      level2 = &level2[idx2nd];

      /* Remember that we stored at least one set of data.  */
      THREAD_SETMEM (self, specific_used, true);
    }

  /* Store the data and the sequence number so that we can recognize
     stale data.  */
  level2->seq = 1;
  level2->data = (void *) value;

  return 0;
}
