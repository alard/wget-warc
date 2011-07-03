
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

#ifndef	__WARC_WGZIP_H__
#define	__WARC_WGZIP_H__

/* to mix C and C++ */
#ifdef __cplusplus
extern "C"
  {
#endif


    /**
     * Portability header file
     */

#include <wport.h>


#include <stdio.h> /* for FILE */

    extern const void * WGzip;

    /* compression levels */
    typedef enum {WARC_GZIP_DEFAULT_COMPRESSION = 0,
                  WARC_GZIP_NO_COMPRESSION,
                  WARC_GZIP_BEST_SPEED,
                  WARC_GZIP_BEST_COMPRESSION
                 } warc_gzlevel_t;

    /* uncompression flags control */
    enum {Z_CONTINUE_DECODING = 0};
    enum {Z_STOP_DECODING = 111};



    extern warc_i32_t WGzip_compress   (const void * const,
                                          FILE *, FILE *,
                                          const warc_gzlevel_t,
                                          warc_u64_t *);

    extern warc_u32_t WGzip_uncompress (const void * const,
                                          FILE *, warc_u64_t,
                                          warc_u64_t *,
                                          warc_u64_t *,
                                          warc_u32_t (*) (const warc_u8_t *,
                                                          const warc_u32_t,
                                                          void *),
                                          void *);
    extern warc_bool_t WGzip_check     (const void *,  FILE *, warc_u64_t);

    extern warc_bool_t WGzip_analyzeHeader (const void *,  FILE *,
                                              warc_u64_t, warc_u64_t *,
                                              warc_u64_t *);


#ifdef __cplusplus
  }

#endif


#endif /* __WARC_WGZIP_H__ */
