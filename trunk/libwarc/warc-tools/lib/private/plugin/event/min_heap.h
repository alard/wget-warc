/*
 * Copyright (c) 2006 Maxim Yegorushkin <maxim.yegorushkin@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _MIN_HEAP_H_
#define _MIN_HEAP_H_

#include <event.h>

typedef struct min_heap
  {

    struct event** p;
    unsigned n, a;
  } min_heap_t;



extern int            min_heap_reserve (min_heap_t* s, unsigned n);
extern void           min_heap_shift_up_ (min_heap_t* s, unsigned hole_index, struct event* e);
extern void           min_heap_shift_down_ (min_heap_t* s, unsigned hole_index, struct event* e);
extern int            min_heap_elem_greater (struct event *a, struct event *b);
extern void           min_heap_ctor (min_heap_t* s);
extern void           min_heap_dtor (min_heap_t* s);
extern void           min_heap_elem_init (struct event* e);
extern int            min_heap_empty (min_heap_t* s);
extern unsigned       min_heap_size (min_heap_t* s);

extern struct event*  min_heap_top (min_heap_t* s);
extern int            min_heap_push (min_heap_t* s, struct event* e);

extern struct event*  min_heap_pop (min_heap_t* s);
extern int            min_heap_erase (min_heap_t* s, struct event* e);

#endif /* _MIN_HEAP_H_ */
