
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
#include <wkv.h>     /* for class's prototypes */
#include "wmisc.h"   /* warc_bool_t ... */
#include <wcsafe.h>


#define SIGN 17

/**
 * WARC WKV class internal
 */


struct WKV
  {

    const void * class;

    /*@{*/
    warc_u32_t      hkey; /* hash key */
    void          * value; /* value object */
    /*@}*/
  };

#define HKEY         (self -> hkey)
#define VALUE        (self -> value)


/**
 * @param _self WKV object
 *
 * @return const pointer to value object
 *
 * Returns the WKV value object as a constant opaque pointer
 */

WIPUBLIC const void * WKV_value (const void * const _self)
{

  const struct WKV * const self   = _self;

  /* preconditions */
  CASSERT (self);

  return (VALUE);
}


/**
 * @param _self WKV object
 *
 * @return hash key
 *
 * Returns the WKV hash of the value object
 */

WIPUBLIC warc_u32_t WKV_key (const void * const _self)
{

  const struct WKV * const self   = _self;

  /* preconditions */
  CASSERT (self);

  return (HKEY);
}



/**
 * WKV_constructor - create a new WKV object instance
 *
 * @param _self WKV class object
 * @param app constructor list parameters
 *
 * @return a valid WKV object or NIL
 *
 * WARC List constructor
 */

WPRIVATE void * WKV_constructor (void * _self, va_list * app)
{

  struct WKV * const self  = _self;

  warc_u32_t hkey  = va_arg (* app, warc_u32_t);
  void *     value = va_arg (* app, void *);

  /* preconditions */
  assert (value);
  assert (self != value);

  HKEY  = hkey;
  VALUE = value;

  return (self);
}


/**
 * @param _self WKV object instance
 *
 * WARC ANVL destructor
 */

WPRIVATE void * WKV_destructor (void * _self)
{

  struct WKV  * const self = _self;

  /* preconditions */
  CASSERT (self);

  if (VALUE)
    destroy (VALUE), VALUE = NIL;

  return (self);
}


/**
 * WARC WKV class
 */

static const struct Class _WKV =
  {
    sizeof (struct WKV),
    SIGN,
    WKV_constructor, WKV_destructor
  };

const void * WKV = & _WKV;
