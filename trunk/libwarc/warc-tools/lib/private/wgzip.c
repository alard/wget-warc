
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

/*
 * Portability header file
 */

#include <wport.h>


/*
 * WARC default headers
 */

#include <wclass.h>   /* bless, destroy, cassert, struct Class */
#include <wsign.h>    /* CASSET macro */
#include <wgzip.h>    /* for class's prototypes */
#include <wmem.h>     /* wmalloc, wfree */
#include <wmisc.h>    /* warc_bool_t ... */
#include <wendian.h>  /* warc_i32ToEndian ... */
#include <wcsafe.h>


#ifndef  MAX_WBITS
#define  MAX_WBITS   15+16 /* 32K LZ77 window */
#include <zlib.h>
#endif
#include <wgzipbit.h>


#ifndef WARC_GZIP_IN_BUFFER_SIZE
#define WARC_GZIP_IN_BUFFER_SIZE   32768 /* 16384 */
#endif

#ifndef WARC_GZIP_OUT_BUFFER_SIZE
#define WARC_GZIP_OUT_BUFFER_SIZE  65536
#endif





/**
 * GzipMetat structure
 */


struct GzipMeta
  {
    warc_u64_t    csize;     /**< compressed raw size */
    warc_u64_t    ocsize;    /**< compressed raw size in EXTRA field */
    warc_u64_t    usize;     /**< uncompressed raw size */
    warc_u64_t    oucsize;   /**< uncompressed raw size in EXTRA field */
    warc_u32_t    gzip_header_size;
    warc_u32_t    gzip_footer_size;
  };

/**
 * WARC WGzip class internal
 */

#define SIGN 8

struct WGzip
  {

    const void * class;

    /*@{*/

    struct GzipMeta * meta; /**< GzipMeta object for uncompressing only  */
    /*@}*/
  };


#define META     (self -> meta)




/**
 * @param _self a WGzip object
 * @param source input file handle to compress
 * @param dest ouput file handle (compressed data will be there)
 * @param _level compression level
 * @param csize: size of the compressed data
 *
 * @return Z_OK (i.e. 0) if the operation succeeds,
 * non zero value otherwise
 *
 * WGzip : compress the data pointed by file handle "source" with
 * compression level "_level" and store the result in file handle
 * pointed by "dest". Store the size of the compessed data in "csize".
 */

