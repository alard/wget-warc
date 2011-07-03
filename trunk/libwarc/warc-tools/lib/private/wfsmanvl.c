
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
#include <wmisc.h>    /* NIL ... */
#include <wmem.h>     /* wmalloc, wfree ... */
#include <wstring.h>  /* WString */
#include <whdline.h>  /* WHDLine */
#include <wanvl.h>    /* WAnvl */
#include <wfsm.x>     /* Transition */
#include <wfsmanvl.h>  /* headers */
#include <wcsafe.h>


#define SIGN 9


/**
 * WARC WFsmANVL class internal
 */


typedef struct
  {
    void * comment; /* WString */
    void * key;     /* WString */
    void * value;   /* WString */

    FILE        * fin;  /* file handle to read from */
    Transition  * state;/* transition state */
    warc_bool_t   err;  /* parsing error flag */
    warc_u8_t c;    /* current char in "fin"*/
  } ANVLState;




struct WFsmANVL
  {

    const void * class;

    /*@{*/
    ANVLState * anvls; /**< ANVL State */
    /*@}*/
  };


#define    ANVLS            (self -> anvls)

#define    FIN             (ANVLS -> fin)
#define    STATE           (ANVLS -> state)
#define    ERROR_FLAG      (ANVLS -> err)
#define    CAR             (ANVLS -> c)

#define    KEY         (ANVLS -> key)
#define    VALUE       (ANVLS -> value)
#define    COMMENT     (ANVLS -> comment)


#define makeS(s) ((warc_u8_t *) s), w_strlen((warc_u8_t *) (s))
#define makeC(s) WString_getText((s)), WString_getLength((s))


/* prototypes of all events in the FSM (defined below) */
warc_bool_t WFsmANVL_isFirstSpace   (void *), WFsmANVL_isSpace   (void *), WFsmANVL_isText    (void *),
WFsmANVL_isVdots (void *), WFsmANVL_isCR      (void *),
WFsmANVL_isLF      (void *), WFsmANVL_isUnknown (void *),
WFsmANVL_isSharp (void *), WFsmANVL_isCtlChar (void *),
WFsmANVL_isOpenPar (void *), WFsmANVL_isClosePar (void * ),
WFsmANVL_isLow (void *), WFsmANVL_isHigher (void * ),
WFsmANVL_isArobase (void *), WFsmANVL_isComma (void * ),
WFsmANVL_isSlash (void *), WFsmANVL_isASlash (void * ),
WFsmANVL_isOpenBar (void *), WFsmANVL_isCloseBar (void *),
WFsmANVL_isQMarc (void *), WFsmANVL_isEqual (void *),
WFsmANVL_isOpenBrace (void *), WFsmANVL_isCloseBrace (void *),
WFsmANVL_isQuot (void *) , WFsmANVL_isSComma (void * );





/* prototypes of all actions in the FSM (defined below) */
void WFsmANVL_setKey         (void *), WFsmANVL_setValue  (void *),
WFsmANVL_pushBack       (void *), WFsmANVL_raiseError (void *),
WFsmANVL_setComment     (void *);

/* prototypes of all states in the FSM (defined below) */
State WANT_ANVL_KEYFIRST, WANT_ANVL_ENDLF,      WANT_ANVL_KEY,
WANT_ANVL_VALUE,    WANT_ANVL_LF,         WANT_ANVL_TEXT,
WANT_ANVL_SPACE,    ANVL_IGNORE_COMMENT,  WANT_ANVL_COMMENTLF;



/* /\* rewind the stream (n character before) *\/ */
/* WIPRIVATE void WFsmANVL_rewind (void * _as, warc_u32_t n) */
/* { */
/*   const ANVLState * const as = _as; */

/*   assert (as); */

/*   w_fseek_from_here (as -> fin, - n); */
/* } */





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



State WANT_ANVL_KEYFIRST =
{
  /* TEST_EVENT             ACTION                 NEXT_STATE */

  {WFsmANVL_isText ,        WFsmANVL_setKey    ,   WANT_ANVL_KEY       },
  {WFsmANVL_isCR   ,        NIL                ,   WANT_ANVL_ENDLF     },
  {WFsmANVL_isSharp,        NIL                ,   ANVL_IGNORE_COMMENT },
  {WFsmANVL_isUnknown,      WFsmANVL_raiseError,   NIL},
};

