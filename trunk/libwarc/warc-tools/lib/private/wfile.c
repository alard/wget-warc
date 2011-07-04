
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

#include <wclass.h>   /* bless, destroy, struct Class */
#include <wsign.h>    /* CASSET macro */
#include <wfile.h>    /* WFile ... */
#include <wmisc.h>    /* WARC_TRUE, WARC_FALSE ... */
#include <wlist.h>    /* WList  */
#include <wstring.h>  /* WString */
#include <wrecord.h>  /* WRecord */
#include <wrecord.x>  /* WRecord_getDataFile, WRecord_getDataFileExtern */
#include <wanvl.h>    /* WAnvl */
#include <wheader.h>  /* WHeader */
#include <wfsmhdl.h>  /* WFsmHDL automata */
#include <wfsmanvl.h> /* WFsmANVL automata */
#include <wmktmp.h>   /* WTempFile */
#include <wgzip.h>    /* WGzip */
#include <wversion.h> /* WARC_VERSION */
#include <wcsafe.h>   /* w_fprintf ... */


#include <limits.h>

#define makeS(s) (s), w_strlen ((s))
#define makeU(s) ((warc_u8_t *) (s))

/**
 * WARC WFile class internal
 */

#define SIGN 6

struct WFile
  {

    const void * class;

    /*@{*/
    void * fname; /**< WString containing WarcFile name*/
    void * dname; /**< working directory name */
    void * tempfile; /**< temporary file object */
/*    void * tempfile2; **< temporary file object */
    FILE * fh;    /**< file handle */
    wfile_mode_t mode;  /**< Warc object access mode*/
    wfile_comp_t compressed; /**< indicates if the record is compressed */
    warc_u64_t maxsize; /**< Maximum Warc file size in BYTES */
    warc_u64_t fsize; /**< The current WarcFile Size */
    warc_bool_t isfull; /**< WARC is full or not */
    /*@}*/
  };


#define FNAME         (self -> fname)
#define WORKING_DIR   (self -> dname)
#define TEMP_FILE     (self -> tempfile)
/* #define TEMP_FILE2    (self -> tempfile2) */
#define FH            (self -> fh)
#define MODE          (self -> mode)
#define MAXSIZE       (self -> maxsize)
#define COMP          (self -> compressed)
#define FSIZE         (self -> fsize)
#define ISFULL        (self -> isfull)

/* Header line field separator definition */
/* redefine it to change the number of spaces to put */
#define TSPS " "

/* Spaces separator size */
#define TSPSIZE (1)


/**
 * callback environnement for compressed reading
 */


struct CallbackEnv
  {
    FILE     * out; /**< output file which will hold the uncompressed data */
    warc_u32_t crlf; /**< to find header end double CRLF */
    warc_u64_t usize; /**< the size of uncompressed data */
  };


/* this callback uncompressing the entire WARC file */
WPRIVATE warc_u32_t wrecover (const warc_u8_t * buffer, const warc_u32_t nbr, void * env)
{
  FILE * out = (FILE *) env;

  /* copy the uncompressed 'nbr' bytes to out */

  if (w_fwrite (buffer, 1, nbr, out) != nbr || w_ferror (out) )
    return (Z_STOP_DECODING); /* return this value to stop the uncompression */

  return (Z_CONTINUE_DECODING);
}

/* this callback stop uncompressing when it find a double CRLF */
WPRIVATE warc_u32_t getHeader (const warc_u8_t * buffer, const warc_u32_t nbr,
                               void * _env)
{

  struct CallbackEnv * env  = (struct CallbackEnv *) _env;
  warc_u64_t           i    = 0;

  switch (env -> crlf)
    {

      case 0 :
        goto CR1;

      case 1 :
        goto LF1;

      case 2 :
        goto CR2;

      case 3 :
        goto LF2;
    }

CR1:

  while (i < nbr)  /* search the first CR  */
    {
      if (buffer [i] == '\r')
        {
          env -> crlf ++;
          ++ i;
          goto LF1;
        }

      ++ i;
    }

  goto CONTINUE;

LF1:

  if (i < nbr)
    {
      if (buffer [i] == '\n')
        {
          env -> crlf ++;
          i++;
          goto CR2;
        }

      else
        {
          env -> crlf = 0;
          goto CONTINUE;
        }
    }

  else
    goto CONTINUE;

CR2:
  if (i < nbr)
    {
      if (buffer [i] == '\r')
        {
          env -> crlf ++;
          ++ i;
          goto LF2;
        }

      else
        {
          env -> crlf = 0;
          goto CONTINUE;
        }
    }

  else
    goto CONTINUE;

LF2:
  if (i < nbr)
    {
      if (buffer [i] == '\n')
        {
          goto STOP;
        }

      else
        {
          env -> crlf = 0;
          goto CONTINUE;
        }
    }

  else
    goto CONTINUE;

STOP:
  /*  we've found a double CRLF (stop uncompressing)   */
  env -> usize += i + 1;

  w_fwrite (buffer, 1, i + 1, env -> out);

  return (Z_STOP_DECODING);

CONTINUE:
  /*    copy the uncompressed 'nbr' bytes to out  */
  if (w_fwrite (buffer, 1, nbr, env -> out) != nbr || w_ferror (env -> out) )
    return (Z_STOP_DECODING);

  return (Z_CONTINUE_DECODING);
}



/**
 * @param _self WFile object
 *
 * @return WARC_TRUE if succeeds, WARC_FALSE otherwise
 *
 * Check the presence of more Warc Records in the Warc File
 */

WIPUBLIC warc_bool_t WFile_hasMoreRecords (const void * const _self)
{

  const struct WFile * const self = _self;
  warc_u64_t off;

  /* preconditions */
  CASSERT (self);

  /* we must be in read mode */

  if (MODE != WARC_FILE_READER)
    return (WARC_TRUE);

  /* if it reach the end of WARC, its start over again */
  w_ftell (FH, off);
  if (off == FSIZE)
    {
      w_fseek_start (FH); /* restet from start */
      return (WARC_FALSE);
    }
  else
    return (WARC_TRUE);


  return (WARC_FALSE);
}


/**
 * @param _self: WFile object instance
 * 
 * @return the size of the warcfile
 *
 * Warc File size providing function
 */

WIPUBLIC warc_u64_t WFile_getFileSize (const void * const _self)
{
    const struct WFile * self = _self;

  /*Preconditions*/
  CASSERT (self);

  return (FSIZE);
}


/**
 * @param _self: WFile object instance
 * 
 * @return WARC_TRUE if it is full, WARC_FALSE otherwise
 *
 * Return is the WARC file is full or not
 */

WIPUBLIC warc_bool_t WFile_isFull (const void * const _self)
{
  const struct WFile * self = _self;
  
  /* Preconditions */
  CASSERT (self);
  
  return (ISFULL);
}

/**
 * @param _self: a WFile object instance
 * 
 * 
 * @return the compression mode as a wfile_comp_t
 *
 * Returtns the compression mode of the WARC File
 */

WIPUBLIC wfile_comp_t WFile_getCompressionMode (const void * const _self)
{
  const struct WFile * const self = _self;
  
  return (COMP);
}


/**
 * @param _self WFile object instance
 * @param wtfile the temporary file to fill
 * @param size the size of data to write
 *
 * @return False if succeeds, True otrherwise
 *
 * fills the temporary file with the warc record data
 */


WPRIVATE warc_bool_t WFile_fillTempFile (const void* const _self,
    FILE * wtfile, warc_u64_t size)
{
#define WFLUSH_SIZ 4096

  const struct WFile * const self = _self;
  char                       buf [WFLUSH_SIZ];
  warc_u32_t                 r ;
  warc_u32_t                 p ;

  /* Preconditions */
  CASSERT (self);
  assert (wtfile);

  while (size)
    {
      if (size > WFLUSH_SIZ)
        r = WFLUSH_SIZ;
      else
        r = size;

      size = size - r;

      p = w_fread (buf, 1, r, FH);

      if (w_ferror (wtfile) || r != p)
        {
          WarcDebugMsg ("error when copying data");
          return (WARC_TRUE);
        }

      if (w_fwrite (buf, 1, r, wtfile) != r)
        return (WARC_TRUE);
    }

  /*    while (size) */
  /*      { */
  /*        w_fread  (& byte, 1, 1, FH); */
  /*        w_fwrite (& byte, 1, 1, wtfile); */
  /*        -- size; */
  /*      } */

  return (WARC_FALSE);
}


