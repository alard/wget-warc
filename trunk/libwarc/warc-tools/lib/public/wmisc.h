
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

#ifndef	__WARC_MISC_H__
#define	__WARC_MISC_H__


/* to mix C and C++ */
#ifdef __cplusplus
extern "C"
  {
#endif

    /*
     * C default headers
     */

#include <wcsafe.h>


    /* Pointer to NULL */
#define NIL    (void *)0

    /* Perl if not */
#define unless(C) if(!(C))

    /* INLINE macro */
#ifndef WINLINE
#define WINLINE inline /* or "extern inline" */
#else
#define WINLINE
#endif

    /* Quikly find functions and protect them */
#ifndef WPRIVATE
#define WPRIVATE static
#endif

#ifndef WIPRIVATE
#define WIPRIVATE static WINLINE
#endif

#ifndef WPUBLIC
#define WPUBLIC
#endif

#ifndef WIPUBLIC
#define WIPUBLIC WINLINE
#endif

    /* To disable warnings about unused variable */
#define UNUSED(obj) ((void) obj)

    /* Get array size */
#define ARRAY_LEN(array) (sizeof(array)/sizeof(array[0]))

    /* forever loops */
#define EVER ;;

    /* print message tp STDERR */
#ifndef WARC_MSG_DEBUG
#define WARC_MSG_DEBUG
#define WarcDebugMsg(m) \
    (fprintf(stderr, "> debug point: caller<" __FILE__ ":%s"  ":%d>" #m "\n", __FUNCTION__, __LINE__))
#else
#define WarcDebugMsg(m) (m)
#endif

    /* Trace any function before calling it */
#ifndef WARC_MSG_DEBUG_ACTION
#define WARC_MSG_DEBUG_ACTION
#define WarcDebugMsgAction(m) \
  (fprintf(stderr, "> debug action: caller<" __FILE__ ":%s:%d> callee<" #m ">\n",  __FUNCTION__, __LINE__), (m))
#else
#define WarcDebugMsgAction(m) (m)
#endif



#ifdef __cplusplus
  }

#endif


#endif /* __WARC_MISC_H__ */
