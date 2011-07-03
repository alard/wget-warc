
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
 * ARC default headers
 */

#include <wclass.h>  /* bless, destroy, cassert, struct Class */
#include <wsign.h>   /* CASSET macro */
#include <wmem.h>    /* wmalloc, wfree */
#include <wmisc.h>   /* unless */
#include <arecord.h> /* for class prototype */
#include <arecord.x> /* private ARecord functions */
#include <wstring.h> /* WString */
#include <wmktmp.h>  /* WTempFile */
#include <wrecord.x> /* for calss prototype */
#include <afile.h>   /* for class prototype */
#include <wcsafe.h>


/**
 * ARecord internal structure
 */

#define SIGN  11

struct ARecord
  {

    const void * class;

    /*@{*/
    warc_u64_t   asize;  /**< The size of data bloc */
    warc_u64_t   aoffset;  /**< The offset of the data bloc in the Arc file */
    void       * url;          /**< The ARC url */
    void       * ip_adress; /**< The ARC record ip adress */
    void       * creation_date; /**< The ARC record date of creation */
    void       * mime_type; /**< The ARC record mime type */
    void       * dname;    /**< working directory name */
    void       * env;           /**< The pointer to user data buffer */
    warc_bool_t (* callback) (void *, const char *, warc_u32_t);
    /**< The pointer to the user call back function */
    void       * datafile; /**< The data File descriptor */
    FILE       * who; /**< The Arc file wich containt the record */
    warc_u64_t   data_length; /**< The ARC record data_length */

    /*@}*/
  };



#define    URL             (self -> url)
#define    DATA_LENGTH     (self -> data_length)
#define    MIME_TYPE       (self -> mime_type)
#define    IP_ADRESS       (self -> ip_adress)
#define    CREATION_DATE   (self -> creation_date)
#define    CBACK           (self -> callback)
#define    ENV             (self -> env)
#define    OFFSET          (self -> aoffset)
#define    DATAF           (self -> datafile)
#define    WHO             (self -> who)
#define    ASIZE           (self -> asize)
#define    WORKING_DIR     (self -> dname)


/**
 * @_self: ARecord object
 * @param[out]: constant url string
 * Returns the URL of the ARC-record
 */

WIPUBLIC const warc_u8_t * ARecord_getUrl (const void * const _self)
{

  const struct ARecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WString_getText (URL) );
}

/**
 * @_self: ARecord object
 * @_uri: URL string
 * @_len: URL string length
 * @param[out]: False if succseeds, True otherwise
 * Sets the ARC URL field of the ARC-record to "uri"
 */

WIPUBLIC warc_bool_t ARecord_setUrl (void * const _self,
                                     const warc_u8_t * uri,
                                     const warc_u32_t len)
{

  struct ARecord * const self = _self;

  /* preconditions */
  CASSERT (self);
  assert (uri);

  /* reset the text */
  unless (WString_setText (URL, uri, len) )
  return (WARC_TRUE);

  return (WARC_FALSE);
}


/**
 * @_self: ARecord object
 * @param[out]: data length as a 32 bits unsigned integer
 * Returns the data length of the ARC-record
 */

WIPUBLIC warc_u64_t ARecord_getDataLength (const void * const _self)
{

  const struct ARecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (DATA_LENGTH);
}

/**
 * @_self: ARecord object
 * @_len: data length in bytes
 * @param[out]: False if succseeds, True otherwise
 * Sets the data length of the ARC-record
 */

WIPUBLIC warc_bool_t ARecord_setDataLength (void * const _self,
                                            const warc_u64_t len)
{

  struct ARecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (len)
  return (WARC_TRUE);

  DATA_LENGTH = len;

  return (WARC_FALSE);
}

/**
 * @_self: ARecord object
 * @param[out]: constant creation date string
 * Returns the creation date of the ARC-record
 */

WIPUBLIC const warc_u8_t * ARecord_getCreationDate (const void * const _self)
{

  const struct ARecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WString_getText (CREATION_DATE) );
}



/**
 * @_self: ARecord object
 * @_cd: creation date string
 * @_len: creation date string length
 * @param[out]: False if succseeds, True otherwise
 * Sets the creation date of the ARC-record
 */

