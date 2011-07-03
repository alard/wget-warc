
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

#ifndef	__WARC_H__
#define	__WARC_H__

/* to mix C and C++ */
#ifdef __cplusplus
extern "C"
  {
#endif



#include <wport.h>       /* portability issues */
#include <wmisc.h>       /* NIL, ARRAY_LEN ... */
#include <wcsafe.h>      /* C safe functions */
#include <wclass.h>      /* bless, destroy */
#include <wstring.h>     /* WString */
#include <wlist.h>       /* WList */
#include <wrecord.h>     /* WRecord */
#include <wanvl.h>       /* WAnvl */
#include <wfile.h>       /* WFile */
#include <wuuid.h>       /* WUUID */
#include <wgetopt.h>     /* WGetOpt */
#include <whash.h>       /* WHash */
#include <wserver.h>     /* WServer */
#include <wclient.h>     /* WClient */
#include <wversion.h>    /* WARC_VERSION */
#include <wrectype.h>    /* WRecord types (i.e. resource, revisit ...) */
#include <wregexp.h>     /* WRegexp */



#ifdef __cplusplus
  }

#endif

#endif /* __WARC_H__ */