WPUBLIC warc_i32_t WGzip_compress (const void * const _self,
                                   FILE * source, FILE * dest,
                                   const warc_gzlevel_t _level,
                                   warc_u64_t * csize)
{
#define EXTRA_GZIP_HEADER 12

  const struct WGzip * const self = _self;

  warc_u8_t in  [WARC_GZIP_IN_BUFFER_SIZE];
  warc_u8_t out [WARC_GZIP_IN_BUFFER_SIZE];
  z_stream   strm;
  warc_i32_t ret;
  warc_u32_t space = EXTRA_GZIP_HEADER;
  warc_u32_t have;
  warc_u32_t flush;
  warc_u32_t level;

  warc_u64_t offset;
  warc_u64_t ucsize = 0;

  warc_u64_t off;

  /* preconditions */
  CASSERT (self);
  assert  (source);
  assert  (dest);

  /* get actual offset from "dest" */
  w_ftell (dest, offset);

  /* compute "source" file size */
  w_ftell (source, off);
  w_file_size_from_offset (source, ucsize, off);

  /* create extra space in GZIP header */
  while (space)
    {
      if (w_fwrite (" ", 1, 1, dest) != 1 || w_ferror (dest) )
        {
          ret = Z_WARC_EXTRA_FIELD_ERROR;
          goto END;
        }

      -- space;
    }

  /* ensure that the compressed file is in binary mode */
  SET_BINARY_MODE (dest);

  /* force the compressed size "csize" to zero */
  * csize = 0;

  /* allocate deflate state */
  strm.zalloc = Z_NULL;

  strm.zfree  = Z_NULL;

  strm.opaque = Z_NULL;

  /* map between user compression level and Zlib one */
  switch (_level)
    {
      case WARC_GZIP_DEFAULT_COMPRESSION:
        level = Z_DEFAULT_COMPRESSION;
        break;

      case WARC_GZIP_NO_COMPRESSION:
        level = Z_NO_COMPRESSION;
        break;

      case WARC_GZIP_BEST_SPEED:
        level = Z_BEST_SPEED;
        break;

      case WARC_GZIP_BEST_COMPRESSION :
        level = Z_BEST_COMPRESSION;
        break;

      default:
        level = Z_DEFAULT_COMPRESSION;
    }

  /* ret = deflateInit(&strm, level); */
  ret = deflateInit2 (& strm,
                      level,
                      Z_DEFLATED,
                      methodBits (GZIP_METHOD),
                      8,
                      Z_DEFAULT_STRATEGY);

  if (ret != Z_OK)
    goto RET;

  /* compress until end of file */
  do
    {
      strm.avail_in = w_fread (in, 1, WARC_GZIP_IN_BUFFER_SIZE, source);

      if (w_ferror (source) )
        {
          ret = Z_ERRNO;
          goto END;
        }

      flush = w_feof (source) ? Z_FINISH : Z_NO_FLUSH;

      strm.next_in = in;

      /* run deflate() on input until output buffer not full, finish
         compression if all of source has been read in */

      do
        {
          strm.avail_out = WARC_GZIP_IN_BUFFER_SIZE;
          strm.next_out  = out;

          ret = deflate (& strm, flush);    /* no bad return value */
          assert (Z_STREAM_ERROR != ret);   /* state not clobbered */

          have = WARC_GZIP_IN_BUFFER_SIZE - strm.avail_out;

          /* how many bytes were compressed */
          * csize += have;

          if (w_fwrite (out, 1, have, dest) != have || w_ferror (dest) )
            {
              ret = Z_ERRNO;
              goto END;
            }

          if (fflush (dest) )
            {
              ret = Z_WARC_FLUSH_ERROR;
              goto END;
            }

        }

      while (strm.avail_out == 0);

      assert (strm.avail_in == 0);    /* all input will be used */

      /* done when last data in file processed */
    }

  while (flush != Z_FINISH);

  assert (Z_STREAM_END == ret);       /* stream will be complete */

END:
  /* clean up and return */
  deflateEnd (& strm);

  /* got Z_STREAM_END is OK */
  if (ret == Z_STREAM_END)
    ret = Z_OK;

  if (ret != Z_OK)
    ret = 0;

RET:

  /* copy the compressed and uncompressed size in GZIP EXTRA field */
  if (ret == Z_OK)
    {
      warc_u8_t  buf [EXTRA_GZIP_HEADER];
      warc_u16_t xlen  = GZIP_STATIC_HEADER_SIZE;
      warc_u64_t ecsize;
      warc_u64_t eucsize;
      warc_u8_t  flg;

      /* adjust the compressed size */
      (* csize) += EXTRA_GZIP_HEADER;

      /* move the GZIP header to the front of file */
/*       w_fseek_from_start (dest, EXTRA_GZIP_HEADER); */
      w_fseek_from_start (dest, offset + EXTRA_GZIP_HEADER);
      w_fread (buf, 1, GZIP_STATIC_HEADER_SIZE, dest);

/*       w_fseek_start (dest); */
      w_fseek_from_start (dest, offset);
      w_fwrite (buf, 1, GZIP_STATIC_HEADER_SIZE, dest);

      /* set the EXTRA field */
      flg = buf[OFF_FLG] | FLG_FEXTRA;

/*       w_fseek_from_start (dest, OFF_FLG); */
      w_fseek_from_start (dest, offset + OFF_FLG);
      w_fwrite (& flg, 1, 1, dest);

      /* goto the EXTRA field land */
/*       w_fseek_from_start (dest, GZIP_STATIC_HEADER_SIZE); */
      w_fseek_from_start (dest, offset + GZIP_STATIC_HEADER_SIZE);

      /* set XLEN in bytes */
      buf [0] = (xlen & 255);
      buf [1] = (xlen >> 8) & 255;

      /* set SL (i.e. 0x73, 0x6C). See WARC spec annexe B.6 */
      buf [2] = 0x73; /* S */
      buf [3] = 0x6C; /* L */

      /* convertion based endianness */
      ecsize  = * csize;
      eucsize = ucsize;
/*       ecsize  = warc_i32ToEndian (* csize); */
/*       eucsize = warc_i32ToEndian (ucsize); */


      /* set compressed line */
      buf [4] = (ecsize & 255);
      buf [5] = (ecsize >> 8) & 255;
      buf [6] = (ecsize >> 16) & 255;
      buf [7] = (ecsize >> 24) & 255;

      /* set uncompressed line */
      buf [8]  = (eucsize & 255);
      buf [9]  = (eucsize >> 8) & 255;
      buf [10] = (eucsize >> 16) & 255;
      buf [11] = (eucsize >> 24) & 255;

      w_fwrite (buf, 1, EXTRA_GZIP_HEADER, dest);

      /* move to the EOF */
      w_fseek_end (dest);
    }

  /* zero fill the GzipMeta structure  */
  memset (META, 0, sizeof (struct GzipMeta) );

  return (ret);
}