/**
 * @return True if succeeds, False otherwise
 *
 * checks if a WRecord ends with two CRLF
 */

WPRIVATE warc_bool_t WFile_checkEndCRLF (FILE *f)
{
  warc_u8_t str [5];


  if (4 != w_fread (str, 4, 1, f) )
    return WARC_TRUE;

  if (w_ferror (f) )
    return WARC_TRUE;

  str[4] = '\0';

  return (! (w_strcmp (str, (warc_u8_t *) "\x0D\x0A\x0D\x0A") ) );
}



/**
 * @param _self WFile object instance
 *
 * @return A Warc Record object reference got from a compressed warc file
 *
 * Warc Record extracting from compressed Warc File
 */


WPRIVATE void * 
WFile_nextRecordGzipCompressedFast (void * _self,
                                    /*     void * objrectfile, */
/*                                     FILE * rectfile, */
                                    void * gzobj,
                                    warc_u64_t offset,
                                    warc_u64_t usize,
                                    warc_u64_t csize)
{
  struct WFile  * self     = _self;

  void 		    * wobject  = NIL; /* to recover the WRecord Object */
  warc_u64_t      wdatal   = 0;   /* to recover the Record dataLength */
  warc_u64_t      datasize = 0;
  warc_u64_t      blbegin  = -1;
  void          * head     = NIL; /* to recover the record Header */
  void          * oldhd    = NIL; /* for destruction of old value*/
  FILE          * rectfile = NIL;
  warc_u64_t      husize   = 0;   /* uncompressed data size */
  warc_u64_t      hcsize   = 0;   /* compressed data size stored in header */
  warc_u32_t      ret      = 0;

  warc_u64_t      off;

  struct CallbackEnv cbenv;

  rectfile = WTempFile_handle (TEMP_FILE);

  cbenv . out   = rectfile;
  cbenv . crlf  = 0;
  cbenv . usize = 0;



  wobject = bless (WRecord);
  assert (wobject);

  if (WRecord_setWFileOffset (wobject, offset) )
    {
      destroy (wobject);
/*       destroy (objrectfile); */
      destroy (gzobj);
      return (NIL);
    }

  if (WRecord_setCompressedSize (wobject, csize) )
    {
      destroy (wobject);
/*       destroy (objrectfile); */
      destroy (gzobj);
      return (NIL);
    }

  if (WRecord_setUncompressedSize (wobject, usize) )
    {
      destroy (wobject);
/*       destroy (objrectfile); */
      destroy (gzobj);
      return (NIL);
    }

  /* setting the offset of the data bloc in the WRecord */

  w_ftell (FH, off);
  if (WRecord_setWoffset (wobject, off) )
    {
/*       destroy (objrectfile); */
      destroy (wobject);
      return (NIL);
    }

  w_ftell (FH, off);
  ret = WGzip_uncompress (gzobj, FH, off, & husize, & hcsize,
                          getHeader, (void *) & cbenv);
  if (ret)
    {
      WarcDebugMsg ("unable to read gzipped record");
/*       destroy (objrectfile); */
      destroy (gzobj);
      destroy (wobject);
      return (NIL);
    }
  destroy (gzobj);

  offset += csize;

  w_fseek_start (rectfile);
  w_fseek_from_start (FH, offset);

  head = bless (WHeader, WARC_UNKNOWN_RECORD, makeS (makeU (" ")), makeS (makeU (" ")));

  unless (head)
    {
     destroy (wobject);
     return (NIL);
    }

  if (WHeader_extractFromWarcFile (head, rectfile))
    {
     destroy (head);
     destroy (wobject);
     return (NIL);
    }

  w_ftell (rectfile, blbegin);

  wdatal = WHeader_getDataLength (head);

/*   destroy (objrectfile); */

  WRecord_setCHeaderPresence (wobject, WARC_TRUE);

  oldhd = WRecord_setHeader (wobject, head);

  destroy (oldhd);

  datasize = wdatal - blbegin;

  if (WRecord_setContentSize (wobject, datasize) )
    {
/*       destroy (objrectfile); */
      destroy (wobject);
      return (NIL);
    }


  /* setting the pointer to the WFile in the WRecord */

  if (WRecord_setWfile (wobject, self) )
    {
/*       destroy (objrectfile); */
      destroy (wobject);
      return (NIL);
    }

  return (wobject);
}


/**
 * @param _sel WFile object instance
 *
 * @return a Warc Record object reference got from a compressed warc file
 *
 * Warc Record extracting from compressed Warc File
 */


WPRIVATE void * WFile_nextRecordGzipCompressed (void * _self)
{

  struct WFile  * self = _self;

  void 		    * wobject  = NIL; /* to recover the WRecord Object */
  warc_u64_t      wdatal   = 0;   /* to recover the Record dataLength */
  warc_u64_t      datasize = 0;
  warc_u64_t      blbegin = -1, recend = -1;
  void          * head    = NIL; /* To recover Header */
  void          * oldhd    = NIL; /* for destruction of old value*/
  void          * gzobj    = NIL; /* for decompression */
  warc_u64_t      usize    = 0;   /* uncompressed data size */
  warc_u64_t      csize    = 0;   /* compressed data size */
  warc_u64_t      offset   = 0;
  warc_u32_t      ret      = 0;
  FILE          * rectfile = NIL; /* to uncompress the WRecord */

  warc_u64_t      off;

  WTempFile_reset (TEMP_FILE);

  gzobj = bless (WGzip);
  w_ftell (FH, offset);


  /* test if the we have the warc extra gzip header */
  w_ftell (FH, off);
  ret = WGzip_analyzeHeader (gzobj, FH, off, & usize, & csize);

  w_fseek_from_start (FH, offset);

  unless (ret)
  return (WFile_nextRecordGzipCompressedFast (self, 
          gzobj, offset, usize, csize) );



  wobject = bless (WRecord);
  assert (wobject);

  if (WRecord_setWFileOffset (wobject, offset) )
    {
      destroy (wobject);
      destroy (gzobj);
      return (NIL);
    }

  if (WRecord_makeDataFile (wobject, WString_getText(WORKING_DIR), WString_getLength(WORKING_DIR)))
    {
      destroy (wobject);
      destroy (gzobj);
      return (NIL);
    }

  rectfile = WTempFile_handle (WRecord_getDataFile (wobject));

  
  w_ftell (FH, off);
  ret = WGzip_uncompress (gzobj, FH, off, & usize, & csize,
                          wrecover, (void *) rectfile);
  if (ret)
    {
      WarcDebugMsg ("unable to read gzipped record");
      destroy (gzobj);
      destroy (wobject);
      return (NIL);
    }

  if (WRecord_setCompressedSize (wobject, csize) )
    {
      destroy (wobject);
      destroy (gzobj);
      return (NIL);
    }


  if (WRecord_setUncompressedSize (wobject, usize))
    {
      destroy (wobject);
      destroy (gzobj);
      return (NIL);
    }

  offset += csize;

  w_fseek_from_start (FH, offset);

  destroy (gzobj);
  w_fseek_start (rectfile);

  head = bless (WHeader, WARC_UNKNOWN_RECORD, makeS (makeU (" ")), makeS (makeU (" ")));
  
  unless (head)
    {
      destroy (wobject);
      return (NIL);
    }

  if (WHeader_extractFromWarcFile (head, rectfile))
    {
      destroy (head);
      destroy (wobject);
      return (NIL);
    }


  w_ftell (rectfile, blbegin);
  wdatal  = WHeader_getDataLength (head);

  w_fseek_start (rectfile);

  w_fseek_from_here (rectfile, wdatal);

  w_ftell (rectfile, recend);

  /* checking the presence of the two end CRLF */
  unless (WFile_checkEndCRLF (rectfile) )
  {
    WarcDebugMsg ("found a corrupted record");
    destroy (head);
    destroy (wobject);
    return (NIL);
  }

  w_ftruncate (w_fileno (rectfile), recend);
  w_fflush(rectfile);

  w_fseek_start (rectfile);

  WRecord_setCHeaderPresence (wobject, WARC_FALSE);

  oldhd  = WRecord_setHeader (wobject, head);
  destroy (oldhd);

  datasize = recend;

  if (WRecord_setContentSize (wobject, datasize) )
    {
      destroy (wobject);
      return (NIL);
    }

  /* setting the offset of the data bloc in the WRecord */
  if (WRecord_setWoffset (wobject, blbegin) )
    {
      destroy (wobject);
      return (NIL);
    }

  /* setting the pointer to the WFile in the WRecord */
  if (WRecord_setWfile (wobject, self) )
    {
      destroy (wobject);
      return (NIL);
    }

  return (wobject);
}


