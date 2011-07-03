
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
#include <wlist.h>   /* for class's prototypes */
#include <wlist.x>
#include <wmem.h>    /* wmalloc, wfree */
#include "wmisc.h"   /* warc_bool_t ... */
#include <wcsafe.h>


#define SIGN 2

/**
 * WARC WLNode internal structure
 */

struct WLNode
  {
    /*@{*/
    void          * object; /**< object node */

    struct WLNode * next; /**< next object */

    struct WLNode * prev; /**< previous object */
    /*@}*/
  };


#define MAKE_LNODE \
  (wmalloc (sizeof (struct WLNode)))

#define DELETE_LNODE(n) \
  (wfree (n), n = NIL)



/**
 * WARC WList class internal
 */


struct WList
  {

    const void * class;

    /*@{*/

    struct WLNode  * dummy; /**< dummy list node */
    warc_u32_t       cnt; /**<  how many objects are there */
    /*@}*/
  };


#define DUMMY    (self -> dummy)
#define CNT      (self -> cnt)



/**
 * @param _self A WList object
 * @param o an object
 *
 * @return false if the operation succeeds, true if not
 *
 * WList : Treats the list as a stack, and pushes the
 * object onto the end of the list.
 */

WPUBLIC warc_bool_t WList_push (void * const _self, void * const o)
{

  struct WList  * const self = _self;

  struct WLNode * n          = NIL;

  /* preconditions */
  CASSERT (self);
  assert (o);

  /* can't add object identical to the container */
  assert(self != o);

  /* create a new list node for the object "o" */
  n = MAKE_LNODE;
  unless (n)
  return (WARC_TRUE);

  /* store the reference to "o" */
  n -> object = o;

  /* add the node to the end of list  */
  DUMMY -> prev -> next = n;
  n -> prev             = DUMMY -> prev;
  n -> next             = DUMMY;
  DUMMY -> prev         = n;

  /* reset list's counter */
  ++ CNT;

  return (WARC_FALSE);
}



/**
 * @param _self a WList object
 *
 * @return a pointer object
 *
 * WList : Pops and returns the last object of the list,
 * shortening the list by one element.
 */

WPUBLIC void * WList_pop (void * const _self)
{

  struct WList  * const self = _self;

  struct WLNode * n          = NIL;
  void          * o          = NIL;

  /* preconditions */
  CASSERT (self);
  unless (CNT)
  return (NIL);

  /* get the last object */
  n                 = DUMMY -> prev;
  DUMMY -> prev     = n -> prev;

  n -> prev -> next = n -> next;
  o                 = n -> object;

  /* delete the list node */
  DELETE_LNODE (n);

  /* reset list's counter */
  -- CNT;

  return (o);
}



/**
 * @param _self a WList object
 *
 * @return a pointer object
 *
 * WList : Shifts the first object of the list off and
 * returns it, shortening the list by 1 and moving everything down.
 */

WPUBLIC void * WList_shift (void * const _self)
{

  struct WList  * const self = _self;

  struct WLNode * n          = NIL;
  void          * o          = NIL;

  /* preconditions */
  CASSERT (self);
  unless (CNT)
  return (NIL);

  /* get the first object */
  n                 = DUMMY -> next;
  o                 = n -> object;

  DUMMY -> next     = n -> next;
  n -> next -> prev = DUMMY;


  /* delete the list node */
  DELETE_LNODE (n);

  /* reset list's counter */
  -- CNT;

  return (o);
}


/**
 * @param _self a WList object
 *
 * @return false if the operation succeeds, true if not
 *
 * WList : Does the opposite of a "shift". Prepends the object to
 * the front of the list.
 */

WPUBLIC warc_bool_t WList_unshift (void * const _self, void * const o)
{

  struct WList  * const self = _self;

  struct WLNode * n          = NIL;

  /* preconditions */
  CASSERT (self);
  assert (o);

  /* can't add object identical to the container */
  assert(self != o);

  /* create a new list node for the object "o" */
  n = MAKE_LNODE;
  unless (n)
  return (WARC_TRUE);

  /* store the reference to "o" */
  n -> object = o;

  /* add the node to the front of list  */
  DUMMY -> next -> prev = n;
  n -> next     = DUMMY -> next;
  n -> prev     = DUMMY;
  DUMMY -> next = n;

  /* reset list's counter */
  ++ CNT;

  return (WARC_FALSE);
}


/**
 * @param _self a WList object
 *
 * @return how many objects
 *
 * Returns the list size
 */

WIPUBLIC warc_u32_t WList_size (const void * const _self)
{

  const struct WList * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (CNT);
}

WPUBLIC const void * WList_getObjectFromNode (const void * const _self,
    void * _node)
{

  const struct WList * const self = _self;

  struct WLNode            * node = _node;

  /* empty list */
  CASSERT (self);
  assert (node);
  unless (CNT)
  return (NIL);

  return (node -> object);
}

WPUBLIC void * WList_nextNode (const void * const _self,
                               void * _node)
{

  const struct WList * const self = _self;

  struct WLNode            * node = _node;

  /* empty list */
  CASSERT (self);
  assert (node);
  unless (CNT)
  return (NIL);

  return (node -> next);
}

