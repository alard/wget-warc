
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
  void           * s       = NIL; /* WServer object */
  void           * p       = NIL; /* WGetOpt object */
  warc_u8_t      * flags   = uS ("i:p:s:x:t:");
  warc_u8_t      * ip      = uS ("0.0.0.0");
  warc_u8_t      * ps      = NIL;
  warc_u8_t      * prefix  = NIL;
  char           * wdir    = ".";
  warc_u8_t      * sn      = uS ("iipc");
  warc_u64_t       port    = 0;
  warc_i32_t       c       = 0;

  if (argc < 7 || argc > 11)
    {
      fprintf (stderr, "Start the WARC Server\n");
      fprintf (stderr, "Usage: %s -i <ip> -p <port> -x <prefix> [-s <name>] [-t <working_dir>]\n", argv [0]);
      fprintf (stderr, "\t[-i]  : ip address\n");
      fprintf (stderr, "\t[-p]  : port number\n");
      fprintf (stderr, "\t[-x]  : directory prefix\n");
      fprintf (stderr, "\t[-s]  : server name (default to \"iipc\")\n");
      fprintf (stderr, "\t[-t]  : temporary working directory (default \".\")\n");
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

                if (w_atou ( ps, w_strlen (ps), & port) )
                  {
                    fprintf (stderr, "invalid port number: %s\n", ps);
                    free_obj (p, 2);
                  }
              }

            break;

          case 's' :

            if (w_index (flags, c) [1] == ':')
              sn = (warc_u8_t *) WGetOpt_argument (p);

            break;

          case 'x' :
            if (w_index (flags, c) [1] == ':')
              prefix = (warc_u8_t *) WGetOpt_argument (p);

            break;

          case 't' :
          
            if (w_index (flags, c) [1] == ':')
              wdir = WGetOpt_argument (p);
            
            break;

          case '?' :  /* illegal option or missing argument */
            free_obj (p, 3);
        }
    }

  if (ip == NIL || * ip == '\0')
    {
      fprintf (stderr, "error: empty IP address\n");
      free_obj (p, 4);
    }

  if (prefix == NIL)
    {
      fprintf (stderr, "error: WARC path prefix is mandatory\n"); 
      free_obj (p, 4);
    }

  s = bless (WServer, makeS (ip), port, makeS (sn), 
             makeS (prefix), makeS (wdir));

  unless (s)
  {
    fprintf (stderr, "Unable to start the server with IP address %s on the port %llu.\n", ip, (unsigned long long) port);

    free_obj (p, 5);
  }

  destroy (p);

  /* start the server (infinite loop) */
  fprintf (stderr, "Starting the WARC Server ...\n");

  if (WServer_start (s) )
    {
      free_obj (s, 6);
    }

  /* stop the server (via a REST request) */
  fprintf (stderr, "WARC Server stopped\n");

  WServer_stop (s);

  destroy (s);

  return (0);
}
