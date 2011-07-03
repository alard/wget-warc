
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


#include <stdio.h>
#include <string.h>


#include <warc.h>
#include <wmktmp.h>


#ifndef WARC_MAX_SIZE
/* 16 Go by default */
#define WARC_MAX_SIZE 17179869184ULL
#endif

#ifndef WARC_CHUNK_SIZE
#define WARC_CHUNK_SIZE 512
#endif

#define uS(s)  ((warc_u8_t *) (s))
#define makeS(s) uS(s), w_strlen (uS(s))


void printOrdinaryDate (warc_u8_t * field)
{
warc_u32_t i = 0;
warc_u32_t size = w_strlen (field);

fprintf (stderr, "\t");

while (i < size)
  {
   if (field[i] != ':' && field[i] != '-' && field[i] != 'Z' && field[i] != 'T' && field[i] != 't' && field[i] != 'z')
     fprintf (stderr, "%c", field [i]);
   i++;
  }
}


int main (int argc, const char ** argv)
{
  warc_u8_t      * off     = NIL;
  warc_u8_t      * fname   = NIL;
  warc_u8_t      * field   = NIL;
  warc_u8_t      * wdir    = uS(".");
  warc_u64_t       offset  = 0;
  warc_u8_t      * flags   = uS ("f:o:t:e");
  warc_u8_t        buffer [WARC_CHUNK_SIZE+1];
  warc_u8_t        http_code [4];
  warc_i32_t       c;
  wfile_comp_t     cmode   = WARC_FILE_DETECT_COMPRESSION;
  warc_bool_t      with_http = WARC_FALSE;
  warc_u32_t       rsize    = 0;
  warc_bool_t      stop = WARC_FALSE;

  void           * r = NIL;
  void           * w = NIL;
  void           * p = NIL;
  void           * tfile = NIL;

  FILE           * otfile = NIL;

  if (argc < 3 || argc > 7)
    {
      fprintf (stderr, "Extract WARC's content block only\n");
      fprintf (stderr, "Usage: %s -f <file.warc> <-o offset> [-e] [-t <working_dir>]\n", argv [0]);
      fprintf (stderr, "\t-f   : valid WARC file name\n");
      fprintf (stderr, "\t-o   : the offset of the record\n");
      fprintf (stderr, "\t[-t] : temporary working directory (default \".\")\n");
      fprintf (stderr, "\t[-e] : whether the payload content is after the HTTP response (default \"no\")\n");
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
              fname = uS(WGetOpt_argument (p));

            break;


          case 'o' :
          
            if (w_index (flags, c) [1] == ':')
              off = (warc_u8_t *) WGetOpt_argument (p);
               if (w_atou (off, w_strlen(off), & offset))
                 {
                   fprintf (stderr, "invalid offset number: %s\n", off);
                   destroy (p);
                   return (3);
                 }
          
            break;

          case 't' :
          
            if (w_index (flags, c) [1] == ':')
                wdir = uS(WGetOpt_argument (p));
          break;

          case 'e' :
              with_http = WARC_TRUE;
          break;

          case '?' :  /* illegal option or missing argument */
            destroy (p);

            return (1);
        }
    }

  unless (fname)

  {
    fprintf (stderr, "missing WARC file name. Use -f option\n");
    return (4);
  }

  w = bless (WFile, fname, WARC_MAX_SIZE,
             WARC_FILE_READER, cmode, wdir);

  unless (w)
   {
    fprintf (stderr, "Could not open WARC file %s\n", fname);
    return (5);
   }

destroy (p);

  if (WFile_seek (w, offset))
     {
       fprintf (stderr, "Could not reach the offset %llu\n", (unsigned long long) offset);
      destroy (w);
      return (9);
     }

  unless (WFile_hasMoreRecords (w))
      {
       fprintf (stderr, "End of file reached\n");
       destroy (w);
       return (6);
      }

  r = WFile_nextRecord (w);

  unless (r)
      {
      fprintf (stderr, "No valid record at this offset \n");
      destroy (w);
      return (7);
      }

  field = uS(WRecord_getTargetUri (r));

  unless (field)
      field = uS("unknown");
  else unless (w_strcmp (field, uS("")))
      field = uS("unknown");

  fprintf (stderr, "\"%s\"", field);


  field = NIL;

  field = uS(WRecord_getDate (r));

  printOrdinaryDate (field);

  field = NIL;

  field = uS(WRecord_getPayloadType (r));

  unless (field || (field != NIL && !w_strcmp (field, uS(""))))
     {
      field = uS(WRecord_getContentType (r));
      unless (field || (field != NIL && !w_strcmp (field, uS(""))))
           field = uS("unknown");
     }

  fprintf (stderr, "\t%s", field);
  field = NIL;

  otfile = WRecord_getBloc (r, w, with_http, http_code);

  unless (otfile)
      {
      fprintf (stderr, "A problem appeared while reading data \n");
       destroy (r);
       destroy (w);
       return (8);
      }

  fprintf (stderr, "\t%s", http_code);
  fprintf (stderr, "\t%d\n", (const warc_u32_t) (WRecord_getCompressedSize (r) + offset));

  tfile = WTempFile_handle (otfile);
  w_fseek_start (tfile);


  while (! stop)
       {
        rsize = w_fread (buffer, 1, WARC_CHUNK_SIZE, tfile);
        if (fwrite( buffer, 1, rsize, stdout) != rsize || ferror(stdout))
          {
           destroy (r);
           destroy (w);
           destroy (otfile);
           return (10);
          }

        if (rsize < WARC_CHUNK_SIZE)
           stop = WARC_TRUE;
       }

destroy (w);
destroy (otfile);
destroy (r);
return (0);
}
