
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
 * This file is used to get a more tolerant version of ARC 2 WARC when
 * ARC files have timestamp not conforming to 14 digits.
 * The missing digits are filled with zeros (000...)
 * Overwrite the file with the same name "lib/private/afsmhdl.c" with this one
 * and rebuild evrything if you want this feature.
 */

/**
 * Portability header file
 */

#include <wport.h>

/**
 * WARC default headers
 */

#include <wclass.h>   /* bless, destroy, cassert, struct Class */
#include <wclass.h>   /* bless, destroy, cassert, struct Class */
#include <wsign.h>    /* CASSET macro */
#include <wmisc.h>    /* NIL ... */
#include <wmem.h>     /* wmalloc, wfree ... */
#include <wstring.h>  /* WString */
#include <arecord.h>  /* ARecord */
#include <arecord.x>  /* private ARecord functions */
#include <wfsm.x>     /* Transition */
#include <afsmhdl.h>  /* headers */
#include <wcsafe.h>



/**
 * ARC AFsmHDL class internal
 */

#define SIGN 10

typedef struct
  {
    void       * url;           /* WString */
    void       * ip_adress;     /* WString */
    void       * creation_date; /* WString */
    void       * mime_type;     /* WString */
    void       * data_length;   /* WString */

    FILE           * fin;       /* file handle to read from */
    Transition     * state;     /* transition state */
    warc_bool_t    err;         /* parsing error flag */
    warc_u8_t  c;             /* current char in "fin"*/
  } HDLState;




struct AFsmHDL
  {

    const void * class;

    /*@{*/
    HDLState * hdls; /**< HDLState */
    void     * dname; /**< working directory name */

    /*@}*/
  };


#define    HDLS            (self -> hdls)
#define    WORKING_DIR     (self -> dname)


#define    FIN             (HDLS -> fin)
#define    STATE           (HDLS -> state)
#define    ERROR_FLAG      (HDLS -> err)
#define    CAR             (HDLS -> c)

#define    URL             (HDLS -> url)
#define    DATA_LENGTH     (HDLS -> data_length)
#define    MIME_TYPE       (HDLS -> mime_type)
#define    IP_ADRESS       (HDLS -> ip_adress)
#define    CREATION_DATE   (HDLS -> creation_date)




/* prototypes of all events in the FSM (defined below) */
warc_bool_t AFsmHDL_isSpace (void *),    AFsmHDL_isText    (void *),
AFsmHDL_isInteger (void *),  AFsmHDL_isCR      (void *),
AFsmHDL_isLF      (void *),  AFsmHDL_isUnknown (void *);

/* prototypes of all actions in the FSM (defined below) */
void AFsmHDL_setDataLength  (void *), 
AFsmHDL_setDataLengthRelaxed (void *), AFsmHDL_setIpAdress       (void *),
AFsmHDL_setUrl         (void *), AFsmHDL_setCreationDate   (void *),
AFsmHDL_setMimeType    (void *), AFsmHDL_pushBack          (void *),
AFsmHDL_checkRecordType (void *), AFsmHDL_raiseError        (void *),
AFsmHDL_checkIpAddress  (void *), AFsmHDL_raiseErrorDlength (void *),
AFsmHDL_raiseErrorCrDate     (void *), AFsmHDL_checkCreationDate (void *);

/* AFsmHDL_checkMimeType (void *), */
/* AFsmHDL_checkUrl       (void *), */


/* prototypes of all states in the FSM (defined below) */
State WANT_ARCHDL_URL,             WANT_ARCHDL_URL_SP,       WANT_ARCHDL_IP_ADRESS,       WANT_ARCHDL_IP_SP,
WANT_ARCHDL_CREATION_DATE,   WANT_ARCHDL_CREATION_SP,  WANT_ARCHDL_MIME_TYPE,       WANT_ARCHDL_MIME_SP,
WANT_ARCHDL_DATA_LENGTH,     WANT_ARCHDL_LF;






/* rewind the stream (n character before) */
WIPRIVATE void AFsmHDL_rewind (void * _hs, warc_u32_t n)
{
  const HDLState * const hs = _hs;

  assert (hs);

  w_fseek_from_here (hs -> fin, - n);
}





