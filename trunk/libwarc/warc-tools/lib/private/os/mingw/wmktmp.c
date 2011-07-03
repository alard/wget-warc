
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

#include <wcsafe.h>

/*
 * WARC default headers
 */

#include <wclass.h>  /* bless, destroy, cassert, struct Class */
#include <wsign.h>   /* CASSET macro */
#include <wmem.h>    /* wmalloc, wfree */
#include <wmisc.h>   /* unless, ... */
#include <wmem.h>    /* wmalloc, wfree */
#include <wstring.h> /* WString */
#include <wmktmp.h>  /* default class */


/*
 * WARC WTempFile internal structure
 */

#define SIGN 13

struct WTempFile
  {

    const void * class;

    /*@{*/
    FILE * handle; /**< temporary file handle */
    void * template; /**< temporary file name */
    /*@}*/
  };


#define HANDLE    (self -> handle)
#define TEMPLATE  (self -> template)



/**
 * @param _self a temporary object
 *
 * @return file handle descriptor, NIL otherwise
 *
 * Returns the temporary file handle descriptor
 */

WIPUBLIC FILE * WTempFile_handle  (const void * const _self)
{

  const struct WTempFile * const self = _self;

  assert (self);

  return (HANDLE);
}



/**
 * @param _self  a temporary object
 *
 * @return void
 *
 * Seek to offset 0
 */

WIPUBLIC void WTempFile_reset (const void * const _self)
{

  const struct WTempFile * const self = _self;

  assert (self);

  w_fflush (HANDLE);
  w_ftruncate(w_fileno (HANDLE), 0);
  w_fseek_start (HANDLE);
}


/**
 * WTempFile_constructor - create a new WTempFile object instance
 *
 * @param _self WTempFile class object
 * @param app constructor list parameters
 *
 * @return a valid WTempFile object or NIL
 *
 * @brief WARC String constructor
 */

WPRIVATE void * WTempFile_constructor (void * _self, va_list * app)
{
#define  TEMPLATE_NAME  "w17XXXXXX"

  struct WTempFile * const self  = _self;
  warc_u8_t        * template    = NIL;
  const warc_u8_t  * dir         = va_arg (* app, const warc_u8_t *);
  const warc_u32_t   dirlen      = va_arg (* app, const warc_u32_t);
  warc_i32_t         tempsize;


  tempsize = w_strlen((const warc_u8_t *) TEMPLATE_NAME);
  template = wmalloc (sizeof (char) * (dirlen + tempsize + 1));
  unless (template)
    {
      destroy (self);
      return (NIL);
    }

  w_strncpy(template, dir, dirlen);
  w_strncpy(template + dirlen, (const warc_u8_t *) TEMPLATE_NAME, tempsize);
  tempsize += dirlen;
  template [tempsize] = '\0';

  if (mktemp ((char *) template) == NIL)
    {
      wfree   (template);
      destroy (self);
      return (NIL);
    }

  TEMPLATE = bless (WString, template, tempsize);
  unless (TEMPLATE)
  {
    wfree   (template);
    destroy (self);
    return  (NIL);
  }

  HANDLE = w_fopen_wb ((char *) template);
  unless (HANDLE)
  {
    wfree   (template);
    destroy (self);
    return  (NIL);
  }

  wfree (template);

  return (self);
}


/**
 * WTempFile_destructor - delete an existing WTempFile object
 *
 * @param _self: WTempFile object instance
 *
 * @brief WARC String destructor
 */

WPRIVATE void * WTempFile_destructor (void * _self)
{

  struct WTempFile * self = _self;

  /* preconditions */
  CASSERT (self);

  if (HANDLE)
    w_fclose (HANDLE);

  unlink ( (const char *) WString_getText (TEMPLATE) );

  if (TEMPLATE)
    destroy (TEMPLATE), TEMPLATE = NIL;

  return (self);
}


/**
 * WARC WTempFile class
 */

static const struct Class _WTempFile =
{
  sizeof (struct WTempFile),
  SIGN,
  WTempFile_constructor, WTempFile_destructor
};

const void * WTempFile = & _WTempFile;


/* WPUBLIC FILE	* wmktemp (void) */
/* { */
/*   char    template [] = TEMPLATE_NAME; */
/*   char  * sfn	 = NIL;	*/

/*   sfn = tempnam("", template); */
/*   unless (sfn) */
/*     { */
/*	 return	(NIL); */
/*     } */

/*   return (fopen (sfn, "w+")); */
/* } */
