
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

#ifndef	__WARC_METHOD_BITS_H__
#define	__WARC_METHOD_BITS_H__

/* to mix C and C++ */
#ifdef __cplusplus
 extern "C" {
#endif


/**
 * Portability header file
 */

#include <wport.h>


#define DEFLATE_METHOD      0
#define GZIP_METHOD         1

warc_u32_t methodBits (warc_u32_t);


#ifndef __RFC1952_H__
# define __RFC1952_H__

/* Optional fields */
#define FLG_FHCRC           0x02
#define FLG_FEXTRA          0x04
#define FLG_FNAME           0x08
#define FLG_FCOMMENT        0x10
#define FLG_RESERVED        0xE0


/* Fixed field sizes */
#define GZIP_STATIC_HEADER_SIZE  10


/* Fixed field offsets */
#define OFF_ID1             0
#define OFF_ID2             1
#define OFF_CM              2
#define OFF_FLG             3
#define OFF_MTIME           4 /* from 4 to 7 */
#define OFF_XFL             8
#define OFF_OS              9

/* Miscellaneous constants */
#define ID1                 0x1F
#define ID2                 0x8B
#define CM                  0x08

/* XFL values */
#define XFL_MAX             2
#define XFL_FASTEST         4

/* OS values */
#define  OS_FAT             0
#define  OS_AMIGA           1
#define  OS_VMS             2
#define  OS_UNIX            3
#define  OS_VM_CMS          4
#define  OS_ATARI_TOS       5
#define  OS_HPFS            6
#define  OS_MACINTOSH       7
#define  OS_Z_SYSTEM        8
#define  OS_CPM             9
#define  OS_TOPS20          10
#define  OS_NTFS            11
#define  OS_QDOS            12
#define  OS_RISCOS          13
#define  OS_UNKNOWN         255 

#endif /* __RFC1952_H__ */


#define  Z_RESERVED_GZIP              22
#define  Z_FSEEK_ERROR                23
#define  Z_HEADER_ERROR               24
#define  Z_READING_ERROR              25
#define  Z_WRITING_ERROR              26
#define  Z_INPUT_FILENAME             27
#define  Z_OUTPUT_FILENAME            28
#define  Z_SEE_MAN                    29
#define  Z_EOF                        30
#define  Z_REWIND_ERROR               31
#define  Z_ADJUST_CSIZE_ERROR         32
#define  Z_UNKNOWN_DECOMPRESSION_MODE 33
#define  Z_DISK_CACHE_READING_ERROR   34
#define  Z_DISK_CACHE_WRITING_ERROR   35
#define  Z_DISK_CACHE_REWIND_ERROR    36
#define  Z_WARC_FLUSH_ERROR           37 
#define  Z_WARC_EXTRA_FIELD_ERROR     38

#define  CMD_MAX_LINE                 1000
#define  FOOTER_CRC32_ISIZE           8

#define  MAX_RBITS                    15          /* 32K LZ77 window */

#ifndef	 INPUT_BUFFSIZE

/*
 * IMPORTANT NOTE : 
 * we need at least this values for buffers size 
 * (see inffast.c and http://www.zlib.org/zlib_tech.html):
 * #define  INPUT_BUFFSIZE       16 * 1024
 * #define  OUTPUT_BUFFSIZE      64 * 1024
 */

#define  INPUT_BUFFSIZE       16 * 1024
#define  OUTPUT_BUFFSIZE      64 * 1024

/* if we have enough memory, we could use: */
/* #define  INPUT_BUFFSIZE       16 * 64 * 1024 */
/* #define  OUTPUT_BUFFSIZE      16 * 64 * 1024 */

#endif /* __RFC1952_H__ */


#ifdef __cplusplus
 }
#endif


#endif /* __WARC_METHOD_BITS_H__ */