/*

@@@@@@@@   @@@@@@   @@@@@@@@@@
@@@@@@@@  @@@@@@@   @@@@@@@@@@@
@@!       !@@       @@! @@! @@!
!@!       !@!       !@! !@! !@!
@!!!:!    !!@@!!    @!! !!@ @!@
!!!!!:     !!@!!!   !@!   ! !@!
!!:            !:!  !!:     !!:
:!:           !:!   :!:     :!:
 ::       :::: ::   :::     ::
 :        :: : :     :      :


 @@@@@@   @@@@@@@   @@@@@@   @@@@@@@  @@@@@@@@   @@@@@@
@@@@@@@   @@@@@@@  @@@@@@@@  @@@@@@@  @@@@@@@@  @@@@@@@
!@@         @@!    @@!  @@@    @@!    @@!       !@@
!@!         !@!    !@!  @!@    !@!    !@!       !@!
!!@@!!      @!!    @!@!@!@!    @!!    @!!!:!    !!@@!!
 !!@!!!     !!!    !!!@!!!!    !!!    !!!!!:     !!@!!!
     !:!    !!:    !!:  !!!    !!:    !!:            !:!
    !:!     :!:    :!:  !:!    :!:    :!:           !:!
:::: ::      ::    ::   :::     ::     :: ::::  :::: ::
:: : :       :      :   : :     :     : :: ::   :: : :

*/



State WANT_ARCHDL_URL =
{
  /* TEST_EVENT             ACTION                   NEXT_STATE */

  {AFsmHDL_isText,          AFsmHDL_setUrl,          WANT_ARCHDL_URL},
  {AFsmHDL_isSpace,         NIL,                     WANT_ARCHDL_URL_SP  },
  {AFsmHDL_isUnknown,       AFsmHDL_raiseError,      NIL             }
};
State WANT_ARCHDL_URL_SP =
{
  /* TEST_EVENT             ACTION                      NEXT_STATE */

  {AFsmHDL_isSpace,         NIL,                        WANT_ARCHDL_URL_SP},
  {AFsmHDL_isText,          AFsmHDL_setIpAdress,        WANT_ARCHDL_IP_ADRESS},
  {AFsmHDL_isUnknown,       AFsmHDL_raiseError,         NIL}
};
State WANT_ARCHDL_IP_ADRESS =
{
  /* TEST_EVENT             ACTION                       NEXT_STATE */

  {AFsmHDL_isText,          AFsmHDL_setIpAdress,          WANT_ARCHDL_IP_ADRESS},
  {AFsmHDL_isSpace,         AFsmHDL_checkIpAddress,        WANT_ARCHDL_IP_SP},
  {AFsmHDL_isUnknown,       AFsmHDL_raiseError,           NIL}
};




State WANT_ARCHDL_IP_SP =
{
  /* TEST_EVENT             ACTION                         NEXT_STATE */

  {AFsmHDL_isSpace,         NIL,                           WANT_ARCHDL_IP_SP},
  {AFsmHDL_isInteger,       AFsmHDL_setCreationDate,       WANT_ARCHDL_CREATION_DATE},
  {AFsmHDL_isUnknown,       AFsmHDL_raiseErrorCrDate,      NIL}
};

State WANT_ARCHDL_CREATION_DATE =
{
  /* TEST_EVENT             ACTION                       NEXT_STATE */

  {AFsmHDL_isInteger,       AFsmHDL_setCreationDate,     WANT_ARCHDL_CREATION_DATE},
  {AFsmHDL_isSpace,         AFsmHDL_checkCreationDate,   WANT_ARCHDL_CREATION_SP},
  {AFsmHDL_isUnknown,       AFsmHDL_raiseErrorCrDate,    NIL}
};