WIPUBLIC warc_bool_t ARecord_setCreationDate (void * const _self,
    const warc_u8_t * cd,
    const warc_u32_t len)
{

  struct ARecord * const self = _self;

  /* preconditions */
  CASSERT (self);
  assert (cd);

  /* reset the text */
  unless (WString_setText (CREATION_DATE, cd, len) )
  return (WARC_TRUE);

  return (WARC_FALSE);
}


/**
 * @_self: ARecord object
 * @param[out]: Mime type constant string
 * Returns the ARC-record mime type
 */

WIPUBLIC const warc_u8_t * ARecord_getMimeType (const void * const _self)
{

  const struct ARecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WString_getText (MIME_TYPE) );
}


/**
 * @_self: ARecord object
 * @_ct: Mime type string
 * @_len: ARC Mime string length
 * @param[out]: False if succseeds, True otherwise
 * Sets the ARC-record Mime type
 */

WIPUBLIC warc_bool_t ARecord_setMimeType (void * const _self,
    const warc_u8_t * ct,
    const warc_u32_t len)
{

  struct ARecord * const self = _self;

  /* preconditions */
  CASSERT (self);
  assert (ct);

  /* reset the text */
  unless (WString_setText (MIME_TYPE, ct, len) )
  return (WARC_TRUE);

  return (WARC_FALSE);
}


/**
 * @_self: ARecord object
 * @param[out]: IP_adress constant string
 * Returns the ARC-record IP_adress.
 */

WIPUBLIC const warc_u8_t * ARecord_getIpAddress (const void * const _self)
{

  const struct ARecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WString_getText (IP_ADRESS) );
}


/**
 * @_self: ARecord object
 * @_ip: IP-adress constant string
 * @_len: IP_adress string length
 * @param[out]: False if succseeds, True otherwise
 * Sets the ARC-record IP-adress
 */

WIPUBLIC warc_bool_t ARecord_setIpAddress (void * const _self,
    const warc_u8_t * ip,
    const warc_u32_t len)
{

  struct ARecord * const self = _self;

  /* preconditions */
  CASSERT (self);
  assert (ip);

  /* reset the text */
  unless (WString_setText (IP_ADRESS, ip, len) )
  return (WARC_TRUE);

  return (WARC_FALSE);
}




/**
 * @_self: Arecord object instance
 * @_dataf: Arc Data File *
 * @param[out]: False if succeeds, True otherwise
 * Arc Record Data File descriptor updating
 */

WIPUBLIC warc_bool_t ARecord_setContentFromFile (void * _self, void * dataf)
{

  struct ARecord * self  = _self;

  /* Preconditions */
  CASSERT (self);

  unless (dataf)
  return (WARC_TRUE);

  DATAF = dataf;
  return (WARC_FALSE);
}


/**
 * @_self: Arecord object instance
 * @param[out]: the Arc Data Bloc File *
 * Arc Record Data File descriptor provider
 */

WPUBLIC FILE * ARecord_getDataFile (const void * const _self)
{

  const struct ARecord * const self = _self;

  /* Preconditions */
  CASSERT (self);

  unless (DATAF)
  return (NIL);

  return (WTempFile_handle (DATAF) );
}

/**
 * @_self: ARecord object instance
 * @param[out]: the Arc Data Bloc File size
 * Arc Record Data Bloc file Size returning function
 */

WIPUBLIC warc_u64_t ARecord_getDataSize (const void * const _self)
{

  const struct ARecord * const self = _self;

  /* Preconditions */
  CASSERT (self);

  return (ASIZE);
}

/**
 * @_self: ARecord object instance
 * @_sz: the new DataBloc size
 * @param[out]: False if success True otherwise
 * Arc Record Data Bloc file set size
 */

WPUBLIC warc_bool_t ARecord_setContentSize (void * _self, warc_u64_t sz)
{

  struct ARecord *  self = _self;

  /* Preconditions */
  CASSERT (self);

  unless (sz)
  return (WARC_TRUE);

  ASIZE = sz;

  return (WARC_FALSE);
}

/**
 * @_self: ARecord object instance
 * @param[out]: True if still there is data in the record data file, False otherwise
 * Arc Record data file access function
 */