/**
 * @param _self WFile object instance
 *
 * @return a Warc Record object reference got from an uncompressed Warc file
 *
 * Warc Record object axtracting from uncompress Warc File
 */

WPRIVATE void * WFile_nextRecordUncompressed (void * _self)
{

  struct WFile  * self = _self;

  void 		    * wobject  = NIL; /* to recover the WRecord Object */
  warc_u64_t      wdatal   = 0;   /* to recover the Record dataLength */
  warc_u64_t      datasize = 0;
  warc_u64_t      recbegin = -1, blbegin = -1, recend = -1;
  void          * head     = NIL; /* To recover Header */
  void          * oldhd    = NIL; /* for destruction of old value*/

  /* preconditions */
  CASSERT (self);


  /* stop at end of file */

  if (w_feof (FH) )
    return (NIL);


  w_ftell (FH, recbegin);

  wobject = bless (WRecord);

  assert (wobject);

  if (WRecord_setWFileOffset (wobject, recbegin) )
    {
      destroy (wobject);
      return (NIL);
    }

  head = bless (WHeader, WARC_UNKNOWN_RECORD, makeS (makeU (" ")), makeS (makeU (" ")));

  unless (head)
    {
      destroy (wobject);
      return (NIL);
    }

  if (WHeader_extractFromWarcFile (head, FH))
     {
      destroy (head);
      destroy (wobject);
      return (NIL);
     }

  
  w_ftell (FH, blbegin);


  wdatal = WHeader_getDataLength (head);

  if (WRecord_setUncompressedSize (wobject, wdatal + 4) )
    {
      destroy (wobject);
      destroy (head);
      return (NIL);
    }


  if (WRecord_setCompressedSize (wobject, wdatal + 4) )
    {
      destroy (wobject);
      destroy (head);
      return (NIL);
    }

  /* checking the presence of the two end CRLF */
  w_fseek_from_start (FH, recbegin);

  w_fseek_from_here (FH, (warc_u64_t) wdatal);

  w_ftell (FH, recend);

  unless (WFile_checkEndCRLF (FH) )
  {
    WarcDebugMsg ("failed to copy the record in a temporary space");
    destroy (head);
    destroy (wobject);
    return (NIL);
  }


  oldhd = WRecord_setHeader (wobject, head);

  destroy (oldhd);


  datasize = recend - blbegin;

  if (WRecord_setContentSize (wobject, datasize) )
    {
      destroy (wobject);
      return (NIL);
    }

  /* setting the offset of the data bloc in the WRecord */

  if (WRecord_setWoffset (wobject, blbegin) )
    {
      destroy (wobject);
      return (NIL);
    }

  /* setting the pointer to the WFile in the WRecord */

  if (WRecord_setWfile (wobject, self) )
    {
      destroy (wobject);
      return (NIL);
    }

  return (wobject);
}


/**
 * @param _self WFile object
 *
 * @return WRecord if succeeds, NIL otherwise
 *
 * Returns a Warc Record from the Warc File
 */

WPUBLIC void * WFile_nextRecord (void * _self)
{

  struct WFile  * self = _self;

  /* preconditions */
  CASSERT (self);

  /* stop at end of file */

  if (w_feof (FH) )
    return (NIL);

  /* We must be in WARC_FILE_READER mode to extract Warc Records */
  if (MODE != WARC_FILE_READER)
    return (NIL);

  switch (COMP)
    {
      case WARC_FILE_COMPRESSED_GZIP:
        return (WFile_nextRecordGzipCompressed (self) );

      case WARC_FILE_UNCOMPRESSED:
        return (WFile_nextRecordUncompressed (self) );

      default:
        WarcDebugMsg ("WFile opened with an unknown mode");
    }

  return (NIL);
}


/**
 * @param _self WFile object instance
 * @param wrec the WRecord to register
 * @param callback function that user will call to extract data
 * @param env user data that will be filled
 *
 * @return False if succeeds, True otherwise
 *
 * Warc Record call back function registration
 */

WPUBLIC warc_bool_t WFile_register (void* _self, void * wrec,
                                    warc_bool_t (* callback)
                                    (void*, const char *, const warc_u32_t),
                                    void * env)
{

  struct WFile * self      = _self;

  warc_u64_t     wcurrent  = -1;   /* to return to the
                                      current postion of the WFile */
  warc_u64_t     offset    = 0;   /* to recover the bloc offset
                                      in the Warc file */
  warc_u64_t     size      = 0;    /* to recover the bloc size from
                                      the record */

  warc_u32_t     ret       = 0;
  void         * objwtfile = NIL; 
  void         * gzobj     = NIL; /* to manage compression */
  warc_u64_t     usize     = 0;
  warc_u64_t     csize     = 0;
  FILE         * wtfile    = NIL;


  /* Precondition */
  CASSERT (self);
  assert (wrec);

  /* We must be in WFILE_READ mode to extract Warc Records */

  if (MODE != WARC_FILE_READER)
    return (WARC_TRUE);

  /* check if "self" is the correct WFile object */
  if (self != WRecord_fromWho (wrec) )
    return (WARC_TRUE);

  unless (callback)
  {
    WarcDebugMsg ("NULL callback pointer");
    return (WARC_TRUE);
  }

  if (WRecord_setCallback (wrec, callback) )
    return (WARC_TRUE);

  if (WRecord_setEnv (wrec, env) )
    return (WARC_TRUE);

  offset = WRecord_getWoffset (wrec);

  if (offset == WARC_U64_MAX)
    return (WARC_TRUE);

  switch (COMP)
    {

      case WARC_FILE_COMPRESSED_GZIP:
        unless (WRecord_getCHeaderPresent (wrec) )
        {
          w_fseek_from_here (WTempFile_handle (WRecord_getDataFile (wrec)), offset);
        }

        else
          {
            warc_u64_t off;

            w_ftell (FH, wcurrent);
            w_fseek_from_start (FH, offset);

            gzobj = bless (WGzip);
            assert (gzobj);

/*            WTempFile_reset (TEMP_FILE); */

             if ( WRecord_makeDataFile (wrec, WString_getText(WORKING_DIR), WString_getLength(WORKING_DIR)))
              {
                WarcDebugMsg ("faild to create record data file");
                return (WARC_TRUE);
              }

             objwtfile = WRecord_getDataFile (wrec);
             unless (objwtfile) 
               { 
                WarcDebugMsg ("unable to create temporary space"); 
                return (WARC_TRUE); 
               }

            wtfile = WTempFile_handle (objwtfile);
            
            w_ftell (FH, off);
            ret = WGzip_uncompress (gzobj, FH, off, & usize,
                                    & csize, wrecover, (void *) wtfile);

            if (ret)
              {
                WarcDebugMsg ("unable to uncompress a gzipped record");
                destroy (gzobj);
                return (WARC_TRUE);
              }

            w_fseek_from_start (FH, wcurrent);

            destroy (gzobj);
            w_fseek_from_end (wtfile, -4);

            w_ftell (wtfile, offset);

            unless (WFile_checkEndCRLF (wtfile) )
            {
              WarcDebugMsg ("missing double CRLF");
              return (WARC_TRUE);
            }

            w_ftruncate (w_fileno (wtfile), offset);
            w_fflush (wtfile);

            w_fseek_from_here (wtfile, (-1 * WRecord_getDataSize (wrec) -4));

          }

        break;

      case WARC_FILE_UNCOMPRESSED:

/*       WTempFile_reset (TEMP_FILE);*/

         if (WRecord_makeDataFile (wrec, WString_getText(WORKING_DIR), WString_getLength(WORKING_DIR)))
           {
            WarcDebugMsg ("faild to create record data file");
            w_fseek_from_start (FH, wcurrent);
            return (WARC_TRUE);
            }

         objwtfile = WRecord_getDataFile (wrec);

         unless (objwtfile) 
         { 
           WarcDebugMsg ("unable to create temporary space"); 
           return (WARC_TRUE); 
         }

        wtfile = WTempFile_handle (objwtfile);

        size = WRecord_getDataSize (wrec);

        w_ftell (FH, wcurrent);
        w_fseek_from_start (FH, offset);

        if (WFile_fillTempFile (self, wtfile, size) )
          {
            w_fseek_from_start (FH, wcurrent);
            return (WARC_TRUE);
          }

        w_fseek_start (wtfile);

        w_fseek_from_start (FH, wcurrent);

        break;

      default:
        WarcDebugMsg ("WFile opened with an unknown mode");
        return (WARC_TRUE);
    }

  return (WARC_FALSE);
}


