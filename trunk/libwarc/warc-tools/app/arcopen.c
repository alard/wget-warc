
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
#include <string.h>

#include <warc.h>

#include <afile.h>
#include <arecord.h>


#ifndef WARC_MAX_SIZE
/* 16 Go by default */
#define WARC_MAX_SIZE 17179869184ULL
#endif


#define uS(s)  ((warc_u8_t *) (s))
#define makeS(s) uS(s), w_strlen (uS(s))
#define makeU(s) (const warc_u8_t *) (s), (warc_u64_t) w_strlen(uS(s))

/* useful macros to simplify releasing objects */
#define free_p \
  destroy (p)

#define free_a \
  destroy (a)


int main (int argc, const char ** argv)
{
  void           * p        = NIL; /* WGetOpt object */ 
  void           * a        = NIL; /* ARC file object */
  char           * aname    = NIL;
  afile_comp_t     amode    = ARC_FILE_DETECT_COMPRESSION;
  warc_i32_t       c        = 0;
  warc_u8_t      * flags    = uS ("a:");
  char           * wdir     = ".";

  if (argc != 3)
    {
      fprintf (stderr, "ARC to WARC convertor\n");
      fprintf (stderr, "Usage: %s -a <file.arc>\n",
               argv [0]);
      fprintf (stderr, "\t-a    : valid ARC file name\n");
      return (1);
    }

  p = bless (WGetOpt, makeS (flags) );
  assert (p);

  /* parse command line parameters */
  while ( (c = WGetOpt_parse (p, argc, argv) ) != -1)
    {
      switch (c)
        {
        case 'a' :
          if (w_index (flags, c) [1] == ':')
            aname = WGetOpt_argument (p);
          break;
          
        case '?' :  /* illegal option or missing argument */
          destroy (p);
          return (2);
        }
    }

  unless (aname)
  {
    fprintf (stderr, "missing ARC file name. Use -a option\n");
    destroy (p);
    return (3);
  }


  /* open an existing ARC file */
  a = bless (AFile, aname, amode, wdir);
  unless (a)
  {
    fprintf (stderr, "unable to create the Arc object\n");
    destroy (p);
    return (4);
  }

  destroy (p);

  if (AFile_hasMoreRecords (a))
    {
      void * ar = AFile_nextRecord (a);

      /* check the next ARC record */
      unless (ar)
      {
        fprintf (stderr, "corrupted ARC\n");
        destroy (a);
        return ( 5);
      }

      destroy (ar);
    }

  destroy (a);

  return (0);
}
