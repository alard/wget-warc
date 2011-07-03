
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

/**
 * Portability header file
 */

#include <wport.h>


/**
 * WARC default headers
 */

#include <wclass.h>   /* bless, destroy, struct Class */
#include <wsign.h>    /* CASSET macro */
#include <afile.h>    /* class prototype */
#include <wmisc.h>    /* WARC_TRUE, WARC_FALE ... */
#include <wlist.h>    /* WList  */
#include <wstring.h>  /* WString */
#include <arecord.h>  /* ARecord */
#include <arecord.x>  /* private ARecord functions */
#include <afsmhdl.h>  /* AFsmHDLine */
#include <wmktmp.h>   /* WTempFile */
#include <wgzip.h>    /* Z_STOP_DECODING ... */
#include <wcsafe.h>

/**
 * WARC WFile class internal
 */

#define SIGN 12

struct AFile
  {

    const void * class;

    /*@{*/
    void * fname; /**< WString containing AFile File name*/
    void * dname; /**< working directory name */
    void * tempfile; /**< temporary file object */
    FILE * fh; /**< The file handle */
    afile_comp_t compressed; /**< indicates if the record is compressed */
    warc_u64_t fsize; /**< Arc file size */
    warc_u64_t sbloc ; /**< The current data bloc file size */
    /*@}*/
  };


#define FNAME         (self -> fname)
#define WORKING_DIR   (self -> dname)
#define TEMP_FILE     (self -> tempfile)
#define FH            (self -> fh)
#define COMP          (self -> compressed)
#define FSIZE         (self -> fsize)
#define SBLOC         (self -> sbloc)



/**
 * callback environnement for compressed reading
 */




/* this is a callback for uncompressing the entire ARC record */
WPRIVATE warc_u32_t arecover (const warc_u8_t * buffer,
                              const warc_u32_t nbr, void * env)
{
  FILE * out = (FILE *) env;

  /* copy the uncompressed 'nbr' bytes to out */

  if (w_fwrite (buffer, 1, nbr, out) != nbr || w_ferror (out) )
    return (Z_STOP_DECODING); /* return this value to stop the uncompression */

  return (Z_CONTINUE_DECODING);
}




/**
 * @_self: AFile object
 * @param[out]: WARC_TRUE if succeeds, WARC_FALSE otherwise
 * Check the presence of more Arc Records in the Arc File
 */

WIPUBLIC warc_bool_t AFile_hasMoreRecords (const void * const _self)
{

  const struct AFile * const self = _self;
  warc_u64_t   off;

  /* preconditions */
  CASSERT (self);

  w_ftell (FH, off);
  if ( off == FSIZE)
    {
      w_fseek_start (FH);
      return (WARC_FALSE);
    }

  else
    return (WARC_TRUE);

  return (WARC_FALSE);
}



/**
 * @param _self AFile object instance
 * @param offset unsigned integer offset
 *
 * @return WARC_TRUE if succeeds, WARC_FALSE otherwise
 *
 * Moves to this offset in the ARC file
 */


WPUBLIC warc_bool_t AFile_seek (void * _self, const warc_u64_t offset)
{

  struct AFile * self = _self;

  /* Preconditions */
  CASSERT (self);

  w_fseek_from_start (FH, offset);

  return (WARC_TRUE);
}


/**
 * @_self: AFile object instance
 * @param[out]: a valid ARecord object if succedds, NIL otherwise
 * Returns the next ARC-record in the ARC-file
 */