WIPUBLIC warc_bool_t ARecord_getContent (const void* const _self)
{
#define CONTENT_BUFSIZ 4096

  const struct ARecord * const self   = _self;
  char                         buff   [CONTENT_BUFSIZ];
  warc_u32_t                   size   = 0;
  FILE                       * handle = NIL;
  CASSERT (self);

  unless (DATAF)
  return (WARC_TRUE);

  handle = WTempFile_handle (DATAF);

  while (WARC_TRUE)
    {
      size = w_fread (buff, 1, CONTENT_BUFSIZ, handle);

      if (w_ferror (handle) )
        return (WARC_TRUE);

      if (size)
        {
          unless (CBACK (ENV, buff, size) )
          break;
        }

      if (w_feof (handle) )
        break;
    }

  return (WARC_FALSE);
}


/**
 * @_self: ARecord object instance
 * @param[out]: the offset of the data bloc in a Arc File
 * ARecord data bloc offset provider
 */

WIPUBLIC warc_u64_t ARecord_getRecordOffset (const void * const _self)
{

  const struct ARecord * const self = _self;

  /* Preconditions */
  CASSERT (self);

  return (OFFSET);
}

/**
 * @_self: ARecord object instance
 * @_offset: the new offset in a Arc file
 * @param[out]: False if succeeds, True otherwise
 * ARecord offset in Arc File updating function
 */

WIPUBLIC warc_bool_t ARecord_setRecordOffset (void * _self,
    const warc_u64_t offset)
{

  struct ARecord * self = _self;

  /* Preconditions */
  CASSERT (self);

  if (offset != WARC_U64_MAX)
    {
      OFFSET = offset;
      return (WARC_FALSE);
    }

  return (WARC_TRUE);
}

/**
 * @_self: ARecord object instance
 * @_cback: the callback function
 * @param[out]: False if succeeds, True otherwise
 * ARecord callback function setting
 */

WIPUBLIC warc_bool_t ARecord_setCallback (void * _self,
    warc_bool_t (* cback)
    (void* env, const char * buff,
     warc_u32_t size) )
{

  struct ARecord * self = _self;

  /* Precondition */
  CASSERT (self);

  unless (cback)
  return (WARC_TRUE);

  CBACK = cback;

  return (WARC_FALSE);
}

/**
 * @_self: ARecord instance
 * @_env: The user environnement
 * @param[out]: False if succeeds, True otherwise
 * ARecord user environnement setting function
 */

WIPUBLIC warc_bool_t ARecord_setEnv (void * _self, void * env)
{

  struct ARecord * self = _self;

  /* Preconditions */
  CASSERT (self);

  ENV = env;

  return (WARC_FALSE);
}


/**
 * @_self: ARecord object instance
 * @param[out]: The pointer to the AFile object containing the Record, NIL otherwise
 * ARecord matching AFile object pointer provider
 */

WIPUBLIC void * ARecord_fromWho (const void * const _self)
{

  const struct ARecord * const self = _self;

  /* Preconditions */
  CASSERT (self);

  return (WHO);
}


/**
 * @_self: ARecord object instance
 * @_wfile: the pointer to the AFile object that fill contain the record
 * @param[out]: False if succeeds, True otherwise
 * Sets the AFile object containing the ARecord
 */

WIPUBLIC warc_bool_t ARecord_setAfile (void * _self, void * wfile)
{

  struct ARecord * self = _self;

  /* Preconditions */
  CASSERT (self);

  unless (wfile)
  return (WARC_TRUE);

  WHO = wfile;
  return (WARC_FALSE);
}


/**
 * callback to extract the record content
 */

struct writingstruct
  {
    FILE * _tfile;
    warc_u32_t _wrtsize;
  };

warc_bool_t temp_writer (void * _envstr, const char * buff, warc_u32_t size)
{

  struct writingstruct * envstr   = _envstr;
  FILE                 * fout     = envstr -> _tfile;
  warc_u32_t             wrtsize  = envstr -> _wrtsize;
  warc_u32_t             realsize = size;

  warc_u64_t             off;

  w_ftell (fout, off);
  if (off + size > wrtsize)
    {
      realsize = wrtsize - off;
    }
  w_fwrite (buff, realsize, 1, fout);

  return (WARC_TRUE);
}


/**
 * @_self: ARecord onject instance
 * @_wrec: the WRecord object to fill
 * @_arcf: the Arc File object wher the Arecord will be registrated
 * @param[out]: False if succeeds, True otherwise
 * Arc Record passing context to Warc Record
 */

