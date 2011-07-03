
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

#ifndef	__WARC_C_SAFE_H__
#define	__WARC_C_SAFE_H__


/* to mix C and C++ */
#ifdef __cplusplus
extern "C"
  {
#endif

    /**
     * Portability header file
     */

#include <wport.h>


    /**
     * C default headers
     */

#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


    /**
     * Macro to test error stream
     */

#ifndef w_ferror
#define w_ferror(s) ferror((s))
#endif



    /**
     * Macro to test EOF
     */

#ifndef w_feof
#define w_feof(s) feof((s))
#endif

    /**
     * Macro to open files in read binary mode
     */

#ifndef w_fopen_rb
#define w_fopen_rb(s) fopen ((s), "rb")
#endif

    /**
     * Macro to open files in append binary mode
     */

#ifndef w_fopen_ab
#define w_fopen_ab(s) fopen ((s), "ab")
#endif

    /**
     * Macro to open files in append binary mode
     */


#ifndef w_fopen_wb
#define w_fopen_wb(s) fopen ((s), "w+b")
#endif


    /**
     * Macro to read in files
     */

#ifndef w_fread
#define w_fread(buff, nbel, size, file) fread (buff, nbel, size, file)
#endif

    /**
     * Macro to saftely write saftely in files
     */

#ifndef w_fwrite
#define w_fwrite(buff, nbel, size, file) fwrite (buff, nbel, size, file)
#endif


    /**
     * Macro to safetly close a file
    */

#ifndef w_fclose
#define w_fclose(file) \
  do { \
      int ret = fclose (file); \
      assert (! ret); \
      UNUSED (ret); \
      file = NIL; \
    } while (0)
#endif


    /**
     * Macro to saftely use seek (32 bits and 64 bits architecture)
     */

#ifndef w_fseek
#define w_fseek fseek
#endif

    /**
     * Macro to get current offset of o file
     */

#ifndef w_ftell
#define w_ftell(file) ftell(file)
#endif


    /**
     * Macro to saftely seek the begining of a file
     */

#ifndef w_fseek_start
#define w_fseek_start(file) \
  do { \
      int ret = w_fseek (file, 0, SEEK_SET); \
      assert (! ret); \
      UNUSED (ret); \
    } while (0)
#endif

    /**
     * Macro to safetly reach an offset from the begining of a file
     */

#ifndef w_fseek_from_start
#define w_fseek_from_start(file, offset) \
  do { \
      int ret = w_fseek (file, offset, SEEK_SET); \
      assert (! ret); \
      UNUSED (ret); \
    } while (0)
#endif

    /**
     * Macro to safetly reach the end of a file
     */

#ifndef w_fseek_end
#define w_fseek_end(file); \
  do { \
      int ret = w_fseek (file, 0, SEEK_END); \
      assert (! ret); \
      UNUSED (ret); \
    } while (0)
#endif

    /**
     * Macro te safetly reach an offset from the end of a file
     */

#ifndef w_fseek_from_end
#define w_fseek_from_end(file, offset) \
  do { \
      int ret = w_fseek (file, offset, SEEK_END); \
      assert (! ret); \
      UNUSED (ret); \
    } while (0)
#endif

    /**
     * Macro to reach an offset from the current position in file
     */

#ifndef w_fseek_from_here
#define w_fseek_from_here(file,offset) \
  do { \
      int ret = w_fseek ((file), (offset), SEEK_CUR); \
      assert (! ret); \
      UNUSED (ret); \
    } while (0)
#endif

    /**
     * Macro to compute file size
     */

#ifndef w_file_size
#define w_file_size(file,size) \
  do { \
      w_fseek_end((file)); \
      (size) = (warc_u64_t) w_ftell((file)); \
      w_fseek_start((file)); \
    } while (0)
#endif

    /**
     * Macro to compute file size starting from an offset
     */

#ifndef w_file_size_from_offset
#define w_file_size_from_offset(file,size,offset) \
  do { \
      warc_u64_t o = (offset); \
      w_fseek_end((file)); \
      (size) = (warc_u64_t) w_ftell((file)) - o; \
      w_fseek_from_start((file), (o)); \
    } while (0)
#endif


    /**
     * Macro to print in the standard system output
     */

    /* #ifndef w_fprintf  */
    /* #define w_fprintf fprintf */
    /* #endif */

#ifndef w_fprintf
#define w_fprintf(s) \
  do{\
      (s); \
    }while(0)
#endif


    /**
     * Macro to write output according to a format into a string
     */

#ifndef w_snprintf
#define w_snprintf snprintf
#endif

    /**
     * Macro to read a character from a file
     */

#ifndef w_fgetc
#define w_fgetc(file) fgetc (file)
#endif

    /**
     * Macro to safetly put a character in a file
     */

#ifndef w_ungetc
#define w_ungetc(c,file) \
  do { \
      int ret = ungetc (c, file); \
      assert (ret); \
      UNUSED (ret); \
    } while (0)
#endif

    /**
     * Macro to safetly zero fill memory area
     */

#ifndef w_memset_z
#define w_memset_z(a, size) \
  do { \
      void * s = memset (a, 0, size); \
      assert (s == a); \
      UNUSED (s); \
    } while (0)
#endif

    /**
     * Macro to safetly return the file descriptor associated with FH
     */
#ifndef w_fileno
#define w_fileno(fh) fileno(fh)
#endif


   /**
     * Macro to safetly zero fill memory area
     */

#ifndef w_ftruncate
#define w_ftruncate(fd, size) \
  do { \
      int ret = ftruncate (fd, 0); \
      assert (! ret); \
      UNUSED (ret); \
    } while (0)
#endif

#ifndef w_fflush
#define w_fflush(fh) \
  do { \
      int ret = fflush(fh); \
      assert (! ret); \
      UNUSED (ret); \
    } while (0)
#endif


    /**
     * Macro to safetly use fdopen for reading
     */
#ifndef w_fdopen_rb
#define w_fdopen_rb(fh) fdopen (fh, "r+b")
#endif

    /**
     * Macro to safetly use fdopen for writing
     */
#ifndef w_fdopen_wb
#define w_fdopen_wb(fh) fdopen (fh, "w+b")
#endif


    /**
     * Macro to open files in append binary mode
     */

#ifndef w_open_cwb
#define w_open_cwb(s,fh) \
 do { \
      struct stat st; \
      int fd; \
      st . st_size = 0; \
      fd = open ((s), O_RDWR | O_CREAT); \
      assert (fd != -1); \
      assert (fstat(fd, & st) != -1); \
      if(st . st_size == 0) chmod(s, 0644); \
      fh=w_fdopen_wb(fd); \
    } while (0)
#endif


    /**
      * Macro to safetly using strstr
      */
#ifndef w_ststr
#define w_strstr(s1,s2) strstr ((const char *)s1,(const char *)s2)
#endif

    extern ptrdiff_t  w_strncpy         (warc_u8_t *, const warc_u8_t *,
                                           size_t);
    extern warc_u8_t * w_strcasestr     (const warc_u8_t *,
                                           const warc_u8_t *);
    extern const warc_u8_t * w_index    (const warc_u8_t *, int);
    extern warc_i32_t  w_strcmp         (const warc_u8_t *, const warc_u8_t *);
    extern warc_u32_t  w_strlen         (const warc_u8_t *);
    extern warc_u32_t  roundToPowerOfTwoUInt (warc_u32_t);
    extern warc_bool_t isPowerOfTwoUInt (warc_u32_t);
    extern warc_u32_t  computeHash      (const char *, warc_u32_t);
    extern warc_bool_t w_check_digital_string (const warc_u8_t *,
          warc_u32_t);
    extern warc_bool_t w_atou           (const warc_u8_t *, warc_u32_t,
                                           warc_u32_t *);
    extern warc_u8_t * w_numToString    (warc_i64_t, warc_u8_t *);

#ifdef __cplusplus
  }

#endif

#endif /* __WARC_C_SAFE_H__ */



