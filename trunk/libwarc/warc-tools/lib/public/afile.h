
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

#ifndef	__ARC_FILE_H__
#define	__ARC_FILE_H__

/* to mix C and C++ */
#ifdef __cplusplus
extern "C"
  {
#endif


    /**
     * Portability header file
     */

#include <wport.h>



    extern const void * AFile;

    typedef enum {ARC_FILE_UNCOMPRESSED = 0,
                  ARC_FILE_COMPRESSED_GZIP,
                  ARC_FILE_DETECT_COMPRESSION
                 } afile_comp_t;

    extern void *          AFile_nextRecord     (void *);
    extern warc_u64_t      AFile_getContentSize (const void * const);
    extern warc_bool_t     AFile_hasMoreRecords (const void * const);
    extern warc_bool_t     AFile_seek           (void *, const warc_u64_t);
    extern warc_bool_t     AFile_register       (void *, void *,
          warc_bool_t (*)
          (void*, const char *,
           const warc_u32_t),
          void *);


#ifdef __cplusplus
  }

#endif


#endif /* __ARC_FILE_H__ */
