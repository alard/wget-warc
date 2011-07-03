
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


#ifndef WARC_MAX_SIZE
/* 16 Go by default */
#define WARC_MAX_SIZE 17179869184ULL
#endif

#define uS(s)  ((warc_u8_t *) (s))
#define makeS(s) uS(s), w_strlen (uS(s))


/* unused callback */
warc_bool_t callback (void * env, const char* buff, const warc_u32_t size)
{
  UNUSED (env);
  UNUSED (size);
  UNUSED (buff);
  return (WARC_TRUE);
}

int main (int argc, const char ** argv)
{
  void           * p       = NIL;  /* WGetOpt object */
  void           * w       = NIL;  /* warc file object */
  void           * r       = NIL;  /* to recover records */
  warc_u32_t       ret     = 0;
  warc_i32_t       c       = 0;
  warc_u8_t      * flags   = (warc_u8_t *) "vf:t:";
  char           * fname   = NIL;
  char           * wdir    = ".";
  wfile_comp_t     cmode   = WARC_FILE_DETECT_COMPRESSION;
  warc_bool_t      verbose = WARC_FALSE;

  if (argc < 3 || argc > 7)
    {
      fprintf (stderr, "Check WARC file consistency\n");
      fprintf (stderr, "Usage: %s -f <file.warc> [-v] [-t <working_dir>]\n", argv [0]);
      fprintf (stderr, "\t-f    : valid WARC file name\n");
      fprintf (stderr, "\t[-v]  : verbose mode (default no)\n");
      fprintf (stderr, "\t[-t]  : temporary working directory (default \".\")\n");
      return (2);
    }



  p = bless (WGetOpt, makeS (flags) );

  assert (p);

  /* parse command line parameters */

  while ( (c = WGetOpt_parse (p, argc, argv) ) != -1)
    {
      switch (c)
        {

          case 'f' :

            if (w_index (flags, c) [1] == ':')
              fname = WGetOpt_argument (p);

            break;

          case 'v' :
            verbose = WARC_TRUE;

            break;

          case 't' :
          
            if (w_index (flags, c) [1] == ':')
              wdir = WGetOpt_argument (p);
          
            break;

          case '?' :  /* illegal option or missing argument */
            destroy (p);

            return (1);
        }
    }

  unless (fname)

  {
    fprintf (stderr, "missing WARC file name. Use -f option\n");
    destroy (p);
    return (1);
  }

  w = bless (WFile, fname, WARC_MAX_SIZE,
             WARC_FILE_READER, cmode, wdir);
  assert (w);

  while (WFile_hasMoreRecords (w) )
    {
      r = WFile_nextRecord (w);
      unless (r)
      {
        ret = 1;
        break;
      }

      WFile_register (w, r, callback, (void *) 0);
      WRecord_getContent (r);

      destroy (r);
    }

  destroy (p);

  destroy (w);

  if (verbose)
    {
      if (ret)
        fprintf (stdout, "invalid\n");
      else
        fprintf (stdout, "valid\n");
    }

  return (ret);
}