/**
 * @param _self WFile object instance
 * @param offset unsigned integer offset
 *
 * @return WARC_TRUE if succeeds, WARC_FALSE otherwise
 *
 * Moves to this offset in the WARC file
 */


WPUBLIC warc_bool_t WFile_seek (void * _self, const warc_u64_t offset)
{

  struct WFile * self = _self;

  /* Preconditions */
  CASSERT (self);

  w_fseek_from_start (FH, offset);

  return (WARC_FALSE);
}

/**
 * @param _self WFile object instance
 * @param max_size the new maximum Warc File size
 *
 * @return WARC_TRUE if succeeds, WARC_FALSE otherwise
 *
 * WFile File Maximum Size value updating function
 */
WPUBLIC warc_bool_t WFile_setMaxSize (void * _self, const warc_u64_t max_size)
{

  struct WFile * self = _self;

  /* Preconditions */
  CASSERT (self);

  if (MAXSIZE < max_size)
    MAXSIZE = max_size;
  else
    return (WARC_FALSE);

  return (WARC_TRUE);
}


/**
 * @param wtfile The output Temporary Warc Record File
 *
 * @return none
 *
 * Warc Record Warc ID field writing function
 */

WIPRIVATE void WFile_writeWarcId (FILE * wtfile)
{


  warc_u32_t l = w_strlen((warc_u8_t *) WARC_VERSION);

/*   w_fwrite (WARC_VERSION "                   ", WARC_ID_LENGTH, 1, wtfile); */
  w_fwrite (WARC_VERSION, l, 1, wtfile);
  w_fwrite ("\r\n", 2, 1, wtfile);

}


/**
 * @param wtfile The output Temporary Warc Record File
 * @param Type The record type as a const WRecordType
 * @param [out]datalength The Warc Record Data Length field to be updated
 *
 * @return none
 *
 * Warc Record Type Field writing function
 */

WPRIVATE void WFile_writeRecordType (FILE * wtfile, warc_rec_t type,
                                     warc_u64_t  * datalength)
{

  w_fwrite ("WARC-Type: ", 11, 1, wtfile);
  (*datalength) += 11;

  switch (type)
    {

      case WARC_UNKNOWN_RECORD:
        w_fwrite ("unknown", 7, 1, wtfile);
        (* datalength) += 7;
        break;

      case WARC_INFO_RECORD:
        w_fwrite ("warcinfo", 8, 1, wtfile);
        (* datalength) += 8;
        break;

      case WARC_RESPONSE_RECORD:
        w_fwrite ("response", 8, 1, wtfile);
        (* datalength) += 8;
        break;

      case WARC_REQUEST_RECORD:
        w_fwrite ("request", 7, 1, wtfile);
        (* datalength) += 7;
        break;

      case WARC_METADATA_RECORD:
        w_fwrite ("metadata", 8, 1, wtfile);
        (* datalength) += 8;
        break;

      case WARC_REVISIT_RECORD:
        w_fwrite ("revisit", 7, 1, wtfile);
        (* datalength) += 7;
        break;

      case WARC_CONVERSION_RECORD:
        w_fwrite ("conversion", 10, 1, wtfile);
        (* datalength) += 10;
        break;

      case WARC_CONTINUATION_RECORD:
        w_fwrite ("continuation", 12, 1, wtfile);
        (* datalength) += 12;
        break;

      case WARC_RESOURCE_RECORD:
        w_fwrite ("resource", 8, 1, wtfile);
        (* datalength) += 8;
        break;
    }

  w_fwrite ("\r\n", 2, 1, wtfile);

  (* datalength) += 2;
}


/**
 * @param wtfile The output Temporary Warc Record File
 * @param suri The Warc Record Subject Uri as a const char *
 * @param [out]datalength: The Warc Record datalength value to be updated
 *
 * @return none
 *
 * Warc Record Subject Uri Field writing function
 */

WIPRIVATE void WFile_writeSubjectUri (FILE * wtfile,
                                      const warc_u8_t * suri,
                                      warc_u32_t s,
                                      warc_u64_t * datalength)
{

  w_fwrite ("WARC-Target-URI: ", 17, 1, wtfile);
  (* datalength) += 17;

  w_fwrite (suri, s, 1, wtfile);
  (* datalength) += s;

  w_fwrite ("\r\n", 2, 1, wtfile);
  (* datalength) += 2;
}


/**
 * @param wtfile The output Temporary Warc Record File
 * @param cdate The Warc Record Creation Date as a const char *
 * @param [out]datalength The Warc Record Data Length Value to be updated
 *
 * @return none
 *
 * Warc Record Creation Date field writing function
 */

WIPRIVATE void WFile_writeCreationDate (FILE * wtfile,
                                        const warc_u8_t * cdate,
                                        warc_u32_t s,
                                        warc_u64_t * datalength)
{
  w_fwrite ("WARC-Date: ", 11, 1, wtfile);
  (* datalength) += 11;

  w_fwrite (cdate, s , 1, wtfile);
  (* datalength) += s ;

  w_fwrite ("\r\n", 2, 1, wtfile);
  (* datalength) += 2;
}


/**
 * @param wtfile The output Temporary Warc Record File
 * @param ctype The Warc Record Contenet Type as a const char*
 * @param [out]datalength The Warc Record Data Length Value to be updated
 *
 *@return none
 *
 * Warc Record Contenet Type writing function
 */

WIPRIVATE void WFile_writeContentType (FILE * wtfile,
                                       const warc_u8_t * ctype,
                                       warc_u32_t s,
                                       warc_u64_t * datalength)
{
  w_fwrite ("Content-Type: ", 14, 1, wtfile);
  (*datalength) += 14;

  w_fwrite (ctype, s , 1, wtfile);
  (* datalength) += s ;

  w_fwrite ("\r\n", 2, 1, wtfile);
  (* datalength) += 2;
}


/**
 * @param wtfile The output Temporary Warc Record File
 * @param rec_id The Warc Record Id as a const char *
 * @param [out]datalength The Warc Record Data Length Value to be updated
 *
 * @return none
 *
 * Warc Record Id Filed writing function
 */

WIPRIVATE void WFile_writeRecordId (FILE * wtfile,
                                    const warc_u8_t * rec_id,
                                    warc_u32_t s,
                                    warc_u64_t * datalength)
{
  w_fwrite ("WARC-Record-ID: ", 16, 1, wtfile);
  (* datalength) += 16;

  w_fwrite (rec_id, s , 1, wtfile);
  (* datalength) += s ;

  w_fwrite ("\r\n", 2, 1, wtfile);
  (* datalength) += 2;
}



