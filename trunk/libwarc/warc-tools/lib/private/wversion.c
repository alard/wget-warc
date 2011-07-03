
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

#include <wversion.h>
#include <wmisc.h>
#include <wcsafe.h>  /* w_strcmp, ... */

#define makeUS(s) ((const warc_u8_t *) s)

const warc_u8_t * WARC_COMPATIBLE_VERSIONS [] = 
  { makeUS(WARC_VERSION), makeUS("WARC/0.17"), 0 };


WPUBLIC warc_bool_t w_checkCompatibleVersions (const warc_u8_t * id)
{
  const warc_u8_t ** p = WARC_COMPATIBLE_VERSIONS;

  /* preconditions */
  assert (id);

  while (* p)
    {
      if (w_strcmp(id, * p) == 0)
        return (WARC_FALSE);

      p ++;
    }

  return (WARC_TRUE);
}
