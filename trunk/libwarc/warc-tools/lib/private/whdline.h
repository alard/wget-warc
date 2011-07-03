
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

#ifndef	__WARC_HEADER_LINE_H__
#define	__WARC_HEADER_LINE_H__

/* to mix C and C++ */
#ifdef __cplusplus
extern "C"
  {
#endif

    /**
     * Portability header file
     */

#include <wport.h>

    extern const void * WHDLine;

#include <wrectype.h>


    /* getters */
    extern const warc_u8_t *  WHDLine_getWarcId (const void * const);
    extern warc_u32_t       WHDLine_getWarcIdLen       (const void * const);
    extern warc_u32_t       WHDLine_getDataLength      (const void * const);
    extern warc_rec_t       WHDLine_getRecordType      (const void * const);
    extern const warc_u8_t * WHDLine_getSubjectUri      (const void * const);
    extern warc_u32_t       WHDLine_getSubjectUriLen   (const void * const);
    extern const warc_u8_t * WHDLine_getCreationDate    (const void * const);
    extern warc_u32_t       WHDLine_getCreationDateLen (const void * const);
    extern const warc_u8_t * WHDLine_getContentType     (const void * const);
    extern warc_u32_t       WHDLine_getContentTypeLen  (const void * const);
    extern const warc_u8_t * WHDLine_getRecordId        (const void * const);
    extern warc_u32_t       WHDLine_getRecordIdLen     (const void * const);

    /* setters */
    extern warc_bool_t WHDLine_setWarcId       (void * const,
          const warc_u8_t *,
          const warc_u32_t len);
    extern warc_bool_t WHDLine_setDataLength      (void * const,
          const warc_u32_t);
    extern warc_bool_t WHDLine_setRecordType      (void * const,
          const warc_rec_t);
    extern warc_bool_t WHDLine_setSubjectUri   (void * const ,
          const warc_u8_t *,
          const warc_u32_t);
    extern warc_bool_t WHDLine_setCreationDate (void * const ,
          const warc_u8_t *,
          const warc_u32_t);
    extern warc_bool_t WHDLine_setContentType  (void * const,
          const warc_u8_t *,
          const warc_u32_t);
    extern warc_bool_t WHDLine_setRecordId  (void * const,
          const warc_u8_t *,
          const warc_u32_t);


#ifdef __cplusplus
  }

#endif

#endif /* __WARC_STRING_H__ */
