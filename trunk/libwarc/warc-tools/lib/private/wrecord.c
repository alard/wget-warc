
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


#include <wclass.h>  /* bless, destroy, cassert, struct Class */
#include <wsign.h>   /* CASSET macro */
#include <wrecord.h> /* for class's prototypes */
#include <wstring.h> /* WString */
#include <wmktmp.h>  /* WTempFile */
#include <wmisc.h>   /* unless */
#include <wcsafe.h>  /*w_strcasestr  */
#include <wheader.h>
#include <wlist.h>
#include <wanvl.h>
#include <wcsafe.h>
#include <wfile.h>

#define _GNU_SOURCE

#define makeU(s) ((warc_u8_t *) (s))
#define uS(s)  ((warc_u8_t *) (s))
#define makeS(s) uS(s), w_strlen (uS(s))

#define CR   ('\r')
#define LF   ('\n')
#define M(c) (((c) == CR) || ((c) == LF))
#define X(c) (! (M(c)) )



enum states { S1 = 1, S2, S3, S4, S5, S11, S12};
enum actions {SEARCHING_HTTP = 1, IGNORE_ALL, WRITEING};

struct user_struct {
   warc_u32_t * fsm;
   warc_u32_t * action;
   void * line; /* a WString object */
   warc_bool_t was_cr;
/*   warc_u32_t next_offset; */
   FILE * file;
   void * http_code;
};


/**
 * WARC WRecord class internal
 */

#define SIGN 5

#define MASK1    0x0000000F
#define AND_MSK1 0x00000001
#define OR_MSK1  0x00000061
#define AND_MSK2 0x00000011
#define OR_MSK2  0x00000457
#define AND_MSK3 0x00000051
#define OR_MSK3  0x0000045F
#define AND_MSK4 0x00000001
#define OR_MSK4  0x0000045F
#define AND_MSK5 0x00000011
#define OR_MSK5  0x00000459
#define AND_MSK6 0x00000190
#define OR_MSK6  0x000007D0

struct WRecord
  {

    const void * class;

    /*@{*/
    void * header; /**< Header object */
    void * tdatafile; /**< Temporary Data File descriptor */
    FILE * externdfile; /**< User given data file descriptor */
    void * arccontent; /**< data File recovered from an Arc Record */
    void * content_from_string; /**< data content from string */
    warc_u32_t check; /**< Mandatory Fields presence cheking value */
    warc_u32_t opt_check; /**< Optional fields checking value */
    warc_u64_t size; /**< Warc Data File size */
    warc_bool_t (* callback) (void* env, const char * buff, warc_u32_t size); /**< Pointer to the user call back function */
    void * env; /**< pointer to user data buffer */
    warc_i64_t woffset; /**< the offset of the data bloc in the warc file */
    void * who; /**< The WFile object representing the WarcFile which contains this record*/
    warc_bool_t presentchdr; /**< in compressed record case, to see if
                           warc the compressionheader where present */
    warc_u64_t  csize; /**< the compressed size of the record (in compressed cas) */
    warc_u64_t  usize; /**< the uncompressed size of the record */
    warc_u64_t  offsetinfile; /**< the begining offset of the record in the Warc File */

    /*@}*/
  };


#define HDL     (self -> header)
#define DATAF   (self -> tdatafile)
#define EDATAF  (self -> externdfile)
#define CHECK   (self -> check)
#define OPT_CHK (self -> opt_check)
#define CBACK   (self -> callback)
#define ENV     (self -> env)
#define SIZE    (self -> size)
#define OFFSET  (self -> woffset)
#define WHO     (self -> who)
#define CHDP    (self -> presentchdr)
#define ACONT   (self -> arccontent)
#define CSIZE   (self -> csize)
#define USIZE   (self -> usize)
#define IOFFSET (self -> offsetinfile)
#define CONTENT (self -> content_from_string)





warc_bool_t b_callback (void * env, const char* buff, const warc_u32_t size)
{
  struct user_struct * user_data = env;

  warc_u32_t * fsm = user_data -> fsm;
  warc_u32_t * action = user_data -> action;
  FILE * file  = user_data -> file;
  void * http_code = user_data -> http_code;

  void * line = user_data -> line;
  
  warc_u32_t i = 0;

check_action:
switch (*action)
   {
    case SEARCHING_HTTP: /* Here, we look for the HTTP response code */

         while (i < size)
             {
              if (buff [i] == CR)
                 {
                  i++;
                  user_data -> was_cr = WARC_TRUE;

                  if (i == size)
                     break;
                 }
               /* if we detecte a CRLF */
              if (user_data -> was_cr && buff[i] == LF)
                 {
                 user_data -> was_cr = WARC_FALSE;
                 unless (w_strcmp (WString_getText (line), uS ("")))
                      {
                       WString_setText (http_code, makeS ("997"));
                       i++;
                       *action = WRITEING; /*what comes next is considered as the content and it will be printed*/
                       goto check_action;
                      }

                  unless (strncmp ("HTTP/", (const char *) WString_getText (line), 5)) /* we chack if it is the HTTP code line */
                     {
                     /* if it is, we print the code */
                      *action = IGNORE_ALL;
                      i++;
                      WString_setText (http_code, WString_getText (line) + 9, 3);
                      *fsm = S11;
                      goto check_action;
                     }
                 /* else, we get an other line */
                 WString_setText (line, makeS (""));
                 }
             else
                 {
                  if (user_data -> was_cr)
                      {
                       user_data -> was_cr = WARC_FALSE;
                       WString_append (line, uS("\r"), 1);
                       i--;
                      }
                  else
                     WString_append (line, uS(buff + i), 1);
                 }
              i++;
             }

         break;

    case IGNORE_ALL: /* here, we ignore th rest of the HTTP response until finding 2 CRLF*/

         while (*fsm != S5 && i < size)
             {
              if (*fsm == S11) /* State 11, we come from a detection of the HTTP code line */
                 {
                  if (buff [i] == CR) /* if the character is a CR, xwe look for the LF */
                    *fsm = S12;
                  else
                    *fsm = S1;
                  }
              else if (*fsm == S12) /* State 12, from state 11 */
                 {
                  if (buff [i] == LF) /* If we find an LF, we end the ognoring phase */
                    *fsm = S5;
                  else
                    *fsm = S1;
                 }
              else if (*fsm == S1) /* state 1, we look for the first CR */
                 {
                  if (buff [i] == CR) /* we go looking for the first LF */
                    *fsm = S2;
                  else
                    *fsm = S1;
                 }
              else if (*fsm == S2) /* state 2, we look for the first LF */
                 {
                  if (buff [i] == LF) /* we go looking for the second CR */
                    *fsm = S3;
                  else
                    *fsm = S1;
                 }
              else if (*fsm == S3) /* state 3, we lok for the second CR */
                 {
                  if (buff [i] == CR) /* we go looking for the second LF */
                    *fsm = S4;
                  else
                    *fsm = S1;
                 }
              else if (*fsm == S4) /* state 4, we lok for the second LF */
                 {
                  if (buff [i] == LF) /* double CRLF found, we end the ignoring phase */
                    *fsm = S5;
                  else
                    *fsm = S1;
                 }

             i++;
             }

         if (*fsm == S5)
            {
             *action = WRITEING;
             if (i < size) /* writing what reamins in the buffer on the stdout */
                if (fwrite( (buff + i), 1, size - i, file) != size - i || ferror(stdout))
                   {
                   fprintf (stderr, "+++++++++++\n");
                    destroy (line);
                    user_data -> line = NIL;
                    return (WARC_FALSE);
                   }
            }
         break;

   case WRITEING: /* writing what reamins in the data bloc on the stdout */
        if (fwrite( buff, 1, size, file) != size)
           {
            fprintf (stderr, "------------\n");}
        else if (ferror(stdout))
            {destroy (line);
            user_data -> line = NIL;
            return (WARC_FALSE);}

        break;

   default: break;
   }

  return (WARC_TRUE);
}


