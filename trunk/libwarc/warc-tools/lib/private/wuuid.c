
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
#include <wmem.h>    /* wmalloc, wfree */
#include <wmisc.h>   /* unless, ... */
#include <wcsafe.h>  /* w_strncpy, ... */
#include <wuuid.h>   /* for class's prototypes */
#include <wstring.h> /* WString */

#include <tiger.h>   /* tiger hashe function */

#define SIGN 14

/**
 * WARC WUUID internal structure
 */

struct WUUID
  {

    const void * class;

    /*@{*/
    warc_u64_t * tiger;  /**< Tiger Hash 3 x 64 bits */
    char * buf;
    /*@}*/
  };


#define TIGER    (self -> tiger)
#define BUF      (self -> buf)


/**
 * @param _self WUUID object
 *
 * Compute a Tiger hash value (192 bits)
 */

WPUBLIC void WUUID_hash (const void * const _self,
                         const warc_u8_t * buf,
                         const warc_u64_t size)
{

  const struct WUUID * const self = _self;

  /* preconditions */
  CASSERT (self);

  /* compute the tiger hash */
  tiger ( (warc_u64_t *) buf, size, TIGER);

}


/**
 * @param _self WUUID object
 *
 * @return 384 bits hash value as a constant character string
 *
 * Returns a const char pointer representing the Tiger Hash
 */

WIPUBLIC const char * WUUID_text (const void * const _self)
{

  const struct WUUID * const self = _self;

  /* preconditions */
  CASSERT (self);

  w_snprintf (BUF, (2 * 3 * sizeof (warc_u64_t) ) + 5 + 1, "%s%08X%08X%08X%08X%08X%08X",
              "uuid:", /* 5 chars */
              (warc_u32_t) (TIGER [0] >> 32),
              (warc_u32_t) (TIGER [0]),
              (warc_u32_t) (TIGER [1] >> 32),
              (warc_u32_t) (TIGER [1]),
              (warc_u32_t) (TIGER [2] >> 32),
              (warc_u32_t) (TIGER [2]) );

  return (BUF);
}


/**
 * @param _self WUUID object
 * Zero fill the Tiger buffer (re-init)
 */

WIPUBLIC void WUUID_reinit (const void * const _self)
{

  const struct WUUID * const self = _self;

  /* preconditions */
  CASSERT (self);

  /* reinialize Tiger seeds for avalanche */
  TIGER [0] = 0x0123456789ABCDEFLL;
  TIGER [1] = 0xFEDCBA9876543210LL;
  TIGER [2] = 0xF096A5B4C3B2E187LL;
}


/**
 * WUUID_constructor - create a new WUUID object instance
 *
 * @param _self WUUID class object
 * @param app: constructor list parameters
 *
 * @return a valid WUUID object or NIL
 *
 * @brief WARC String constructor
 */

WPRIVATE void * WUUID_constructor (void * _self, va_list * app)
{

  struct WUUID     * const self = _self;

  UNUSED (app);

  /* allocate memory of 3 x 64 bits */
  TIGER = wmalloc (3 * sizeof (warc_u64_t) );
  unless (TIGER)
  {
    destroy (self);
    return (NIL);
  }

  /* allocate memory of 2 * 3 x 8 bits */
  BUF = wmalloc ( (2 * 3 * sizeof (warc_u64_t) ) + 5 + 1); /* 384 bits + "uuid:" + 1 */
  unless (BUF)
  {
    destroy (self);
    return (NIL);
  }

  /* zero fill the tiger buffer */
  WUUID_reinit (self);

  return (self);
}


/**
 * WUUID_destructor - delete an existing WUUID object
 *
 * @param _self WUUID object instance
 *
 * WARC String destructor
 */

WPRIVATE void * WUUID_destructor (void * _self)
{

  struct WUUID * self = _self;

  /* preconditions */
  CASSERT (self);


  if (TIGER)
    wfree (TIGER), TIGER = NIL;

  if (BUF)
    wfree (BUF), BUF = NIL;

  return (self);
}


/**
 * WARC WUUID class
 */

static const struct Class _WUUID =
  {
    sizeof (struct WUUID),
    SIGN,
    WUUID_constructor, WUUID_destructor
  };

const void * WUUID = & _WUUID;
