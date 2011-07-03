
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
#include <whash.h>   /* for class's prototypes */
#include "wmisc.h"   /* warc_bool_t ... */
#include "wmem.h"    /* w_malloc ... */
#include "wlist.h"   /* WList */
#include <wlist.x>
#include "wkv.h"     /* WKV */
#include <wcsafe.h>  /* computeHash, */


#define SIGN 16

/**
 * WARC WHash class internal
 */

typedef warc_bool_t (* hcomp_t) (const void *, const void *);
typedef void        (* hfree_t) (void *);


struct WHash
  {

    const void * class;

    /*@{*/
    void          * map; /**< Hash buckets array */
    warc_u32_t      size;  /* Hashtable size */
    warc_u32_t      hmask; /* Hashtable mask */
    /*     hcomp_t         cmp; /\* Comparison function *\/ */

    /*@}*/
  };


#define MAP           (self -> map)
#define SIZE          (self -> size)
#define HMASK         (self -> hmask)
/* #define CMP           (self -> cmp) */
#define BUCKET(index) (((int **) MAP) [ (index) ])

/* some global macros for hashing */
#define INIT_BITS	 10
#define INIT_SIZE	 (1UL << (INIT_BITS))
#define INIT_MASK	 ((INIT_SIZE) - 1)
#define MINIMUM_SIZE (1UL << 5)



/**
 * @param a object
 * @param b object
 * @return boolean
 *
 * Pointer comparison between 2 objects "a" and "b"
 */
/* WIPRIVATE warc_bool_t WHash_defaultCmp (const void * a, const void * b) */
/* { */
/*   return (a == b); */
/* } */


/**
 * @param _self WHash object
 * @param key pointer to any object
 * @param len "key" length
 *
 * @return pointer object
 *
 * Returns a pointer to the value associated with the "key"
 */

WIPUBLIC const void * WHash_get (const void * const _self,
                                 const void * key, warc_u32_t len)
{

  const struct WHash * const self   = _self;
  void               *       bucket = NIL;
  const void         *       obj    = NIL;
  /*   const void         *       value  = NIL; */
  void               *       node   = NIL;
  warc_u32_t                 hkey;
  warc_u32_t                 hkeyobj;
  warc_u32_t                 index;

  /* preconditions */
  CASSERT (self);

  if (key == NIL || len  == 0)
    {
      return (NIL);
    }

  hkey   = computeHash ( (const char *) key, len);

  index  = hkey & HMASK;

  bucket = BUCKET (index);
  unless (bucket) /* no object found */
  {
    return (NIL);
  }

  /* here the bucket must exist */
  node = WList_firstNode (bucket);

  while (node)
    {
      obj     = WList_getObjectFromNode (bucket, node);
      hkeyobj = WKV_key (obj);

      if (hkey == hkeyobj)
        {
          return (WKV_value (obj) );
        }

      node = WList_nextNode (bucket, node);
    }

  return (NIL);
}


/**
 * @param _self WHash object
 * @param key pointer to any object
 * @param len "key" length
 *
 * @return boolean
 *
 * WARC_FALSE if insertion success, WARC_TRUE otherwise
 */

WIPUBLIC warc_bool_t WHash_insert (const void * const _self,
                                   const void * key, warc_u32_t len,
                                   void * value)
{

  const struct WHash * const self   = _self;
  void               *       bucket = NIL;
  void               *       obj    = NIL;
  warc_u32_t                 hkey;
  warc_u32_t                 index;

  /* can't add object identical to the actual hash object */
  assert(self != key);

  /* test if the KEY already exists */
  if (WHash_get (self, key, len) )
    {
      w_fprintf (fprintf (stderr, "WHash error - key already exists\n" ) );
      return (WARC_TRUE);
    }

  /* preconditions */
  CASSERT (self);

  if (key == NIL || len  == 0)
    {
      return (WARC_TRUE);
    }

  hkey  = computeHash ( (const char *) key, len);

  index = hkey & HMASK;

  /* create the bucket if it doesn't exist */
  bucket = BUCKET (index);
  unless (bucket)
  {
    bucket = bless (WList);
    unless (bucket)
    {
      w_fprintf (fprintf (stderr, "WHash error - unable to create a new bucket\n" ) );
      return (WARC_TRUE);
    }

    BUCKET (index) = bucket;
  }


  /* here the bucket must exist */
  obj = bless (WKV, hkey, value);
  unless (obj)
  {
    w_fprintf (fprintf (stderr, "WHash error - unable to create a new WKV object\n" ) );
    return (WARC_TRUE);
  }

  if (WList_unshift (bucket, obj) )
    {
      return (WARC_FALSE);
    }

  return (WARC_FALSE);
}