/* void printOrdinaryDate (warc_u8_t * field)
{
warc_u32_t i = 0;
warc_u32_t size = w_strlen (field);

while (i < size)
  {
   if (field[i] != ':' && field[i] != '-' && field[i] != 'Z' && field[i] != 'T' && field[i] != 't' && field[i] != 'z')
     fprintf (stderr, "%c", field [i]);
   i++;
  }
}
*/



/**
 * @param _self: WRecord object instance
 *
 * @return The Compressed size of the record if succeeeds, 0 otherwise
 *
 * Returns the Warc Record compressed size in compressed Warc File case
 */

WIPUBLIC warc_u64_t WRecord_getCompressedSize (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* Preconditions */
  CASSERT (self);

  unless (CSIZE)
  return (0);

  return (CSIZE);

}


/**
 * @param _self WRecord object instance
 * @param csize the compressed size of the record
 *
 * @return false if succeeds, true otherwise
 *
 * Warc Record compressed size updating function
 */

WIPUBLIC warc_bool_t WRecord_setCompressedSize (void * _self, warc_u64_t csize)
{

  struct WRecord * self = _self;

  /* Preconditions */
  CASSERT (self);

  unless (csize)
  return (WARC_TRUE);

  CSIZE = csize;

  return (WARC_FALSE);
}


/**
 * @param _self WRecord object instance
 * @param[out]: the uncompressed size of the record
 * Returns sthe size of the recird when it is uncompressed
 */

WIPUBLIC warc_u64_t WRecord_getUncompressedSize (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* Preconditions */
  CASSERT (self);

  unless (USIZE)
  return (0);

  return (USIZE);
}


/**
 * @param _self WRecord object instance
 * @param usize The uncompreesed size of the record
 *
 * @return false if succeeds, True otherwise
 *
 * Warc Record uncompressed size updating function
 */

WIPUBLIC warc_bool_t WRecord_setUncompressedSize (void * _self, warc_u64_t usize)
{

  struct WRecord * self = _self;

  /* Precondition */
  CASSERT (self);

  unless (usize)
  return (WARC_TRUE);

  USIZE = usize;

  return (WARC_FALSE);
}


/**
 * @param _self WRecord object instance
 *
 * @return The offset off the record in its Warc File
 *
 * Returns the WRecord offset in the Warc File
 */

WIPUBLIC warc_u64_t WRecord_getOffset (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* Precondition */
  CASSERT (self);

  return (IOFFSET);
}


/**
 * @param _self WRecord object instance
 * @param inwoffset The offset of the Wrecord in its Warc File
 *
 * @return False if succeeds, True otherwise
 *
 * Warc Record in Warc File offset updating function
 */

WIPUBLIC warc_bool_t WRecord_setWFileOffset (void * _self, warc_u64_t inwoffset)
{

  struct WRecord * self = _self;

  /* Preconditions */
  CASSERT (self);

  /* if (inwoffset < 0) */
/*     return (WARC_TRUE); */

  IOFFSET = inwoffset;

  return (WARC_FALSE);
}


/**
 * @param _self WRecord object
 *
 * @return constant WARC id string
 *
 * Returns the WARC id of the WARC-record
 */

WIPUBLIC const warc_u8_t * WRecord_getWarcId (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WHeader_getWarcId (HDL));
}


/**
 * @param _self WRecord object
 *
 * @return data length as a 32 bits unsigned integer
 *
 * Returns the data length of the WARC-record
 */

WIPUBLIC warc_u64_t WRecord_getDataLength (const void * const _self)
{

  const struct WRecord * const self = _self; 

  /* preconditions */


  CASSERT (self);

  return (WHeader_getDataLength (HDL) );

}



/**
 * @param _self WRecord object
 *
 * @return WARC-record type as an integer (see "wheader.h")
 *
 * Returns the WARC-record type
 */

WIPUBLIC warc_rec_t WRecord_getRecordType (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WHeader_getRecordType (HDL) );

}


/**
 * @param _self WRecord object
 *
 * @return WARC-record target URI string
 *
 * Returns a constant string which represent the
 * target URI of the WARC-record
 */


WIPUBLIC const warc_u8_t * WRecord_getTargetUri (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WHeader_getSubjectUri (HDL) );
}


/**
 * @param _self WRecord object
 *
 * @return creation date constant string
 *
 * Returns the WARC-record creation date
 */

WIPUBLIC const warc_u8_t * WRecord_getDate (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WHeader_getCreationDate (HDL) );
}


/**
 * @param _self WRecord object
 *
 * @return content type constant string
 *
 * Returns the WARC-record content type
 */

WIPUBLIC const warc_u8_t * WRecord_getContentType (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WHeader_getContentType (HDL) );
}


/**
 * @param _self WRecord object
 *
 * @return record id constant string
 *
 * Returns the WARC-record id.
 */

WIPUBLIC const warc_u8_t * WRecord_getRecordId (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WHeader_getRecordId (HDL) );
}

/**
 * @param _self WRecord object
 *
 * @return content length as a 32 bits unsigned integer
 *
 * Returns the content length of the WARC-record
 */

WIPUBLIC  warc_u64_t  WRecord_getContentLength (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WHeader_getContentLength (HDL) );
}

/**
 * @param _self WRecord object
 *
 * @return concurrent record id constant string
 *
 * Returns the Record id which this one is concurrent to.
 */

WIPUBLIC const warc_u8_t * WRecord_getConcurrentTo  (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WHeader_getConcurrentTo (HDL) );
}

/**
 * @param _self WRecord object
 *
 * @return The string describing the block digest algorithm and value
 *
 * Returns the Block digest string value
 */

WIPUBLIC const warc_u8_t * WRecord_getBlockDigest (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WHeader_getBlockDigest (HDL) );
}

/**
 * @param _self WRecord object
 *
 * @return The PayLoad Digest algorithm and value as a string
 *
 * Returns the WArc Record PayLoad Digest string
 */

WIPUBLIC const warc_u8_t * WRecord_getPayloadDigest (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WHeader_getPayloadDigest (HDL) );
}

/**
 * @param _self WRecord object
 *
 * @return The material Source ip Address string
 *
 * Returns the WARC Record IP Address
 */

WIPUBLIC const warc_u8_t * WRecord_getIpAddress (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WHeader_getIpAddress (HDL) );
}

/**
 * @param _self WRecord object
 *
 * @return The referenced record id string
 *
 * Returns the Referenced WARC Record Id
 */

WIPUBLIC const warc_u8_t * WRecord_getRefersTo (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WHeader_getRefers (HDL) );
}

/**
 * @param _self WRecord object
 *
 * @return Truncation cause describing string
 *
 * Returns the WARC Record Truncated String
 */

WIPUBLIC const warc_u8_t * WRecord_getTruncated (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WHeader_getTruncated (HDL) );
}