WPUBLIC void * AFile_nextRecord ( void * _self)
{

  struct AFile * self   = _self;
  void       * arec     = NIL;
  void       * arcfsm   = NIL;
  void       * gzobj    = NIL; /* for decompression */
  warc_u64_t   usize    = 0; /* uncompressed data size */
  warc_u64_t   csize    = 0; /* compressed data size */
  warc_u32_t   ret      = 0;
  FILE       * rectfile = NIL; /* to uncompress the ARecord */
  warc_u64_t   offset   = 0;

  warc_u64_t   off;


  /* Preconditions */
  CASSERT (self);

  if (COMP == ARC_FILE_COMPRESSED_GZIP)
    {
      WTempFile_reset (TEMP_FILE);
      rectfile = WTempFile_handle (TEMP_FILE);

      gzobj = bless (WGzip);

      w_ftell (FH, offset);

      /* fprintf(stderr, "arc record offset: %llu\n", (unsigned long long) offset); */

      ret = WGzip_uncompress (gzobj, FH, offset, & usize, & csize,
                              arecover, (void *) rectfile);

      if (ret)
        {
          WarcDebugMsg ("unable to uncompress the gzipped record");
          destroy (gzobj);
          return (NIL);
        }

      offset += csize;

      w_fseek_from_start (FH, offset);
      destroy (gzobj);
      w_fseek_start (rectfile);
      arcfsm = bless (AFsmHDL, rectfile, WORKING_DIR);

      if (AFsmHDL_run (arcfsm) )
        {

          destroy (arcfsm);
          return (NIL);
        }

      arec = AFsmHDL_transform (arcfsm);

      /* read the record data and strip out the \n\n */
/*       SBLOC = ARecord_getDataLength (arec) - 2; */
      SBLOC = ARecord_getDataLength (arec);

      if (ARecord_setAfile (arec, self) )
        {
          destroy (arec);
          destroy (arcfsm);
          return (NIL);
        }

      w_ftell (rectfile, off);
      if (ARecord_setRecordOffset (arec, off ) )
        {
          destroy (arec);
          destroy (arcfsm);
          return (NIL);
        }

      if (ARecord_setContentSize (arec, SBLOC) )
        {
          destroy (arec);
          destroy (arcfsm);
          return (NIL);
        }


      if (ARecord_setContentFromFile (arec, TEMP_FILE) )
        {
          destroy (arec);
          return (NIL);
        }

      destroy (arcfsm);

      return (arec);
    }

  else if (COMP == ARC_FILE_UNCOMPRESSED)
    {
      warc_u64_t off;

      arcfsm = bless (AFsmHDL, FH, WORKING_DIR);

      if (AFsmHDL_run (arcfsm) )
        {
          w_ftell (FH, off);
          w_fprintf (fprintf (stderr , "error in FSM state address %p, at offset %llu in the arc file\n", AFsmHDL_state (arcfsm), (long long unsigned int) off ) );
          destroy (arcfsm);
          return (NIL);
        }

      arec = AFsmHDL_transform (arcfsm);

      SBLOC = ARecord_getDataLength (arec);


      if (ARecord_setAfile (arec, self) )
        {
          destroy (arec);
          destroy (arcfsm);
          return (NIL);
        }

      w_ftell (FH, off);
      if (ARecord_setRecordOffset (arec, off ) )
        {
          destroy (arec);
          destroy (arcfsm);
          return (NIL);
        }

      w_fseek_from_here (FH , SBLOC + 1);

      if (ARecord_setContentSize (arec, SBLOC) )
        {
          destroy (arec);
          destroy (arcfsm);
          return (NIL);
        }

      destroy (arcfsm);

      return (arec);
    }

  return (NIL);
}

/**
 * @_self: AFile object instance
 * @param[out]: The size of the data bloc
 * ARC Record Data Bloc Size returning function
 */


WIPUBLIC warc_u64_t AFile_getContentSize (const  void * const _self)
{

  const struct AFile * self = _self;

  /* Preconditions */
  CASSERT (self);

  return (SBLOC);
}

/**
 * @_self: AFile object instance
 * @_atfile: the temporary file to fill
 * @_size: the size of data to write
 * @param[out]: False if succeeds, True otrherwise
 * fills the temporary file with the arc record
 */


WPRIVATE warc_bool_t AFile_fillTempFile (const void* const _self,
    FILE * atfile, warc_u64_t size)
{
#define WFLUSH_SIZ 4096

  const struct AFile * const self = _self;
  char                       buf [WFLUSH_SIZ];
  warc_u32_t                 r ;
  warc_u32_t                 p ;

  /* Preconditions */
  CASSERT (self);
  assert (atfile);

  while (size)
    {
      if (size > WFLUSH_SIZ)
        r = WFLUSH_SIZ;
      else
        r = size;

      size = size - r;

      p = w_fread (buf, 1, r, FH);

      if (w_ferror (atfile) || r != p)
        {
          WarcDebugMsg ("error when copying data");
          return (WARC_TRUE);
        }

      if (w_fwrite (buf, 1, r, atfile) != r)
        return (WARC_TRUE);
    }

  return (WARC_FALSE);

  /*    while (size) */
  /*      { */
  /*        w_fread  (& byte, 1, 1, FH); */
  /*        w_fwrite (& byte, 1, 1, atfile); */
  /*        -- size; */
  /*      } */
  /*    return (WARC_FALSE); */

}


/**
 * @_self: AFile object instance
 * @_arec: the ARecord to register
 * @_callback: function that user will call to extract data
 * @_env: user data that will be filled
 * @param[out]: False if succeeds, True otherwise
 * Arc Record call back function registration
 */