/**
 * @param wtfile The output Temporary Warc Record File
 * @param clen The Warc Record Content length
 * @param [out]datalength The Warc Record Data Length Value to be updated
 *
 * @return none
 *
 * Warc Content Length Filed writing function
 */

WIPRIVATE void WFile_writeContentLength (FILE * wtfile,
                                    warc_u64_t clen,
                                    warc_u64_t * datalength)
{
  warc_u8_t   strdat[20];

  w_fwrite ("Content-Length: ", 16, 1, wtfile);
  (* datalength) += 16;

  w_numToString (clen, strdat);

  w_fwrite (strdat, w_strlen (strdat) , 1, wtfile);
  (* datalength) += w_strlen (strdat);

  w_fwrite ("\r\n", 2, 1, wtfile);
  (* datalength) += 2;
}


/**
 * @param wtfile The output Temporary Warc Record File
 * @param rec_id The Warc Concurrent To as a const char *
 * @param [out]datalength The Warc Record Data Length Value to be updated
 *
 * @return none
 *
 * Warc Concurrent To Filed writing function
 */

WIPRIVATE void WFile_writeConcurrentTo (FILE * wtfile,
                                      const warc_u8_t * rec_id,
                                      warc_u32_t s,
                                      warc_u64_t * datalength)
{
  w_fwrite ("WARC-Concurrent-To: ", 20, 1, wtfile);
  (* datalength) += 20;

  w_fwrite (rec_id, s , 1, wtfile);
  (* datalength) += s ;

  w_fwrite ("\r\n", 2, 1, wtfile);
  (* datalength) += 2;
}

/**
 * @param wtfile The output Temporary Warc Record File
 * @param pdig The Warc Record Digest as a const char *
 * @param [out]datalength The Warc Record Data Length Value to be updated
 *
 * @return none
 *
 * Warc Record Block Digest Filed writing function
 */

WIPRIVATE void WFile_writeBlockDigest (FILE * wtfile,
                                    const warc_u8_t * dig,
                                    warc_u32_t s,
                                    warc_u64_t * datalength)
{
  w_fwrite ("WARC-Block-Digest: ", 19, 1, wtfile);
  (* datalength) += 19;

  w_fwrite (dig, s , 1, wtfile);
  (* datalength) += s ;

  w_fwrite ("\r\n", 2, 1, wtfile);
  (* datalength) += 2;
}

/**
 * @param wtfile The output Temporary Warc Record File
 * @param pdig The Warc Record Payload Digest as a const char *
 * @param [out]datalength The Warc Record Data Length Value to be updated
 *
 * @return none
 *
 * Warc Record Payload digest Filed writing function
 */

WIPRIVATE void WFile_writePayloadDigest (FILE * wtfile,
                                    const warc_u8_t * pdig,
                                    warc_u32_t s,
                                    warc_u64_t * datalength)
{
  w_fwrite ("WARC-Payload-Digest: ", 21, 1, wtfile);
  (* datalength) += 21;

  w_fwrite (pdig, s , 1, wtfile);
  (* datalength) += s ;

  w_fwrite ("\r\n", 2, 1, wtfile);
  (* datalength) += 2;
}

/**
 * @param wtfile The output Temporary Warc Record File
 * @param ip The Warc Record IP address as a const char *
 * @param [out]datalength The Warc Record Data Length Value to be updated
 *
 * @return none
 *
 * Warc Record IP Adress Filed writing function
 */

WIPRIVATE void WFile_writeIpAddress (FILE * wtfile,
                                    const warc_u8_t * ip,
                                    warc_u32_t s,
                                    warc_u64_t * datalength)
{
  w_fwrite ("WARC-IP-Address: ", 17, 1, wtfile);
  (* datalength) += 17;

  w_fwrite (ip, s , 1, wtfile);
  (* datalength) += s ;

  w_fwrite ("\r\n", 2, 1, wtfile);
  (* datalength) += 2;
}

/**
 * @param wtfile The output Temporary Warc Record File
 * @param rec_id The Referenced Warc Record Id as a const char *
 * @param [out]datalength The Warc Record Data Length Value to be updated
 *
 * @return none
 *
 * Warc Record Refers to Filed writing function
 */

WIPRIVATE void WFile_writeRefersTo (FILE * wtfile,
                                    const warc_u8_t * rec_id,
                                    warc_u32_t s,
                                    warc_u64_t * datalength)
{
  w_fwrite ("WARC-Refers-To:", 16, 1, wtfile);
  (* datalength) += 16;

  w_fwrite (rec_id, s , 1, wtfile);
  (* datalength) += s ;

  w_fwrite ("\r\n", 2, 1, wtfile);
  (* datalength) += 2;
}


/**
 * @param wtfile The output Temporary Warc Record File
 * @param trunc The Warc Record Truncation cause as a const char *
 * @param [out]datalength The Warc Record Data Length Value to be updated
 *
 * @return none
 *
 * Warc Record Truncated Filed writing function
 */

WIPRIVATE void WFile_writeTruncated (FILE * wtfile,
                                    const warc_u8_t * trunc,
                                    warc_u32_t s,
                                    warc_u64_t * datalength)
{
  w_fwrite ("WARC-Truncated: ", 16, 1, wtfile);
  (* datalength) += 16;

  w_fwrite (trunc, s , 1, wtfile);
  (* datalength) += s ;

  w_fwrite ("\r\n", 2, 1, wtfile);
  (* datalength) += 2;
}


/**
 * @param wtfile The output Temporary Warc Record File
 * @param rec_id The Related Warc Info Record Id as a const char *
 * @param [out]datalength The Warc Record Data Length Value to be updated
 *
 * @return none
 *
 * Warc Record Warc Info Filed writing function
 */

WIPRIVATE void WFile_writeWarcInfo (FILE * wtfile,
                                    const warc_u8_t * rec_id,
                                    warc_u32_t s,
                                    warc_u64_t * datalength)
{
  w_fwrite ("WARC-Warcinfo-ID: ", 18, 1, wtfile);
  (* datalength) += 18;

  w_fwrite (rec_id, s , 1, wtfile);
  (* datalength) += s ;

  w_fwrite ("\r\n", 2, 1, wtfile);
  (* datalength) += 2;
}


/**
 * @param wtfile: The output Temporary Warc Record File
 * @param fname: The Warc File name as a const char *
 * @param [out]datalength: The Warc Record Data Length Value to be updated
 *
 * @return none
 *
 * Warc Record File Name Filed writing function
 */

WIPRIVATE void WFile_writeFilename (FILE * wtfile,
                                    const warc_u8_t * fname,
                                    warc_u32_t s,
                                    warc_u64_t * datalength)
{
  w_fwrite ("WARC-Filename: ", 15, 1, wtfile);
  (* datalength) += 15;

  w_fwrite (fname, s , 1, wtfile);
  (* datalength) += s ;

  w_fwrite ("\r\n", 2, 1, wtfile);
  (* datalength) += 2;
}


/**
 * @param wtfile The output Temporary Warc Record File
 * @param rec_id The Warc Record Profile as a const char *
 * @param [out]datalength The Warc Record Data Length Value to be updated
 *
 * @return none
 *
 * Warc Record Profile Filed writing function
 */

WIPRIVATE void WFile_writeProfile (FILE * wtfile,
                                    const warc_u8_t * rec_id,
                                    warc_u32_t s,
                                    warc_u64_t * datalength)
{
  w_fwrite ("WARC-Profile: ", 14, 1, wtfile);
  (* datalength) += 14;

  w_fwrite (rec_id, s , 1, wtfile);
  (* datalength) += s ;

  w_fwrite ("\r\n", 2, 1, wtfile);
  (* datalength) += 2;
}


/**
 * @param wtfile The output Temporary Warc Record File
 * @param ptype The Warc Record payload type as a const char *
 * @param [out]datalength The Warc Record Data Length Value to be updated
 *
 * @return none
 *
 * Warc Record pyload Type  Filed writing function
 */

