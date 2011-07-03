
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

#ifndef	__WARC_HEADER_H__
#define	__WARC_HEADER_H__

/* to mix C and C++ */
#ifdef __cplusplus
extern "C"
  {
#endif

    /**
     * Portability header file
     */

#include <wport.h>

    extern const void * WHeader;

#include <wrectype.h>


    /* getters */
/*    extern const warc_u8_t *  WHDLine_getWarcId (const void * const);
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
    extern warc_u32_t       WHDLine_getRecordIdLen     (const void * const);*/

    /* setters */
/*    extern warc_bool_t WHDLine_setWarcId       (void * const,
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
          const warc_u32_t);*/


    extern const warc_u8_t * WHeader_getWarcId (const void * const );

    extern warc_u32_t WHeader_getWarcIdLen (const void * const );

    extern warc_u64_t WHeader_getContentLength (const void * const );
 
    extern warc_bool_t WHeader_setContentLength (void * const , 
                                                 const warc_u64_t );

    extern warc_u64_t WHeader_getDataLength (const void * const );

    extern warc_bool_t WHeader_setDataLength (void * const _self, 
                                              const warc_u64_t );

    extern warc_rec_t WHeader_getRecordType (const void * const );

    extern warc_bool_t WHeader_setRecordType (void * const , const warc_rec_t );

    extern const warc_u8_t * WHeader_getSubjectUri (const void * const );

    extern warc_u32_t WHeader_getSubjectUriLen (const void * const );

    extern warc_bool_t WHeader_setSubjectUri (void * const ,
                                            const warc_u8_t * ,
                                            const warc_u32_t );

    extern const warc_u8_t * WHeader_getCreationDate (const void * const );

    extern warc_u32_t WHeader_getCreationDateLen (const void * const );

    extern warc_bool_t WHeader_setCreationDate (void * const ,
                                              const warc_u8_t * ,
                                              const warc_u32_t );

    extern const warc_u8_t * WHeader_getContentType (const void * const );

    extern warc_u32_t WHeader_getContentTypeLen (const void * const );

    extern warc_bool_t WHeader_setContentType (void * const ,
                                             unsigned const char * ,
                                             const warc_u32_t );

    extern const warc_u8_t * WHeader_getRecordId (const void * const );

    extern warc_u32_t WHeader_getRecordIdLen (const void * const );

    extern warc_bool_t WHeader_setRecordId (void * const ,
                                         const warc_u8_t * ,
                                         const warc_u32_t );

    extern const warc_u8_t * WHeader_getConcurrentTo (const void * const );

    extern warc_u32_t WHeader_getConcurrentToLen (const void * const );

    extern warc_bool_t WHeader_setConcurrentTo (void * const ,
                                             const warc_u8_t * ,
                                             const warc_u32_t );

    extern const warc_u8_t * WHeader_getBlockDigest (const void * const );

    extern warc_u32_t WHeader_getBlockDigestLen (const void * const );

    extern warc_bool_t WHeader_setBlockDigest (void * const ,
                                            const warc_u8_t * ,
                                            const warc_u32_t );

    extern const warc_u8_t * WHeader_getPayloadDigest (const void * const );

    extern warc_u32_t WHeader_getPayloadDigestLen (const void * const );

    extern warc_bool_t WHeader_setPayloadDigest (void * const ,
                                              const warc_u8_t * ,
                                              const warc_u32_t );

    extern const warc_u8_t * WHeader_getIpAddress (const void * const );

    extern warc_u32_t WHeader_getIpAddressLen (const void * const );

    extern warc_bool_t WHeader_setIpAddress (void * const ,
                                         const warc_u8_t * ,
                                         const warc_u32_t );

    extern const warc_u8_t * WHeader_getRefers (const void * const );

    extern warc_u32_t WHeader_getRefersToLen (const void * const );

    extern warc_bool_t WHeader_setRefersTo (void * const ,
                                         const warc_u8_t * ,
                                         const warc_u32_t );

    extern const warc_u8_t * WHeader_getTruncated (const void * const );

    extern warc_u32_t WHeader_getTruncatedLen (const void * const );

    extern warc_bool_t WHeader_setTruncated (void * const ,
                                          const warc_u8_t * ,
                                          const warc_u32_t );

    extern const warc_u8_t * WHeader_getInfoId (const void * const );

    extern warc_u32_t WHeader_getInfoIdLen (const void * const );

    extern warc_bool_t WHeader_setInfoId (void * const ,
                                       const warc_u8_t * ,
                                       const warc_u32_t );

    extern const warc_u8_t * WHeader_getFilename (const void * const );

    extern warc_u32_t WHeader_getFilenameLen (const void * const );

    extern warc_bool_t WHeader_setFilename (void * const ,
                                            const warc_u8_t * ,
                                            const warc_u32_t );


    extern const warc_u8_t * WHeader_getProfile (const void * const );

    extern warc_u32_t WHeader_getProfileLen (const void * const );

    extern warc_bool_t WHeader_setProfile (void * const ,
                                        const warc_u8_t * ,
                                        const warc_u32_t );

    extern const warc_u8_t * WHeader_getPayloadType (const void * const );

    extern warc_u32_t WHeader_getPayloadTypeLen (const void * const );

    extern warc_bool_t WHeader_setPayloadType (void * const ,
                                            const warc_u8_t * ,
                                            const warc_u32_t );

    extern const warc_u8_t * WHeader_getSegmentOriginId (const void * const );

    extern warc_u32_t WHeader_getSegmentOriginIdLen (const void * const );

    extern warc_bool_t WHeader_setSegmentOriginId (void * const ,
                                                  const warc_u8_t * ,
                                                  const warc_u32_t );

    extern warc_u32_t WHeader_getSegmentNumber (const void * const );

    extern warc_bool_t WHeader_setSegmentNumber (void * const , const warc_u32_t );

    extern warc_u32_t WHeader_getSegTotalLength (const void * const );

    extern warc_bool_t WHeader_setSegTotalLength (void * const , const warc_u32_t );

    extern const warc_u8_t * WHeader_getAnvlValue (const void* const ,  const warc_u8_t * );

    extern warc_bool_t WHeader_addAnvl (void * ,
                                        const warc_u8_t * ,
                                        const warc_u32_t ,
                                        const warc_u8_t * ,
                                        const warc_u32_t );

    extern const void * WHeader_getAnvl (const void* const _self);

    extern warc_bool_t WHeader_extractFromWarcFile (void * , FILE * );


#ifdef __cplusplus
  }

#endif

#endif /* __WARC_HEADER_H__ */