WPUBLIC warc_bool_t AFile_register (void* _self, void * arec,
                                    warc_bool_t (* callback)
                                    (void *, const char *, const warc_u32_t),
                                    void * env)
{

  struct AFile * self      = _self;
/*   void         * objatfile = NIL; */
  FILE         * atfile    = NIL;  /* the temporary file
                                      which will hold the record bloc */
  warc_i64_t     acurrent  = -1;   /* to return to the current postion
                                      of the AFile */
  warc_i64_t     offset    = -1;   /* to recover the bloc offset in the
                                      Arc file */
  warc_u32_t     size      = 0;    /* to recover the bloc size from the
                                      record */

  /* Precondition */
  CASSERT (self);

  unless (arec)
  return (WARC_TRUE);

  /* check if "self" is the correct AFile object */
  unless (self == ARecord_fromWho (arec) )
    return (WARC_TRUE);

  unless (callback)
  {
    WarcDebugMsg ("NULL callback pointer");
    return (WARC_TRUE);
  }

  if (ARecord_setCallback (arec, callback) )
    return (WARC_TRUE);

  if (ARecord_setEnv (arec, env) )
    return (WARC_TRUE);

  offset = ARecord_getRecordOffset (arec);
  if (offset < 0)
    return (WARC_TRUE);

  if (COMP == ARC_FILE_COMPRESSED_GZIP)
    {
      w_fseek_from_start (ARecord_getDataFile (arec), offset);
    }
  else if (COMP == ARC_FILE_UNCOMPRESSED)
    {
      WTempFile_reset (TEMP_FILE);
      
      atfile = WTempFile_handle (TEMP_FILE);
      
      size = ARecord_getDataSize (arec);
      
      w_ftell (FH, acurrent);
      w_fseek_from_start (FH, offset);
      
      if (AFile_fillTempFile (self, atfile, size) )
        {
          w_fseek_from_start (FH, acurrent);
          return (WARC_TRUE);
        }
      
      w_fseek_start (atfile);

      w_fseek_from_start (FH, acurrent);
      
      if (ARecord_setContentFromFile (arec, TEMP_FILE))
        {
          w_fseek_from_start (FH, acurrent);
          return (WARC_TRUE);
        }
    }
  else
    {
      WarcDebugMsg ("ARC file opened with an unknown mode");
      return (WARC_TRUE);
    }

  


  return (WARC_FALSE);
}



/**
 * AFile_constructor - create a new AFile object instance
 * @_self: AFile class object
 * @app: constructor list parameters
 * @param[out] a valid AFile object or NIL
 * ARC constructor
 */


WPRIVATE void * AFile_constructor (void * _self, va_list * app)
{

  struct AFile         * const self = _self;
  const char           * fname      = va_arg (* app, const char *);
  const afile_comp_t     compressed = va_arg (* app, const afile_comp_t);
  const char           * dname      = va_arg (* app, const char *);
  warc_u32_t             dname_len  = w_strlen ((warc_u8_t *) dname);

  FNAME = bless (WString, fname, w_strlen ( (warc_u8_t *) fname) );
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

  TEMP_FILE = bless (WTempFile, dname, dname_len);
  unless (TEMP_FILE)
    {
      WarcDebugMsg ("unable to create temporary space");
      destroy (self);
      return (NIL);
    }


  COMP  = compressed ;
  SBLOC = 0;


  /* check if it's a valid GZIP file */

  if ((COMP == ARC_FILE_COMPRESSED_GZIP) || (COMP == ARC_FILE_DETECT_COMPRESSION))
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
        if (COMP == ARC_FILE_DETECT_COMPRESSION)
          COMP = ARC_FILE_UNCOMPRESSED;
        else
          {
           w_fprintf (fprintf (stderr, "not a valid GZIP ARC file\n") );
           destroy (g);
           destroy (self);
           return (NIL);
          }
        }
      else
        COMP = ARC_FILE_COMPRESSED_GZIP;

      w_fseek_from_start (FH, where);

      destroy (g);
    }

  return (self);
}




/**
 * @_self: AFile object instance
 * ARC destructor
 */

WPRIVATE void * AFile_destructor (void * _self)
{

  struct AFile  * const self = _self;

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

  COMP =  0;

  FSIZE = 0;

  SBLOC = 0;

  return (self);
}




/**
 * ARC WFile class
 */

static const struct Class _AFile =
  {
    sizeof (struct AFile),
    SIGN,
    AFile_constructor, AFile_destructor
  };

const void * AFile = & _AFile;