WIPRIVATE void WFile_writePayloadType (FILE * wtfile,
                                    const warc_u8_t * ptype,
                                    warc_u32_t s,
                                    warc_u64_t * datalength)
{
  w_fwrite ("WARC-Identified-Payload-Type: ", 30, 1, wtfile);
  (* datalength) += 30;

  w_fwrite (ptype, s , 1, wtfile);
  (* datalength) += s ;

  w_fwrite ("\r\n", 2, 1, wtfile);
  (* datalength) += 2;
}


/**
 * @param wtfile The output Temporary Warc Record File
 * @param rec_id The Warc Record Origin Segment Id as a const char *
 * @param [out]datalength The Warc Record Data Length Value to be updated
 *
 * @return none
 *
 * Warc Record origin Segment Id Filed writing function
 */

WIPRIVATE void WFile_writeSegOriginId (FILE * wtfile,
                                    const warc_u8_t * rec_id,
                                    warc_u32_t s,
                                    warc_u64_t * datalength)
{
  w_fwrite ("WARC-Segment-Origin-ID: ", 24, 1, wtfile);
  (* datalength) += 24;

  w_fwrite (rec_id, s , 1, wtfile);
  (* datalength) += s ;

  w_fwrite ("\r\n", 2, 1, wtfile);
  (* datalength) += 2;
}


/**
 * @param wtfile The output Temporary Warc Record File
 * @param snum The Segment Warc Record row 
 * @param [out]datalength The Warc Record Data Length Value to be updated
 *
 * @return none
 *
 * Segmented Warc Record Number Filed writing function
 */

WIPRIVATE void WFile_writeSegNumber (FILE * wtfile,
                                    warc_u64_t snum,
                                    warc_u64_t * datalength)
{
  warc_u8_t   strdat[20];

  strdat [0] = '\0';

  w_fwrite ("WARC-Segment-Number: ", 21, 1, wtfile);
  (* datalength) += 21;

  w_numToString (snum, strdat);

  w_fwrite (strdat, w_strlen (strdat) , 1, wtfile);
  (* datalength) += w_strlen (strdat);

  w_fwrite ("\r\n", 2, 1, wtfile);
  (* datalength) += 2;
}



/**
 * @param wtfile The output Temporary Warc Record File
 * @param tlen The segmented Warc Record Total length
 * @param [out]datalength The Warc Record Data Length Value to be updated
 *
 * @return none
 *
 * Segmented Warc Record Total Length Filed writing function
 */

WIPRIVATE void WFile_writeSegTotalLength (FILE * wtfile,
                                    warc_u64_t tlen,
                                    warc_u64_t * datalength)
{
  warc_u8_t   strdat[20];

  w_fwrite ("WARC-Segment-Total-Length: ", 27, 1, wtfile);
  (* datalength) += 27;

  w_numToString (tlen, strdat);

  w_fwrite (strdat, w_strlen (strdat) , 1, wtfile);
  (* datalength) += w_strlen (strdat);

  w_fwrite ("\r\n", 2, 1, wtfile);
  (* datalength) += 2;
}


/**
 * @param wtfile The output Temporary Warc Record File
 * @param lanvl list of anvl fields
 * @param [out]datalength The Warc Record Data Length Value to be updated
 *
 * @return none
 *
 * Warc Record anvl fields writing function
*/

WPRIVATE void WFile_writeAnvls (FILE * wtfile, const void * lanvl,
                                warc_u64_t * datalength)
{
  const void * anvl; /* recover anvl fields from Warc Record */
  warc_u32_t size, i, s;

  if (lanvl)
    {
      i    = 0;
      size = WList_size (lanvl);

      while (i < size)
        {
          /* recovering an anvl field */
          anvl = WList_getElement (lanvl, i);

          /* writing anvl field */

          /* writing key */
          s = WAnvl_getKeyLen (anvl);
          w_fwrite (WAnvl_getKey (anvl),
                    s, 1, wtfile);
          (* datalength) += s + 1;
          w_fwrite (":", 1, 1, wtfile);

          /* writing anvl value */
          s = WAnvl_getValueLen (anvl);
          w_fwrite (WAnvl_getValue (anvl),
                    s, 1, wtfile);

          (* datalength) += s + 2;
          w_fwrite ("\x0D\x0A", 2, 1, wtfile);

          ++ i;
        }
    }

  w_fwrite ("\x0D\x0A", 2, 1, wtfile);

  (* datalength) += 2;
}

/**
 * @param wtfile The output Temporary Warc Record File
 * @param bloc The Data Bloc as a File descriptor
 * @param [out]datalength The Warc Record Data Length Value to be updated
 *
 * @return none
 *
 * Warc Reord Data Bloc Writing function
 */


WPRIVATE warc_bool_t WFile_writeDataBloc (FILE * wtfile, 
                                          FILE * bloc,
                                          warc_u64_t size)
{
#define WFLUSH_SIZ 4096
  
  char          buf [WFLUSH_SIZ];
  warc_u32_t    r;
  warc_u32_t    p;
  
  /* Preconditions */
  assert (wtfile);
  assert (bloc);

  while (size)
    {
      if (size > WFLUSH_SIZ)
        r = WFLUSH_SIZ;
      else
        r = size;
      
      size = size - r;
      
      p = w_fread (buf, 1, r, bloc);
      
      if (w_ferror (wtfile) || r != p)
        {
          WarcDebugMsg ("data copying error");
          return (WARC_TRUE);
        }
      
      if (w_fwrite (buf, 1, r, wtfile) != r)
        return (WARC_TRUE);
    }
  

  /*   char Byte; */

  /*   while (size) */
  /*     { */
  /*       w_fread  (& Byte, 1, 1, bloc); */
  /*       w_fwrite (& Byte, 1, 1, wtfile); */
  /*       -- size; */
  /*     } */

  return (WARC_FALSE);
}


/**
 * @param warcf the Warc File descriptor
 * @param wtfile The output Temporary Warc Record File
 *
 * @return none
 *
 * Warc Record Temporary File Flushing function into the Warc File
 */


WPRIVATE warc_bool_t WFile_flushTemporary (FILE * warcf, FILE * wtfile)
{
#define WFLUSH_SIZ 4096

  warc_u32_t size;
  char       buf [WFLUSH_SIZ];

  /* Preconditions */
  assert (warcf);
  assert (wtfile);

  w_fseek_start (wtfile);

  while (WARC_TRUE)
    {
      size = w_fread  (buf, 1, WFLUSH_SIZ, wtfile);

      if (w_ferror (wtfile) )
        {
          WarcDebugMsg ("data flush error");
          return (WARC_TRUE);
        }

      unless (size)

      break;

      if (w_fwrite (buf, 1, size, warcf) != size)
        return (WARC_TRUE);
    }

  return (WARC_FALSE);
}


/**
 * @param _self WFile object instance
 * @param wrec the Warc record to store
 * @param datalength the size of data to write
 * @param bloc the data file from where data are stored
 *
 * @return False if succeeds, True otherwise
 *
 * Warc Record storing in compressed Warc File
 */