State WANT_ARCHDL_CREATION_SP =
{
  /* TEST_EVENT             ACTION                   NEXT_STATE */

  {AFsmHDL_isSpace,         NIL,                     WANT_ARCHDL_CREATION_SP},
  {AFsmHDL_isText,          AFsmHDL_setMimeType,     WANT_ARCHDL_MIME_TYPE},
  {AFsmHDL_isUnknown,       AFsmHDL_raiseError,      NIL}
};


State WANT_ARCHDL_MIME_TYPE =
{
  /* TEST_EVENT             ACTION                 NEXT_STATE */

  {AFsmHDL_isCR,            AFsmHDL_setDataLengthRelaxed, WANT_ARCHDL_LF},
  {AFsmHDL_isLF,            AFsmHDL_setDataLengthRelaxed, WANT_ARCHDL_LF},
  {AFsmHDL_isText,          AFsmHDL_setMimeType,   WANT_ARCHDL_MIME_TYPE},
  {AFsmHDL_isSpace,         NIL,                   WANT_ARCHDL_MIME_SP},
  {AFsmHDL_isUnknown,       AFsmHDL_raiseError,    NIL}
};
State WANT_ARCHDL_MIME_SP =
{
  /* TEST_EVENT             ACTION                       NEXT_STATE */

  /*ZZZ*/
  {AFsmHDL_isCR,            AFsmHDL_setDataLengthRelaxed, WANT_ARCHDL_LF},
  {AFsmHDL_isLF,            AFsmHDL_setDataLengthRelaxed, WANT_ARCHDL_LF},
  {AFsmHDL_isSpace,         NIL,                         WANT_ARCHDL_MIME_SP},
  {AFsmHDL_isInteger,       AFsmHDL_setDataLength,       WANT_ARCHDL_DATA_LENGTH},
  {AFsmHDL_isUnknown,       AFsmHDL_raiseErrorDlength,   NIL}
};

State WANT_ARCHDL_DATA_LENGTH =
{
  /* TEST_EVENT             ACTION                     NEXT_STATE */

  {AFsmHDL_isCR,            NIL,                       WANT_ARCHDL_LF},
  {AFsmHDL_isLF,            AFsmHDL_pushBack,          WANT_ARCHDL_LF},
  {AFsmHDL_isInteger,       AFsmHDL_setDataLength,     WANT_ARCHDL_DATA_LENGTH},
  {AFsmHDL_isUnknown,       AFsmHDL_raiseErrorDlength, NIL}
};


State WANT_ARCHDL_LF =
{
  /* TEST_EVENT             ACTION                   NEXT_STATE */

  {AFsmHDL_isLF,            NIL,                     NIL},
  {AFsmHDL_isUnknown,       NIL,                     NIL}
/*   {AFsmHDL_isUnknown,       AFsmHDL_raiseError,      NIL} */
};




/*

@@@@@@@@   @@@@@@   @@@@@@@@@@
@@@@@@@@  @@@@@@@   @@@@@@@@@@@
@@!       !@@       @@! @@! @@!
!@!       !@!       !@! !@! !@!
@!!!:!    !!@@!!    @!! !!@ @!@
!!!!!:     !!@!!!   !@!   ! !@!
!!:            !:!  !!:     !!:
:!:           !:!   :!:     :!:
 ::       :::: ::   :::     ::
 :        :: : :     :      :


@@@@@@@@  @@@  @@@  @@@@@@@@  @@@  @@@  @@@@@@@   @@@@@@
@@@@@@@@  @@@  @@@  @@@@@@@@  @@@@ @@@  @@@@@@@  @@@@@@@
@@!       @@!  @@@  @@!       @@!@!@@@    @@!    !@@
!@!       !@!  @!@  !@!       !@!!@!@!    !@!    !@!
@!!!:!    @!@  !@!  @!!!:!    @!@ !!@!    @!!    !!@@!!
!!!!!:    !@!  !!!  !!!!!:    !@!  !!!    !!!     !!@!!!
!!:       :!:  !!:  !!:       !!:  !!!    !!:         !:!
:!:        ::!!:!   :!:       :!:  !:!    :!:        !:!
 :: ::::    ::::     :: ::::   ::   ::     ::    :::: ::
: :: ::      :      : :: ::   ::    :      :     :: : :

 */