WIPUBLIC warc_bool_t ARecord_transferContent (void * _self, void * wrec,
    void * arcf)
{

  struct ARecord * self      = _self;
  void           * objatfile = NIL; /* for temporeary file creation */
  FILE           * fin       = NIL;

  struct writingstruct        envstr;

  /* Precondition */
  CASSERT (self);

  if (WRecord_setContentSize (wrec, ARecord_getDataSize (self) ) )
    {
      return  (WARC_TRUE);
    }


  objatfile = bless (WTempFile, WString_getText(WORKING_DIR), WString_getLength(WORKING_DIR));
  assert (objatfile);

  fin = WTempFile_handle (objatfile);
  unless (fin)
  {
    destroy (objatfile);
    return  (WARC_TRUE);
  }

  envstr . _tfile = fin;
  envstr . _wrtsize = ASIZE;

  if (AFile_register (arcf, self, temp_writer, (void *) & envstr) )
    {
      w_fprintf (fprintf (stderr , "Unable to register the extracted ArcRecord object\n") );
      destroy (objatfile);
      return (WARC_TRUE);
    }

  ARecord_getContent (self);

  w_fseek_end (fin);

  if (WRecord_setContentFromArc (wrec, objatfile) )
    {
      destroy (objatfile);
      return  (WARC_TRUE);
    }

  return (WARC_FALSE);
}


/**
 * @_self: ARecord object
 * @param[out]: none
 * Free ARecord's object memory
 */

WPRIVATE void freeARecord (void * _self)
{

  struct ARecord * self = _self;

  /* preconditions */
  CASSERT (self);

  if (URL)
    destroy (URL), URL = NIL;

  if (CREATION_DATE)
    destroy (CREATION_DATE), CREATION_DATE = NIL;

  if (MIME_TYPE)
    destroy (MIME_TYPE), MIME_TYPE = NIL;

  if (IP_ADRESS)
    destroy (IP_ADRESS), IP_ADRESS = NIL;

/*   if (DATAF) */
/*     destroy (DATAF), DATAF = NIL; */

  if (WHO)
    WHO = NIL;

  if (CBACK)
    CBACK = NIL;

  if (ENV)
    ENV = NIL;

  if (OFFSET)
    OFFSET = -1;

  if (ASIZE)
    ASIZE = 0;
}

/**
 * ARecord_constructor - create a new ARecord object instance
 * @_self: ARecord class object
 * @app: constructor list parameters
 * @param[out] a valid ARecord object or NIL
 * ARC Record Line constructor
 */

WPRIVATE void * ARecord_constructor (void * const _self, va_list * app)
{

  struct ARecord * const   self  = _self;

  const char             * url   = va_arg (* app, const char *);
  const warc_u32_t         url_l = va_arg (* app, const warc_u32_t);

  const char             * ip    = va_arg (* app, const char *);

  const warc_u32_t         ip_l    = va_arg (* app, const warc_u32_t);

  const char             * cd    = va_arg (* app, const char *);
  const warc_u32_t         cd_l  = va_arg (* app, const warc_u32_t);

  const char             * ct    = va_arg (* app, const char *);
  const warc_u32_t         ct_l  = va_arg (* app, const warc_u32_t);
  const warc_u32_t         dt    = va_arg (* app, const warc_u32_t);

  void                   * wdir  = va_arg (* app, void *);

  URL = bless (WString, url, url_l);
  unless (URL)
  {
    freeARecord (self);
    return (NIL);
  }

  IP_ADRESS = bless (WString, ip, ip_l);
  unless (IP_ADRESS)
  {
    freeARecord (self);
    return (NIL);
  }

  CREATION_DATE = bless (WString, cd, cd_l);
  unless (CREATION_DATE)
  {
    freeARecord (self);
    return (NIL);
  }

  MIME_TYPE = bless (WString, ct, ct_l);
  unless (MIME_TYPE)
  {
    freeARecord (self);
    return (NIL);
  }

  WORKING_DIR = wdir;


  DATAF = NIL;
  DATA_LENGTH = dt;
  CBACK = NIL;

  return self;
}


/**
 * ARecord_destructor - delete an existing WString object
 * @_self: ARecord object instance
 * ARC Header Line destructor
 */

WPRIVATE void * ARecord_destructor (void * _self)
{
  freeARecord (_self);

  return (_self);
}


/**
 * ARC ARecord class
 */

static const struct Class _ARecord =
  {
    sizeof (struct ARecord),
    SIGN,
    ARecord_constructor, ARecord_destructor
  };

const void * ARecord = & _ARecord;