/**
 * @param _self WHash object
 * @param key pointer to any object
 * @param len "key" length
 *
 * @return delete object
 *
 * Deletes and returns and object from the hash associated with "key"
 */

WIPUBLIC void * WHash_delete (const void * const _self,
                              const void * key, warc_u32_t len)
{

  const struct WHash * const self   = _self;
  void               *       bucket = NIL;
  const void         *       obj    = NIL;
  void               *       node   = NIL;
  warc_u32_t                 hkey;
  warc_u32_t                 hkeyobj;
  warc_u32_t                 index;

  /* preconditions */
  CASSERT (self);

  if (key == NIL || len  == 0)
    {
      return (NIL);
    }

  hkey   = computeHash ( (const char *) key, len);

  index  = hkey & HMASK;

  bucket = BUCKET (index);
  unless (bucket) /* no object found */
  {
    return (NIL);
  }

  /* here the bucket must exist */
  node = WList_firstNode (bucket);

  while (node)
    {
      obj     = WList_getObjectFromNode (bucket, node);
      hkeyobj = WKV_key (obj);

      if (hkey == hkeyobj)
        {
          return (WList_deleteNode (bucket, node) );
        }

      node = WList_nextNode (bucket, node);
    }

  return (NIL);
}



/**
 * WHash_constructor - create a new WHash object instance
 *
 * @param _self WHash class object
 * @param app constructor list parameters
 *
 * @return a valid WHash object or NIL
 *
 * WARC List constructor
 */

WPRIVATE void * WHash_constructor (void * _self, va_list * app)
{

  struct WHash * const self  = _self;
  const warc_u32_t     max   = va_arg (* app, const warc_u32_t);
  /*   hcomp_t              cmp   = va_arg (* app, hcomp_t); */
  warc_u32_t          i;

  /* preconditions */
  unless (max)
  {
    destroy (self);
    return (NIL);
  }

  if (max > MINIMUM_SIZE)
    {
      SIZE = 2 * max; /* double the max size */

      /* round it to a power of two */
      unless (isPowerOfTwoUInt (SIZE) )
      SIZE = roundToPowerOfTwoUInt (SIZE);

      HMASK = SIZE - 1;
    }

  else
    {
      /* use default value of 1024 buckets */
      SIZE  = INIT_SIZE;
      HMASK = INIT_MASK;
    }

  MAP = wmalloc (SIZE * sizeof (void *) );

  unless (MAP)
  {
    destroy (self);
    return (NIL);
  }

  /* empty all buckets */
  i = 0;

  while ( i < SIZE )
    {
      ( (warc_u32_t **) MAP) [ i ] = NIL;
      ++ i;
    }

  /* set comparison function */
  /*   CMP = cmp ? cmp : WHash_defaultCmp; */

  return (self);
}


/**
 * @param _self WHash object instance
 *
 * WARC ANVL destructor
 */

WPRIVATE void * WHash_destructor (void * _self)
{

  struct WHash  * const self = _self;
  void          * bucket     = NIL;
  warc_u32_t      i          = 0;


  /* preconditions */
  CASSERT (self);


  /* free all buckets */

  while ( i < SIZE )
    {
      if ( NIL != (bucket = BUCKET (i) ) )
        destroy (bucket), BUCKET (i) = NIL;

      ++ i;
    }

  if (MAP)
    wfree (MAP), MAP = NIL;

  /*   CMP = NIL; */

  return (self);
}


/**
 * WARC WHash class
 */

static const struct Class _WHash =
  {
    sizeof (struct WHash),
    SIGN,
    WHash_constructor, WHash_destructor
  };

const void * WHash = & _WHash;