WPRIVATE warc_u32_t
WGzip_getCompUncompSize (FILE * source, struct GzipMeta * meta,
                         warc_u32_t xlen)
{
  warc_u8_t buf [GZIP_STATIC_HEADER_SIZE];
  warc_u64_t csize  = 0;
  warc_u64_t ucsize = 0;

  assert (meta) ;

  if (w_fread (buf, 1, GZIP_STATIC_HEADER_SIZE, source) != GZIP_STATIC_HEADER_SIZE
          || w_ferror (source) )
    return (Z_READING_ERROR);

  /* found the SL tag (see WARC spec annex B.6) */
  if (buf[0] == 0x73 &&  buf [1] == 0x6C)
    {
      csize   = buf[2];
      csize  += (buf[3] << 8);
      csize  += (buf[4] << 16);
      csize  += (buf[5] << 24);
/*       printf("+++ csize: %llu\n", csize); */
/*      csize   = warc_i32ToEndian (csize); */

      ucsize  = buf[6];
      ucsize += (buf[7] << 8);
      ucsize += (buf[8] << 16);
      ucsize += (buf[9] << 24);
/*       printf("+++ ucsize: %llu\n", ucsize); */
/*       ucsize  = warc_i32ToEndian (ucsize); */

      meta -> ocsize  = csize;
      meta -> oucsize = ucsize;
    }

  else /* skip XLEN bytes */
    {
      /* rewind GZIP_STATIC_HEADER_SIZE bytes */
      w_fseek_from_here (source, - GZIP_STATIC_HEADER_SIZE);

      /* consume XLEN bytes */

      while (xlen)
        {
          -- xlen;
          w_fgetc (source);

          if (w_ferror (source) )
            return (Z_READING_ERROR);
        }
    }

  return (Z_OK);
}


WPRIVATE warc_u32_t
WGzip_skip_header_extra (FILE * source, struct GzipMeta * meta)
{

  warc_u8_t p[2];
  warc_u16_t    xlen = 0;

  if (2 != w_fread (p, 1, 2, source) || w_ferror (source) )
    return (Z_READING_ERROR);

  meta -> gzip_header_size += 2;

  xlen = (p[0] | (p[1] << 8) );

  meta -> gzip_header_size += xlen;

  return (WGzip_getCompUncompSize (source, meta, xlen) );
}

WPRIVATE warc_u32_t
WGzip_skip_header_name_or_comment (FILE * source, struct GzipMeta * meta)
{
  while (0 != w_fgetc (source) && ! w_ferror (source) )
    meta -> gzip_header_size ++;

  if (w_ferror (source) )
    return (Z_READING_ERROR);

  return (Z_OK);
}

