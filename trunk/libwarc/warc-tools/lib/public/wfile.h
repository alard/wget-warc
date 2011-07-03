
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

#ifndef	__WFILE_H__
#define	__WFILE_H__


/* to mix C and C++ */
#ifdef __cplusplus
extern "C"
  {
#endif


    /**
     * Portability header file
     */

#include <wport.h>

    extern const void * WFile;

    typedef enum {WARC_FILE_UNCOMPRESSED = 0,
                  WARC_FILE_COMPRESSED_GZIP,
                  WARC_FILE_COMPRESSED_GZIP_DEFAULT_COMPRESSION,
                  WARC_FILE_COMPRESSED_GZIP_NO_COMPRESSION,
                  WARC_FILE_COMPRESSED_GZIP_BEST_SPEED,
                  WARC_FILE_COMPRESSED_GZIP_BEST_COMPRESSION,
                  WARC_FILE_DETECT_COMPRESSION
                 } wfile_comp_t;

    typedef enum {WARC_FILE_INVALID = 0,
                  WARC_FILE_READER,
                  WARC_FILE_WRITER
                 } wfile_mode_t;

    extern warc_bool_t     WFile_hasMoreRecords (const void * const);
    extern void *          WFile_nextRecord     (void *);
    extern warc_bool_t     WFile_isFull          (const void * const);
    extern warc_bool_t     WFile_setMaxSize     (void *, const warc_u64_t);
    extern warc_bool_t     WFile_storeRecord    (void *, const void *);
    extern warc_bool_t     WFile_seek           (void *, const warc_u64_t);
    extern warc_bool_t     WFile_register       (void *, void *,
                                                 warc_bool_t (*)
                                                 (void *, const char *,
                                                  const warc_u32_t),
                                                 void *);



#ifdef __cplusplus
  }

#endif


#endif /* __WFILE_H__ */