/**
 * @param _self WRecord object
 *
 * return Related Warc Info Id string
 *
 * Returns the WARC info Record id related to this one
 */

WIPUBLIC const warc_u8_t * WRecord_getWarcInfoId (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WHeader_getInfoId (HDL) );
}

/**
 * @param _self WRecord object
 *
 * @return The Profile field string
 *
 * Returns the WARC Record Profile
 */

WIPUBLIC const warc_u8_t * WRecord_getProfile (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WHeader_getProfile (HDL) );
}

/**
 * @param _self WRecord object
 *
 * @return The Payload type string
 *
 * Returns the WARC Record Payload Type field
 */

WIPUBLIC const warc_u8_t * WRecord_getPayloadType (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WHeader_getPayloadType (HDL) );
}




/**
 * @param _self WRecord object
 *
 * @return The SegmentOriginId field string
 *
 * Returns the WARC Record Id origin of this segment Record
 */

WIPUBLIC const warc_u8_t * WRecord_getSegmentOriginId (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WHeader_getSegmentOriginId (HDL) );
}

/**
 * @param _self WRecord object
 *
 * @return The segment number field as a 32 bits unsigned integer
 *
 * Returns the Row of a segment Warc Record
 */

WIPUBLIC  warc_u32_t  WRecord_getSegmentNumber (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WHeader_getSegmentNumber (HDL) );
}

/**
 * @param _self WRecord object
 *
 * @return The total segmented record size as a 32 bits unsigned integer
 *
 * Returns the total size of a segmented WARC-record (only in the last segment)
 */

WIPUBLIC  warc_u32_t WRecord_getSegTotalLength (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WHeader_getSegTotalLength (HDL) );
}

/**
 * @param _self WRecord object
 *
 * @return Related Warc File name string
 *
 * Returns the WARC File name related to this record
 */

WIPUBLIC const warc_u8_t * WRecord_getFilename (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WHeader_getFilename (HDL) );
}




/**
 * @param _self WRecord object
 * @param t WARC-record type integer (see "wheader.h")
 *
 * @return boolean ,false if is failed true if success
 *
 * Sets the WARC-record type
 */


WIPUBLIC warc_bool_t WRecord_setRecordType (void * _self, const warc_rec_t t)
{

  struct WRecord * self = _self;
  warc_bool_t wrt;

  /* preconditions */
  CASSERT (self);

  wrt = WHeader_setRecordType (HDL, t);

  /* mime check */

  if (wrt == WARC_FALSE)
    CHECK = CHECK | 0x0001;

  return wrt;
}


/**
 * @param _self WRecord object
 * @param suri target URI string
 * @param len WARC id string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the tagget URI of the WARC-record
 */

WIPUBLIC warc_bool_t WRecord_setTargetUri (void * _self,
                                            const warc_u8_t * suri,
                                            const warc_u32_t len)
{

  struct WRecord * self = _self;
  warc_bool_t wrt;

  /* preconditions */
  CASSERT (self);
  assert (suri);

  /* Uri check */

  wrt = WHeader_setSubjectUri (HDL, suri, len);

  if (wrt == WARC_FALSE)
    OPT_CHK = OPT_CHK | 0x0010;

  return wrt;
}



/**
 * @param date: the date string;
 * @param datele: the date string length
 *
 * @return True if succeeds, False otherwise
 * 
 * Checks if a date string is conform to the ISO norm
 */

WPRIVATE warc_bool_t WRecord_checkDate (const warc_u8_t * date, warc_u32_t datelen)
{
    warc_u32_t    i = 0;
    warc_u32_t    j = 0;

   if (datelen != 20)
      return (WARC_FALSE);

   i = datelen-1;

   if ((date [i] != 'z') && (date[i] != 'Z'))
      return (WARC_FALSE);

   i--;

   for (j = 0; j<3; j++)
      {
      if (! isdigit (date[i]) || (! isdigit (date[i-1])))
         return (WARC_FALSE);

     if ((j <2) && (date[i-2] != ':'))
          return (WARC_FALSE);

     else if ((j == 2) && ((date[i-2] != 't') && (date[i-2] != 'T')))
            return (WARC_FALSE);

      i = i-3;
      }

   for (j = 0; j<2; j++)
      {
      if ((! isdigit (date[i])) || (! isdigit(date[i-1])))
         return (WARC_FALSE);

      if (date[i-2] != '-')
         return (WARC_FALSE);

      i = i-3;
      }

   for (j =0; j<4; j++)
       {
       if (! isdigit (date [i]))
          return (WARC_FALSE);

       i--;
       }
return (WARC_TRUE);
}


/**
 * @param _self WRecord object
 * @param cd creation date string
 * @param len creation date string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the creation date of the WARC-record
 */

WIPUBLIC warc_bool_t WRecord_setDate (void *  _self,
                                              const warc_u8_t * cd,
                                              const warc_u32_t len) 
{

  struct WRecord *  self = _self;
  warc_bool_t wrt;

  /* preconditions */
  CASSERT (self);
  assert (cd);

  /* Check of the date format */
  unless (WRecord_checkDate (cd, len))
     return (WARC_TRUE);

  wrt = WHeader_setCreationDate (HDL, cd, len);

  if (wrt == WARC_FALSE)
    CHECK = CHECK | 0x0004;

  return wrt;
}


/**
 * @param date: the date in  w3c-iso8601 format
 * 
 * @return 
 *
 * Convert date extract from arc file to w3c-iso8601 format
 */


WPRIVATE void * WRecord_convertDate (const warc_u8_t * date)
{
 warc_u32_t i  = 0;

 void * item = bless (WString, "", 0);  /* the return item */
 warc_u8_t achar[2] = {' ', '\0'};     /* to append item characters */
 
 for (i = 0; i < w_strlen (date); i ++)
     {
  
      achar [0] = date [i];
      WString_append (item, achar, 1);
           
      if (( i == 3) || (i == 5))
         {
          achar [0] = '-';
          WString_append (item, achar, 1);
         }

       if (i == 7)
          {
            achar [0] = 'T' ;
            WString_append (item, achar, 1);
          }

       if ((i == 9) || (i == 11))
          {
           achar [0] = ':' ;
           WString_append (item, achar, 1);
          }

        if (i == 13)
          {
           achar [0] = 'Z' ;
           WString_append (item, achar, 1);
          }

      }
  return item;
}

/**
 * @param _self WRecord object
 * @param cd creation date string
 * @param len creation date string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the creation date of the WARC-record From an Arc Record
 */

WIPUBLIC warc_bool_t WRecord_setDateFromArc   (void *  _self,
                                              const warc_u8_t * cd,
                                              const warc_u32_t len) 
{

  struct WRecord *  self = _self;
  void * item;
  warc_bool_t wrt;

  UNUSED (len);

  /* preconditions */
  CASSERT (self);
  assert (cd);

  item = WRecord_convertDate (cd);

  wrt = WRecord_setDate (self, WString_getText (item), WString_getLength (item));
  destroy (item);

  return wrt;
}


/**
 * @param _self WRecord object
 * @param ct content type string
 * @param len content type string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the WARC-record's content type
 */

WIPUBLIC warc_bool_t WRecord_setContentType (void * _self,
    const warc_u8_t * ct,
    const warc_u32_t len)
{

  struct WRecord * self = _self;
  warc_bool_t wrt;

  /* preconditions */
  CASSERT (self);
  assert (ct);

  /* mime check */

  wrt = WHeader_setContentType (HDL, ct, len);

  if (wrt == WARC_FALSE)
    OPT_CHK = OPT_CHK | 0x0001;

  return wrt;
}