WPRIVATE warc_u32_t
WGzip_skip_nb_chars (FILE * source, const warc_u32_t offset,
                     struct GzipMeta * meta)
{
  /*   if (0 != fseek (source, offset, SEEK_CUR)) */
  /*     return (Z_FSEEK_ERROR); */
  w_fseek_from_here (source, offset);

  meta -> gzip_header_size += offset;

  return (Z_OK);
}

WPRIVATE warc_u32_t
WGzip_skip_nb_chars_footer (FILE * source, const warc_u32_t offset,
                            struct GzipMeta * meta)
{
  /*  if (0 != fseek (source, offset, SEEK_CUR)) */
  /*     return Z_FSEEK_ERROR; */

  w_fseek_from_here (source, offset);

  meta -> gzip_footer_size = offset;

  return (Z_OK);
}

WPRIVATE warc_u32_t
WGzip_skip_header (FILE * source, struct GzipMeta * meta)
{
  /* declare variables with maximum alignment */
  warc_u8_t flg = 0;
  warc_u8_t p [GZIP_STATIC_HEADER_SIZE];
  unsigned int  ret = Z_OK;

  if (w_fread (p, 1, GZIP_STATIC_HEADER_SIZE, source) != GZIP_STATIC_HEADER_SIZE
          || w_ferror (source) )
    {
      if (w_feof (source) != 0)
        return (Z_EOF);
      else
        return (Z_READING_ERROR);
    }

  meta -> gzip_header_size = GZIP_STATIC_HEADER_SIZE;

  if (ID1 != * (p + OFF_ID1) || ID2 != * (p + OFF_ID2) || CM != * (p + OFF_CM) )
    {
      return (Z_HEADER_ERROR);
    }

  flg = p[OFF_FLG];

  /* test extra flags existance */

  if (flg != 0)
    {
      /* maybe skip the extra field */
      if (flg & FLG_FEXTRA)
        if ( (ret = WGzip_skip_header_extra (source, meta) ) &&
                ret != Z_OK)
          return (ret);

      /* maybe skip the name */
      if (flg & FLG_FNAME)
        if ( (ret = WGzip_skip_header_name_or_comment (source, meta) ) &&
                ret != Z_OK)
          return (ret);

      /* maybe skip the comment */
      if (flg & FLG_FCOMMENT)
        if ( (ret = WGzip_skip_header_name_or_comment (source, meta) ) &&
                ret != Z_OK)
          return (ret);

      /* maybe skip the CRC16 header */
      if (flg & FLG_FHCRC)
        if ( (ret = WGzip_skip_nb_chars (source, 2, meta) ) &&
                ret != Z_OK)
          return (ret);

      /* maybe skip an encrypted gzip */
      if (flg & FLG_RESERVED)
        return (Z_RESERVED_GZIP);
    }

  return (Z_OK);
}


WIPRIVATE void
WGzip_adjustRecord (FILE * source, struct GzipMeta * meta)
{
  while (ID1 != w_fgetc (source) && 0 == w_feof (source) )
    meta -> csize ++;
}


