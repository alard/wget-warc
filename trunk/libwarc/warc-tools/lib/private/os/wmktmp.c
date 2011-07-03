
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
#include <wmem.h>    /* wmalloc, wfree */
#include <wmktmp.h>  /* default class */
#include <wcsafe.h>


/**
 * WARC WTempFile internal structure
 */

#define SIGN 13

struct WTempFile
  {

    const void * class;

    /*@{*/
    FILE * handle; /**< temporary file handle */
    /*@}*/
  };


#define HANDLE  (self -> handle)




/**
 * @param _self  a temporary object
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
 * @brief WARC Temporary File object constructor
 */

WPRIVATE void * WTempFile_constructor (void * _self, va_list * app)
{
#define  TEMPLATE_NAME  "w17XXXXXX"

  struct WTempFile * const self        = _self;

  warc_u8_t        * template = NIL;
  const warc_u8_t  * dir      = va_arg (* app, const warc_u8_t *);
  const warc_u32_t   dirlen   = va_arg (* app, const warc_u32_t);
  warc_i32_t         tempsize;
  warc_i32_t         wfd;
  mode_t             old_mode;
   
  tempsize = w_strlen((const warc_u8_t *) TEMPLATE_NAME);
  template = wmalloc (sizeof (char) * (dirlen + tempsize + 2));
  unless (template)
    {
      destroy (self);
      return (NIL);
    }
  w_strncpy(template, dir, dirlen);
  w_strncpy(template + dirlen, (const warc_u8_t *) "/" , (warc_i32_t) 1 );
  w_strncpy(template + dirlen + 1 , (const warc_u8_t *) TEMPLATE_NAME, tempsize);
  template [dirlen + 1 + tempsize] = '\0';
  /* create file with restrictive permissions */
  old_mode = umask (077);

  if ( (wfd = mkstemp ((char *) template) ) < 0)
    {
      wfree   (template);
      destroy (self);
      return  (NIL);
    }
  umask (old_mode);

  if ( (HANDLE = w_fdopen_wb (wfd) ) == NIL || unlink ((char *) template) < 0)
    {
      close   (wfd);
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
 * @brief WARC Temporary File object destructor
 */

WPRIVATE void * WTempFile_destructor (void * _self)
{

  struct WTempFile * self = _self;

  /* preconditions */
  CASSERT (self);

  if (HANDLE)
    fclose (HANDLE), HANDLE = NIL;

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
