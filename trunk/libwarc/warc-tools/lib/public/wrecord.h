
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
    extern warc_rec_t         WRecord_getRecordType  (const void * const);
    extern const warc_u8_t *  WRecord_getTargetUri   (const void * const);
    extern const warc_u8_t *  WRecord_getDate        (const void * const);
    extern const warc_u8_t *  WRecord_getContentType (const void * const);
    extern const warc_u8_t *  WRecord_getRecordId    (const void * const);
    extern warc_bool_t        WRecord_getContent     (const void * const);
    extern const warc_u8_t *  WRecord_getAnvlValue   (const void * const,
                                                      const warc_u8_t *);
    extern        warc_u64_t  WRecord_getContentLength   (const void * const);
    extern const  warc_u8_t * WRecord_getConcurrentTo    (const void * const);
    extern const  warc_u8_t * WRecord_getBlockDigest     (const void * const);
    extern const  warc_u8_t * WRecord_getPayloadDigest   (const void * const);
    extern const  warc_u8_t * WRecord_getIpAddress       (const void * const);
    extern const  warc_u8_t * WRecord_getRefersTo        (const void * const);
    extern const  warc_u8_t * WRecord_getTruncated       (const void * const);
    extern const  warc_u8_t * WRecord_getWarcInfoId      (const void * const);
    extern const  warc_u8_t * WRecord_getProfile         (const void * const);
    extern const  warc_u8_t * WRecord_getPayloadType     (const void * const);
    extern const  warc_u8_t * WRecord_getSegmentOriginId (const void * const);
    extern  warc_u32_t  WRecord_getSegmentNumber         (const void * const);
    extern  warc_u32_t WRecord_getSegTotalLength         (const void * const);
    extern const warc_u8_t * WRecord_getFilename         (const void * const);


    extern const void * WRecord_getAnvl             (const void * const);
    extern warc_u64_t   WRecord_getOffset           (const void * const);
    extern warc_u64_t   WRecord_getUncompressedSize (const void * const);
    extern warc_u64_t   WRecord_getCompressedSize   (const void * const);
   
    extern warc_bool_t  WRecord_getAnvlField        (const void * const, 
                                                     const warc_u32_t , 
                                                     const warc_u8_t **, 
                                                     const warc_u8_t **);
    
    extern warc_u32_t WRecord_getAnvlFieldsNumber (const void * const );
    
    extern void * WRecord_getBloc (void *, void *,  warc_bool_t , warc_u8_t * );




    extern warc_bool_t   WRecord_setRecordType   (void *, const warc_rec_t);
    extern warc_bool_t   WRecord_setTargetUri    (void *, const warc_u8_t *,
                                                  const warc_u32_t);
    extern warc_bool_t   WRecord_setDate         (void *, const warc_u8_t *,
                                                  const warc_u32_t);
    extern warc_bool_t   WRecord_setContentType  (void *, const warc_u8_t *,
                                                  const warc_u32_t );
    extern warc_bool_t   WRecord_setRecordId     (void *, const warc_u8_t *,
                                                  const warc_u32_t );

    extern  warc_bool_t WRecord_setConcurrentTo  (void *,  const warc_u8_t *,
                                                  const warc_u32_t );

    extern warc_bool_t WRecord_setBlockDigest    (void *, const warc_u8_t *,
                                                  const warc_u32_t);

    extern warc_bool_t WRecord_setPayloadDigest  (void *, const warc_u8_t *,
                                                  const warc_u32_t);

    extern warc_bool_t  WRecord_setIpAddress     (void *, const warc_u8_t *,
                                                  const warc_u32_t );

    extern warc_bool_t  WRecord_setRefersTo      (void *,  const warc_u8_t *,
                                                  const warc_u32_t);

    extern warc_bool_t  WRecord_setTruncated    (void *, const warc_u8_t *,
                                                 const warc_u32_t );
    
    extern warc_bool_t  WRecord_setWarcInfoId   (void *, const warc_u8_t *,
                                                 const warc_u32_t );

    extern warc_bool_t  WRecord_setProfile      (void *, const warc_u8_t *,
                                                 const warc_u32_t);

    extern warc_bool_t  WRecord_setPayloadType  (void *, const warc_u8_t *,
                                                 const warc_u32_t );

    extern warc_bool_t WRecord_setSegmentOriginId (void *, const warc_u8_t *,
                                                   const warc_u32_t );

    extern warc_bool_t WRecord_setSegmentNumber  (void *,  const warc_u32_t);

    extern warc_bool_t WRecord_setSegTotalLength (void *, const warc_u32_t);

    extern warc_bool_t  WRecord_setFilename      (void *, const warc_u8_t *,
                                                  const warc_u32_t );

    extern warc_bool_t WRecord_addAnvl           (void *, const warc_u8_t *,
                                                  const warc_u32_t, 
                                                  const warc_u8_t *,
                                                  const warc_u32_t);
    
    extern warc_bool_t WRecord_setContentFromFileName (void *, const char *);
    
    extern warc_bool_t WRecord_setContentFromFile (void *, FILE *);
    
    extern warc_bool_t WRecord_setDateFromArc       (void *  ,
                                                     const warc_u8_t * ,
                                                     const warc_u32_t );
 
    extern warc_bool_t WRecord_setContentFromString (void *, const warc_u8_t *,
                                                     const warc_u32_t);
    
    extern warc_bool_t  WRecord_setContentFromStringConcat (void *,
                                                            const warc_u8_t *,
                                                            const warc_u32_t);

#ifdef __cplusplus
  }

#endif


#endif /* __WARC_WRecord_H__ */