WPRIVATE warc_i32_t
WGzip_decode (const void * _self,  FILE * source, warc_u64_t offset,
              warc_u32_t (* cb) (const warc_u8_t *,
                                 const warc_u32_t, void *),
              void * env)
{

  struct WGzip       * self  = (void *) _self;

  struct GzipMeta    * meta  = META;

  warc_u32_t          have   = 0;
  warc_i32_t          ret    = 0;
  z_stream            strm;
  warc_u8_t           in     [WARC_GZIP_IN_BUFFER_SIZE];
  warc_u8_t           out    [WARC_GZIP_OUT_BUFFER_SIZE];

  /* seek to the correct record offset */
  /*   if (0 != fseek (source, offset, SEEK_SET)) */
  /*     { */
  /*       ret = Z_REWIND_ERROR; */
  /*       goto END; */
  /*     } */
  w_fseek_from_start (source, offset);

  ret = WGzip_skip_header (source, meta);

  if (ret != Z_OK)
    return (ret);

  meta -> csize = meta -> gzip_header_size;

  /* allocate inflate state */
  strm . zalloc    = Z_NULL;

  strm . zfree     = Z_NULL;

  strm . opaque    = Z_NULL;

  strm . avail_in  = 0;

  strm . next_in   = Z_NULL;

  ret = inflateInit2 (& strm, - MAX_RBITS);

  if (ret != Z_OK)
    return (ret);

  /* decompress until deflate stream ends or end of file */
  do
    {
      warc_u32_t av_in;

      strm.avail_in = w_fread (in, 1, WARC_GZIP_IN_BUFFER_SIZE, source);

      if (w_ferror (source) || 0 == strm.avail_in)
        {
          ret = Z_READING_ERROR;
          goto END;
        }

      /* set input buffer */
      strm.next_in = (Bytef *) in;

      /* run inflate() on input until output buffer not full */
      do
        {
          /* set available compressed chars */
          av_in = strm.avail_in;

          strm.avail_out = OUTPUT_BUFFSIZE;
          strm.next_out  = (Bytef *) out;

          /* start decompression */
          ret = inflate (& strm, Z_NO_FLUSH);

          meta -> csize += av_in - strm . avail_in;

          /* state not clobbered */

          if (ret != Z_OK && ret != Z_STREAM_END)
            {
              /* state not clobbered */
              if (ret == Z_NEED_DICT)
                ret = Z_DATA_ERROR;

              goto END;
            }

          have = OUTPUT_BUFFSIZE - strm . avail_out;

          meta -> usize += have;

          /*
           * callback based parsing
           */

          if (Z_STOP_DECODING == cb (out, have, env) )
            {
              ret = Z_OK;
              goto END;
            }

          if (ret == Z_STREAM_END)
            {
              goto STREAM_END;
            }

        }

      while (strm . avail_out == 0);

    }

  while (Z_STREAM_END != ret);

STREAM_END:

  ret = WGzip_skip_nb_chars_footer (source, FOOTER_CRC32_ISIZE, meta);

  meta -> csize  += meta -> gzip_footer_size;

  /* adjust CSize on record boundary */
  if (Z_OK == ret)
    {
      /*       if (0 == fseek(source, offset + meta -> csize, SEEK_SET))  */
      /*         WGzip_adjustRecord (source, meta); */
      /*       else */
      /*         ret = Z_ADJUST_CSIZE_ERROR; */
      w_fseek_from_start (source, offset + meta -> csize);
      WGzip_adjustRecord (source, meta);
    }

END:

  inflateEnd (&strm);

  return (ret);
}



/**
 * @param _self a WGzip object
 * @param source input file handle to compress
 * @param offset: file offset in source
 * @param usize: pointer the uncompressed data size
 * @param csize: pointer the compressed data size
 *
 * @return Z_OK (i.e. 0) if the operation succeeds,
 * non zero value otherwise
 *
 * WGzip : compress the data pointed by file handle "source" with
 * compression level "_level" and store the result in file handle
 * pointed by "dest". Store the size of the compessed data in "csize".
 */

WPUBLIC warc_u32_t WGzip_uncompress (const void * const _self,
                                     FILE *       source,
                                     warc_u64_t   offset,
                                     warc_u64_t * usize,
                                     warc_u64_t * csize,
                                     warc_u32_t (* cb) (const warc_u8_t *,
                                                        const warc_u32_t,
                                                        void *),
                                     void * env)
{

  const struct WGzip * const self = _self;
  warc_i32_t                 ret;

  /* preconditions */
  CASSERT (self);
  assert  (source);

  /* empty any previous GzipMeta settings */
  memset (META, 0, sizeof (struct GzipMeta) );

  /* set the uncompression size to 0 */
  * usize = 0;

  /* set the compression size to 0 */
  * csize = 0;

  /* try to uncompress the data */
  ret = WGzip_decode (self, source, offset, cb, env);

  if (Z_OK != ret)
    {
      /* maybe uncompress failure or EOF */
      if (Z_EOF != ret)
        {
          w_fprintf (fprintf (stdout, "error when uncompressing data at offset %llu (Gzip error number: %d)\n", (unsigned long long) offset, ret) );
        }
    }

  else
    {
      * usize = META -> usize;
      * csize = META -> csize;
    }

  /* zero fill the GzipMeta structure  */
  memset (META, 0, sizeof (struct GzipMeta) );

  return (ret);
}


