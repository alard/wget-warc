
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

#ifndef	__WARC_HTTRACK_H__
#define	__WARC_HTTRACK_H__


/* to mix C and C++ */
#ifdef __cplusplus
extern "C"
  {
#endif

    /**
     * Portability header file
     */

#include <wport.h>

#define HTTRACK_DEFAULT_TMPDIR     "."
#define HTTRACK_WARC_SIZE          (1024 * 1024 * 1024)

    extern void   destroyWARC  (void *);
    
    extern void * blessWARC    (const char *);
    
    extern warc_bool_t  writeRequest  (const char *, const char *, void *);

    extern warc_bool_t  writeResponse (const char *, const char *, void *);

    extern warc_bool_t  writeResource (const char *, const char *,
                                       const char *, void *);

    extern void   setURL       (const char *, const char *, void *);
    extern void   setMIME      (const char *, void *);
    extern void   setIP        (const char *, void *);

#ifdef __cplusplus
  }

#endif


#endif /* __WARC_HTTRACK_H__ */