State WANT_ANVL_KEY =
{
  /* TEST_EVENT             ACTION                 NEXT_STATE */

  {WFsmANVL_isText,         WFsmANVL_setKey,        WANT_ANVL_KEY  },
  {WFsmANVL_isSharp,        WFsmANVL_setKey,        WANT_ANVL_KEY  },
  {WFsmANVL_isVdots,        NIL,                    WANT_ANVL_VALUE}, 
  {WFsmANVL_isUnknown,      WFsmANVL_raiseError,    NIL}
};

State WANT_ANVL_VALUE =
{
  /* TEST_EVENT             ACTION                   NEXT_STATE */

/*   {WFsmANVL_isFirstSpace,   NIL,                     WANT_ANVL_VALUE}, */
  {WFsmANVL_isText,         WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isCR,           WFsmANVL_setValue,       WANT_ANVL_LF   },
  {WFsmANVL_isSpace,        WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isVdots,        WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isLF,           WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isSharp,        WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isCtlChar,      WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isOpenPar,      WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isClosePar,     WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isLow,          WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isHigher,       WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isArobase,      WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isComma,        WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isSComma,       WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isSlash,        WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isASlash,       WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isOpenBar,      WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isCloseBar,     WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isQMarc,        WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isEqual,        WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isOpenBrace,    WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isCloseBrace,   WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isQuot,         WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isUnknown,      WFsmANVL_raiseError,     NIL}
};

State WANT_ANVL_LF =
{
  /* TEST_EVENT             ACTION                   NEXT_STATE */

  {WFsmANVL_isText,         WFsmANVL_setValue,      WANT_ANVL_VALUE},
  {WFsmANVL_isLF,           WFsmANVL_setValue,      WANT_ANVL_SPACE},
  {WFsmANVL_isSpace,        WFsmANVL_setValue,      WANT_ANVL_VALUE},
  {WFsmANVL_isSharp,        WFsmANVL_setValue,      WANT_ANVL_VALUE},
  {WFsmANVL_isVdots,        WFsmANVL_setValue,      WANT_ANVL_VALUE},
  {WFsmANVL_isCR,           WFsmANVL_setValue,      WANT_ANVL_VALUE},
  {WFsmANVL_isCtlChar,      WFsmANVL_setValue,      WANT_ANVL_VALUE},
  {WFsmANVL_isOpenPar,      WFsmANVL_setValue,      WANT_ANVL_VALUE},
  {WFsmANVL_isClosePar,     WFsmANVL_setValue,      WANT_ANVL_VALUE},
  {WFsmANVL_isLow,          WFsmANVL_setValue,      WANT_ANVL_VALUE},
  {WFsmANVL_isHigher,       WFsmANVL_setValue,      WANT_ANVL_VALUE},
  {WFsmANVL_isArobase,      WFsmANVL_setValue,      WANT_ANVL_VALUE},
  {WFsmANVL_isComma,        WFsmANVL_setValue,      WANT_ANVL_VALUE},
  {WFsmANVL_isSComma,       WFsmANVL_setValue,      WANT_ANVL_VALUE},
  {WFsmANVL_isSlash,        WFsmANVL_setValue,      WANT_ANVL_VALUE},
  {WFsmANVL_isASlash,       WFsmANVL_setValue,      WANT_ANVL_VALUE},
  {WFsmANVL_isOpenBar,      WFsmANVL_setValue,      WANT_ANVL_VALUE},
  {WFsmANVL_isCloseBar,     WFsmANVL_setValue,      WANT_ANVL_VALUE},
  {WFsmANVL_isQMarc,        WFsmANVL_setValue,      WANT_ANVL_VALUE},
  {WFsmANVL_isEqual,        WFsmANVL_setValue,      WANT_ANVL_VALUE},
  {WFsmANVL_isOpenBrace,    WFsmANVL_setValue,      WANT_ANVL_VALUE},
  {WFsmANVL_isCloseBrace,   WFsmANVL_setValue,      WANT_ANVL_VALUE},
  {WFsmANVL_isQuot,         WFsmANVL_setValue,      WANT_ANVL_VALUE},
  {WFsmANVL_isUnknown,      WFsmANVL_raiseError,    NIL}
};


State WANT_ANVL_SPACE =
{
  /* TEST_EVENT             ACTION                   NEXT_STATE */

  {WFsmANVL_isText,         WFsmANVL_pushBack,       NIL},
  {WFsmANVL_isSpace,        WFsmANVL_setValue,       WANT_ANVL_TEXT },
  {WFsmANVL_isCR,           WFsmANVL_pushBack,       NIL},
  {WFsmANVL_isLF,           WFsmANVL_pushBack,       NIL},
  {WFsmANVL_isSharp,        WFsmANVL_pushBack,       NIL},
  {WFsmANVL_isVdots,        WFsmANVL_pushBack,       NIL},
  {WFsmANVL_isUnknown,      WFsmANVL_raiseError,     NIL}
};