/**
 * @param _self a WGzip object
 * @param source compressed input file handle
 * @param offset file offset in source
 * @param usize pointer the uncompressed data size
 * @param csize pointer the compressed data size
 *
 * @return WARC_FALSE if succeeds,WARC_TRUE otherwise
 *
 * Try to extract uncompressed and compressed size from GZIP header
 * if any.
 */

WPUBLIC warc_bool_t
WGzip_analyzeHeader (const void * _self,  FILE * source,
                     warc_u64_t offset, warc_u64_t * usize,
                     warc_u64_t * csize)
{

  const struct WGzip * const self = _self;

  /* preconditions */
  CASSERT (self);
  assert  (source);

  /* seek to the correct record offset */
  w_fseek_from_start (source, offset);

  /* try to extract uncompressed and compressed length from the
     GZIP headers
   */

  if (WGzip_skip_header (source, META) != Z_OK)
    return (WARC_TRUE);

  (* usize) = META -> oucsize;
  (* csize) = META -> ocsize;

/*   printf(">>> csize: %llu\n", *csize); */
/*   printf(">>> usize: %llu\n", *usize); */

  /* zero fill the GzipMeta structure  */
  memset (META, 0, sizeof (struct GzipMeta) );

  return ( (* usize) == 0 && (* csize) == 0);
}


/**
 * @param _self a WGzip object
 * @param source compressed input file handle
 * @param offset file offset in source
 *
 * @return WARC_FALSE if succeeds,WARC_TRUE otherwise
 *
 * Check if the file is a valid GZIP
 */

WPUBLIC warc_bool_t
WGzip_check (const void * _self,  FILE * source, warc_u64_t offset)
{

  const struct WGzip * const self  = _self;

  /* preconditions */
  CASSERT (self);
  assert  (source);

  /* seek to the correct record offset */
  w_fseek_from_start (source, offset);

  /* try to extract uncompressed and compressed length from the
     GZIP headers
   */

  if (WGzip_skip_header (source, META) != Z_OK)
    return (WARC_TRUE);

  /* zero fill the GzipMeta structure  */
  memset (META, 0, sizeof (struct GzipMeta) );

  return (WARC_FALSE);
}


/**
 * WGzip_constructor - create a new WGzip object instance
 *
 * @param _self: WGzip class object
 * @param app constructor list parameters
 *
 * @return A valid WGzip object or NIL
 *
 * WARC List constructor
 */

WPRIVATE void * WGzip_constructor (void * _self, va_list * app)
{

  struct WGzip * const self = _self;

  struct GzipMeta   *  meta = NIL;
  size_t               size = sizeof (struct GzipMeta);

  UNUSED (app);

  /* avoid any end of line transformation */
  SET_BINARY_MODE (stdin);
  SET_BINARY_MODE (stdout);

  meta = wmalloc (size);
  assert (meta);

  /* zero fill the GzipMeta structure  */
  memset (meta, 0, size);

  META = meta;

  return (self);
}


/**
 * @param _self WGzip object instance
 *
 * WARC List destructor
 */

WPRIVATE void * WGzip_destructor (void * _self)
{

  struct WGzip  * const self = _self;

  /* preconditions */
  CASSERT (self);

  if (META)
    wfree (META), META = NIL;

  return (self);
}


/**
 * WARC WGzip class
 */

static const struct Class _WGzip =
  {
    sizeof (struct WGzip),
    SIGN,
    WGzip_constructor, WGzip_destructor
  };

const void * WGzip = & _WGzip;