WPRIVATE warc_bool_t
WFile_storeRecordGzipCompressed (void * _self,
                                 const void * wrec, 
                                 warc_u64_t datalength,
                                 FILE * bloc, FILE * wtfile
                                )
{
  struct WFile * self = _self;

  void         * gzobj   = NIL;
  warc_u64_t     csize   = 0;    /* the size of the compressed data */
  warc_u64_t     where   = 0;
  warc_gzlevel_t level;

  warc_u64_t     off;

  UNUSED (datalength);

  /* map the correct compression level */
  switch (COMP)
    {
      case WARC_FILE_COMPRESSED_GZIP:
      case WARC_FILE_COMPRESSED_GZIP_DEFAULT_COMPRESSION:
        level = WARC_GZIP_DEFAULT_COMPRESSION;
        break;
      case WARC_FILE_COMPRESSED_GZIP_NO_COMPRESSION:
        level = WARC_GZIP_NO_COMPRESSION;
        break;
      case WARC_FILE_COMPRESSED_GZIP_BEST_SPEED:
        level = WARC_GZIP_BEST_SPEED;
        break;
      case WARC_FILE_COMPRESSED_GZIP_BEST_COMPRESSION:
        level = WARC_GZIP_BEST_COMPRESSION;
        break;

      default:
        return (WARC_TRUE);
    }

  w_fseek_end (wtfile);

  /* writing Data Bloc */
  WFile_writeDataBloc (wtfile, bloc, WRecord_getDataSize (wrec));

  /* writing final double CRLF */
  w_fwrite ("\r\n\r\n", 4, 1, wtfile);


  /* Compressing the  record */
  w_fseek_start (wtfile);
  gzobj = bless (WGzip);
  w_ftell (FH, where);

  if (WGzip_compress (gzobj, wtfile, FH, level, & csize) )
    {
      WarcDebugMsg ("failed to GZIP compress the record");

      w_fflush (FH);
      w_ftruncate(w_fileno (FH), where);
      w_fseek_from_start (FH, where);
      destroy (gzobj);
      return (WARC_TRUE);
    }

  destroy (gzobj), gzobj = NIL;

  w_ftell(FH, off);
  FSIZE = off;

  /* check if the WARC File is full */
  if (MAXSIZE > 0 && MAXSIZE <= off)
    {
      /* set WARC file to FULL, no more records could be added */
      ISFULL = WARC_TRUE;
    }

  return (WARC_FALSE);
}


/**
 * @param _self WFile object instance
 * @param wrec the Warc record to store
 * @param bloc the data file from where data are stored
 *
 * @return False if succeeds, True otherwise
 *
 * Warc Record storing in compressed Warc File
 */

WPRIVATE warc_bool_t
WFile_storeRecordUncompressed (void* _self, const void * wrec,
                               FILE * bloc,
                               FILE * wtfile
                               )
{

  struct WFile * self = _self;

  warc_u64_t off;


  /* flushing the Warc Record Header Temporary file into the Warc File */
  WFile_flushTemporary (FH, wtfile);

  /* writing Data Bloc */
  WFile_writeDataBloc (FH, bloc, WRecord_getDataSize (wrec) );

  /* writing final double CRLF */
  w_fwrite ("\r\n\r\n", 4, 1, FH);

  w_ftell(FH, off);
  FSIZE = off;

  /* check if the WARC File is full */
  if (MAXSIZE > 0 && MAXSIZE <= off)
    {
      /* set WARC file to FULL, no more records could be added */
      ISFULL = WARC_TRUE;
    }

  return (WARC_FALSE);
}


/**
 * @param _self WFile object instance
 * @param wrec The Warc Record to write
 *
 * @return False if succeeds, True otherwise;
 *
 * Warc Records storing funtion
 */

WPUBLIC warc_bool_t WFile_storeRecord (void* _self, const void * wrec)
{

  struct WFile * self = _self;

  FILE         * bloc;   /* to open the data bloc file */
  const void   * head;   /* recover Header from the WRecord */
  const void   * lanvl;  /* recover the anvl list from the Warc Record Header */
  FILE         * wtfile      = NIL; /* for creation of the temporary file */
  const warc_u8_t * field    = NIL; /*wil; hold the different fields value*/
  warc_u64_t     datalength  = 0;

  warc_u64_t     off;

  /* Preconditions */
  CASSERT (self);

  unless (wrec)
    return (WARC_TRUE);

  assert (self != wrec);

  /* reject any other mode except for writing */
  if (MODE != WARC_FILE_WRITER)
    {
      return (WARC_TRUE);
    }

  /* ensure that we're at the end of file */
  w_fseek_end (FH);

  bloc = WRecord_getDataFileExtern (wrec);
  unless (bloc)
    return (WARC_TRUE);

  unless (WRecord_check (wrec) )
  {
    WarcDebugMsg ("filled fields are incompatible with the WARC Record type");
    return (WARC_TRUE);
  }

  w_fseek_end (bloc);

  w_ftell (bloc, off);
  if (WRecord_getDataSize (wrec) != off )
    return (WARC_TRUE);
  w_fseek_start (bloc);

  WTempFile_reset (TEMP_FILE);
  
  wtfile = WTempFile_handle (TEMP_FILE);

  head = WRecord_getHeader (wrec);
  unless (head)
    return (WARC_TRUE);


  lanvl = WHeader_getAnvl (head);
  unless (lanvl)
    return (WARC_TRUE);

  /* writing header */

  /* Writing warc_id */
  WFile_writeWarcId (wtfile);
  datalength += w_strlen ((warc_u8_t *) WARC_VERSION) +2;

 /* writing record_type field */
  WFile_writeRecordType (wtfile, WHeader_getRecordType (head),
                         & datalength);

  /*writing record id field */
  WFile_writeRecordId (wtfile, WHeader_getRecordId (head),
                       WHeader_getRecordIdLen (head),
                       & datalength);

  /* writing creation date field */
  WFile_writeCreationDate (wtfile, WHeader_getCreationDate (head),
                           WHeader_getCreationDateLen (head),
                           & datalength);

  /* writing the Content Length field */
  WFile_writeContentLength (wtfile, WRecord_getDataSize (wrec), 
                            &datalength);

  /* writing content type field */
  field = WHeader_getContentType (head);
  if (field)
     {
      WFile_writeContentType (wtfile, field,
                          WHeader_getContentTypeLen (head),
                          & datalength);
     }

  /* writing concurrent to field */
  field = WHeader_getConcurrentTo (head);
  if (field)
     {
      WFile_writeConcurrentTo (wtfile, field,
                          WHeader_getConcurrentToLen (head),
                          & datalength);
     }

  /* writing Block Digest field */
  field = WHeader_getBlockDigest (head);
  if (field)
     {
      WFile_writeBlockDigest (wtfile, field,
                          WHeader_getBlockDigestLen (head),
                          & datalength);
     }

  /* writing Payload Digest field */
  field = WHeader_getPayloadDigest (head);
  if (field)
     {
      WFile_writePayloadDigest (wtfile, field,
                          WHeader_getPayloadDigestLen (head),
                          & datalength);
     }

  /* writing Ip Address field */
  field = WHeader_getIpAddress (head);
  if (field)
     {
      WFile_writeIpAddress (wtfile, field,
                          WHeader_getIpAddressLen (head),
                          & datalength);
     }

  /* writing Refers to field */
  field = WHeader_getRefers (head);
  if (field)
     {
      WFile_writeRefersTo (wtfile, field,
                          WHeader_getRefersToLen (head),
                          & datalength);
     }

  /* writing target Uri to field */
  field = WHeader_getSubjectUri (head);
  if (field)
     {
      WFile_writeSubjectUri (wtfile, field,
                          WHeader_getSubjectUriLen (head),
                          & datalength);
     }

  /* writing Truncated field */
  field = WHeader_getTruncated (head);
  if (field)
     {
      WFile_writeTruncated (wtfile, field,
                          WHeader_getTruncatedLen (head),
                          & datalength);
     }

  /* writing Warc Info Id field */
  field = WHeader_getInfoId (head);
  if (field)
     {
      WFile_writeWarcInfo (wtfile, field,
                          WHeader_getInfoIdLen (head),
                          & datalength);
     }

  /* writing File name field */
  field = WHeader_getFilename (head);
  if (field)
     {
      WFile_writeFilename (wtfile, field,
                          WHeader_getFilenameLen (head),
                          & datalength);
     }

  /* writing Profile field */
  field = WHeader_getProfile (head);
  if (field)
     {
      WFile_writeProfile (wtfile, field,
                          WHeader_getProfileLen (head),
                          & datalength);
     }

  /* writing payload type field */
  field = WHeader_getPayloadType (head);
  if (field)
     {
      WFile_writePayloadType (wtfile, field,
                          WHeader_getPayloadTypeLen (head),
                          & datalength);
     }

  /* writing Segmentation information field */
  field = WHeader_getSegmentOriginId (head);
  if (field)
     {
      WFile_writeSegOriginId (wtfile, field,
                          WHeader_getSegmentOriginIdLen (head),
                          & datalength);

     WFile_writeSegNumber (wtfile, WHeader_getSegmentNumber (head), & datalength);
     }

  /* writing Total segment length field */
  if (WHeader_getSegTotalLength (head))
    WFile_writeSegTotalLength (wtfile, WHeader_getSegTotalLength (head), & datalength);


  /* Writing the anvl fields */
  WFile_writeAnvls (wtfile, lanvl, & datalength);

  datalength = datalength + (warc_u32_t) WRecord_getDataSize (wrec);

  switch (COMP)
    {
      case WARC_FILE_COMPRESSED_GZIP:
      case WARC_FILE_COMPRESSED_GZIP_DEFAULT_COMPRESSION:
      case WARC_FILE_COMPRESSED_GZIP_NO_COMPRESSION:
      case WARC_FILE_COMPRESSED_GZIP_BEST_SPEED:
      case WARC_FILE_COMPRESSED_GZIP_BEST_COMPRESSION:
        return (WFile_storeRecordGzipCompressed (self, wrec, datalength, bloc,
                                                 wtfile
/*                                                , objwtfile  */
                                                ));

      case WARC_FILE_UNCOMPRESSED:
        return (WFile_storeRecordUncompressed (self, wrec, bloc,
                                               wtfile
/*                                                , objwtfile  */
                                               ));
      default:
        WarcDebugMsg ("WARC file opened with an unknown mode");
    }

  return (WARC_TRUE);
}



