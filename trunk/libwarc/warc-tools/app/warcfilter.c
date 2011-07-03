
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

#define free_expr(u,m) \
do \
  {\
   if (u) destroy (u);\
   if (m) destroy (m);\
  }\
while (0)

warc_bool_t match (const warc_u8_t * bigstring, warc_u32_t   bigstringlen,
                   const warc_u8_t * pattern,   warc_u32_t   patternlen)
{
  unless (bigstring)
  return (WARC_TRUE);

  if (patternlen > bigstringlen)
    return (WARC_TRUE);

  unless (w_strcasestr (bigstring, pattern) )
  return (WARC_TRUE);

  return (WARC_FALSE);
}



int main (int argc, const char ** argv)
{
  void            * p        = NIL; /* WGetOpt object */
  void            * w        = NIL; /* warc file object */
  void            * r        = NIL; /* to recover records */
  void            * uexpr    = NIL; /* regular expression foruri filtering */
  void            * mexpr    = NIL; /* regular expression for mime filtering */
  const warc_u8_t * string   = NIL;
  warc_u8_t       * flags    = uS ("vf:u:m:r:t:");
  char            * fname    = NIL;
  char            * wdir     = ".";
  char            * uri      = NIL;
  char            * mime     = NIL;
  warc_rec_t        rtype    = WARC_UNKNOWN_RECORD;
  warc_i32_t        c        = 0;
  wfile_comp_t      cmode    = WARC_FILE_DETECT_COMPRESSION;
  warc_bool_t       amode    = WARC_FALSE;
  warc_u32_t        ret      = 0;


  if (argc < 5 || argc > 13)
    {
      fprintf (stderr, "Filter WARC records based on MIME, URI and record type\n");
      fprintf (stderr, "Usage: %s -f <file.warc> [-u <uri>] [-m <mime>] [-r <rtype>] [-v] [-t <working_dir>]\n", argv [0]);
      fprintf (stderr, "\t-f    : valid WARC file name\n");
      fprintf (stderr, "\t[-u]  : regular expression comparison with URI\n");
      fprintf (stderr, "\t[-m]  : regular expression comparison with MIME\n");
      fprintf (stderr, "\t[-r]  : regular expression comparison with record types (see \"public/wrectype.h\" for possible values)\n");
      fprintf (stderr, "\t[-v]  : dump ANVL (default no)\n");
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

          case 'u' :
            if (w_index (flags, c) [1] == ':')
              {
               uri = WGetOpt_argument (p);
               uexpr = bless (WRegexp, makeS (uri));
              }

            break;

          case 'm' :
            if (w_index (flags, c) [1] == ':')
              {
               mime = WGetOpt_argument (p);
               mexpr = bless (WRegexp, makeS (mime));
              }

            break;

          case 'r' :
            if (w_index (flags, c) [1] == ':')
              rtype = (warc_rec_t) atoi (WGetOpt_argument (p) );

            break;

          case 'v' :
            amode = WARC_TRUE;

            break;

          case 't' :
          
            if (w_index (flags, c) [1] == ':')
              wdir = WGetOpt_argument (p);
          
            break;

          case '?' :  /* illegal option or missing argument */
            destroy (p);
            free_expr (uexpr,mexpr);

            return (1);
        }
    }

  unless (fname)

  {
    fprintf (stderr, "missing WARC file name. Use -f option\n");
    destroy (p);
    free_expr (uexpr,mexpr);
    return (1);
  }

  w = bless (WFile, fname, WARC_MAX_SIZE,
             WARC_FILE_READER, cmode, wdir);
  assert (w);

  fprintf (stderr, "%-20s %-20s %-10s %-20s %-45s %-22s %-86s \n",
           "Offset", "CSize", "WarcId", "Content-Length",
           "WARC-Type", "WARC-Date",  "WARC-Record-ID");

  while (WFile_hasMoreRecords (w) )
    {
      warc_bool_t  m1   = WARC_FALSE;
      warc_bool_t  m2   = WARC_FALSE;
      warc_bool_t  m3   = WARC_FALSE;
      warc_u32_t   tlen = 0;

      unless ( (r = WFile_nextRecord (w) ) )
      {
        ret = 1;
        break;
      }

      if (uri != NIL && * uri != '\0' && uexpr)
        {
          string = WRecord_getTargetUri (r);
          if (string)
            m1 = WRegexp_search (uexpr, makeS (string));
          else
            m1 = WARC_FALSE;
        }

      if (mime != NIL && * mime != '\0' && mexpr)
        {
          string = WRecord_getContentType (r);
          if (string)
            m2     = WRegexp_search (mexpr, makeS (string));
          else
            m2 = WARC_FALSE;
        }

      if (rtype != WARC_UNKNOWN_RECORD && rtype == WRecord_getRecordType (r) )
        {
          m3 = WARC_TRUE;
        }

      /* no match in any field */

      if (!m1 && !m2 && !m3)
        {
          destroy (r);
          continue;
        }

      /* dump WRecord */

      /* mandatory fields */
      fprintf (stdout, "%-20llu ", (unsigned long long) WRecord_getOffset (r) );

      fprintf (stdout, "%-20llu ", (unsigned long long) WRecord_getCompressedSize (r) );

      fprintf (stdout, "%-10s ",   WRecord_getWarcId        (r) );

      fprintf (stdout, "%-20llu ", (long long unsigned int)   WRecord_getContentLength (r) );

      fprintf (stdout, "%-45u ",   WRecord_getRecordType    (r) );

      fprintf (stdout, "%-22s ",   WRecord_getDate  (r) );

      fprintf (stdout, "%-86s\n",   WRecord_getRecordId      (r) );

      /* optionnal fields */

      m1 = WARC_FALSE;
      fprintf (stdout, "More Fields:\n\n"   );

      string =  WRecord_getContentType (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "Content-Type",  string);
        m1 = WARC_TRUE;
        }

      string =  WRecord_getConcurrentTo (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "WARC-Concurrent-To",  string);
        m1 = WARC_TRUE;
        }

      string =  WRecord_getBlockDigest (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "WARC-Block-Digest",  string);
        m1 = WARC_TRUE;
        }

      string =  WRecord_getPayloadDigest (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "WARC-Payload-Digest", string);
        m1 = WARC_TRUE;
        }

      string =  WRecord_getIpAddress (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "WARC-IP-Address", string);
        m1 = WARC_TRUE;
        }

      string =  WRecord_getRefersTo (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "WARC-Refers-To", string);
        m1 = WARC_TRUE;
        }

      string =  WRecord_getTargetUri (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "WARC-Target-URI",  string);
        m1 = WARC_TRUE;
        }

      string =  WRecord_getTruncated (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "WARC-Truncated",  string);
        m1 = WARC_TRUE;
        }

      string =  WRecord_getWarcInfoId (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "WARC-Warcinfo-ID", string);
        m1 = WARC_TRUE;
        }

      string =  WRecord_getFilename (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "WARC-Filename", string);
        m1 = WARC_TRUE;
        }

      string =  WRecord_getProfile (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "WARC-Profile",  string);
        m1 = WARC_TRUE;
        }

      string =  WRecord_getPayloadType (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "WARC-Identified-Payload-type", string);
        m1 = WARC_TRUE;
        }

      string =  WRecord_getSegmentOriginId (r) ;
      if (string)
        {
        fprintf (stdout, "%-35s: %-20s\n", "WARC-Segment-Origin-ID",  string);

        fprintf (stdout, "%-35s: %-20d\n", "WARC-Segment-Number", WRecord_getSegmentNumber (r));
        m1 = WARC_TRUE;
        }

      tlen = WRecord_getSegTotalLength (r) ;
      if (tlen)
        {
        fprintf (stdout, "%-35s: %-20d\n", "WARC-Segment-Total-Length", tlen);
        m1 = WARC_TRUE;
        }

     unless (m1)
        fprintf (stdout, "-- No One --\n");

     fprintf (stdout, "\n");
     /* dump ANVLs */

      if (amode == WARC_TRUE)
        {
          warc_u32_t  i = 0;
          warc_u32_t  j = WRecord_getAnvlFieldsNumber (r); /* how many ANVL are there? */
          const warc_u8_t * key = NIL;
          const warc_u8_t * value = NIL;

          if (j)
            {
              fprintf (stdout, "-- More Info--\n\n");

              while (i < j)
                {
                  if (WRecord_getAnvlField (r, i, & key, & value))
                     break;

                  fprintf (stdout, "key: %s\n", (const char *) key );

                  /* we assume here that the ANVL value was in ASCII. */
                  /* use your own encoding to print it otherwise. */
                  fprintf (stdout, "val: %s\n", (const char *) value);

                  ++ i;
                }
            }
        }


      destroy (r);
    }

  destroy (p);

  destroy (w);
  free_expr (uexpr,mexpr);
  return (ret);
}
