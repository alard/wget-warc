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

/* record.i */
%include "typemaps.i"
 %{
   #include <wport.h>
   #include <wrectype.h>
   #include <wrecord.h>
 %}

%apply unsigned int { warc_u32_t }
%apply unsigned long long { warc_u64_t }

/* %typemap(out) warc_u64_t { */
/*     $result = (unsigned long long) PyLong_FromUnsignedLongLong((warc_u64_t) $1); */
/* } */

    typedef enum
    {
      WARC_UNKNOWN_RECORD = 0,
      WARC_INFO_RECORD,
      WARC_RESPONSE_RECORD,
      WARC_REQUEST_RECORD,
      WARC_METADATA_RECORD,
      WARC_REVISIT_RECORD,
      WARC_CONVERSION_RECORD,
      WARC_CONTINUATION_RECORD,
      WARC_RESOURCE_RECORD
     } warc_rec_t;

    extern const void * WRecord;

    extern const char *  WRecord_getWarcId           (const void * const);
    extern warc_rec_t    WRecord_getRecordType       (const void * const);
    extern const char *  WRecord_getTargetUri        (const void * const);
    extern const char *  WRecord_getDate             (const void * const);
    extern const char *  WRecord_getContentType      (const void * const);
    extern const char *  WRecord_getRecordId         (const void * const);
    extern       int     WRecord_getContent          (const void * const);
    extern const char *   WRecord_getAnvlValue       (const void * const,
                                                     const char *);
    extern  warc_u64_t  WRecord_getContentLength     (const void * const );
    extern const   char * WRecord_getConcurrentTo    (const void * const );
    extern const   char * WRecord_getBlockDigest     (const void * const   );
    extern const   char * WRecord_getPayloadDigest   (const void * const );
    extern const   char * WRecord_getIpAddress       (const void * const );
    extern const   char * WRecord_getRefersTo        (const void * const );
    extern const   char * WRecord_getTruncated       (const void * const );
    extern const   char * WRecord_getWarcInfoId      (const void * const );
    extern const   char * WRecord_getProfile         (const void * const );
    extern const   char * WRecord_getPayloadType     (const void * const);
    extern const   char * WRecord_getSegmentOriginId (const void * const);
    extern  unsigned int  WRecord_getSegmentNumber   (const void * const );
    extern  unsigned int  WRecord_getSegTotalLength  (const void * const );
    extern const char *   WRecord_getFilename        (const void * const );


    extern const void * WRecord_getAnvl         (const void * const);


    extern int   WRecord_setRecordType   (void *, const warc_rec_t);
    extern int   WRecord_setTargetUri    (void *, const char *,
                                          const unsigned int);
    extern int   WRecord_setDate         (void *, const char *,
                                          const unsigned int);
    extern int   WRecord_setContentType  (void *, const char *,
                                          const unsigned int );
    extern int   WRecord_setRecordId     (void *, const char *,
                                          const unsigned int );

    extern  int WRecord_setConcurrentTo  (void *,  const char *,
                                          const unsigned int );

    extern int WRecord_setBlockDigest    (void *, const char *,
                                          const unsigned int);

    extern int WRecord_setPayloadDigest  (void *, const char *,
                                          const unsigned int);

    extern int  WRecord_setIpAddress     (void *, const char *,
                                          const unsigned int );

    extern int  WRecord_setRefersTo (void *,  const char *,
                                     const unsigned int);

    extern int  WRecord_setTruncated (void *, const char *,
                                      const unsigned int );

    extern int   WRecord_setWarcInfoId (void *, const char *,
                                        const unsigned int );

    extern int   WRecord_setProfile (void *, const char *,
                                     const unsigned int );

    extern int  WRecord_setPayloadType (void *, const char *,
                                        const unsigned int );

    extern int   WRecord_setSegmentOriginId (void *, const char *,
                                             const unsigned int );

    extern int    WRecord_setSegmentNumber (void *,  const unsigned int);

    
    extern int    WRecord_setSegTotalLength (void *, const unsigned int);

    extern int    WRecord_setFilename (void *, const char *,
                                       const unsigned int );

    extern int WRecord_setContentFromString (void *, const char *,
                                             const unsigned int);
    
    extern int  WRecord_setContentFromStringConcat (void *,
                                                    const char *,
                                                    const unsigned int);


    extern int   WRecord_addAnvl         (void *, const char *,
                                          const unsigned int, const char *,
                                          const unsigned int);

    extern int   WRecord_setContentFromFileName (void *, const char *);

    extern warc_u64_t    WRecord_getOffset           (const void * const );
    extern warc_u64_t    WRecord_getUncompressedSize (const void * const );
    extern warc_u64_t    WRecord_getCompressedSize   (const void * const);

    extern int WRecord_setDateFromArc   (void *  ,
                                         const char * ,
                                         const unsigned int );

    extern unsigned int WRecord_getAnvlFieldsNumber (const void * const );

    extern int WRecord_getAnvlField (const void * const , const unsigned int, 
                                     const char ** , const char **);
