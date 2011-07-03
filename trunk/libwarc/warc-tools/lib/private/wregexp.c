
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
#include <regex.h>

#define SIGN 7

/**
 * WARC Regular expressions matcher internal structur
 */


struct WRegexp
  {

    const void * class;

    /*@{*/
    struct re_pattern_buffer pattern; /**< The buffer that will hold the patterns to match */
    struct re_registers reg; /**< to hold the registers result */
    warc_u32_t * fast_map; /**< The fast mapping array of the pattern */
    warc_u8_t * transtable ; /**< the translation table of the pattern */
    
    /*@}*/
  };


#define PATTERN      (self -> pattern)
#define REGISTER     (self -> reg)
#define FMAP         (self -> fast_map)
#define TRANSTABLE   (self -> transtable)


/**
 * @param _self: a WRegexp object instance
 * @param string: the string to match with
 * @param len: the length of the string
 *
 * @return True if succeed, false otherwise
 *
 * Matches the sstring with the  reglar expression stord in the WARC Regexp object
 */

WIPUBLIC warc_bool_t WRegexp_match (void * const _self, const warc_u8_t * string, warc_u32_t len)
{
    struct WRegexp * const self = _self;
    warc_i32_t i = 0;

  /* Preconditions  */
  CASSERT (self);

  unless (string && len)
    return (WARC_FALSE);

  i = re_match (&PATTERN, (char *) string, len, 0, &REGISTER);
  if (i >= 0)
     return (WARC_TRUE);


  return (WARC_FALSE);
}


/**
 * @param _self: a WRegexp object instance
 * @param string: the string to match with
 * @param len: the length of the string
 *
 * @return True if succeed, false otherwise
 *
 * Search, in the string,  for a pattern matching the regular expression 
 * stored in the WARC Regexp object
 */

WIPUBLIC warc_bool_t WRegexp_search (void * const _self, const warc_u8_t * string, warc_u32_t len)
{
    struct WRegexp * const self = _self;
    warc_i32_t i = 0;

  /* Preconditions  */
  CASSERT (self);

  unless (string && len)
    return (WARC_FALSE);

  i = re_search (&PATTERN, (char *) string, len, 0, len, &REGISTER);
  if (i >= 0)
     return (WARC_TRUE);


  return (WARC_FALSE);
}


#define MAPSIZE  1024

/**
 * WRegexp_constructor - create a new WRegexp object instance
 *
 * @param _self WRegexp class object
 * @param app: constructor list parameters
 *
 * @return a valid WRegexp object or NIL
 *
 * @brief WARC Regexp constructor
 */

WPRIVATE void * WRegexp_constructor (void * _self, va_list * app)
{

  struct WRegexp      * const self = _self;
  const warc_u8_t     * text = va_arg (* app, const warc_u8_t *);
  const warc_u32_t      len  = va_arg (* app, const warc_u32_t);
  warc_u32_t            i = 0;
  warc_u8_t           * comperror = NIL;

  /* preconditions */
  unless (text)
    {
     destroy (self);
     return (NIL);
    }

  FMAP = wmalloc (MAPSIZE);
  unless (FMAP)
    {
    destroy (self);
    return (NIL);
    }

  TRANSTABLE = wmalloc (256);
  unless (TRANSTABLE)
    {
    destroy (self);
    return (NIL);
    }

  /* table initialisation */
  for (i = 0; i < 256; i++)
     {
     if ((i >= 'a') && (i <= 'z'))
        TRANSTABLE [i] = i - 'a' + 'A';
     else TRANSTABLE [i] = i;
     }

  PATTERN . buffer = NIL;
  PATTERN . fastmap = (char *) FMAP;
  PATTERN . translate = (char *) TRANSTABLE;
  PATTERN . allocated = 0;

  REGISTER . start = NIL;
  REGISTER . end = NIL;

  comperror = (warc_u8_t *)re_compile_pattern ((char *) text, len, &PATTERN);

  if (comperror)
    {
    wfree (comperror);
    destroy (self);
    return (NIL);
    }

  if (re_compile_fastmap (&PATTERN) < 0)
    {
    destroy (self);
    return (NIL);
    }

  return (self);
}


/**
 * WRegexp_destructor - delete an existing WRegexp object
 *
 * @param _self WRegexp object instance
 *
 * WARC Regexp destructor
 */

WPRIVATE void * WRegexp_destructor (void * _self)
{

  struct WRegexp * self = _self;

  /* preconditions */
  CASSERT (self);

   regfree (&PATTERN);

  if (REGISTER . start)
     wfree (REGISTER . start), REGISTER . start= NIL;

  if (REGISTER . end)
     wfree (REGISTER . end), REGISTER . end = NIL;

  return (self);
}


/**
 * WARC WRegexp class
 */

static const struct Class _WRegexp =
  {
    sizeof (struct WRegexp),
    SIGN,
    WRegexp_constructor, WRegexp_destructor
  };

const void * WRegexp = & _WRegexp;