warc_bool_t AFsmHDL_isSpace (void * _hs)
{
  const HDLState * const hs = _hs;

  assert (hs);

  return ( (hs -> c == ' ')  || (hs -> c == '\t') ||
           (hs -> c == '\r') || (hs -> c == '\n') );
}


warc_bool_t AFsmHDL_isText (void * _hs)
{
  const HDLState * const hs = _hs;

  assert (hs);

  return ( (hs -> c != ' ')  && (hs -> c != '\t') &&
           (hs -> c != '\r') && (hs -> c != '\n') );
}

warc_bool_t AFsmHDL_isInteger (void * _hs)
{
  const HDLState * const hs = _hs;

  assert (hs);

  return ( (hs -> c >= 48) && (hs -> c <= 57) );
}


warc_bool_t  AFsmHDL_isCR (void * _hs)
{
  const HDLState * const hs = _hs;

  assert (hs);

  return (hs -> c == '\r');
}

warc_bool_t  AFsmHDL_isLF (void * _hs)
{
  const HDLState * const hs = _hs;

  assert (hs);

  return (hs -> c == '\n');
}


warc_bool_t AFsmHDL_isUnknown (void * _hs)
{
  assert (_hs);
  UNUSED (_hs);
  return (WARC_TRUE);
}



/*

@@@@@@@@   @@@@@@   @@@@@@@@@@
@@@@@@@@  @@@@@@@   @@@@@@@@@@@
@@!       !@@       @@! @@! @@!
!@!       !@!       !@! !@! !@!
@!!!:!    !!@@!!    @!! !!@ @!@
!!!!!:     !!@!!!   !@!   ! !@!
!!:            !:!  !!:     !!:
:!:           !:!   :!:     :!:
 ::       :::: ::   :::     ::
 :        :: : :     :      :


 @@@@@@    @@@@@@@  @@@@@@@  @@@   @@@@@@   @@@  @@@   @@@@@@
@@@@@@@@  @@@@@@@@  @@@@@@@  @@@  @@@@@@@@  @@@@ @@@  @@@@@@@
@@!  @@@  !@@         @@!    @@!  @@!  @@@  @@!@!@@@  !@@
!@!  @!@  !@!         !@!    !@!  !@!  @!@  !@!!@!@!  !@!
@!@!@!@!  !@!         @!!    !!@  @!@  !@!  @!@ !!@!  !!@@!!
!!!@!!!!  !!!         !!!    !!!  !@!  !!!  !@!  !!!   !!@!!!
!!:  !!!  :!!         !!:    !!:  !!:  !!!  !!:  !!!       !:!
:!:  !:!  :!:         :!:    :!:  :!:  !:!  :!:  !:!      !:!
::   :::   ::: :::     ::     ::  ::::: ::   ::   ::  :::: ::
 :   : :   :: :: :     :     :     : :  :   ::    :   :: : :

*/






void AFsmHDL_setDataLength (void * _hs)
{
  const HDLState * const hs = _hs;

  assert (hs);

  WString_append (hs -> data_length, & (hs -> c), 1);
}

void AFsmHDL_setDataLengthRelaxed (void * _hs)
{
  const HDLState * const hs = _hs;

  assert (hs);

  /* push back the char */
  AFsmHDL_pushBack(_hs);

  /* Maybe the MIME TYPE is missing. So we got DataLength instead */
  if (strspn ((char *) WString_getText(hs -> mime_type), "0123456789") != WString_getLength (hs -> mime_type) )
    {
      AFsmHDL_raiseErrorDlength(_hs);
      return;
    }

 /* We're almost sure that MIME TYPE is missing due to 
    a malformed header. Set DataLength to MIME TYPE value and set
   the MIME TYPE to "unknown_mime" */
  WString_setText (hs -> data_length, 
                   WString_getText   (hs -> mime_type),
                   WString_getLength (hs -> mime_type));

#define UNKNOWN_MIME "unknown_mime"
  WString_setText (hs -> mime_type, (warc_u8_t *) UNKNOWN_MIME, strlen(UNKNOWN_MIME));
}