/**
 * @param _self WRecord object
 * @param rid Warc Record id constant string
 * @param len WARC Record id string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the Warc Concurrent Record id
 */

WIPUBLIC warc_bool_t WRecord_setConcurrentTo (void * _self,
    const warc_u8_t * rid,
    const warc_u32_t len)
{

  struct WRecord * self = _self;
  warc_bool_t wrt;

  /* preconditions */
  CASSERT (self);
  assert (rid);

  /* check Uri */

  wrt = WHeader_setConcurrentTo (HDL, rid, len);

  if (wrt == WARC_FALSE)
    OPT_CHK = OPT_CHK | 0x0002;

  return wrt;
}



/**
 * @param digest;l the digest to check
 *
 * @return True if succeeds, False otherwise
 *
 * Checks if a Digest Format is correct
 */

WPRIVATE warc_bool_t WRecord_checkDigest (const warc_u8_t * digest)
{
    warc_u32_t    i    = 0;
    warc_u32_t    size = w_strlen (digest);

  while ((digest[i] != ':') && (i < size))
      i++;
 
  if (i == size)
    return (WARC_FALSE);

    i++;

  while (i < size)
    {
   
    if ((digest[i] <= 31) || (digest[i] == 127))
       return (WARC_FALSE);

    switch (digest[i])
      {
      case '(':
      case '\r':
      case '\n':
      case ')':
      case '<':
      case '>':
      case '@':
      case ',':
      case ';':
      case ':':
      case '\\':
      case '"':
      case '/':
      case '[':
      case ']':
      case '?':
      case '=':
      case '{':
      case '}':
      case ' ':
      case '\t': return (WARC_FALSE);

      default  : break;
      }
    i++;
    }

  return (WARC_TRUE);
}


/**
 * @param _self WRecord object
 * @param digest Block Digest Constant string
 * @param len Block Digest string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the Block Digest string
 */

WIPUBLIC warc_bool_t WRecord_setBlockDigest (void * _self,
    const warc_u8_t * digest,
    const warc_u32_t len)
{

  struct WRecord * self = _self;
  warc_bool_t wrt;

  /* preconditions */
  CASSERT (self);
  assert (digest);

  /* Check of the digest */
  unless (WRecord_checkDigest (digest))
     return (WARC_TRUE);

  wrt = WHeader_setBlockDigest (HDL, digest, len);

  /*if (wrt == WARC_FALSE)
    CHECK = CHECK | 0x0008;*/

  return wrt;
}

/**
 * @param _self WRecord object
 * @param paydigest Payload Digest string
 * @param len Payload digest string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the WARC Header Payload Digest field
 */

WIPUBLIC warc_bool_t WRecord_setPayloadDigest (void * _self,
    const warc_u8_t * paydigest,
    const warc_u32_t len)
{

  struct WRecord * self = _self;
  warc_bool_t wrt;

  /* preconditions */
  CASSERT (self);
  assert (paydigest);

  /* check the digest */
  unless (WRecord_checkDigest (paydigest))
     return (WARC_TRUE);

  wrt = WHeader_setPayloadDigest (HDL, paydigest, len);

  /*if (wrt == WARC_FALSE)
    CHECK = CHECK | 0x0008;*/

  return wrt;
}



/**
 * @param ipadd: the IP Address string
 *
 * @return True if correct, false otherwise
 *
 * Checks if an IP Address is correct
 */

WPRIVATE warc_bool_t  WRecord_checkIpAddress (const warc_u8_t * ipadd)
{
#define PORTMAX 65535 /* or whatever appropriate */

  warc_u32_t   rc;
  warc_u32_t   b1, b2, b3, b4, port = 0;
  warc_u8_t    c;

  rc = sscanf ( (const char *) ipadd, "%3u.%3u.%3u.%3u:%u%c",
                & b1, & b2, & b3, & b4, & port, & c);

  if (rc != 4 && rc != 5)
    {
      w_fprintf (fprintf (stderr, "error> found IP address: %s\n", (char *) ipadd) );
      /* raise the flag error */
      WarcDebugMsg ("expecting a valid IP address: accept only IPv4 and IPv6 adresses");

      return   WARC_FALSE;
    }


  if ( (b1 | b2 | b3 | b4) > 255 || port > PORTMAX)
    {
      w_fprintf (fprintf (stderr, "error> found IP address: %s\n", (char *) ipadd) );
      /* raise the flag error */
      WarcDebugMsg ("expecting a valid IP address: IP range greater than 255 or port number greater than" PORTMAX);

      return WARC_FALSE;
    }

  if (strspn ( (const char *) ipadd, "0123456789.:") < w_strlen (ipadd))
    {
      w_fprintf (fprintf (stderr, "error> found IP address: %s\n", (char *) ipadd) );
      /* raise the flag error */
      WarcDebugMsg ("expecting a valid IP address: only \"0123456789.:\" are valid");
      
      return WARC_FALSE;
    }

  /* Valid IP address */
  return WARC_TRUE;
}



/**
 * @param _self WRecord object
 * @param ip IP Ardess constant string
 * @param len The ip Address string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the WARC Record IP Adress field
 */

WIPUBLIC warc_bool_t  WRecord_setIpAddress (void * _self,
    const warc_u8_t * ip,
    const warc_u32_t len)
{

  struct WRecord * self = _self;
  warc_bool_t wrt;

  /* preconditions */
  CASSERT (self);
  assert (ip);

  /* check Ip address */
  unless (WRecord_checkIpAddress (ip))
    return (WARC_TRUE);

  wrt = WHeader_setIpAddress (HDL, ip, len);

  if (wrt == WARC_FALSE)
    OPT_CHK = OPT_CHK | 0x0004;

  return wrt;
}

/**
 * @param _self WRecord object
 * @param ref referenced Warc Record id constant string
 * @param len referenced WARC Record id string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the WARC Record Refers_To Record id string
 */

WIPUBLIC warc_bool_t  WRecord_setRefersTo (void * _self,
    const warc_u8_t * ref,
    const warc_u32_t len)
{

  struct WRecord * self = _self;
  warc_bool_t wrt;

  /* preconditions */
  CASSERT (self);
  assert (ref);

  /* Uri check */

  wrt = WHeader_setRefersTo (HDL, ref, len);

  if (wrt == WARC_FALSE)
    OPT_CHK = OPT_CHK | 0x0008;

  return wrt;
}


/**
 * @param trunc: the trucnation to check
 *
 * @return True if succeed, False otherwise
 *
 * Checks the possible trucations causes
 */

WPRIVATE  warc_bool_t  WRecord_checkTruncated (const warc_u8_t * trunc)
{
if (w_strcmp (trunc, makeU ("length")))
   if (w_strcmp (trunc, makeU ("time")))
     if (w_strcmp (trunc, makeU ("disconnect")))
        if (w_strcmp (trunc, makeU ("unspecifed")))
            return WARC_FALSE;
 

return WARC_TRUE;
}


/**
 * @param _self WRecord object
 * @param trunc record id constant string
 * @param len WARC Record id string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the WARC Record Truncation Field
 */

