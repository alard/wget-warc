
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

#ifndef	__WARC_CLIENT_H__
#define	__WARC_CLIENT_H__


/* to mix C and C++ */
#ifdef __cplusplus
extern "C"
  {
#endif



    /**
     * Portability header file
     */

#include <wport.h>
#include <wrectype.h>

#include <stdio.h> /* FILE */

    extern const void * WClient;

    extern warc_bool_t WClient_getWRecord (void * const, warc_u64_t, 
                                           const warc_u8_t *, warc_u32_t,
                                           const warc_u8_t * outf);

    extern warc_bool_t WClient_getWFile   (void * const, warc_u64_t,
                                           const warc_u8_t *, warc_u32_t,
                                           const warc_u8_t *);

    extern warc_bool_t WClient_getFiltredWFile (void * const _self,
                                               warc_u64_t , const warc_u8_t *,
                                               warc_u32_t , const warc_u8_t *,
                                               warc_u32_t , const warc_u8_t *,
                                               warc_u32_t , const warc_u8_t *);

    extern warc_bool_t WClient_getList (void * const , 
                                        warc_u64_t ,
                                        const warc_u8_t * ,
                                        warc_u32_t ,
                                        const warc_u8_t *,
                                        warc_u32_t ,
                                        const warc_u8_t * );

#ifdef __cplusplus
  }

#endif


#endif /* __WARC_CLIENT_H__ */