/* void AFsmHDL_checkUrl  (void * _hs) */
/* { */
/*   HDLState * const hs  = _hs; */

/*   warc_i32_t        i; */
/*   assert (hs); */
/*   i= WString_strstr(hs->url,"://"); */

/*  /\* if unknown uri, stop parsing *\/ */
/*  if (i == -1) */
/*    { */
/*      /\* rewind the stream *\/ */
/*      AFsmHDL_rewind (hs, WString_getLength (hs -> url) + 1); */

/*      /\* raise the flag errore *\/ */
/*      WarcDebugMsg("expecting a valid URL"); */
/*      hs -> err = WARC_TRUE; */
/*    } */
/* } */

/* void   AFsmHDL_checkMimeType (void * _hs) */
/* { */
/*   HDLState * const hs  = _hs; */

/*   warc_i32_t i; */
/*   assert (hs); */
/*   i= WString_strstr(hs->mime_type,"/"); */
/*  /\* if unknown Content type, stop parsing *\/ */
/*  if (i == -1) */
/*    { */
/*      /\* rewind the stream *\/ */
/*      AFsmHDL_rewind (hs, WString_getLength (hs -> mime_type) + 1); */

/*      /\* raise the flag errore *\/ */
/*      WarcDebugMsg("expecting a valid MimeType"); */
/*      hs -> err = WARC_TRUE; */
/*    } */
/* } */



void AFsmHDL_checkCreationDate (void * _hs)
{
  HDLState            * const hs  = _hs;
  const warc_u8_t     *       strtompon;
  warc_u32_t                  len;
  warc_u32_t            diff = 0;
  warc_u8_t             zero[2];
  warc_u32_t             i = 0;

  assert (hs);

  len       = WString_getLength (hs -> creation_date);
  strtompon = WString_getText (hs -> creation_date);

  /*  printf(">>> CREATION DATE: %s %u\n",  WString_getText (hs -> creation_date), len); */

  if (len != 14)
    {

     diff = 14 - len;
     zero [0] = '0';
     zero [1] = '\0';

     for (i = 0; i < diff; i++)
       WString_append(hs -> creation_date, zero, 1);

/*      w_fprintf (fprintf (stderr, "error> found creation date: %s\n", (char *) strtompon) );*/

      /* raise the flag error */
/*      WarcDebugMsg ("expecting a valid creation date with 14 digits");*/
/*      hs -> err = WARC_TRUE;  */

      /* rewind the stream */
/*      AFsmHDL_rewind (hs, WString_getLength (hs -> creation_date) + 1);*/
    }


    while (len)
      {
        len --;

        if (! isdigit (strtompon[len]) )
          {
            w_fprintf (fprintf (stderr, "error> found creation date: %s\n", (char *) strtompon) );

            /* raise the flag error */
            WarcDebugMsg ("expecting a valid creation date: not digit character");
            hs -> err = WARC_TRUE;

            /* rewind the stream */
            AFsmHDL_rewind (hs, WString_getLength (hs -> creation_date) + 1);
            break;
          }
      }
}




