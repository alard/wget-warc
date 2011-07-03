
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

#ifndef	__WARC_WRECORD_H__
#define	__WARC_WRECORD_H__


/* to mix C and C++ */
#ifdef __cplusplus
extern "C"
  {
#endif



    /**
     * Portability header file
     */

#include <wport.h>
#include <wrectype.h>

#include <stdio.h> /* FILE */

    extern const void * WRecord;

    extern const warc_u8_t *  WRecord_getWarcId      (const void * const);
    extern warc_u32_t    WRecord_getDataLength  (const void * const);
    extern warc_rec_t    WRecord_getRecordType  (const void * const);
    extern const warc_u8_t *  WRecord_getSubjectUri  (const void * const);
    extern const warc_u8_t *  WRecord_getCreationDate (const void * const);
    extern const warc_u8_t *  WRecord_getContentType (const void * const);
    extern const warc_u8_t *  WRecord_getRecordId    (const void * const);
    extern warc_bool_t   WRecord_getContent     (const void * const);
    extern const warc_u8_t *   WRecord_getAnvlValue   (const void * const,
          const warc_u8_t *);
    extern const void * WRecord_getAnvl         (const void * const);


    extern warc_bool_t   WRecord_setRecordType   (void *, const warc_rec_t);
    extern warc_bool_t   WRecord_setSubjectUri   (void *, const warc_u8_t *,
          const warc_u32_t);
    extern warc_bool_t   WRecord_setCreationDate (void *, const warc_u8_t *,
          const warc_u32_t);
    extern warc_bool_t   WRecord_setContentType  (void *, const warc_u8_t *,
          const warc_u32_t );
    extern warc_bool_t   WRecord_setRecordId     (void *, const warc_u8_t *,
          const warc_u32_t );
    extern warc_bool_t   WRecord_addAnvl         (void *, const warc_u8_t *,
          const warc_u32_t, const warc_u8_t *,
          const warc_u32_t);
    extern warc_bool_t   WRecord_setContentFromFileName (void *, const char *);

    extern warc_i64_t    WRecord_getOffset           (const void * const );
    extern warc_u64_t    WRecord_getUncompressedSize (const void * const );
    extern warc_u64_t    WRecord_getCompressedSize   (const void * const);


#ifdef __cplusplus
  }

#endif


#endif /* __WARC_WRecord_H__ */