WIPUBLIC warc_bool_t  WRecord_setTruncated (void * _self,
    const warc_u8_t * trunc,
    const warc_u32_t len)
{

  struct WRecord * self = _self;
  warc_bool_t wrt;

  /* preconditions */
  CASSERT (self);
  assert (trunc);

  /* truncation check */
  unless (WRecord_checkTruncated (trunc))
    return (WARC_TRUE);

  wrt = WHeader_setTruncated (HDL, trunc, len);

  /*if (wrt == WARC_FALSE)
    CHECK = CHECK | 0x0008;*/

  return wrt;
}

/**
 * @param _self WRecord object
 * @param idinfo Warcinfo Record id constant string
 * @param len Warcinfo Record id string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the WARC Record Related Warcinfo Record Id
 */

WIPUBLIC warc_bool_t   WRecord_setWarcInfoId (void * _self,
    const warc_u8_t * idinfo,
    const warc_u32_t len)
{

  struct WRecord * self = _self;
  warc_bool_t wrt;

  /* preconditions */
  CASSERT (self);
  assert (idinfo);

  /* check Uri */

  wrt =  WHeader_setInfoId (HDL, idinfo, len);

  if (wrt == WARC_FALSE)
    OPT_CHK = OPT_CHK | 0x0400;

  return wrt;
}

/**
 * @param _self WRecord object
 * @param prof Record Profile string
 * @param len WARC Profile string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the WARC Record Profile field
 */

WIPUBLIC warc_bool_t   WRecord_setProfile (void * _self,
    const warc_u8_t * prof,
    const warc_u32_t len)
{

  struct WRecord * self = _self;
  warc_bool_t wrt;

  /* preconditions */
  CASSERT (self);
  assert (prof);

  /* check Uri */

  wrt =  WHeader_setProfile (HDL, prof, len);

  if (wrt == WARC_FALSE)
    OPT_CHK = OPT_CHK | 0x0040;

  return wrt;
}

/**
 * @param _self WRecord object
 * @param paytype Payload type string
 * @param len Payload type string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the WARC Record Payload type field
 */

WIPUBLIC warc_bool_t   WRecord_setPayloadType (void * _self,
    const warc_u8_t * paytype,
    const warc_u32_t len)
{

  struct WRecord * self = _self;
  warc_bool_t wrt;

  /* preconditions */
  CASSERT (self);
  assert (paytype);

  /* mime check */

  wrt =  WHeader_setPayloadType (HDL, paytype, len);

  /*if (wrt == WARC_FALSE)
    CHECK = CHECK | 0x0008;*/

  return wrt;
}

/**
 * @param _self WRecord object
 * @param segid Origin Warc Record id constant string
 * @param len Origin Warc Record id string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the sgement WARC-Record Origin record id
 */

WIPUBLIC warc_bool_t   WRecord_setSegmentOriginId (void * _self,
    const warc_u8_t * segid,
    const warc_u32_t len)
{

  struct WRecord * self = _self;
  warc_bool_t wrt;

  /* preconditions */
  CASSERT (self);
  assert (segid);

  /* uri check */

  wrt =  WHeader_setSegmentOriginId (HDL, segid, len);

  if (wrt == WARC_FALSE)
    OPT_CHK = OPT_CHK | 0x0100;

  return wrt;
}

/**
 * @param _self WRecord object
 * @param snumber Segment number as a 32 bits unsigned integer
 * 
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the Segment number of a segment Warc Record
 */

WIPUBLIC warc_bool_t    WRecord_setSegmentNumber (void * _self,
    const warc_u32_t snumber)
{

  struct WRecord * self = _self;
  warc_bool_t wrt;

  /* preconditions */
  CASSERT (self);
  assert (snumber);

  wrt =   WHeader_setSegmentNumber (HDL, snumber);

  if (wrt == WARC_FALSE)
    OPT_CHK = OPT_CHK | 0x0080;

  return wrt;
}

/**
 * @param _self WRecord object
 * @param stlength The total size as 32 bits unsigfned integer
 * 
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the Total size of segmented WARC-record (only in the last segment)
 */

WIPUBLIC warc_bool_t     WRecord_setSegTotalLength (void * _self,
    const warc_u32_t stlength)
{

  struct WRecord * self = _self;
  warc_bool_t wrt;

  /* preconditions */
  CASSERT (self);
  assert (stlength);

  wrt =    WHeader_setSegTotalLength (HDL, stlength);

  if (wrt == WARC_FALSE)
    OPT_CHK = OPT_CHK | 0x0200;

  return wrt;
}


/**
 * @param fname: the filename to check
 * 
 * @return True if succeeds, False otherwise
 *
 * Checks if a file name is correct
 */

WPRIVATE warc_bool_t WRecord_checkFilename (const warc_u8_t * fname)
{
    warc_u32_t   i    = 0;
    warc_u32_t   size = w_strlen (fname);

    while (i < size)
      {
      if ((fname[i] <= 31) || (fname[i] == 127))
         return (WARC_FALSE);
      i++;
      }

  return (WARC_TRUE);
}



/**
 * @param _self WRecord object
 * @param fname Warc File name constant string
 * @param len Warc File name string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the WARC Record Related Warc File name
 */

WPUBLIC warc_bool_t WRecord_setFilename (void * _self,
    const warc_u8_t * fname,
    const warc_u32_t len)
{

  struct WRecord * self = _self;
  warc_bool_t wrt;

  /* preconditions */
  CASSERT (self);
  assert (fname);

  /* check the file name */
  unless (WRecord_checkFilename (fname))
     return (WARC_TRUE);

  wrt = WHeader_setFilename (HDL, fname, len);

  if (wrt == WARC_FALSE)
    OPT_CHK = OPT_CHK | 0x0020;

  return (wrt);
}



/**
 * @param _self WRecord object
 * @param rid record id constant string
 * @param len record id string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the WARC-record id
 */

WPUBLIC warc_bool_t WRecord_setRecordId (void * _self,
    const warc_u8_t * rid,
    const warc_u32_t len)
{

  struct WRecord * self = _self;
  warc_bool_t wrt;

  /* preconditions */
  CASSERT (self);
  assert (rid);

  /*check URI */

  wrt = WHeader_setRecordId (HDL, rid, len);

  if (wrt == WARC_FALSE)
    CHECK = CHECK | 0x0002;

  return (wrt);
}


/**
 * @param _self: WRecord object instance
 *
 * @return WList object if succeeds, NIL otherwise
 *
 * Return a list of Non predefined named fileds 
 */

WIPUBLIC const void * WRecord_getAnvl (const void * const _self)
{
    const struct WRecord * const self = _self;

  /* Preconditions */
  CASSERT (self);

return (WHeader_getAnvl (HDL));
} 


/**
 * @param _self WRecord object instance
 * @param key the key of an anvl field as a const char *
 *
 * @return A valid anvl field value correspnding to the key, NIL otherwise
 *
 * Warc Record anvl field value corrsponding to a key
 */

WIPUBLIC const warc_u8_t * WRecord_getAnvlValue (const void* const _self,
                                                const warc_u8_t * key)
{

  const struct WRecord * const self = _self;

  /* Preconditions */
  CASSERT (self);

  return (WHeader_getAnvlValue (HDL, key));
}


/**
 * @param _self WRecord object instance
 * @param key the key of the anvl Field as a const char *
 * @param klen the key string length in bytes
 * @param value the value of the anvl field as a const char *
 * @param vlen the value string length in bytes
 *
 * @return False if succeeds, True otherwise
 *
 * WRecord new anvl field adding function
 */