State WANT_ANVL_TEXT =
{
  /* TEST_EVENT             ACTION                   NEXT_STATE */

  {WFsmANVL_isText,         WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isVdots,        WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isSharp,        WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isCtlChar,      WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isOpenPar,      WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isClosePar,     WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isLow,          WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isHigher,       WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isArobase,      WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isComma,        WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isSComma,       WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isSlash,        WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isASlash,       WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isOpenBar,      WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isCloseBar,     WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isQMarc,        WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isEqual,        WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isOpenBrace,    WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isCloseBrace,   WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isQuot,         WFsmANVL_setValue,       WANT_ANVL_VALUE},
  {WFsmANVL_isUnknown,      WFsmANVL_raiseError,     NIL}
};


State WANT_ANVL_ENDLF =
{
  /* TEST_EVENT             ACTION                   NEXT_STATE */

  {WFsmANVL_isLF,           NIL                ,     NIL},
  {WFsmANVL_isUnknown,      WFsmANVL_raiseError,     NIL}

};


State ANVL_IGNORE_COMMENT =
{
  /* TEST_EVENT             ACTION                   NEXT_STATE */

  {WFsmANVL_isCR     ,      WFsmANVL_setComment,     WANT_ANVL_COMMENTLF},
  {WFsmANVL_isText   ,      WFsmANVL_setComment,     ANVL_IGNORE_COMMENT},
  {WFsmANVL_isSpace  ,      WFsmANVL_setComment,     ANVL_IGNORE_COMMENT},
  {WFsmANVL_isVdots  ,      WFsmANVL_setComment,     ANVL_IGNORE_COMMENT},
  {WFsmANVL_isLF     ,      WFsmANVL_setComment,     ANVL_IGNORE_COMMENT},
  {WFsmANVL_isText   ,      WFsmANVL_setComment,     ANVL_IGNORE_COMMENT},
  {WFsmANVL_isSharp  ,      WFsmANVL_setComment,     ANVL_IGNORE_COMMENT},
  {WFsmANVL_isUnknown,      WFsmANVL_raiseError,     NIL}

};


