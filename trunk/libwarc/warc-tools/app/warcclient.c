
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

#define uS(s)  ((warc_u8_t *) (s))
#define makeS(s) uS(s), w_strlen (uS(s))

#define free_obj(x,ret) destroy (x);return (ret)

int main (int argc, const char ** argv)
{
  void           * s       = NIL; /* WClient object */
  void           * p       = NIL; /* WGetOpt object */
  warc_u8_t      * flags   = uS("i:p:s:o:u:n:d:rf:t:l:");
  warc_u8_t      * ip      = uS("0.0.0.0");
  warc_u8_t      * ps      = NIL;
  warc_u8_t      * off     = NIL;
  warc_u8_t      * format  = NIL;
  warc_u8_t      * wfn     = NIL;
  warc_u8_t      * target  = NIL;
  warc_u8_t      * server  = NIL;
  warc_u8_t      * pattern = uS ("");
  warc_u8_t        what [12];
  warc_bool_t      request = WARC_FALSE;
  warc_bool_t      filter  = WARC_FALSE;
  warc_u64_t       port    = 0;
  warc_u64_t       offset  = 0;
  warc_i32_t       c       = 0;

  if (argc <= 6)
    {
      fprintf (stderr, "WARC client to access remote WARC resources\n");
      fprintf (stderr, "Usage: %s -i <ip> -p <port> -t <remote_warc> -o <local_warc> -s <server_name> [-f <offset>] -[u <uri_pattern>] \
[-n <content_type_pattern>] [-d <record_type_string>] [-c] [-r] [-l <output format>]\n", argv [0]);
      fprintf (stderr, "\t[-i]  : ip address\n");
      fprintf (stderr, "\t[-p]  : port number\n");
      fprintf (stderr, "\t[-s]  : server name\n");
      fprintf (stderr, "\t[-u]  : filter file on Record uri value (default no)\n");
      fprintf (stderr, "\t[-n]  : filter file on content type value (default no) \n");
      fprintf (stderr, "\t[-d]  : filter file on Record type value (deefault unkown)\n");
      fprintf (stderr, "\t[-t]  : remote WARC filename\n");
      fprintf (stderr, "\t[-o]  : output WARC filename\n");
      fprintf (stderr, "\t[-f]  : WARC offset (default 0)\n");
      fprintf (stderr, "\t[-r]  : get all the WARC file (default no)\n");
      fprintf (stderr, "\t[-l]  : lists WARC's record using output formats {xml, json, text, html} (default no)\n");
      return (1);
    }


  /* parse command line parameters */
  p = bless (WGetOpt, makeS (flags) );
  assert (p);

  while ( (c = WGetOpt_parse (p, argc, argv) ) != -1)
    {
      switch (c)
        {
          case 'i' :
            if (w_index (flags, c) [1] == ':')
              ip = (warc_u8_t *) WGetOpt_argument (p);

            break;

          case 'p' :
           if (w_index (flags, c) [1] == ':')
             {
               ps = (warc_u8_t *) WGetOpt_argument (p);
               if (w_atou ( ps, w_strlen(ps), & port))
                 {
                   fprintf (stderr, "invalid port number: %s\n", ps);
                   free_obj(p, 2);
                 }
             }
            
            break;
            
        case 'o' :
          if (w_index (flags, c) [1] == ':')
            wfn = (warc_u8_t *) WGetOpt_argument (p);

          break;
          
        case 'f' :
          if (w_index (flags, c) [1] == ':')
            {
               off = (warc_u8_t *) WGetOpt_argument (p);
               if (w_atou (off, w_strlen(off), & offset))
                 {
                   fprintf (stderr, "invalid offset number: %s\n", off);
                   free_obj(p, 3);
                 }
             }
          
          break;

        case 'l' :
          if (w_index (flags, c) [1] == ':')
            {
               format = (warc_u8_t *) WGetOpt_argument (p);

             }
          
          break;

        case 's' :
          if (w_index (flags, c) [1] == ':')
             server = (warc_u8_t *) WGetOpt_argument (p);
          
          break;

        case 'u' :
          if (w_index (flags, c) [1] == ':')
             pattern = (warc_u8_t *) WGetOpt_argument (p);
             w_strncpy (what, makeS("uri"));
             filter = WARC_TRUE;
          
          break;


        case 'n' :
          if (w_index (flags, c) [1] == ':')
             pattern = (warc_u8_t *) WGetOpt_argument (p);
             w_strncpy (what, makeS("contenttype"));
             filter = WARC_TRUE;
          
          break;

        case 'd' :
          if (w_index (flags, c) [1] == ':')
             pattern = (warc_u8_t *) WGetOpt_argument (p);
             w_strncpy (what, makeS("recordtype"));
             filter = WARC_TRUE;
          
          break;

        case 't' :
          if (w_index (flags, c) [1] == ':')
            target = (warc_u8_t *) WGetOpt_argument (p);

          break;

        case 'r' :
          request = WARC_TRUE;
          break;


        case '?' :  /* illegal option or missing argument */
          free_obj(p, 4);
        }
    }
  

  if (ip == NIL || * ip == '\0')
    {
      fprintf (stderr, "error: empty IP address\n");
      free_obj(p, 5);
    }

 if (wfn == NIL || * wfn == '\0')
    {
      fprintf (stderr, "error: empty output WARC filename\n");
      free_obj(p, 6);
    }

 if (target == NIL || * target == '\0')
    {
      fprintf (stderr, "error: empty input WARC filename\n");
      free_obj(p, 6);
    }

 if (server == NIL || * server == '\0')
    {
      fprintf (stderr, "error: empty server name\n");
      free_obj(p, 6);
    }

  s = bless (WClient, makeS(ip), port, makeS (server));
  unless (s)
    {
     fprintf (stderr, "Unable to create the WARC client using the IP address %s on the port %llu.\n", ip, (unsigned long long) port);
     free_obj(p, 7);
    }
  
  destroy (p);

  if (request)
    {
     if (filter || format)
       {
        fprintf (stderr, "Options conflict: can not send two requests once \n");
        free_obj (s,10);
       }
     else
       if (WClient_getWFile (s, offset, makeS(target), wfn))
         {
           fprintf (stderr, "WClient_getWFile error : request not satisfied\n");
           free_obj(s, 8);
         }
    }
  else if (filter)
      {
        if (request || format)
          {
           fprintf (stderr, "Options conflict: can not send two requests once \n");
           free_obj (s,10);
          }
        else
          {
           if (WClient_getFiltredWFile (s, offset, makeS (what), makeS (pattern), makeS (target), wfn))
             {
              fprintf (stderr, "WClient_getFiltredWFile error: request not satisfied\n");
              free_obj(s, 11);
             }
          }
      }
  else if (format)
      {
        if (request || filter)
          {
           fprintf (stderr, "Options conflict: can not send two requests once \n");
           free_obj (s,10);
          }
        else
          {
           if (WClient_getList (s, offset, makeS (format), makeS (target), wfn))
             {
              fprintf (stderr, "WClient_getList error: request not satisfied\n");
              free_obj(s, 11);
             }
          }
      }
    else
      {
        if (WClient_getWRecord (s, offset, makeS(target), wfn))
          {
            fprintf (stderr, "WClient_getWRecord error: request not satisfied\n");
            free_obj(s, 9);
          }
      }

  destroy (s);
  
  return (0);
}
