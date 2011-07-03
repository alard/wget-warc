
/* ------------------------------------------------------------------- */
/* Copyright (c) 2007-2008 Hanzo Archives Limited.                     */
/*                                                                     */
/* Licensed under the Apache License, Version 2.0 (the "License");     */
/* you may not use this file except in compliance with the License.    */
/* You may obtain a copy of the License at                             */
/*                                                                     */
/*     http://www.apache.org/licenses/LICENSE-2.0                      */
/*                                                                     */
/* Unless required by applicable law or agreed to in writing, software */
/* distributed under the License is distributed on an "AS IS" BASIS,   */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or     */
/* implied.                                                            */
/* See the License for the specific language governing permissions and */
/* limitations under the License.                                      */
/*                                                                     */
/* You may find more information about Hanzo Archives at               */
/*                                                                     */
/*     http://www.hanzoarchives.com/                                   */
/*                                                                     */
/* You may find more information about the WARC Tools project at       */
/*                                                                     */
/*     http://code.google.com/p/warc-tools/                            */
/* ------------------------------------------------------------------- */

#ifndef	__WARC_WLIST_H__
#define	__WARC_WLIST_H__

/* to mix C and C++ */
#ifdef __cplusplus
extern "C"
  {
#endif


    /**
     * Portability header file
     */

#include <wport.h>


    extern const void * WList;

    extern warc_bool_t  WList_push       (void * const, void * const);
    extern void *       WList_pop        (void * const);
    extern void *       WList_shift      (void * const);
    extern warc_bool_t  WList_unshift    (void * const, void * const);
    extern warc_u32_t   WList_size       (const void * const);
    extern const void * WList_getElement (const void * const, 
                                          const warc_u32_t);
    extern void       * WList_remove      (void * const, const warc_u32_t);
    extern void       * WList_setElement  (void * const, const warc_u32_t,
                                           void * const);

#ifdef __cplusplus
  }

#endif


#endif /* __WARC_WLIST_H__ */