WPUBLIC void * WList_firstNode (const void * const _self)
{

  const struct WList * const self = _self;

  /* empty list */
  CASSERT (self);
  unless (CNT)
  return (NIL);

  return (DUMMY -> next);
}

WPUBLIC void * WList_deleteNode (void * const _self, void * _node)
{

  struct WList * const self = _self;

  struct WLNode            * n    = _node;
  void                     * o    = NIL;

  /* empty list */
  CASSERT (self);
  assert (n);
  unless (CNT)
  return (NIL);

  /* return the object and free its node */
  o                 = n -> object;
  n -> next -> prev = n -> prev;
  n -> prev -> next = n -> next;
  DELETE_LNODE (n);

  -- CNT;

  return (o);
}

/**
 * @param _self a WList object
 * @param index index position in the list object (starting from 0)
 *
 * @return an object if succeeds, otherwise NIL
 *
 * Returns a constant reference to the object at index "i" in the list.
 * NIL is returned if empty list.
 */

WPRIVATE struct WLNode * gotoIndex (const void * const _self,
                                          const warc_u32_t index)
  {

    const struct WList * const self = _self;

    struct WLNode            * node;
    warc_u32_t                 i;

    /* empty list */
    unless (CNT)
    return (NIL);

    if (index > CNT)
      return (NIL);

    /* start from the first node */
    i    = 0;

    node = DUMMY -> next;

    while (i < index)
      {
        node = node -> next;
        ++ i;
      }

    return (node);
  }


/**
 * @param _self A WList object
 * @param index index position in the list object (starting from 0)
 *
 * @return an object if succeeds, otherwise NIL
 *
 * Returns a constant reference to the object at index "i" in the list.
 * NIL is returned if empty list.
 */

WPUBLIC const void * WList_getElement (const void * const _self,
                                const warc_u32_t index)
{

  const struct WList * const self = _self;

  struct WLNode            * n;

  /* preconditions */
  CASSERT (self);

  /* position node on the target node */
  n = gotoIndex (self, index);
  unless (n)
  return (NIL);

  return (n -> object);
}


/**
 * @param _self a WList object
 * @param index index position in the list object (starting from 0)
 * @param o a constant reference to an object
 *
 * @return a reference to the old object at index
 *
 * Returns the object at index "index" and replace it by object "o".
 * NIL is returned if empty list.
 */

WPUBLIC void * WList_setElement (void * const _self, const warc_u32_t index,
                          void * const o)
{

  struct WList * const self = _self;

  struct WLNode      * n;
  void               * old;

  /* preconditions */
  CASSERT (self);
  assert (o);

  /* can't add object identical to the container */
  assert(self != o);

  /* position node on the target node */
  n = gotoIndex (self, index);
  unless (n)
  return (NIL);

  /* save the new object and return the old one */
  old         = n -> object;
  n -> object = o;

  return (old);
}


/**
 * @param _self a WList object
 * @param index index position in the list object (starting from 0)
 * @param o a constant reference to an object
 *
 * @return a reference to the old object at index
 *
 * Returns the object at index "index" and remove it from the list.
 * NIL is returned if empty list.
 */

WPUBLIC void * WList_remove (void * const _self, const warc_u32_t index)
{

  struct WList * const self = _self;

  struct WLNode      * n;
  void               * o;

  /* preconditions */
  CASSERT (self);

  /* position node on the target node */
  n = gotoIndex (self, index);
  unless (n)
  return (NIL);

  /* return the object and free its node */
  o                 = n -> object;
  n -> next -> prev = n -> prev;
  n -> prev -> next = n -> next;
  DELETE_LNODE (n);

  return (o);
}


/**
 * WList_constructor - create a new WList object instance
 *
 * @param _self WList class object
 * @param app constructor list parameters
 *
 * @return a valid WList object or NIL
 *
 * WARC List constructor
 */

WPRIVATE void * WList_constructor (void * _self, va_list * app)
{

  struct WList * const self = _self;

  UNUSED (app);

  /* create dummy head node */
  DUMMY = MAKE_LNODE;
  unless (DUMMY)
  return (NIL);

  /* set object's counter */
  CNT = 0;

  /* link prev and next to dummy */
  DUMMY -> prev = DUMMY -> next = DUMMY;

  return (self);
}


/**
 * @param _self WList object instance
 *
 * WARC List destructor
 */

WPRIVATE void * WList_destructor (void * _self)
{

  struct WList  * const self = _self;

  struct WLNode       * n;

  /* preconditions */
  CASSERT (self);

  /* destory all objects in the list */
  n = DUMMY -> next;

  while (n != DUMMY)
    {
      n = n -> next;
      destroy (n -> prev -> object);
      DELETE_LNODE (n -> prev);
    }

  CNT = 0;

  /* free the dummy node */

  if (DUMMY)
    DELETE_LNODE (DUMMY);

  return (self);
}


/**
 * WARC WList class
 */

static const struct Class _WList =
  {
    sizeof (struct WList),
    SIGN,
    WList_constructor, WList_destructor
  };

const void * WList = & _WList;