void AFsmHDL_checkIpAddress (void * _hs)
{
#define PORTMAX 65535 /* or whatever appropriate */

  HDLState        * const hs  = _hs;
  const warc_u8_t * ipadd     = NIL;
  warc_u32_t   rc;
  warc_u32_t   b1, b2, b3, b4, port = 0;
  warc_u8_t    c;

  assert (hs);

  ipadd = WString_getText (hs -> ip_adress);

  rc = sscanf ( (const char *) ipadd, "%3u.%3u.%3u.%3u:%u%c",
                & b1, & b2, & b3, & b4, & port, & c);

  if (rc != 4 && rc != 5)
    {
      w_fprintf (fprintf (stderr, "error> found IP address: %s\n", (char *) ipadd) );
      /* raise the flag error */
      WarcDebugMsg ("expecting a valid IP address: accept only IPv4 adresses");
      hs -> err = WARC_TRUE;

      /* rewind the stream */
      AFsmHDL_rewind (hs, WString_getLength (hs -> creation_date) + 1);
      return;
    }


  if ( (b1 | b2 | b3 | b4) > 255 || port > PORTMAX)
    {
      w_fprintf (fprintf (stderr, "error> found IP address: %s\n", (char *) ipadd) );
      /* raise the flag error */
      WarcDebugMsg ("expecting a valid IP address: IP range greater than 255 or port number greater than" PORTMAX);
      hs -> err = WARC_TRUE;

      /* rewind the stream */
      AFsmHDL_rewind (hs, WString_getLength (hs -> creation_date) + 1);
      return;
    }

  if (strspn ( (const char *) ipadd, "0123456789.:") < WString_getLength (hs -> creation_date) )
    {
      w_fprintf (fprintf (stderr, "error> found IP address: %s\n", (char *) ipadd) );
      /* raise the flag error */
      WarcDebugMsg ("expecting a valid IP address: only \"0123456789.:\" are valid");
      hs -> err = WARC_TRUE;

      /* rewind the stream */
      AFsmHDL_rewind (hs, WString_getLength (hs -> creation_date) + 1);
      return;
    }

  /* Valid IP address */
}


/* WPRIVATE warc_i32_t stroccur (const warc_u8_t * str, unsigned char c) */
/* { */
/*   warc_u32_t counter = 0; */
/*   warc_u32_t index   = 0; */

/*   while (index < w_strlen (str) ) */
/*     { */
/*       if (c == str [index]) */
/*         { */
/*           ++ counter; */

/*           if (c == str [index + 1]) */
/*             return -1; */
/*         } */

/*       ++ index; */
/*     } */

/*   return counter; */
/* } */

/* void   AFsmHDL_checkIpAddress (void * _hs) */
/* { */
/*   HDLState            * const hs  = _hs; */
/*   const warc_u8_t * strtompon; */
/*   warc_u32_t        index; */
/*   warc_i32_t        Error       = 0; */
/*   warc_i32_t        Digit_Count = 1; */
/*   warc_u32_t        len; */

/*   assert (hs); */

/*   strtompon = WString_getText (hs -> ip_adress); */

/*   if (! isdigit (strtompon[0]) ) */
/*     { */
/*       Error = 1; */
/*     } */

/*   if (! isdigit (strtompon [w_strlen (strtompon) - 1]) */
/*           && Error != 1) */
/*     { */
/*       Error = 1; */
/*     } */

/*   if (Error != 1 && stroccur (strtompon, '.') != 3) */
/*     { */
/*       Error = 1; */
/*     } */

/*   if (Error != 1) */
/*     { */
/*       len = w_strlen (strtompon); */

/*       for (index = 1; index < len; ++ index) */
/*         { */
/*           if (strtompon [index] != '.') */
/*             { */

/*               if (! isdigit (strtompon [index]) ) */
/*                 { */

/*                   Error = 1; */
/*                   break; */
/*                 } */

/*               if (isdigit (strtompon [index]) ) */
/*                 { */
/*                   Digit_Count ++; */

/*                   if (Digit_Count == 4) */
/*                     { */
/*                       Error = 1; */
/*                       break; */
/*                     } */
/*                 } */
/*             } */

/*           if (strtompon [index] == '.') */
/*             { */
/*               Digit_Count = 0; */
/*             } */
/*         } */
/*     } */

/*   if (Error == 1) */
/*     { */
/*       char buf [20]; */

/*       /\* rewind the stream *\/ */
/*       AFsmHDL_rewind (hs, WString_getLength (hs -> ip_adress) + 1); */

/*       fread (buf, sizeof(char), 19, hs -> fin); */
/*       buf [19] = '\0'; */
/*       printf (">>> invalid IP address: %s\n", buf); */

/*       /\* raise the flag error *\/ */
/*       WarcDebugMsg ("expecting a valid IP address"); */
/*       hs -> err = WARC_TRUE; */
/*     } */
/* } */

void AFsmHDL_setUrl (void * _hs)
{
  const HDLState * const hs = _hs;

  assert (hs);

  WString_append (hs -> url, & (hs -> c), 1);
}

