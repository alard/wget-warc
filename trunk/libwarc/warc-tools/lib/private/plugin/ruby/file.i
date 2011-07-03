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

/* file.i */
 %{
   #include <wport.h>
   #include <wfile.h>  /* WARC base class structure */
 %}


%apply unsigned int { warc_u32_t }
%apply unsigned long long { warc_u64_t }
  
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

  typedef enum {WARC_FALSE = 0,
                WARC_TRUE  = 1
  } warc_bool_t;

  extern const void * WFile;

  extern warc_bool_t     WFile_hasMoreRecords     (const void * const);
  extern void *          WFile_nextRecord         (void *);
  extern warc_bool_t     WFile_storeRecord        (void *, const void *);
  extern wfile_comp_t    WFile_getCompressionMode (const void * const);