WIPUBLIC warc_bool_t WRecord_addAnvl (void * _self,
                                      const warc_u8_t * key,
                                      const warc_u32_t klen,
                                      const warc_u8_t * value,
                                      const warc_u32_t vlen)
{

  struct WRecord * self = _self;

  /* Precondition */
  CASSERT (self);

  return (WHeader_addAnvl (HDL, key, klen, value, vlen));

}


/**
 * @param _self a WRecord object
 *
 * @return the WRecord header  object if succeeds, NIL otherwise
 *
 * Returns the Warc Record Header object
 */

WIPUBLIC const void * WRecord_getHeader (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (HDL);
}


/**
 * @param _self A WRecord object
 * @param h a WHeader object reference
 *
 * @return The old stored WHDeader object reference if succeeds, NIL otherwise
 *
 * Set a new header for the Warc Record and returns the old one for uage or destruction
 */

WIPUBLIC void * WRecord_setHeader (void * const _self, void * h)
{

  struct WRecord * const self = _self;
  void                 *       old;

  /* preconditions */
  CASSERT (self);

  if (h)
    {
      old = HDL;
      HDL = h;
      return (old);
    }

  return (NIL);
}



/**
 * @param _self WRecord object instance
 * @param file data file name as const char *
 *
 * @return False if succeeds, True otherwise
 *
 * WRecord data file setting function
 */

WIPUBLIC warc_bool_t WRecord_setContentFromFileName (void * _self,
                                                     const char * file)
{

  struct WRecord * self  = _self;
  FILE           * dfile = NIL;

  /* Preconditions */
  CASSERT (self);
  assert (! EDATAF);
  assert (! DATAF);
  assert (! CONTENT);

  unless (file)
    return (WARC_TRUE);

  dfile = w_fopen_rb (file);
  unless (dfile)
    return (WARC_TRUE);

  EDATAF = dfile;

  w_file_size (dfile, SIZE);

  if (WHeader_setContentLength (HDL, SIZE))
    {
      w_fclose (dfile);
      return (WARC_TRUE);
    }

  CHECK = CHECK | 0x0008;
  
  return (WARC_FALSE);
}


/**
 * @param _self Wrecord object instance
 * @param dataf Warc Data File *
 *
 * @return False if succeeds, True otherwise
 *
 * Warc Record Data File descriptor updating
 */

WIPUBLIC warc_bool_t WRecord_setContentFromFileHandle (void * _self,
                                                       void * dataf)
{

  struct WRecord * self  = _self;
  
  /* Preconditions */
  CASSERT (self);
  assert (! EDATAF); /* EDATAF must be NIL (WRecord manually created) */
  assert (! DATAF);
  assert (! CONTENT);

  unless (dataf)
    return (WARC_TRUE);
  
  DATAF = dataf;
  
  return (WARC_FALSE);
}

/**
 * @param _self: WRecord object insance
 * @param dir; the directory where the temporary file must be created
 * @param dirlen: the length of the dir string
 *
 * @return False if succeeds, True otherwise
 *
 * Set record content from string
 */

WPUBLIC warc_bool_t  WRecord_setContentFromString (void * _self,
                                                  const warc_u8_t * str,
                                                  const warc_u32_t strlen)
{
  struct WRecord * self  = _self;

  /* Preconditions */
  CASSERT (self);
  assert (! EDATAF);
  assert (! DATAF);
  assert (! CONTENT);

  CONTENT = bless (WTempFile, makeS("."));
  unless (CONTENT)
    return (WARC_TRUE);

  EDATAF = WTempFile_handle (CONTENT);
  if(w_fwrite (str, 1, strlen, EDATAF) != strlen)
    {
      EDATAF = NIL;
      destroy (CONTENT), CONTENT = NIL;
      return (WARC_TRUE);
    }

  SIZE = strlen;

  if (WHeader_setContentLength (HDL, SIZE))
    {
      EDATAF = NIL;
      destroy (CONTENT), CONTENT = NIL;
      return (WARC_TRUE);
    }

  CHECK = CHECK | 0x0008;

  /* not needed because "WRecord_getDataFileExtern" already check that */
  /* w_fseek_start (EDATAF); */

  return (WARC_FALSE);
}


/**
 * @param _self: WRecord object insance
 * @param dir; the directory where the temporary file must be created
 * @param dirlen: the length of the dir string
 *
 * @return False if succeeds, True otherwise
 *
 * Append string to a content previously written by "WRecord_setContentFromString"
 */

WPUBLIC warc_bool_t  WRecord_setContentFromStringConcat (void * _self,
                                                         const warc_u8_t * str,
                                                         const warc_u32_t strlen)
{
  struct WRecord * self  = _self;

  /* Preconditions */
  CASSERT (self);
  assert (! DATAF);
  assert (CONTENT);
  assert (EDATAF);

  /* be sure to got to the end */
  w_fseek_end (EDATAF);

  if(w_fwrite (str, 1, strlen, EDATAF) != strlen)
    {
      EDATAF = NIL;
      destroy (CONTENT), CONTENT = NIL;
      return (WARC_TRUE);
    }

  SIZE += strlen;

  if (WHeader_setContentLength (HDL, SIZE))
    {
      EDATAF = NIL;
      destroy (CONTENT), CONTENT = NIL;
      return (WARC_TRUE);
    }

  return (WARC_FALSE);
}


/**
 * @param _self: WRecord object insance
 * @param dir; the directory where the temporary file must be created
 * @param dirlen: the length of the dir string 
 *
 * @return False if succeeds, True otherwise
 *
 * Create a tremporary file for the record to hold data bloc
 */

WPUBLIC warc_bool_t WRecord_makeDataFile (void * _self, const warc_u8_t * dir, 
                                          const warc_u32_t dirlen)
{
  struct WRecord * self = _self;

  /* Preconditions */
  CASSERT (self);

  DATAF = bless (WTempFile, dir, dirlen);
  unless (DATAF)
    return (WARC_TRUE);
    
  return (WARC_FALSE);
}


/**
 * @param _self Wrecord object instance
 *
 * @return the WRecord data bloc file descriptor
 *
 * Warc Record intern Data File descriptor provider (from file handle)
 */

WPUBLIC void * WRecord_getDataFile (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* Preconditions */
  CASSERT (self);
  assert (DATAF);

  return (DATAF);
}


/**
 * @param _self Wrecord object instance
 *
 * @return the WRecord data bloc file descriptor
 *
 * Warc  Record Extern Data File descriptor provider
 */

WPUBLIC FILE * WRecord_getDataFileExtern (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* Preconditions */
  CASSERT (self);

  assert (EDATAF);

  w_fseek_start (EDATAF);
  
  return (EDATAF);
}


/**
 * @param _self WRecord object instance
 *
 * @return the Warc Data Bloc File size
 *
 * Warc Record Data Bloc file Size returning function
 */

WIPUBLIC warc_u64_t WRecord_getDataSize (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* Preconditions */
  CASSERT (self);

  return (SIZE);
}


/**
 * @param _self WRecord object instance
 * @param sz the new DataBloc Size
 *
 * @return boolean false if success true otherwise
 *
 * Warc Record Data Bloc file set size
 */

WPUBLIC warc_bool_t WRecord_setContentSize (void * _self, warc_u64_t sz)
{

  struct WRecord *  self = _self;

  /* Preconditions */
  CASSERT (self);

  unless (sz)
  return (WARC_TRUE);

  SIZE = sz;

  if (WHeader_setContentLength (HDL, SIZE))
     return (WARC_TRUE);

  CHECK = CHECK | 0x0008;

  return (WARC_FALSE);
}