void AFsmHDL_setCreationDate (void * _hs)
{
  const HDLState * const hs = _hs;

  assert (hs);

  WString_append (hs -> creation_date, & (hs -> c), 1);
}

void AFsmHDL_setMimeType (void * _hs)
{
  const HDLState * const hs = _hs;

  assert (hs);

  WString_append (hs -> mime_type, & (hs -> c), 1);
}

void AFsmHDL_setIpAdress (void * _hs)
{
  const HDLState * const hs = _hs;

  assert (hs);

  WString_append (hs -> ip_adress, & (hs -> c), 1);
}

void AFsmHDL_pushBack (void * _hs)
{
  const HDLState * const hs = _hs;

  assert (hs);

  w_ungetc (hs -> c, hs -> fin);
}


void AFsmHDL_raiseError (void * _hs)
{
  HDLState * const hs  = _hs;

  assert (hs);

  /* raise "on" the error flag */
  hs -> err = WARC_TRUE;
}

void AFsmHDL_raiseErrorDlength (void * _hs)
{
  HDLState * const hs  = _hs;
#define buf_size 30
  char buf [buf_size];

  assert (hs);

  w_ungetc (hs -> c, hs -> fin);

  fread (buf, sizeof(char), buf_size-1, hs -> fin);
  buf [buf_size-1] = '\0';
  printf (">>> invalid data length: %s\n", buf);
  
  WarcDebugMsg ("expecting a valid data length");
  /* raise "on" the error flag */
  hs -> err = WARC_TRUE;
}

void  AFsmHDL_raiseErrorCrDate (void * _hs)
{
  HDLState * const hs  = _hs;
  /*   char buf [14]; */

  assert (hs);
  w_ungetc (hs -> c, hs -> fin);

  /*   fread (buf, sizeof(char), 13, hs -> fin); */
  /*   buf [13] = '\0'; */
  /*   printf (">>> invalid date : %s\n", buf); */

  WarcDebugMsg ("expecting a valid creation date");
  /* raise "on" the error flag */
  hs -> err = WARC_TRUE;
}


/*

@@@@@@@@   @@@@@@   @@@@@@@@@@       @@@@@@   @@@@@@@   @@@
@@@@@@@@  @@@@@@@   @@@@@@@@@@@     @@@@@@@@  @@@@@@@@  @@@
@@!       !@@       @@! @@! @@!     @@!  @@@  @@!  @@@  @@!
!@!       !@!       !@! !@! !@!     !@!  @!@  !@!  @!@  !@!
@!!!:!    !!@@!!    @!! !!@ @!@     @!@!@!@!  @!@@!@!   !!@
!!!!!:     !!@!!!   !@!   ! !@!     !!!@!!!!  !!@!!!    !!!
!!:            !:!  !!:     !!:     !!:  !!!  !!:       !!:
:!:           !:!   :!:     :!:     :!:  !:!  :!:       :!:
 ::       :::: ::   :::     ::      ::   :::   ::        ::
 :        :: : :     :      :        :   : :   :        :

*/


/**
 * @_self: AFsmHDL object
 * @param[out]: address pointer
 * Returns the address of the actual state.
 * Useful for debugging purposes.
 */

WIPUBLIC const void * AFsmHDL_state (const void * const _self)
{

  const struct AFsmHDL * const self    = _self;

  /* preconditions */
  CASSERT (self);

  return (STATE);
}


/**
 * @_self: AFsmHDL object
 * @param[out]: warc_bool_t
 * Runs the FSM to detect a WARC header line. Returns a warc_bool_t FSM
 * detection succeeds or not.
 * FSM scheduler for WHDLine detection
 */

