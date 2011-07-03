
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
#include <wstring.h> /* for class's prototypes */
#include <wmem.h>    /* wmalloc, wfree */
#include <wmisc.h>   /* unless, ... */
#include <wcsafe.h>  /* w_strncpy, ... */

#define SIGN 1

/**
 * WARC WString internal structure
 */


struct WString
  {

    const void * class;

    /*@{*/
    warc_u8_t   * text; /**< text string */
    warc_u32_t    len;  /**< string length */
    warc_u32_t    max;  /**< max string length */
    /*@}*/
  };


#define TEXT    (self -> text)
#define LEN     (self -> len)
#define MAX     (self -> max)

#define EXTEND_BY   (1U << 3) /* extend the string length by this at leas */


#define NEXT_POWER_OF(min,p) \
  do{ \
      warc_u32_t i = 2;   \
      (p) = EXTEND_BY;    \
      while((p) <= (min)) \
        { \
          (p) = EXTEND_BY * i; \
          i = i + 1; \
        } \
    }while(0)


/**
 * @param _self WString object
 *
 * @return 32 bits unsigned integer
 *
 * Returns the string object length
 */

WIPUBLIC warc_u32_t WString_getLength (const void * const _self)
{

  const struct WString * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (LEN);
}

/**
 * @param _self: WString object
 *
 * @return constant string
 *
 * Returns the object's string as a constant "char *"
 */

WIPUBLIC const warc_u8_t * WString_getText (const void * const _self)
{

  const struct WString * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (TEXT);
}


/**
 * @param _self WString object
 * @param s constant null terminated string
 *
 * @return match position, or -1 if failed to match
 *
 * Finds the first occurrence of a substring in the string text
 */

WIPUBLIC warc_i32_t WString_strstr (const void * const _self,
                                    const warc_u8_t * s)
{

  const struct WString * const self = _self;
  warc_u8_t        *       ptr;

  /* preconditions */
  CASSERT (self);
  assert (s);

  ptr = (warc_u8_t *) w_strstr (TEXT, s);
  unless (ptr)
  return (-1);

  return (ptr - TEXT);
}


/**
 * @param _self WString object
 *
 * @return false if succeeds, true otherwise
 *
 * Concats the text of WString object "b" to the text string
 * of WString "a". Object "b" isn't affected
 */

WPUBLIC warc_bool_t WString_concat (void * const _self, const void * const b)
{

  struct WString * const self = _self;

  /* preconditions */
  CASSERT (self);
  CASSERT (b);

  return (WString_append (self, WString_getText (b), WString_getLength (b) ) );
}


/**
 * @param _self WString object
 *
 * @return false if succeeds, true otherwise
 *
 * Appends the text of "b" to the text string
 * of WString "a".
 */

WPUBLIC warc_bool_t WString_append (void * const _self,
                                    const warc_u8_t * b,
                                    const warc_u32_t blen)
{

  struct WString * const self = _self;
  warc_u8_t  *           text = NIL;
  warc_u32_t             ablen;

  /* preconditions */
  CASSERT (self);
  assert  (b);

  /* avoid append from the same string */
  assert(TEXT != b);
  assert(b < TEXT || b > (TEXT + MAX));

  ablen = LEN + blen;
  text  = TEXT;

  /* re-allocate only if necessary */

  if (ablen >= MAX)
    {
      warc_u32_t max;

      NEXT_POWER_OF (ablen, max);

      /* allocate new text */
      TEXT = wmalloc (max * sizeof (char) );
      unless (TEXT)
      {
        TEXT = text;
        return (WARC_TRUE);
      }

      /* reset the max string length */
      MAX = max;

      /* copy the old string text */
      /* w_strncpy (TEXT, text, LEN); */
      w_strncpy (TEXT, text, LEN);

      /* free old text */
      wfree (text);
    }

  /* append the "b" string one */
  w_strncpy (TEXT + LEN, b, blen);

  /* set the end of string */
  TEXT [ ablen ] = '\0';

  /* set the new string length */
  LEN = ablen;

  return (WARC_FALSE);
}


/**
 * @param _self WString object
 * @param text new text string
 * @param len string length in bytes
 *
 * @return returns false is succeeds, true otherwise
 *
 * Sets a new text string
 */

WPUBLIC warc_bool_t WString_setText (void * const _self,
                                     const warc_u8_t * text,
                                     const warc_u32_t len)
{

  struct WString * const self = _self;

  /* preconditions */
  CASSERT (self);
  assert (text);

  /* avoid set text from the same string */
  assert(TEXT != text);
  assert(text < TEXT || text > (TEXT + MAX));

  /* re-allocate only if necessary */

  if (len >= MAX)
    {
      warc_u8_t * nt   = NIL;
      warc_u32_t      max;

      NEXT_POWER_OF (len, max);

      /* allocate new text */
      nt = wmalloc (max * sizeof (char) );
      unless (nt)
      return (WARC_TRUE);

      /* release previous text */

      if (TEXT)
        wfree (TEXT);

      TEXT = nt;

      /* reset the max string length */
      MAX = max;
    }

  /* make the string copy (only "len" bytes) */
  w_strncpy (TEXT, text, len);

  /* set the end of string */
  TEXT [ len ] = '\0';

  /* set the new string length */
  LEN = len;

  return (WARC_FALSE);
}


/**
 * WString_constructor - create a new WString object instance
 *
 * @param _self WString class object
 * @param app: constructor list parameters
 *
 * @return a valid WString object or NIL
 *
 * @brief WARC String constructor
 */

WPRIVATE void * WString_constructor (void * _self, va_list * app)
{

  struct WString    * const self = _self;
  const warc_u8_t   *       text = va_arg (* app, const warc_u8_t *);
  const warc_u32_t          len  = va_arg (* app, const warc_u32_t);

  /* preconditions */
  assert (text);

  /* compute maximum string length */
  NEXT_POWER_OF (len, MAX);

  /* allocate memory */
  TEXT = wmalloc (MAX * sizeof (char) );
  unless (TEXT)
  {
    destroy (self);
    return (NIL);
  }

  w_strncpy (TEXT, text, len);

  TEXT [len] = '\0';
  LEN        = len;

  return (self);
}


/**
 * WString_destructor - delete an existing WString object
 *
 * @param _self WString object instance
 *
 * WARC String destructor
 */

WPRIVATE void * WString_destructor (void * _self)
{

  struct WString * self = _self;

  /* preconditions */
  CASSERT (self);

  if (TEXT)
    wfree (TEXT), TEXT = NIL;

  MAX = 0;

  LEN = 0;

  return (self);
}


/**
 * WARC WString class
 */

static const struct Class _WString =
  {
    sizeof (struct WString),
    SIGN,
    WString_constructor, WString_destructor
  };

const void * WString = & _WString;