/**
 * @param _self WRecord object instance
 *
 * @return True if still there is data in the record data file,
 * false otherwise Warc Record data file access function
 *
 * Warc Record content access function
 */

WIPUBLIC warc_bool_t WRecord_getContent (const void* const _self)
{
#define CONTENT_BUFSIZ 4096

  const struct WRecord * const self = _self;
  char          buff [CONTENT_BUFSIZ];
  warc_u64_t    where  = 0;
  warc_u32_t    size   = 0;
  FILE        * handle = NIL;


  /* preconditions */
  CASSERT (self);
  assert  (DATAF);
  assert  (! EDATAF);
  assert  (! CONTENT);

  handle = WTempFile_handle (DATAF);

  /* save where we're */
  w_ftell (handle, where);

  while (WARC_TRUE)
    {
      size = w_fread (buff, 1, CONTENT_BUFSIZ, handle);

      if (size)
        {
          unless (CBACK (ENV, buff, size) )
          break;
        }

      if (w_feof (handle) )
        break;
    }

  /* return the we're we were */
  w_fseek_from_start (handle, where);

  return (WARC_FALSE);
}


/**
 * @param _self WRecord object instance
 *
 * @return the offset of the data bloc in a Warc File
 *
 * WRecord data bloc offset provider
 */

WIPUBLIC warc_i64_t WRecord_getWoffset (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* Preconditions */
  CASSERT (self);

  if (OFFSET >= 0)
    return (OFFSET);

  return (-1);
}


/**
 * @param _self WRecord object instance
 * @param offset the new offset in a Warc file
 *
 * @return False if succeeds, True otherwise
 *
 * WRecord offset in Warc File updating function
 */

WIPUBLIC warc_bool_t WRecord_setWoffset (void * _self, const warc_i64_t offset)
{

  struct WRecord * self = _self;

  /* Preconditions */
  CASSERT (self);

  if (offset >= 0)
    {
      OFFSET = offset;
      return (WARC_FALSE);
    }

  return (WARC_TRUE);
}


/**
 * @param _self WRecord object instance
 * @param cback the callback function
 *
 * @return False if succeeds, True otherwise
 *
 * WRecord callback function setting
 */

WIPUBLIC warc_bool_t WRecord_setCallback (void * _self,
    warc_bool_t (* cback)
    (void* env, const char * buff,
     warc_u32_t size) )
{

  struct WRecord * self = _self;

  /* Precondition */
  CASSERT (self);

  unless (cback)
  return (WARC_TRUE);

  CBACK = cback;

  return (WARC_FALSE);
}


/**
 * @param _self WRecord instance
 * @param env The user environnement
 *
 * @return False if succeeds, True otherwise
 *
 * WRecord user environnement setting function
 */

WIPUBLIC warc_bool_t WRecord_setEnv (void * _self, void * env)
{

  struct WRecord * self = _self;

  /* Preconditions */
  CASSERT (self);

  ENV = env;

  return (WARC_FALSE);
}




/**
 * @param _self WRecord object instance
 *
 * @return True if all filled, False otherwise
 *
 * WRecord - fields checking function
 */

WPUBLIC warc_bool_t WRecord_check (const void * const _self)
{

  const struct WRecord * const self = _self;
  CASSERT (self);

  if (MASK1 != CHECK)
    return (WARC_FALSE);

  switch (WHeader_getRecordType (HDL))
    {
    case WARC_INFO_RECORD:
         if ((OPT_CHK & AND_MSK1) != AND_MSK1)
           return (WARC_FALSE);
         if ((OPT_CHK | OR_MSK1) != OR_MSK1)
           return (WARC_FALSE);

         break;

    case WARC_RESPONSE_RECORD:
    case WARC_RESOURCE_RECORD:
    case WARC_REQUEST_RECORD:
         if ((OPT_CHK & AND_MSK2) != AND_MSK2)
           return (WARC_FALSE);
         if ((OPT_CHK | OR_MSK2) != OR_MSK2)
           return (WARC_FALSE);

         break;

    case WARC_REVISIT_RECORD:
         if ((OPT_CHK & AND_MSK3) != AND_MSK3)
           return (WARC_FALSE);
         if ((OPT_CHK | OR_MSK3) != OR_MSK3)
           return (WARC_FALSE);

         break;

    case WARC_METADATA_RECORD:
         if ((OPT_CHK & AND_MSK4) != AND_MSK4)
           return (WARC_FALSE);
         if ((OPT_CHK | OR_MSK4) != OR_MSK4)
           return (WARC_FALSE);

         break;

    case WARC_CONVERSION_RECORD:
         if ((OPT_CHK & AND_MSK5) != AND_MSK5)
           return (WARC_FALSE);
         if ((OPT_CHK | OR_MSK5) != OR_MSK5)
           return (WARC_FALSE);

         break;

    case WARC_CONTINUATION_RECORD:
         if ((OPT_CHK & AND_MSK6) != AND_MSK6)
           return (WARC_FALSE);
         if ((OPT_CHK | OR_MSK6) != OR_MSK6)
           return (WARC_FALSE);

         break;

    case WARC_UNKNOWN_RECORD:
    default: break;
    }

  return (WARC_TRUE);
}


/**
 * @param _self WRecord object instance
 *
 * @return The pointer to the WFile object containing the Record, NIL otherwise
 *
 * WRecord matching WFile object pointer provider
 */

WIPUBLIC void * WRecord_fromWho (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* Preconditions */
  CASSERT (self);

  return (WHO);
}


/**
 * @param _self WRecord object instance
 * @param wfile the pointer to the WFile object that cill contain the record
 *
 * @return False if succeeds, True otherwise
 *
 * Sets the WFile object containing the WRecord
 */

WIPUBLIC warc_bool_t WRecord_setWfile (void * _self, void * wfile)
{

  struct WRecord * self = _self;

  /* Preconditions */
  CASSERT (self);

  unless (wfile)
  return (WARC_TRUE);

  WHO = wfile;
  return (WARC_FALSE);
}


/**
 * @param _self WRecord object instance
 * @param present warc_bool_t to indicate the presence of the warc compression
 * header in compressed record
 *
 * @return none
 *
 * Warc Record warc compression header field updating function
 */

WIPUBLIC void WRecord_setCHeaderPresence (void * _self, const warc_bool_t present)
{

  struct WRecord * self = _self;

  /* Precondition */
  CASSERT (self);

  CHDP = present;

}

/**
 * @param _self WRecord object instance
 *
 * @return True if the warc comlpression header was present, False otherwise
 *
 * compressed Warc Record warc compression header presence
 */

WIPUBLIC warc_bool_t WRecord_getCHeaderPresent (const void * const _self)
{

  const struct WRecord * const self = _self;

  /* Precondition */
  CASSERT (self);

  return (CHDP);

}


/**
 * @param _self WRecord object instanse
 * @param arcontent A data bloc file recovered from an Arc Record
 *
 * @return False if succeeds, True otherwise
 *
 * Sets the Warc Record context from an Arc record context
 * (Used only by Arc Records)
 */

