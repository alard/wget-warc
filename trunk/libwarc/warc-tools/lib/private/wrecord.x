
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

#ifndef __WARC_RECORD_X_H__
#  define __WARC_RECORD_X_H__

/* to mix C and C++ */
#ifdef __cplusplus
extern "C"
  {
#endif


#include <stdlib.h> /* FILE */

    extern warc_u64_t    WRecord_getDataLength  (const void * const);

    extern warc_bool_t  WRecord_check             (const void * const);
    extern FILE       * WRecord_getDataFile       (const void * const);
    extern FILE       * WRecord_getDataFileExtern (const void * const);
    extern const void * WRecord_getHeader         (const void * const);
    extern const void * WRecord_getHDLine         (const void * const);
    extern warc_u64_t   WRecord_getDataSize       (const void * const);
    extern void       * WRecord_setHeader         (void * const, void *);
    extern void       * WRecord_setHDLine         (void * const, void *);
    extern void       * WRecord_setAnvl           (void * const, void * const);
    extern const void * WRecord_getAnvl           (const void * const );
    extern warc_bool_t WRecord_setContentSize     (void *, warc_u64_t);
    extern warc_bool_t WRecord_setContentFromFileHandle (void *, void *);
    extern warc_bool_t WRecord_setWoffset         (void *, warc_u64_t);
    extern warc_u64_t  WRecord_getWoffset         (const void * const);
    extern warc_bool_t WRecord_setEnv             (void *, void * );
    extern warc_bool_t WRecord_setWfile           (void * , void * );
    extern void      * WRecord_fromWho            (const void * const );
    extern warc_bool_t WRecord_setCallback        (void * _self,
          warc_bool_t (* cback)
          (void* env, const char * buff,
           warc_u32_t size) );

    extern warc_bool_t WRecord_getCHeaderPresent  (const void * const );
    extern void WRecord_setCHeaderPresence        (void * , const warc_bool_t );
    extern warc_bool_t WRecord_setContentFromArc  (void * , void *);

    extern warc_bool_t WRecord_setWFileOffset (void * , warc_u64_t);
    extern warc_bool_t WRecord_setUncompressedSize (void * , warc_u64_t);
    extern warc_bool_t WRecord_setCompressedSize (void * , warc_u64_t);
    extern warc_bool_t WRecord_makeDataFile      (void *, const warc_u8_t *, 
                                                 const warc_u32_t);

#ifdef __cplusplus
  }

#endif


#endif /* __WARC_RECORD_X_H__ */
