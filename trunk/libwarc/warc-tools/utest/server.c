
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

#include <assert.h>
#include <stdio.h>

#include <warc.h>

#define makeS(s) ((warc_u8_t *) s), w_strlen ((warc_u8_t *) (s))
int test1 (void)
{

  void * s = bless (WServer, makeS ("0.0.0.0"), 8080,
                    makeS ("iipc"), makeS ("."), makeS ("./") );

  assert (s);

  fprintf (stderr, "Starting the WARC Server ...\n");

  if (WServer_start (s) )
    {
      destroy (s);
      return (1);
    }

  fprintf (stderr, "WARC Server stopped\n");

  WServer_stop (s);

  destroy (s);

  return 0;
}


int main (void)
{
  int (* tests []) () = {test1 };

  warc_u32_t  i      = 0;

  for (i = 0; i < ARRAY_LEN (tests); ++ i)
    {
      tests[i] ();
    }

  return 0;
}
