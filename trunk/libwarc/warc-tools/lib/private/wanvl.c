
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
#include <wanvl.h>   /* for class's prototypes */
#include <wstring.h> /* WString */
#include "wmisc.h"   /* warc_bool_t ... */
#include <wcsafe.h>


#define SIGN 4

/**
 * WARC WAnvl class internal
 */


struct WAnvl
  {

    const void * class;

    /*@{*/
    void * key; /**< WString key */
    void * value; /**< WString value */
    /*@}*/
  };


#define KEY     (self -> key)
#define VALUE   (self -> value)


/**
 * @param _self WAnvl object
 *
 * @return boolean
 *
 * Returns if yes or no the ANVL is valid (means has a key length > 0)
 */

WIPUBLIC warc_bool_t WAnvl_isValid (const void * const _self)
{

  const struct WAnvl * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WString_getLength (KEY) > 0);
}


/**
 * @param _self WAnvl object
 *
 * @return constant string
 *
 * Returns the ANVL key as a constant "char *"
 */

WIPUBLIC const warc_u8_t * WAnvl_getKey (const void * const _self)
{

  const struct WAnvl * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WString_getText (KEY) );
}


/**
 * @param _self Anvl object instance
 *
 * @return the key length as 4 bytes integer
 *
 * Abvl key length provider
 */

WIPUBLIC warc_u32_t WAnvl_getKeyLen (const void * const _self)
{

  const struct WAnvl * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WString_getLength (KEY) );
}


/**
 * @param _self WAnvl object
 * @param text new text string
 * @param len string length in bytes
 *
 * @return returns false is succeeds, true otherwise
 *
 * Sets a new text string as a ANVL key
 */

WPUBLIC warc_bool_t WAnvl_setKey (void * const _self,
                                  const warc_u8_t * text,
                                  const warc_u32_t len)
{

  struct WAnvl  * const self = _self;
  warc_u32_t    i = 1;
  warc_u8_t ctl [2] = {'\x01', '\0'};

  /* preconditions */
  CASSERT (self);
  assert (text);

  /* reject any key length = 0 */
  unless (len)
    return (WARC_TRUE);
  
  if (w_strcasestr (text, (warc_u8_t *) " ") )
    return (WARC_TRUE);

  if (w_strcasestr (text, (warc_u8_t *) "\x7F") )
    return (WARC_TRUE);

  for (i = 1; i < 32 ;i++)
    {
      ctl [0] = i;

      if (w_strcasestr (text, ctl) )
        return (WARC_TRUE);
    }

  return (WString_setText (KEY, text, len) );
}


/**
 * @param _self WAnvl object
 *
 * @return constant string
 *
 * Returns the ANVL value as a constant "char *"
 */

WIPUBLIC const warc_u8_t * WAnvl_getValue (const void * const _self)
{

  const struct WAnvl * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WString_getText (VALUE) );
}


/**
 * @param _self Anvl object instance
 *
 * @return the Anvl value length as a 4 bytes integer
 *
 * Anvl value length provider
 */

WIPUBLIC warc_u32_t WAnvl_getValueLen (const void * const _self)
{

  const struct WAnvl * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WString_getLength (VALUE) );
}


/**
 * @param _self WAnvl object
 * @param UTF-8 text string
 * @param len string in bytes
 *
 * @return returns false is succeeds, true otherwise
 *
 * Check if the text is a valid UTF-8
 */

WPRIVATE warc_bool_t WAnvl_validUTF8 (void * const _self,
                                      const warc_u8_t * text)
{
  warc_u32_t        nb = 0;
  const warc_u8_t * c;

  UNUSED (_self);

  for (c = text;  *c;  c += (nb + 1) )
    {
      if      (! (*c & 0x80) ) nb = 0;
      else if ( (*c & 0xc0) == 0x80) return (WARC_TRUE);
      else if ( (*c & 0xe0) == 0xc0) nb = 1;
      else if ( (*c & 0xf0) == 0xe0) nb = 2;
      else if ( (*c & 0xf8) == 0xf0) nb = 3;
      else if ( (*c & 0xfc) == 0xf8) nb = 4;
      else if ( (*c & 0xfe) == 0xfc) nb = 5;

      while (nb > 0)
        {
          -- nb;

          if ( (* (c + nb) & 0xc0) != 0x80)
            return (WARC_TRUE);
        }
    }

  return (WARC_FALSE);
}


/**
 * @param _self WAnvl object
 * @param UTF-8 text text string
 * @param len string in bytes
 *
 * @return returns false is succeeds, true otherwise
 *
 * Sets a new text string as a ANVL value
 */

WPUBLIC warc_bool_t WAnvl_setValue (void * const _self,
                                    const warc_u8_t * text,
                                    const warc_u32_t len)
{

  struct WAnvl  * const self = _self;

  /* preconditions */
  CASSERT (self);
  assert (text);

  /* check if the string is a valid UTF-8 */

  if (WAnvl_validUTF8 (self, text) )
    {
      WarcDebugMsg ("invalid UTF-8 string\n");
      return (WARC_TRUE);
    }

  return (WString_setText (VALUE, text, len) );
}



/**
 * WAnvl_constructor - create a new WAnvl object instance
 *
 * @param _self WAnvl class object
 * @param app constructor list parameters
 *
 * @return a valid WAnvl object or NIL
 *
 * WARC List constructor
 */

WPRIVATE void * WAnvl_constructor (void * _self, va_list * app)
{

  struct WAnvl * const self  = _self;
  const warc_u8_t    * key   = va_arg (* app, const warc_u8_t *);
  const warc_u32_t     klen  = va_arg (* app, const warc_u32_t);
  const warc_u8_t    * value = va_arg (* app, const warc_u8_t *);
  const warc_u32_t     vlen  = va_arg (* app, const warc_u32_t);

  /* preconditions */
  assert (key);
  assert (value);

  /* reject any key length = 0 */
  unless (klen)
  {
    destroy (self);
    return (NIL);
  }

  if (w_strcasestr (key, (warc_u8_t *) " ") )
    {
      destroy (self);
      return (NIL);
    }

  /* key object */
  KEY = bless (WString, key, klen);

  assert (KEY);

  /* key object */
  VALUE = bless (WString, value, vlen);

  assert (VALUE);


  return (self);
}


/**
 * @param _self WAnvl object instance
 *
 * WARC ANVL destructor
 */

WPRIVATE void * WAnvl_destructor (void * _self)
{

  struct WAnvl  * const self = _self;


  /* preconditions */
  CASSERT (self);

  /* free the key - value object */

  if (KEY)
    destroy (KEY), KEY = NIL;

  if (VALUE)
    destroy (VALUE), VALUE = NIL;


  return (self);
}


/**
 * WARC WAnvl class
 */

static const struct Class _WAnvl =
  {
    sizeof (struct WAnvl),
    SIGN,
    WAnvl_constructor, WAnvl_destructor
  };

const void * WAnvl = & _WAnvl;
