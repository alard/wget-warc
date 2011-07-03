
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

#ifndef	__WARC_RECORD_TYPE_H__
#define	__WARC_RECORD_TYPE_H__


/* to mix C and C++ */
#ifdef __cplusplus
extern "C"
  {
#endif


    /**
     * Portability header file
     */

#include <wport.h>

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



#ifdef __cplusplus
  }

#endif

#endif /* __WARC_RECORD_TYPE_H__ */