WIPUBLIC warc_bool_t WRecord_setContentFromArc (void * _self, void * arcontent)
{

  struct WRecord * self = _self;

  /* Preconditions */
  CASSERT (self);
  assert (! EDATAF);
  assert (! DATAF);
  assert (! CONTENT);

  unless (arcontent)
    return (WARC_TRUE);

  ACONT  = arcontent;
  EDATAF = WTempFile_handle (ACONT);

  return (WARC_FALSE);

}

/**
 * @param _self: a WRecord object instance
 * @param rank: the rank of the wanted anvl
 * @param key[out]: the key of the wanted anvl
 * @param value: the value of the wanted anvl
 * 
 * @return False if succeed, True otherwise
 *
 * Returns the anvl field corresponding to the rank in the WARC Record
 */

WPUBLIC warc_bool_t WRecord_getAnvlField (const void * const _self, const warc_u32_t rank,
                                          const warc_u8_t ** key, const warc_u8_t ** value)
{
    const struct WRecord * const self  = _self;
    const void           * inner_anvl  = NIL; /* for the manipulation of the inner anvl fields lit */
    const void           * wanted_anvl = NIL; /* for the manipulation of the wanted anvl field */
    warc_u32_t                   nbel  = 0;

  /* Preconditions */
  CASSERT (self);
  

  inner_anvl = WHeader_getAnvl (HDL);
  unless (inner_anvl)
     return (WARC_TRUE);

  nbel = WList_size (inner_anvl);
  if (nbel == 0 || nbel <= rank)
     return (WARC_TRUE);

  wanted_anvl = WList_getElement (inner_anvl, rank);
  unless (wanted_anvl)
     return (WARC_TRUE);

  * key   = (warc_u8_t *) WAnvl_getKey (wanted_anvl);
  * value = (warc_u8_t *) WAnvl_getValue (wanted_anvl);

  unless (* key)
    return (WARC_TRUE);

  return (WARC_FALSE);
}


/**
 * @param _self: A WRecord object instance
 *
 * @return the number of anvl fields in the WRecord header as a warc_u32_t
 *
 * Returns the number of the Anvl fields of the WARC Record contained int its header
 */

WPUBLIC  warc_u32_t WRecord_getAnvlFieldsNumber (const void * const _self)
{
    const struct WRecord * const self = _self;
    const void           * inner_anvl = NIL; /* For the manipulation of the Anvl fields list */

  /*Preconditions*/
  CASSERT (self);

  inner_anvl = WHeader_getAnvl (HDL);
  unless (inner_anvl)
     return (0);

  return (WList_size (inner_anvl));
}



/**
 * @param  _self: A WRecord object instance
 * @param  withhttp: a boolean indicating is the Http response header is encapsulated before the stored data
 * @param w: the WFile object representing the warc file where the data will be read
 * @param[out]: http_code: a string that will contain the http response code
 *
 * @return a pointer to the temporary file containing the payload of the data bloc if success, NIL otherwise
 *
 * WARC Record data bloc extraction function
 */

WPUBLIC void * WRecord_getBloc (void *  _self, void * w, 
                                warc_bool_t withhttp, warc_u8_t * http_code)
{
  struct WRecord *  self = _self;

  void           * line       = NIL;
  void           * tfile      = NIL;
  void           * whttp_code = NIL;

  warc_u32_t       fsm   = 0;
  warc_u32_t       act   = 0;
  warc_u8_t      * field = NIL;

  struct user_struct user_data;

  FILE           * file = NIL;

  /* Preconditions */
  CASSERT (self);


  field = uS(WRecord_getTargetUri (self));

  unless (field)
      field = uS("unknown");
  else unless (w_strcmp (field, uS("")))
      field = uS("unknown");

  whttp_code = bless (WString, makeS(""));

  unless (whttp_code)
      return (NIL);

  unless (strncmp ("filedesc://", (const char *) field, 11))
     WString_setText (whttp_code, makeS("998"));
  else unless (strncmp ("dns:", (const char *) field, 4))
     WString_setText (whttp_code, makeS("999"));


  line = bless (WString, makeS (""));
  fsm = S1;

  if (w_strcmp(WString_getText(whttp_code), uS("")))
       act = WRITEING;
  else
     {
      act = SEARCHING_HTTP;
      user_data . http_code = whttp_code;
     }

  unless (withhttp)
     act = WRITEING;

  tfile = bless (WTempFile, makeS("."));

  unless (tfile)
     {
      destroy (whttp_code);
      destroy (line);
      return (NIL);
     }

  file = WTempFile_handle (tfile);

  unless (file)
    {
      destroy (tfile);
      destroy (whttp_code);
      destroy (line);
      return (NIL);
    }
 
  user_data . fsm    = & fsm;
  user_data . action = & act;
  user_data . line   = line;
  user_data . was_cr = WARC_FALSE;
  user_data . file   = file;

  WFile_register (w, self, b_callback, (void *) & user_data);

  WRecord_getContent (self);

  unless (line)
     {
      fprintf (stderr, "A problem appeared while reading data from REcord\n");
      destroy (whttp_code);
      destroy (tfile);
      return (NIL);
     }

  w_strncpy (http_code, WString_getText (whttp_code), 
             WString_getLength (whttp_code));

  destroy (whttp_code);
  destroy (line);

  return (tfile);
}



/**
 * WRecord_constructor - create a new WRecord object instance
 *
 * @param _self WRecord class object
 * @param app: constructor list parameters
 *
 * @return a valid WRecord object or NIL
 *
 * @brief WARC Record constructor
 */

WPRIVATE void * WRecord_constructor (void * _self, va_list * app)
{

  struct WRecord * const self = _self;
  UNUSED (app);

  /* create a WHeader object */
  HDL = bless (WHeader, WARC_UNKNOWN_RECORD, "", 0, "", 0);
  assert (HDL);

  DATAF   = NIL;
  EDATAF  = NIL;
  CONTENT = NIL;
  ACONT   = NIL;
  CHECK   = 0;
  OPT_CHK = 0;
  CBACK   = NIL;
  SIZE    = 0;
  OFFSET  = 0;
  WHO     = NIL;
  CHDP    = WARC_FALSE;

  return (self);
}


/**
 * @param _self WRecord object instance
 *
 * WRecord destructor
 */
WPRIVATE void * WRecord_destructor (void * _self)
{

  struct WRecord  * self = _self;

  /* preconditions */
  CASSERT (self);

  /* free the WHeader object */
  if (HDL)
    destroy (HDL), HDL = NIL;

  if (DATAF) 
     destroy (DATAF), DATAF = NIL;

  if (ACONT)
    destroy (ACONT), ACONT = NIL, EDATAF = NIL;
  
  if (CONTENT)
    destroy (CONTENT), CONTENT = NIL, EDATAF = NIL;

  /* EDATAF destruction must be after CONTENT and DATAF destruction,
     because both are freeying EDATAF behind the scene
   */
  if (EDATAF)
   {
     w_fclose (EDATAF); EDATAF = NIL;
    }
 
  if (CHECK)
    CHECK = 0;
  
  if (OPT_CHK)
    OPT_CHK = 0;

  if (CBACK)
    CBACK = NIL;

  if (SIZE)
    SIZE = 0;

  if (OFFSET)
    OFFSET = 0;

  if (WHO)
    WHO = NIL;

  if (CHDP)
    CHDP = WARC_FALSE;


  return (self);
}


/**
 * WARC WRecord class
 */

static const struct Class _WRecord =
  {
    sizeof (struct WRecord),
    SIGN,
    WRecord_constructor, WRecord_destructor
  };

const void * WRecord = & _WRecord;
