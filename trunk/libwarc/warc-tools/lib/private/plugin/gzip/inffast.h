#ifndef	__GZIP_INFFAST_H__
#define	__GZIP_INFFAST_H__


/* to mix C and C++ */
#ifdef __cplusplus
 extern "C" {
#endif


/* inffast.h -- header to use inffast.c
 * Copyright (C) 1995-2003 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */

void inflate_fast OF((z_streamp strm, unsigned start));


#ifndef __INFFAST_H__
#define __INFFAST_H__  1

#define F_INVALID_DISTANCE_TOO_FAR_BACK  801
#define F_INVALID_DISTANCE_CODE          802
#define F_INVALID_LITERAL_LENGTH_CODE    803
/* #define F_  803 */
/* #define F_  804 */
/* #define F_  805 */
/* #define F_  806 */
/* #define F_  807 */
/* #define F_  808 */
/* #define F_  809 */
#endif


#ifdef __cplusplus
 }
#endif


#endif /* __GZIP_INFFAST_H__ */