/**
 * @param _self WFile object instance
 *
 * @return The Warc File descriptor if succeeds, NIL otherwise
 *
 * Warc File Desctriptor providing function
 */

WIPUBLIC FILE * WFile_getFile (const void * const _self)
{

  const struct WFile * const self = _self;
  CASSERT (self);

  return (FH);
}

/**
 * @param _self: a WFile object instance
 *
 * @return The warc file descriptor as a long integer, -1 otherwise
 *
 * Gives the long integer descriptor corresponding to the opened WARC file
 */

WIPUBLIC warc_i32_t WFile_fileno (void *_self)
{

  struct WFile * self = _self;

  /* Preconditions */
  CASSERT (self);

  unless (FH)
  return (-1);

  return (fileno (FH) );
}

/**
 * @param _self: a WFile object instance
 * @param buf: the buffer char * to fill
 * @param size: the size of data to fill
 *
 * @return the number of charcaters filled into the buffer
 *
 * Function that fills a buffer with an amount of bytes
 */

WIPUBLIC warc_bool_t WFile_fillBuffer (void * _self, char * buffer,
                                       warc_i64_t size, warc_i64_t * ret)
{

  struct WFile * self = _self;

  /* Precdonditions */
  CASSERT (self);

  * ret = w_fread (buffer, 1, size, FH);

  if (w_ferror (FH) )
    return WARC_TRUE;

  if ( (* ret) < size && w_feof (FH) )
    return WARC_FALSE;

  return (WARC_FALSE);
}


/**
 * WFile_constructor - create a new WFile object instance
 *
 * @param _self WFile class object
 * @param app constructor list parameters
 *
 * @return a valid WFile object or NIL
 *
 * WARC constructor
 */

WPRIVATE void * WFile_constructor (void * _self, va_list * app)
{
  struct WFile            * const self = _self;
  const char              * fname      = va_arg (* app, const char *);
  const warc_u64_t          max_size   = va_arg (* app, const warc_u64_t);
  wfile_mode_t              mode       = va_arg (* app, wfile_mode_t);
  const wfile_comp_t        compressed = va_arg (* app, const warc_bool_t);
  const char              * dname      = va_arg (* app, const char *);
  warc_u32_t                dname_len  = w_strlen ((warc_u8_t *) dname);

  FNAME = bless (WString, fname, w_strlen ((warc_u8_t *) fname) );
  unless (FNAME)
    {
      destroy (self);
      return (NIL);
    }

  WORKING_DIR = bless (WString, dname, dname_len);
  unless (WORKING_DIR)
    {
      destroy (self);
      return (NIL);
    }

  TEMP_FILE = bless (WTempFile, dname, dname_len);
  unless (TEMP_FILE)
    {
      WarcDebugMsg ("unable to create temporary space");
      destroy (self);
      return (NIL);
    }

  COMP    = compressed;
  MODE    = mode;

  MAXSIZE = max_size;
  

  if (MODE == WARC_FILE_READER) /* reading from a WARC file */
    {
      FH = w_fopen_rb (fname);
      unless (FH)
      {
        destroy (self);
        return NIL;
      }

      w_file_size (FH, FSIZE);
      unless (FSIZE)
      {
        destroy (self);
        return (NIL);
      }

      /* uncompression levels meaning nothing when reading */
      switch ( (warc_u32_t) (COMP) )
        {

          case WARC_FILE_COMPRESSED_GZIP_DEFAULT_COMPRESSION:

          case WARC_FILE_COMPRESSED_GZIP_NO_COMPRESSION:

          case WARC_FILE_COMPRESSED_GZIP_BEST_SPEED:

          case WARC_FILE_COMPRESSED_GZIP_BEST_COMPRESSION:
            COMP = WARC_FILE_COMPRESSED_GZIP;
            break;
        }

      /* check if it's a valid GZIP file */

      if ((COMP == WARC_FILE_COMPRESSED_GZIP) || (COMP == WARC_FILE_DETECT_COMPRESSION))
        {
          warc_u64_t   where = 0;
          void       * g     = bless (WGzip);

          unless (g)
          {
            destroy (self);
            return (NIL);
          }

          w_ftell (FH, where);

          if (WGzip_check (g, FH, 0) )
            {
             if (COMP == WARC_FILE_DETECT_COMPRESSION)
                COMP = WARC_FILE_UNCOMPRESSED;
             else
              {
               w_fprintf (fprintf (stderr, "not a valid GZIP WARC file\n") );
               destroy (g);
               destroy (self);
               return (NIL);
              }
            }
           else
            COMP = WARC_FILE_COMPRESSED_GZIP;

          w_fseek_from_start (FH, where);

          destroy (g);
        }

    }

  else if (MODE == WARC_FILE_WRITER) /* writing to a WARC file */
    {
      warc_u64_t off;

/*       FH = w_fopen_wb (fname); */
      w_open_cwb (fname, FH);
      unless (FH)
      {
        destroy (self);
        return NIL;
      }

      w_fseek_end (FH);

      w_ftell (FH, off);
      FSIZE = off;

      if (MAXSIZE > 0 && FSIZE >= MAXSIZE)
        {
          w_fprintf (fprintf (stderr , "Couldn't open a WARC file with MAX size limit lesser than its actual size\n") );
          destroy (self);
          return NIL;
        }

 /*  TEMP_FILE2 = bless (WTempFile, dname, dname_len); 
   unless (TEMP_FILE2) 
     { 
       WarcDebugMsg ("unable to create temporary space"); 
       destroy (self); 
       return (NIL); 
     } */
    }

  else
    {
      destroy (self);
      return (NIL);
    }

  /* by default, assume it's not full */
  ISFULL = WARC_FALSE;
  
  return (self);
}

/**
 * @param _self WFile object instance
 *
 * WARC destructor
 */

WPRIVATE void * WFile_destructor (void * _self)
{

  struct WFile  * const self = _self;


  /* preconditions */
  CASSERT (self);

  /* free the file handle */

  if (FH)
    w_fclose (FH);

  if (FNAME)
    destroy (FNAME), FNAME = NIL;

  if (WORKING_DIR)
    destroy (WORKING_DIR), WORKING_DIR = NIL;

  if (TEMP_FILE)
    destroy (TEMP_FILE), TEMP_FILE = NIL;

/*   if (TEMP_FILE2) 
     destroy (TEMP_FILE2), TEMP_FILE2 = NIL; */

  MODE = WARC_FILE_INVALID;

  COMP = WARC_FALSE;

  MAXSIZE = 0;

  FSIZE   = 0;

  return (self);
}



/**
 * WARC WFile class
 */

static const struct Class _WFile =
  {
    sizeof (struct WFile),
    SIGN,
    WFile_constructor, WFile_destructor
  };

const void * WFile = & _WFile;
