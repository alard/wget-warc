
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

#ifndef	__ARC_RECORD_X__
#define	__ARC_RECORD_X__


/* to mix C and C++ */
#ifdef __cplusplus
extern "C"
  {
#endif



    /**
     * Portability header file
     */

#include <wport.h>



    extern const void * ARecord;

    /* getters */

    extern warc_u64_t ARecord_getDataLength   (const void * const);
    extern FILE *     ARecord_getDataFile     (const void * const);
    extern warc_u64_t ARecord_getDataSize     (const void * const);
    extern warc_u64_t ARecord_getRecordOffset (const void * const);
    extern void *     ARecord_fromWho         (const void * const);

    /* setters */

    extern warc_bool_t ARecord_setDataLength   (void * const, const warc_u64_t);
    extern warc_bool_t ARecord_setUrl          (void * const, const warc_u8_t *,
          const warc_u32_t);
    extern warc_bool_t ARecord_setCreationDate (void * const, const warc_u8_t *,
          const warc_u32_t);
    extern warc_bool_t ARecord_setMimeType     (void * const, const warc_u8_t *,
          const warc_u32_t);
    extern warc_bool_t  ARecord_setIpAddress   (void * const, const warc_u8_t *,
          const warc_u32_t);
    extern warc_bool_t ARecord_setContentFromFile (void *, void *);
    extern warc_bool_t ARecord_setContentSize  (void *, warc_u64_t);
    extern warc_bool_t ARecord_setRecordOffset (void *, const warc_u64_t);
    extern warc_bool_t ARecord_setEnv          (void *, void *);
    extern warc_bool_t ARecord_setAfile        (void *, void *);


#ifdef __cplusplus
  }

#endif


#endif /* __ARC_RECORD_X__ */
