
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
 * C default headers
 */

#include <assert.h>  /* assert */
#include <stdlib.h>  /* abort */

/*
 * WARC default headers
 */

#include <wclass.h>  /* for class's prototypes */
#include <wmem.h>    /* wmalloc, wfree, wcalloc */
#include <wmisc.h>   /* WPUBLIC, WIPUBLIC */


/**
 * bless - polymorphic object constructor
 *
 * @param _class Class object to create
 *
 * @return a new created object
 *
 * WARC class creator (mimics C++ new)
 */

WPUBLIC void * bless (const void * _class, ...)
{

  const struct Class * class = _class;
  void * p = wcalloc (1, class -> size);

  assert (p);

  * (const struct Class **) p = class;

  if (class -> constructor)
    {
      va_list ap;
      va_start (ap, _class);

      p = class -> constructor (p, & ap);
      va_end (ap);
    }

  return (p);
}


/**
 * destroy - polymorphic object constructor
 *
 * @param self object instance to release
 *
 * @return none
 *
 * WARC class destrcutor (mimics C++ delete)
 */

WIPUBLIC void destroy (void * self)
{

  const struct Class ** cp = self;

  if (self && * cp && (* cp) -> destructor)
    {
      self = (* cp) -> destructor (self);
    }

  wfree (self);
}


/**
 * cassert - check if the object has the correct class signature
 *
 * @param _self object instance
 * @param sign class signature
 *
 * @return none
 *
 * Class consistency checker
 */

WIPUBLIC void cassert (void * const self, const warc_u32_t sign)
{

  const struct Class ** cp = self;

  assert (self);
  assert (* cp);

  if (sign != (* cp) -> sign)
    abort ();
}