WPUBLIC warc_bool_t AFsmHDL_run (void * const _self)
{

  struct AFsmHDL * const self = _self;
  char                   c;

  /* preconditions */
  CASSERT (self);

  while (STATE != NIL)
    {
      Transition * tp;

      assert (HDLS);

      /* read 1 byte from FIN stream */
      c = w_fgetc (FIN);

      /* EOF or "read error" ? */

      if (! w_feof (FIN) && ! w_ferror (FIN) )
        CAR = c;
      else
        return (WARC_TRUE);

      /* check all events in the current state */
      for (tp = STATE; tp -> thisEvent (HDLS) == WARC_FALSE; ++ tp)
        /* empty body */ ;

      /* call the action corresponding to the event */
      if (tp -> action != NIL)
        tp -> action (HDLS);

      /* move to the new state if no error */
      /* don't forget to advance the "id" state number */
      unless (ERROR_FLAG)
      STATE = tp -> newState;
      else
        break;
    }

  /* STATE != NIL means "error", otherwise "success" */
  return (STATE != NIL);
}



/**
 * @_self: AFsmHDL object
 * @param[out]: a WHDLine object or NIL
 * Returns a WHDLine record object found by the FSM
 */

WPUBLIC void * AFsmHDL_transform (const void * const _self)
{
#define makeC(s) WString_getText((s)), WString_getLength((s))

  const struct AFsmHDL * const self    = _self;

  /* preconditions */
  CASSERT (self);

  return (bless (ARecord,
                 makeC (URL),
                 makeC (IP_ADRESS),
                 makeC (CREATION_DATE),
                 makeC (MIME_TYPE),
                 (warc_u32_t) atoi ( (const char *) WString_getText (DATA_LENGTH)),
                 WORKING_DIR ) );

}




/**
 * AFsmHDL_constructor - create a new AFsmHDL object instance
 * @_self: AFsmHDL class object
 * @app: input stream opened for reading (ex.stdin ...)
 * @param[out] a valid AFsmHDL object or NIL
 * Initialize all WString members of the HDLState pointer "_hs"
 */

WPRIVATE void * AFsmHDL_constructor (void * _self, va_list * app)
{
#define makeS(s) (s), w_strlen((s))

  struct AFsmHDL * const self  = _self;
  FILE           *       fin   = va_arg (* app, FILE *);
  void           *       wdir  = va_arg (* app, void *);

  HDLS = wmalloc (sizeof (HDLState) );
  assert (HDLS);

  FIN   = fin;              /* read from this readable stream */
  STATE = WANT_ARCHDL_URL; /* start state */
  ERROR_FLAG = WARC_FALSE;       /* no error when starting */


  DATA_LENGTH = bless (WString, makeS ( (warc_u8_t *) "") );
  assert (DATA_LENGTH);


  URL = bless (WString, makeS ( (warc_u8_t *) "") );
  assert (URL);

  CREATION_DATE = bless (WString, makeS ( (warc_u8_t *) "") );
  assert (CREATION_DATE);

  MIME_TYPE = bless (WString, makeS ( (warc_u8_t *) "") );
  assert (MIME_TYPE);

  IP_ADRESS = bless (WString, makeS ( (warc_u8_t *) "") );
  assert (IP_ADRESS);

  WORKING_DIR = wdir;

  return (self);
}


/**
 * @_self: AFsmHDL object instance
 * WARC List destructor
 */

WPRIVATE void * AFsmHDL_destructor (void * _self)
{

  struct AFsmHDL  * const self = _self;

  /* preconditions */
  CASSERT (self);

  FIN        = NIL;
  STATE      = NIL;
  ERROR_FLAG = WARC_FALSE;

  if (DATA_LENGTH)
    destroy (DATA_LENGTH), DATA_LENGTH = NIL;

  if (URL)
    destroy (URL), URL = NIL;

  if (CREATION_DATE)
    destroy (CREATION_DATE), CREATION_DATE = NIL;

  if (MIME_TYPE)
    destroy (MIME_TYPE), MIME_TYPE = NIL;

  if (IP_ADRESS)
    destroy (IP_ADRESS), IP_ADRESS = NIL;

  wfree (HDLS), HDLS = NIL;

  return (self);
}


/**
 * WARC AFsmHDL class
 */

static const struct Class _AFsmHDL =
  {
    sizeof (struct AFsmHDL),
    SIGN,
    AFsmHDL_constructor, AFsmHDL_destructor
  };

const void * AFsmHDL = & _AFsmHDL;
