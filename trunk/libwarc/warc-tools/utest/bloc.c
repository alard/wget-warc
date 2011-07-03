
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


#include <Basic.h>
#include <Console.h>
#include <Automated.h>
#include <CUnit.h>
#include <CUError.h>
#include <TestDB.h>
#include <TestRun.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <menu.h>
#include <wcsafe.h>
#include <warc.h>
#include <wbloc.h>
#include <menu.h>

#define makeS(s) (warc_u8_t *) (s), w_strlen ((warc_u8_t *) (s))

#define WARC_MAX_SIZE 629145600LLU

int test1 (void)
{
  void * b = NIL;
  void * w = NIL;
  void * r = NIL;
  warc_u8_t * buff = NIL;
 /*  warc_u8_t   string [20];*/


  w = bless (WFile, "app/wdata/testwfile/onerec.warc", WARC_MAX_SIZE,  WARC_FILE_READER, WARC_FILE_DETECT_COMPRESSION, ".");
  assert (w);

  if (WFile_hasMoreRecords (w))
      { r = WFile_nextRecord (w);
        assert (r);
      }
  else
      {
       destroy (w);
       return 1;
      }

  b = bless (WBloc, w, r, 64 * 1024);
  assert (b);


  fprintf (stdout, "Http code is: %s\n\n", WBloc_getHttpCode(b));

  while (  (buff = WBloc_next (b)) != NIL)
      fprintf (stdout, "%s\n", buff);




  destroy (w);
  destroy (b);
  destroy (r);

return 0;

}


int main (void)
{
 
return test1 ();
}