State WANT_ANVL_COMMENTLF =
{
  /* TEST_EVENT             ACTION                   NEXT_STATE */

  {WFsmANVL_isCR     ,      WFsmANVL_setComment,     ANVL_IGNORE_COMMENT},
  {WFsmANVL_isText   ,      WFsmANVL_setComment,     ANVL_IGNORE_COMMENT},
  {WFsmANVL_isSpace  ,      WFsmANVL_setComment,     ANVL_IGNORE_COMMENT},
  {WFsmANVL_isVdots  ,      WFsmANVL_setComment,     ANVL_IGNORE_COMMENT},
  {WFsmANVL_isText   ,      WFsmANVL_setComment,     ANVL_IGNORE_COMMENT},
  {WFsmANVL_isSharp  ,      WFsmANVL_setComment,     ANVL_IGNORE_COMMENT},
  {WFsmANVL_isLF     ,      WFsmANVL_setComment,     WANT_ANVL_KEYFIRST },
  {WFsmANVL_isUnknown,      WFsmANVL_raiseError,     NIL}

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


warc_bool_t WFsmANVL_isFirstSpace (void * _as)
{
  const ANVLState * const as = _as;

  assert (as);

  return ( (as -> c == ' ')  || (as -> c == '\t') );
}

warc_bool_t WFsmANVL_isSpace (void * _as)
{
  const ANVLState * const as = _as;

  assert (as);

  return ( (as -> c == ' ')  || (as -> c == '\t') );
}


warc_bool_t WFsmANVL_isText (void * _as)
{
  const ANVLState * const as = _as;

  assert (as);

  return ( (as -> c != ' ')  && (as -> c != '\t') &&
           (as -> c != '\r') && (as -> c != '\n') &&
           (as -> c != ':') && (as -> c != '#') &&
           (as -> c > 31) && (as -> c != 127)  &&
           (as -> c != '(') && (as -> c != ')') &&
           (as -> c != '<') && (as -> c != '>') &&
           (as -> c != '@') && (as -> c != ',') &&
           (as -> c != ';') && (as -> c != '\\') &&
           (as -> c != '/') && (as -> c != '[') &&
           (as -> c != ']') && (as -> c != '?') &&
           (as -> c != '=') && (as -> c != '{') &&
           (as -> c != '}') && (as -> c != '"') 
           );
}

warc_bool_t WFsmANVL_isVdots (void * _as)
{
  const ANVLState * const as = _as;

  assert (as);

  return ( (as -> c == ':') );
}


warc_bool_t  WFsmANVL_isCR (void * _as)
{
  const ANVLState * const as = _as;

  assert (as);

  return (as -> c == '\r');
}


warc_bool_t  WFsmANVL_isLF (void * _as)
{
  const ANVLState * const as = _as;

  assert (as);

  return (as -> c == '\n');
}

warc_bool_t  WFsmANVL_isSharp (void * _as)
{
  const ANVLState * const as = _as;

  assert (as);

  return (as -> c == '#');
}

warc_bool_t  WFsmANVL_isOpenPar (void * _as)
{
  const ANVLState * const as = _as;

  assert (as);

  return (as -> c == '(');
}

warc_bool_t  WFsmANVL_isClosePar (void * _as)
{
  const ANVLState * const as = _as;

  assert (as);

  return (as -> c == ')');
}

warc_bool_t  WFsmANVL_isLow (void * _as)
{
  const ANVLState * const as = _as;

  assert (as);

  return (as -> c == '<');
}

warc_bool_t  WFsmANVL_isHigher (void * _as)
{
  const ANVLState * const as = _as;

  assert (as);

  return (as -> c == '>');
}

warc_bool_t  WFsmANVL_isArobase (void * _as)
{
  const ANVLState * const as = _as;

  assert (as);

  return (as -> c == '@');
}


warc_bool_t  WFsmANVL_isComma (void * _as)
{
  const ANVLState * const as = _as;

  assert (as);

  return (as -> c == ',');
}

warc_bool_t  WFsmANVL_isSComma (void * _as)
{
  const ANVLState * const as = _as;

  assert (as);

  return (as -> c == ';');
}


warc_bool_t  WFsmANVL_isSlash (void * _as)
{
  const ANVLState * const as = _as;

  assert (as);

  return (as -> c == '\\');
}


warc_bool_t  WFsmANVL_isASlash (void * _as)
{
  const ANVLState * const as = _as;

  assert (as);

  return (as -> c == '/');
}


warc_bool_t  WFsmANVL_isOpenBar (void * _as)
{
  const ANVLState * const as = _as;

  assert (as);

  return (as -> c == '[');
}

warc_bool_t  WFsmANVL_isCloseBar (void * _as)
{
  const ANVLState * const as = _as;

  assert (as);

  return (as -> c == ']');
}

warc_bool_t  WFsmANVL_isQMarc (void * _as)
{
  const ANVLState * const as = _as;

  assert (as);

  return (as -> c == '?');
}


warc_bool_t  WFsmANVL_isEqual (void * _as)
{
  const ANVLState * const as = _as;

  assert (as);

  return (as -> c == '=');
}

warc_bool_t  WFsmANVL_isOpenBrace (void * _as)
{
  const ANVLState * const as = _as;

  assert (as);

  return (as -> c == '{');
}

warc_bool_t  WFsmANVL_isCloseBrace (void * _as)
{
  const ANVLState * const as = _as;

  assert (as);

  return (as -> c == '}');
}

warc_bool_t  WFsmANVL_isQuot (void * _as)
{
  const ANVLState * const as = _as;

  assert (as);

  return (as -> c == '"');
}

warc_bool_t WFsmANVL_isUnknown (void * _as)
{
  assert (_as);
  UNUSED (_as);

  return (WARC_TRUE);
}


warc_bool_t WFsmANVL_isCtlChar (void * _as)
{
  const ANVLState * const as = _as;

  assert (as);

  return (as -> c <= 31 || as -> c == 127);
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



void WFsmANVL_setKey (void * _as)
{
  const ANVLState * const as  = _as;

  assert (as);

  WString_append (as -> key, & (as -> c), 1);
}


void WFsmANVL_setValue (void * _as)
{
  const ANVLState * const as = _as;
  assert (as);

  WString_append (as -> value, & (as -> c), 1);
}


void WFsmANVL_pushBack (void * _as)
{
  const ANVLState * const as = _as;

  assert (as);

  w_ungetc (as -> c, as -> fin);
}


void WFsmANVL_setComment (void * _as)
{
  const ANVLState * const as  = _as;

  assert (as);

  WString_append (as -> comment, & (as -> c), 1);
}


void WFsmANVL_raiseError (void * _as)
{
  ANVLState * const as  = _as;

  assert (as);

  /* raise "on" the error flag */
  as -> err = WARC_TRUE;
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
 * @param _self WFsmANVL object
 *
 * @return address pointer
 *
 * Returns the address of the actual state.
 * Useful for debugging purposes.
 */

WIPUBLIC const void * WFsmANVL_state (const void * const _self)
{

  const struct WFsmANVL * const self    = _self;

  /* preconditions */
  CASSERT (self);

  return (STATE);
}


/**
 * @param _self WFsmANVL object
 *
 * @return warc_bool_t
 *
 * Runs the FSM to detect a WARC anvl field. Returns a warc_bool_t FSM
 * detection succeeds or not.
 *
 * @brief FSM scheduler for WAnvl detection
 */

WPUBLIC warc_bool_t WFsmANVL_run (void * const _self)
{

  struct WFsmANVL * const self = _self;
  char                   c;

  /* preconditions */
  CASSERT (self);

  while (STATE != NIL)
    {
      Transition * tp;

      assert (ANVLS);

      /* read 1 byte from FIN stream */
      c = w_fgetc (FIN);

      /* EOF or "read error" ? */

      if (! w_feof (FIN) && ! w_ferror (FIN) )
        CAR = c;
      else
        return (WARC_TRUE);

      /* check all events in the current state */
      for (tp = STATE; tp -> thisEvent (ANVLS) == WARC_FALSE; ++ tp)
        /* empty body */ ;

      /* call the action corresponding to the event */
      if (tp -> action != NIL)
        tp -> action (ANVLS);

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
 * @param _self WFsmANVL object
 *
 * @return a WAnvl object or NIL
 *
 * Returns a WAnvl record object found by the FSM
 */

WPUBLIC void * WFsmANVL_transform (const void * const _self)
{

  const struct WFsmANVL * const self = _self;
  warc_u32_t                    dec1  = 2, dec2 = 2;

  /* preconditions */
  CASSERT (self);

  unless (WString_getLength (KEY) )
  dec1 = 0;

  unless (WString_getLength (COMMENT) )
  dec2 = 0;


  return (bless (WAnvl,
                 makeC (KEY),
                 makeC (VALUE) - dec1,
                 makeC (COMMENT) - dec2) );
}




/**
 * WFsmANVL_constructor - create a new WFsmANVL object instance
 *
 * @param _self WFsmANVL class object
 * @param app input stream opened for reading (ex.stdin ...)
 *
 * @return a valid WFsmANVL object or NIL
 *
 * Initialize all WString members of the ANVLState pointer "_as"
 */

WPRIVATE void * WFsmANVL_constructor (void * _self, va_list * app)
{

  struct WFsmANVL * const self = _self;
  FILE           *       fin  = va_arg (* app, FILE *);

  ANVLS = wmalloc (sizeof (ANVLState) );
  assert (ANVLS);

  FIN   = fin;              /* read from this readable stream */
  STATE = WANT_ANVL_KEYFIRST;   /* start state */
  ERROR_FLAG = WARC_FALSE;     /* no error when starting */

  KEY = bless (WString, makeS ("") );
  assert (KEY);

  VALUE = bless (WString, makeS ("") );
  assert (VALUE);

  COMMENT = bless (WString, makeS ("") );
  assert (COMMENT);

  return (self);
}


/**
 * @param _self WFsmANVL object instance
 *
 * WARC Anvk SFM destructor
 */

WPRIVATE void * WFsmANVL_destructor (void * _self)
{

  struct WFsmANVL  * const self = _self;

  /* preconditions */
  CASSERT (self);

  FIN        = NIL;
  STATE      = NIL;
  ERROR_FLAG = WARC_FALSE;

  if (KEY)
    destroy (KEY), KEY = NIL;

  if (VALUE)
    destroy (VALUE), VALUE = NIL;

  if (COMMENT)
    destroy (COMMENT), COMMENT = NIL;

  wfree (ANVLS), ANVLS = NIL;

  return (self);
}


/**
 * WARC WFsmANVL class
 */

static const struct Class _WFsmANVL =
  {
    sizeof (struct WFsmANVL),
    SIGN,
    WFsmANVL_constructor, WFsmANVL_destructor
  };

const void * WFsmANVL = & _WFsmANVL;
