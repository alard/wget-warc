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
  Here is two foctions to recover an anvl field key and value from a wWarc Record
  the Anvl field is identified by its rank

*/

#include <warc.h> 


WPUBLIC const warc_u8_t * getFieldKey (void * r, warc_u32_t rank)
{
  const warc_u8_t * key = NIL;
  const warc_u8_t * value = NIL;

  unless (r)
    return (NIL);

  if (WRecord_getAnvlField (r, rank, &key, &value))
    return (NIL);

  return (key);
}

WPUBLIC const warc_u8_t * getFieldValue (void * r, warc_u32_t rank)
{
  const warc_u8_t * key = NIL;
  const warc_u8_t * value = NIL;

  unless (r)
    return (NIL);

  if (WRecord_getAnvlField (r, rank, &key, &value))
    return (NIL);

  return (value);
}
