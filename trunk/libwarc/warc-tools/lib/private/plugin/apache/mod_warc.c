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


#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "ap_config.h"
#include <http_log.h>
#include <apr_strings.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <warc.h>



#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "ap_config.h"
#include <warc.h>
#include <stdio.h>
#include <string.h>
#include <string.h>
#include <stdlib.h>
#include <wmem.h>     /* wmalloc, wfree */

#define PTR_OR(a, b) (((a) != NULL) ? (a) : (b))

#define uS(s) (warc_u8_t *)(s)
#define makeS(s) uS(s),w_strlen(uS(s))
#define makeWString(s) bless (WString, uS(s), w_strlen((uS(s))))

#ifndef WARC_MAX_SIZE
#define WARC_MAX_SIZE 629145600
#define MAX_BUFFER_SIZE 512
#endif

module AP_MODULE_DECLARE_DATA warc_module;

typedef struct {

  const char * warc_tmp_dir;
               } warc_dir_config;

char dname[100]; /* buffer who contains a name of a warc directorie */



/**
 * @param str: sample string
 
 * 
 * @return True if the string is digital value, False otherwise
 *
 * cheek the offset record parameter of the uri
 **/

WPRIVATE warc_bool_t strIsDigit (const char * str)
{
     warc_u32_t strsize = strlen (str);
     warc_u32_t i = 0;

     for (i =0; i< strsize; i++)
       {
         if (! isdigit (str [i]))
            return WARC_FALSE;
       }

      return WARC_TRUE;
}

/**
 * @param str: sample string
 * @param position: intger 
 * 
 * @return The actual Uri Field
 *
 * return the field of uri at the actual position
 **/

WPRIVATE void  * recoverUriField ( const char * uri,warc_u32_t * position)
{

  void * item = bless (WString, "", 0);  /* the return item */
  warc_u8_t achar[2] = {' ', '\0'};     /* to append item characters */
  warc_u32_t i = * (position);


  while ( uri [i] == '/')
        i ++ ;
  
  while ( uri [i] != '/')
        {
          achar [0] = uri [i];
          if (WString_append (item, achar, 1))
             {
               destroy (item);
               return (OK);
             }
          i ++;
        }
  * (position) = i ;

  return item;
}


/**
 * @param str: sample string
 * @param position: intger 
 * 
 * @return The file name from uri
 *
 * return the file name (the last field of uri)
 **/

WPRIVATE void  * recoverFName ( const char * uri,warc_u32_t * position)
{

  void * item = bless (WString, "", 0);  /* the return item */
  warc_u8_t achar[2] = {' ', '\0'};     /* to append item characters */
  warc_u32_t i = * (position);
  warc_u32_t  k; 


  while ( uri [i] == '/')
        i ++ ;

  for (k = i; k < strlen (uri ); k++)
      {
      achar [0] = uri [k];
      if (WString_append (item, achar, 1))
             {
               destroy (item);
               return (OK);
             }
      
      }
  * (position) = i ;

  return item;
}

/**
 * @param fname: the name of the wanted file
 * @param prefix: the directory leading to the file location
 * @param offset: the offset from where the data will be sent
 * @param r: a request object
 *
 * @return False if succeed, True otherwise
 *
 * Sends a full file response to the client
 */

WPRIVATE warc_bool_t WSend_fullResponse (const warc_u8_t * fname, const warc_u8_t * prefix, warc_u32_t offset, request_rec * r)
{
   FILE          *   f        = NIL;
   warc_u8_t     *   buffer   = NIL;
   wfile_comp_t      cmode    = WARC_FILE_UNCOMPRESSED;
   warc_u32_t        fsize    = 0  ;
   warc_u32_t        filled   = 0  ;
   warc_u32_t        sentsize = 0  ;
   void          *   w        = 0  ;


  buffer = wmalloc (MAX_BUFFER_SIZE);
  
  unless (buffer)
    {
    r->content_type = "text/html";
    ap_rprintf (r, "<font color=red size = 4>Failed in allocation of memory for respponse sending </font><br>");
    return (WARC_TRUE);
    }

  f = w_fopen_rb (fname);

  unless (f)
    {
    r->content_type = "text/html";
    ap_rprintf (r, "<font color=red size = 4>File not found</font><br>");
    wfree (buffer);
    return (WARC_TRUE);
    }

    w = bless (WFile, fname, WARC_MAX_SIZE,
               WARC_FILE_READER, WARC_FILE_DETECT_COMPRESSION, prefix);


    unless (w)
      {
      r->content_type = "text/html";
      ap_rprintf (r, "<font color=red size = 4>File not found</font><br>");
      w_fclose (f);
      return (WARC_TRUE);
      }

   cmode = WFile_getCompressionMode (w);

   destroy (w);

      if (cmode == WARC_FILE_COMPRESSED_GZIP)
          ap_set_content_type (r, "application/x-gzip");
      else
          ap_set_content_type (r, "text/random");


  w_file_size_from_offset (f, fsize, offset);
  w_fseek_from_start (f, offset);


  while (fsize)
    {
     if (fsize > MAX_BUFFER_SIZE)
        {
        sentsize = MAX_BUFFER_SIZE;
        fsize = fsize - MAX_BUFFER_SIZE;
        }
     else
        {
        sentsize = fsize;
        fsize = 0;
        }
    filled = w_fread (buffer, 1, sentsize, f);
    ap_rwrite ((const void *) buffer, filled, r);
    }
 
  wfree (buffer);
  w_fclose (f);
  return (WARC_FALSE);

}


/**
 * @param fname: the name of the wanted file
 * @param prefix: the directory leading to the file location
 * @param offset: the offset from where the data will be sent
 * @param r: a request object
 *
 * @return False if succeed, True otherwise
 *
 * Sends a Warc Record Response
 */

WPRIVATE warc_bool_t WSend_record (const warc_u8_t * fname, const warc_u8_t * prefix, warc_u32_t offset, request_rec * r)
{
    void        *     w         = NIL;
    void        *     record    = NIL;
    warc_u8_t   *     buffer    = NIL;
    wfile_comp_t      cmode     = WARC_FILE_UNCOMPRESSED;
    warc_u32_t        rsize     = 0  ;
    warc_u32_t        sentsize  = 0  ;
    warc_u32_t        filled    = 0  ;


    w = bless (WFile, fname, WARC_MAX_SIZE,
               WARC_FILE_READER, WARC_FILE_DETECT_COMPRESSION, prefix);

    unless (w)
      {
      r->content_type = "text/html";
      ap_rprintf (r, "<font color=red size = 4>File not found</font><br>");
      return (WARC_TRUE);
      }

    if (WFile_seek (w, offset))
       {
        r->content_type = "text/html";
        ap_rprintf (r, "<font color = red size = 4 > Can not reach the reclaimed offset </font><br>");
        destroy (w);
        return (WARC_TRUE);
       }

     cmode = WFile_getCompressionMode (w);

     if (cmode == WARC_FILE_COMPRESSED_GZIP)
        ap_set_content_type (r, "application/x-gzip");
    else
        ap_set_content_type (r, "text/random");


    record = WFile_nextRecord (w);

    unless (record)
       {
        r->content_type = "text/html";
        ap_rprintf (r, "<font color = red size = 4 > No records at this offset</font><br>");
        destroy (w);
        return (WARC_TRUE);
       } 

    WFile_seek (w, offset);

    buffer = wmalloc (MAX_BUFFER_SIZE);
 
    unless (buffer)
     {
      r->content_type = "text/html";
      ap_rprintf (r, "<font color = red size = 4 >Failed in allocation of memory for respponse sending </font><br>");
      destroy (w);
      destroy (record);
      return (WARC_TRUE);
     }

    rsize = WRecord_getCompressedSize (record);
  
  
    while (rsize)
      {
       if (rsize > MAX_BUFFER_SIZE)
          {
          sentsize = MAX_BUFFER_SIZE;
          rsize = rsize - MAX_BUFFER_SIZE;
          }
       else
          {
          sentsize = rsize;
          rsize = 0;
          }
      WFile_fillBuffer  (w, buffer, sentsize, & filled);
      ap_rwrite ((const void *) buffer, filled, r);
      }
 
    wfree (buffer);
    destroy (record);
    destroy (w);
    return (WARC_FALSE);
}


/**
 * @param fname: the name of the wanted file
 * @param prefix: the directory leading to the file location
 * @param offset: the offset from where the data will be sent
 * @param fdfilter: fileld to filter (uri or content type)
 * @param filter: value of filter
 * @param r: a request object
 *
 * @return False if succeed, True otherwise
 *
 * Sends a filtred Warc file Response
 */


WPRIVATE warc_bool_t WSend_filtredResponse (const warc_u8_t * fname, const warc_u8_t * prefix, warc_u32_t offset,
                                const warc_u8_t * fdfilter,const warc_u8_t * filter, request_rec * r)
{
  void         *    w         = NIL;
  void         *    record    = NIL;
  void         *    rexpr     = NIL;
  warc_u8_t    *    buffer    = NIL;
  const char   *    hofld     = NIL;
  wfile_comp_t      cmode     = WARC_FILE_UNCOMPRESSED;
  warc_u32_t        rsize     = 0  ;
  warc_u32_t        sentsize  = 0  ;
  warc_u32_t        filled    = 0  ;
  warc_u32_t        rectype = 0;
 
 
  w = bless (WFile, fname, WARC_MAX_SIZE,
               WARC_FILE_READER, WARC_FILE_DETECT_COMPRESSION, prefix);

  unless (w)
      {
      r->content_type = "text/html";
      ap_rprintf (r, "<font color=red size = 4>File not found</font><br>");
      return (WARC_TRUE);
      }

   if (WFile_seek (w, offset))
      {
       r->content_type = "text/html";
       ap_rprintf (r, "<font color = red size = 4 > Can not reach the reclaimed offset </font><br>");
       destroy (w);
       return (WARC_TRUE);
      }

  cmode = WFile_getCompressionMode (w);

  if (cmode == WARC_FILE_COMPRESSED_GZIP)
       ap_set_content_type (r, "application/x-gzip");
  else
       ap_set_content_type (r, "text/random");
 
  rexpr = bless (WRegexp, makeS (filter));
  unless (rexpr)
     {
      r->content_type = "text/html";
      ap_rprintf (r, "<font color = red size = 4 >Bad filter string</font><br>");
      destroy (w);
      return (WARC_TRUE);
     }

   buffer = wmalloc (MAX_BUFFER_SIZE);
    
   while (WFile_hasMoreRecords (w) )
      {
       unless ( (record = WFile_nextRecord (w) ) )
         {
          r->content_type = "text/html";
          ap_rprintf (r, "<FONT SIZE = 4 COLOR = red>Bad record found</FONT><br>");
          wfree (buffer);
          destroy (w);
          destroy (rexpr);
          return (WARC_TRUE);
         }

       rsize = WRecord_getCompressedSize (record);

       unless (w_strcmp (fdfilter, "uri"))
              hofld =   WRecord_getTargetUri  (record);
       else
         {
         unless (w_strcmp(fdfilter, "contenttype"))
            hofld =   WRecord_getContentType (record);
         else
            {
             rectype =   WRecord_getRecordType (record);
             hofld = "";

             switch (rectype)
                {
                case 0:
                     hofld = "unknownrecord";
                     break;

                case 1:
                     hofld = "warcinfo";
                     break;

                case 2:
                     hofld = "response";
                     break;

                case 3:
                     hofld = "request";
                     break;

                case 4:
                     hofld = "metadata";
                     break;

                case 5:
                     hofld = "revisit";
                     break;
 
                case 6:
                     hofld = "conversion";
                     break;
 
                case 7:
                     hofld = "contunuation";
                     break;

                case 8:
                     hofld = "resource";
                     break;

                default: break;

             }
          }
       }

       if ( (hofld) && WRegexp_search (rexpr, makeS (hofld)))
          {
           WFile_seek (w, offset);
           offset += rsize;          
 
          unless (buffer)
                 {
                 r->content_type = "text/html";
                 ap_rprintf (r, "<font color = red size = 4 >Failed in allocation of memory for respponse sending </font><br>");
                 destroy (w);
                 destroy (record);
                 destroy (rexpr);
                 wfree (buffer);
                 return (WARC_TRUE);
                 }

          while (rsize)
                {
                if (rsize > MAX_BUFFER_SIZE)
                   {
                   sentsize = MAX_BUFFER_SIZE;
                   rsize = rsize - MAX_BUFFER_SIZE;
                   }
                else
                  {
                  sentsize = rsize;
                  rsize = 0;
                  }
                WFile_fillBuffer  (w, buffer, sentsize, & filled);
                ap_rwrite ((const void *) buffer, filled, r);
                }
          }
       else
          offset += rsize;

    
    destroy (record);
    }
  destroy (rexpr);
  wfree (buffer);
  destroy (w);
  return (WARC_FALSE);
}


/**
 * @param src_string: the origin string to convert
 * @param srclen: the length of the source string
 * @param xml_string: the de output XML string
 *
 * @return a warc_u8_t * if succeed, NIL otherwise
 *
 * Recode the XML special characters in the source string 
 * by their equivalent XML encoding string
 */


WPRIVATE warc_u8_t * xml_recode (const warc_u8_t * src_string, warc_u32_t srclen, warc_u8_t * xml_string)
{
    warc_u32_t i = 0;
    warc_u32_t j = 0;
    warc_u8_t  c = ' ';

  unless (src_string || srclen)
    return (NIL);

  xml_string[0] = 0;

  for (i = 0; (i < srclen && src_string[i] != 0); i++)
    {
    c = src_string[i];
    switch (c)
        {
        case '&': xml_string[j]='&';
                  xml_string[j+1]='a';
                  xml_string[j+2]='m';
                  xml_string[j+3]='p';
                  xml_string[j+4]=';';
                  j = j+5;
                  break;

        case '<': xml_string[j]='&';
                  xml_string[j+1]='l';
                  xml_string[j+2]='t';
                  xml_string[j+3]=';';
                  j = j+4;
                  break;

        case '>': xml_string[j]='&';
                  xml_string[j+1]='g';
                  xml_string[j+2]='t';
                  xml_string[j+3]=';';
                  j = j+4;
                  break;

        case '"': xml_string[j]='&';
                  xml_string[j+1]='q';
                  xml_string[j+2]='u';
                  xml_string[j+3]='o';
                  xml_string[j+4]='t';
                  xml_string[j+5]=';';
                  j = j+6;
                  break;

        case '\'': xml_string[j]='&';
                   xml_string[j+1]='a';
                   xml_string[j+2]='q';
                   xml_string[j+3]='u';
                   xml_string[j+4]='o';
                   xml_string[j+5]='t';
                   xml_string[j+6]=';';
                  j = j+7;
                  break;

        default: xml_string[j] = src_string[i];
                 j++;
        }
    }
xml_string [j] = '\0';

return (xml_string);

}


  
/**
 * @param fname: the name of the concerned warc file
 * @param wfile: the warc file to dump
 * @param req: the request object used in the response
 *
 * @return False if succeeds, True otherwise
 *
 * Builds an XML output to be sent by the list request
 */

WPRIVATE warc_bool_t WBuildXmlOutput (const warc_u8_t * fname, void * wfile, request_rec * req)
{
    void * wrecord     =  NIL;
    warc_u32_t cpt     =  0;
    warc_u8_t        xmlstr[256];


   ap_rprintf (req, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
   ap_rprintf (req, "<warcfile name=\"%s\" size=\"%llu\">\r\n", (const char *) fname, (unsigned long long) WFile_getFileSize (wfile)); 

  while (WFile_hasMoreRecords (wfile) )
    {
      const void * al  = NIL; /* ANVL list object */
      warc_bool_t  m   = WARC_FALSE;
      const warc_u8_t * string = NIL;
      warc_u32_t   tlen = 0;

      unless ( (wrecord = WFile_nextRecord (wfile) ) )
          {
          req -> content_type = "text/html";
          ap_rprintf (req, "<center><font color=darkred>Bad record found!\r\n</font></center><br>");

          break;
         }

      cpt ++;
      /* dump WRecord */
      ap_rprintf (req, "<warc-record rank=\"%d\" ", cpt );

      ap_rprintf (req, "offset=\"%llu\" ", (unsigned long long) WRecord_getOffset (wrecord) );

      ap_rprintf (req, "compressed-size=\"%llu\" ", (unsigned long long) WRecord_getCompressedSize (wrecord) );

      ap_rprintf (req, "uncompressed-size=\"%llu\" ", (unsigned long long) WRecord_getUncompressedSize (wrecord) );

      /* Preprocessing for xml viewing */
      xml_recode (makeS (WRecord_getWarcId (wrecord)), xmlstr);
      ap_rprintf (req, "warc-id=\"%s\" ", (const char *) xmlstr);

      ap_rprintf (req, "content-length=\"%u\" ", WRecord_getContentLength  (wrecord) );

      ap_rprintf (req, "warc-type=\"%u\" ", WRecord_getRecordType  (wrecord) );

      xml_recode (makeS (WRecord_getDate (wrecord)), xmlstr);
      ap_rprintf (req, "warc-date=\"%s\" ", (const char *) xmlstr );

      xml_recode ( makeS (WRecord_getRecordId (wrecord)), xmlstr);
      ap_rprintf (req, "warc-record-id=\"%s\">\r\n", (const char *) xmlstr);

      string = WRecord_getContentType (wrecord);
      if (string)
         {
         xml_recode ( makeS (string), xmlstr);
         ap_rprintf (req, "<content-type value=\"%s\"/>\r\n", (const char *) xmlstr);
         m = WARC_TRUE;
         }

      string = WRecord_getConcurrentTo (wrecord);
      if (string)
         {
         xml_recode ( makeS (string), xmlstr);
         ap_rprintf (req, "<warc-concurrent-to value=\"%s\"/>\r\n", (const char *) xmlstr);
         m = WARC_TRUE;
         }

      string = WRecord_getBlockDigest (wrecord);
      if (string)
         {
         xml_recode ( makeS (string), xmlstr);
         ap_rprintf (req, "<warc-block-digest value=\"%s\"/>\r\n", (const char *) xmlstr);
         m = WARC_TRUE;
         }

      string = WRecord_getPayloadDigest (wrecord);
      if (string)
         {
         xml_recode ( makeS (string), xmlstr);
         ap_rprintf (req, "<warc-payload-digest value=\"%s\"/>\r\n", (const char *) xmlstr);
         m = WARC_TRUE;
         }

      string = WRecord_getIpAddress (wrecord);
      if (string)
         {
         xml_recode ( makeS (string), xmlstr);
         ap_rprintf (req, "<warc-ip-address value=\"%s\"/>\r\n", (const char *) xmlstr);
         m = WARC_TRUE;
         }

      string = WRecord_getRefersTo (wrecord);
      if (string)
         {
         xml_recode ( makeS (string), xmlstr);
         ap_rprintf (req, "<warc-refers-to value=\"%s\"/>\r\n", (const char *) xmlstr);
         m = WARC_TRUE;
         }

      string = WRecord_getTargetUri (wrecord);
      if (string)
        {
         xml_recode ( makeS (string), xmlstr);
         ap_rprintf (req, "<warc-taget-uri value=\"%s\"/>\r\n", (const char *) xmlstr );
         m = WARC_TRUE;
        }

      string = WRecord_getTruncated (wrecord);
      if (string)
         {
         xml_recode ( makeS (string), xmlstr);
         ap_rprintf (req, "<warc-truncated value=\"%s\"/>\r\n", (const char *) xmlstr);
         m = WARC_TRUE;
         }

      string = WRecord_getWarcInfoId (wrecord);
      if (string)
         {
         xml_recode ( makeS (string), xmlstr);
         ap_rprintf (req, "<warc-info-id value=\"%s\"/>\r\n", (const char *) xmlstr);
         m = WARC_TRUE;
         }

      string = WRecord_getFilename (wrecord);
      if (string)
         {
         xml_recode ( makeS (string), xmlstr);
         ap_rprintf (req, "<warc-filename value=\"%s\"/>\r\n", (const char *) xmlstr);
         m = WARC_TRUE;
         }

      string = WRecord_getProfile (wrecord);
      if (string)
         {
         xml_recode ( makeS (string), xmlstr);
         ap_rprintf (req, "<warc-profile value=\"%s\"/>\r\n", (const char *) xmlstr);
         m = WARC_TRUE;
         }

      string = WRecord_getPayloadType (wrecord);
      if (string)
         {
         xml_recode ( makeS (string), xmlstr);
         ap_rprintf (req, "<warc-identrified-payload-type value=\"%s\"/>\r\n", (const char *) xmlstr);
         m = WARC_TRUE;
         }

      string = WRecord_getSegmentOriginId (wrecord);
      if (string)
         {
         xml_recode ( makeS (string), xmlstr);
         ap_rprintf (req, "<warc-segment-origin-id value=\"%s\"/>\r\n", (const char *) xmlstr);

         ap_rprintf (req, "<warc-segment-nummber value=\"%d\"/>\r\n",  WRecord_getSegmentNumber (wrecord));
         m = WARC_TRUE;
         }

      tlen = WRecord_getSegTotalLength (wrecord);
      if (tlen)
         {
         ap_rprintf (req, "<warc-segment-total-length value=\"%d\"/>\r\n", tlen);
         m = WARC_TRUE;
         }


      /* dump ANVLs */

      if ((al = WRecord_getAnvl (wrecord) ))
        {

          warc_u32_t  i = 0;
          warc_u32_t  j = WList_size (al); /* how many ANVL are there? */


         if (j)
           {
           ap_rprintf (req, "<extra-fields>\r\n");
           while ( i < j )
               {
                 const void  * a = WList_getElement (al, i); /* ANVL record */
   
                 /* we assume here that the ANVL value was in ASCII. */
                 /* use your own encoding to print it otherwise. */

                 ap_rprintf (req , "<field key=\"%s\" value=\"%s\"/>", (char *) xml_recode (makeS (WAnvl_getKey (a)), xmlstr), 
                                      (char *) xml_recode (makeS (WAnvl_getValue (a)), xmlstr) );

                 ++ i;
               }
          ap_rprintf (req, "</extra-fields>\r\n");
           }
        }

      destroy (wrecord);
      ap_rprintf (req, "</warc-record>\r\n");
    }
   
   ap_rprintf (req, "</warcfile>\r\n");


  return (WARC_FALSE);
}



/**
 * @param fname: the name of the concerned warc file
 * @param wfile: the warc file to dump
 * @param req: the request object used in the response
 *
 * @return False if succeeds, True otherwise
 *
 * Builds an HtML output to be sent by the list request
 */

WPRIVATE warc_bool_t WBuildHtmlOutput (const warc_u8_t * fname, void * wfile, request_rec * req)
{
    void * wrecord     =  NIL;
    warc_u32_t cpt     =  0;
/*    warc_u8_t        xmlstr[256]; */


   ap_rprintf (req, "<font size=5><b>warcfile:</B></font><font size=3> name=\"%s\" size=\"%llu\"\r\n</font><BR><br>", (const char *) fname, 
                                                                 (unsigned long long)WFile_getFileSize (wfile));

  while (WFile_hasMoreRecords (wfile) )
    {
      const void * al  = NIL; /* ANVL list object */
      warc_bool_t  m   = WARC_FALSE;
      const warc_u8_t * string = NIL;
      warc_u32_t   tlen = 0;

      unless ( (wrecord = WFile_nextRecord (wfile) ) )
          {
          ap_rprintf (req, "<center><font color=darkred>Bad record found!\r\n</font></center><br>");

          req -> content_type = "text/html";
     
          break;
         }

      cpt ++;
      /* dump WRecord */
      ap_rprintf (req, "<font color=darkblue size = 5> <b> Warc Record: %d</b> </font>", cpt);
      ap_rprintf (req, "<blockquote> <table BORDER = 2 bgcolor=lightgray>");
      ap_rprintf (req, "<tr><td><font color=darkgreen size = 3> Offset </font></td><td>%llu</td></tr>",
                              (unsigned long long) WRecord_getOffset (wrecord) );

      ap_rprintf (req, "<tr><td><font color=darkgreen size = 3> Compressed size </font></td><td>%llu</td></tr>",
                              (unsigned long long) WRecord_getCompressedSize (wrecord) );

      ap_rprintf (req, "<tr><td><font color=darkgreen size = 3> Warc ID </font></td><td>%s</td></tr> ",
                              (const char *) WRecord_getWarcId      (wrecord) );

      ap_rprintf (req, "<tr><td><font color=darkgreen size = 3> Content-Length </font></td><td>%u</td></tr>",
                              WRecord_getContentLength  (wrecord) );

      ap_rprintf (req, "<tr><td><font color=darkgreen size = 3> WARC-Type </font></td><td>%u</td></tr>",
                              WRecord_getRecordType  (wrecord) );

      ap_rprintf (req, "<tr><td><font color=darkgreen size = 3> WARC-Date </font></td><td>%s</td></tr>",
                              (const char *) WRecord_getDate (wrecord) );

      ap_rprintf (req, "<tr><td><font color=darkgreen size = 3> WARC-Record-ID </font></td><td>%s</td></tr>",
                              (const char *) WRecord_getRecordId    (wrecord) );

      ap_rprintf (req, "</table></blockquote><br>");

      ap_rprintf (req, "<font color=darkgreen size = 4> <b> Other Fields </b> </font>");
      ap_rprintf (req, "<blockquote> <table BORDER = 2 bgcolor=lightgray>");

      string = WRecord_getContentType (wrecord);
      if (string)
         {
         ap_rprintf (req, "<tr><td><font color=darkgreen size = 3> Content-Type </font></td><td>%s</td></tr>",
                              (const char *) string);
         m = WARC_TRUE;
         }

      string = WRecord_getConcurrentTo (wrecord);
      if (string)
         {
         ap_rprintf (req, "<tr><td><font color=darkgreen size = 3> WARC-Concurrent-To </font></td><td>%s</td></tr>",
                              (const char *) string);
         m = WARC_TRUE;
         }

      string = WRecord_getBlockDigest (wrecord);
      if (string)
         {
         ap_rprintf (req, "<tr><td><font color=darkgreen size = 3> WARC-Block-Digest </font></td><td>%s</td></tr>",
                              (const char *) string);
         m = WARC_TRUE;
         }

      string = WRecord_getPayloadDigest (wrecord);
      if (string)
         {
         ap_rprintf (req, "<tr><td><font color=darkgreen size = 3> WARC-Payload-Digest </font></td><td>%s</td></tr>",
                              (const char *) string);
         m = WARC_TRUE;
         }

      string = WRecord_getIpAddress (wrecord);
      if (string)
         {
         ap_rprintf (req, "<tr><td><font color=darkgreen size = 3> WARC-IP-Address </font></td><td>%s</td></tr>",
                              (const char *) string);
         m = WARC_TRUE;
         }

      string = WRecord_getRefersTo (wrecord);
      if (string)
         {
         ap_rprintf (req, "<tr><td><font color=darkgreen size = 3> WARC-Refers-To </font></td><td>%s</td></tr>",
                              (const char *) string);
         m = WARC_TRUE;
         }

      string = WRecord_getTargetUri (wrecord);
      if (string)
        {
         ap_rprintf (req, "<tr><td><font color=darkgreen size = 3> WARC-Target-URI </font></td><td>%s</td></tr>",
                              (const char *) string );
         m = WARC_TRUE;
         }

      string = WRecord_getTruncated (wrecord);
      if (string)
         {
         ap_rprintf (req, "<tr><td><font color=darkgreen size = 3> WARC-Truncated </font></td><td>%s</td></tr>",
                              (const char *) string);
         m = WARC_TRUE;
         }

      string = WRecord_getWarcInfoId (wrecord);
      if (string)
         {
         ap_rprintf (req, "<tr><td><font color=darkgreen size = 3> WARC-warcinfo-ID </font></td><td>%s</td></tr>",
                              (const char *) string);
         m = WARC_TRUE;
         }

      string = WRecord_getFilename (wrecord);
      if (string)
         {
         ap_rprintf (req, "<tr><td><font color=darkgreen size = 3> WARC-Filename </font></td><td>%s</td></tr>",
                              (const char *) string);
         m = WARC_TRUE;
         }

      string = WRecord_getProfile (wrecord);
      if (string)
         {
         ap_rprintf (req, "<tr><td><font color=darkgreen size = 3> WARC-Profile </font></td><td>%s</td></tr>",
                              (const char *) string);
         m = WARC_TRUE;
         }

      string = WRecord_getPayloadType (wrecord);
      if (string)
         {
         ap_rprintf (req, "<tr><td><font color=darkgreen size = 3> WARC-Identified-Payload-Type </font></td><td>%s</td></tr>",
                              (const char *) string);
         m = WARC_TRUE;
         }

      string = WRecord_getSegmentOriginId (wrecord);
      if (string)
         {
         ap_rprintf (req, "<tr><td><font color=darkgreen size = 3> WARC-Segment-Origin-ID </font></td><td>%s</td></tr>",
                              (const char *) string);

         ap_rprintf (req, "<tr><td><font color=darkgreen size = 3> WARC-Segment-Number </font></td><td>%d</td></tr>",
                             WRecord_getSegmentNumber (wrecord));
         m = WARC_TRUE;
         }

      tlen = WRecord_getSegTotalLength (wrecord);
      if (tlen)
         {
         ap_rprintf (req, "<tr><td><font color=darkgreen size = 3> WARC-Segment-Total-Length </font></td><td>%d</td></tr>",
                              tlen);
         m = WARC_TRUE;
         }

     unless (m)
        ap_rprintf (req, "<tr><td><font color=darkgreen size =3 > --No One-- </font></td></tr>");

      ap_rprintf (req, "</table></blockquote><br>");



      /* dump ANVLs */

      if ((al = WRecord_getAnvl (wrecord) ))
        {
          warc_u32_t  i = 0;
          warc_u32_t  j = WList_size (al); /* how many ANVL are there? */
        
         if (j)
           {
           ap_rprintf (req, "<font color=darkred size = 4> <b> More information </b> </font>");
           ap_rprintf (req, "<blockquote> <table BORDER = 2 bgcolor=lightgray>");
       
           while ( i < j )
               {
                 const void  * a = WList_getElement (al, i); /* ANVL record */
   
                 /* we assume here that the ANVL value was in ASCII. */
                 /* use your own encoding to print it otherwise. */
                 ap_rprintf (req , "<tr><td><font color=darkgreen size = 3> %s </font></td><td>%s</td></tr>",
                                          (const char *) WAnvl_getKey (a), (const char *) WAnvl_getValue (a) );

                 ++ i;
               }
           ap_rprintf (req, "</table></blockquote><br><br>");
           }

        }

      destroy (wrecord);
    }

  return (WARC_FALSE);
}


/**
 * @param fname: the name of the concerned warc file
 * @param wfile: the warc file to dump
 * @param req: the request object used in the response
 *
 * @return False if succeeds, True otherwise
 *
 * Builds a Plain text output to be sent by the list request
 */

WPRIVATE warc_bool_t WBuildTextOutput (const warc_u8_t * fname, void * wfile, request_rec * req)
{
    void * wrecord     =  NIL;
    warc_u32_t cpt     =  0;
/*    warc_u8_t        xmlstr[256]; */


   ap_rprintf (req, "warcfile: name=\"%s\" size=\"%llu\"\r\n", (const char *) fname, (unsigned long long)WFile_getFileSize (wfile));

  while (WFile_hasMoreRecords (wfile) )
    {
      const void * al  = NIL; /* ANVL list object */
      warc_bool_t  m   = WARC_FALSE;
      const warc_u8_t * string = NIL;
      warc_u32_t   tlen = 0;

      unless ( (wrecord = WFile_nextRecord (wfile) ) )
          {
          ap_rprintf (req, "<center><font color=darkred>Bad record found!\r\n</font></center><br>");

          req -> content_type = "text/html";
          break;
         }

      cpt ++;
      /* dump WRecord */
      ap_rprintf (req, "- Warc Record: %d\r\n", cpt);
      ap_rprintf (req, "\t* Offset: %llu\r\n",
                              (unsigned long long) WRecord_getOffset (wrecord) );

      ap_rprintf (req, "\t* Compressed size: %llu\r\n",
                              (unsigned long long) WRecord_getCompressedSize (wrecord) );

      ap_rprintf (req, "\t* Warc ID: %s\r\n",
                              (const char *) WRecord_getWarcId      (wrecord) );

      ap_rprintf (req, "\t* Content-Length: %u\r\n",
                              WRecord_getContentLength  (wrecord) );

      ap_rprintf (req, "\t* WARC-Type: %u\r\n",
                              WRecord_getRecordType  (wrecord) );

      ap_rprintf (req, "\t* WARC-Date: %s\r\n",
                              (const char *) WRecord_getDate (wrecord) );

      ap_rprintf (req, "\t* WARC-Record-ID: %s\r\n",
                              (const char *) WRecord_getRecordId    (wrecord) );

      ap_rprintf (req, "\r\n\t* Other Fields:\r\n");

      string = WRecord_getContentType (wrecord);
      if (string)
         {
         ap_rprintf (req, "\t\t@ Content-Type: %s\r\n",
                              (const char *) string);
         m = WARC_TRUE;
         }

      string = WRecord_getConcurrentTo (wrecord);
      if (string)
         {
         ap_rprintf (req, "\t\t@ WARC-Concurrent-To: %s\r\n",
                              (const char *) string);
         m = WARC_TRUE;
         }

      string = WRecord_getBlockDigest (wrecord);
      if (string)
         {
         ap_rprintf (req, "\t\t@ WARC-Block-Digest: %s\r\n",
                              (const char *) string);
         m = WARC_TRUE;
         }

      string = WRecord_getPayloadDigest (wrecord);
      if (string)
         {
         ap_rprintf (req, "\t\t@ WARC-Payload-Digest: %s\r\n",
                              (const char *) string);
         m = WARC_TRUE;
         }

      string = WRecord_getIpAddress (wrecord);
      if (string)
         {
         ap_rprintf (req, "\t\t@ WARC-IP-Address: %s\r\n",
                              (const char *) string);
         m = WARC_TRUE;
         }

      string = WRecord_getRefersTo (wrecord);
      if (string)
         {
         ap_rprintf (req, "\t\t@ WARC-Refers-To: %s\r\n",
                              (const char *) string);
         m = WARC_TRUE;
         }

      string = WRecord_getTargetUri (wrecord);
      if (string)
        {
         ap_rprintf (req, "\t\t@ WARC-Target-URI: %s\r\n",
                              (const char *) string );
         m = WARC_TRUE;
         }

      string = WRecord_getTruncated (wrecord);
      if (string)
         {
         ap_rprintf (req, "\t\t@ WARC-Truncated: %s\r\n",
                              (const char *) string);
         m = WARC_TRUE;
         }

      string = WRecord_getWarcInfoId (wrecord);
      if (string)
         {
         ap_rprintf (req, "\t\t@ WARC-Warcinfo-ID: %s\r\n",
                              (const char *) string);
         m = WARC_TRUE;
         }

      string = WRecord_getFilename (wrecord);
      if (string)
         {
         ap_rprintf (req, "\t\t@ WARC-Filename: %s\r\n",
                              (const char *) string);
         m = WARC_TRUE;
         }

      string = WRecord_getProfile (wrecord);
      if (string)
         {
         ap_rprintf (req, "\t\t@ WARC-Profile: %s\r\n",
                              (const char *) string);
         m = WARC_TRUE;
         }

      string = WRecord_getPayloadType (wrecord);
      if (string)
         {
         ap_rprintf (req, "\t\t@ WARC-Identified-Payload-Type: %s\r\n",
                              (const char *) string);
         m = WARC_TRUE;
         }

      string = WRecord_getSegmentOriginId (wrecord);
      if (string)
         {
         ap_rprintf (req, "\t\t@ WARC-Segment-Origin-ID: %s\r\n",
                              (const char *) string);

         ap_rprintf (req, "\t\t@ WARC-Segment-Number: %d\r\n",
                             WRecord_getSegmentNumber (wrecord));
         m = WARC_TRUE;
         }

      tlen = WRecord_getSegTotalLength (wrecord);
      if (tlen)
         {
         ap_rprintf (req, "\t\t@ WARC-Segment-Total-Length: %d\r\n",
                              tlen);
         m = WARC_TRUE;
         }

     unless (m)
        ap_rprintf (req, "---No One---\r\n");



      /* dump ANVLs */

      if ((al = WRecord_getAnvl (wrecord) ))
        {
          warc_u32_t  i = 0;
          warc_u32_t  j = WList_size (al); /* how many ANVL are there? */
        
         if (j)
           {
           ap_rprintf (req, "\r\n* More information:\r\n");
       
           while ( i < j )
               {
                 const void  * a = WList_getElement (al, i); /* ANVL record */
   
                 /* we assume here that the ANVL value was in ASCII. */
                 /* use your own encoding to print it otherwise. */
                 ap_rprintf (req , "\t\t@ %s: %s\r\n",
                                          (const char *) WAnvl_getKey (a), (const char *) WAnvl_getValue (a) );

                 ++ i;
               }
           }

        }

      destroy (wrecord);
    }



  return (WARC_FALSE);
}


/**
 * @param fname: the name of the concerned warc file
 * @param wfile: the warc file to dump
 * @param req: the request object used in the response
 *
 * @return False if succeeds, True otherwise
 *
 * Builds a Json output to be sent by the list request
 */

WPRIVATE warc_bool_t WBuildJsonOutput (const warc_u8_t * fname, void * wfile, request_rec * req)
{
    void       *   wrecord     =  NIL;
    warc_u32_t     cpt         =  0;
    warc_u8_t      comma[2]    = {'\0','\0'};
    warc_bool_t    first       = WARC_TRUE;
    warc_bool_t    firstr      = WARC_TRUE;
/*    warc_u8_t        xmlstr[256]; */


   ap_rprintf (req, "{\"warcfile\":[{\"name\":\"%s\"},{\"size\":\"%llu\"},{\"records\":[", (const char *) fname, 
                        (unsigned long long)WFile_getFileSize (wfile));

  while (WFile_hasMoreRecords (wfile) )
    {
      const void * al  = NIL; /* ANVL list object */
      const warc_u8_t * string = NIL;
      warc_u32_t   tlen = 0;

      unless ( (wrecord = WFile_nextRecord (wfile) ) )
          {
          ap_rprintf (req, "<center><font color=darkred>Bad record found!\r\n</font></center><br>");

          req -> content_type = "text/html";
          break;
         }

      cpt ++;
      /* dump WRecord */
      if (!firstr)
         ap_rprintf (req, ",");
      else
         firstr = WARC_FALSE;
      ap_rprintf (req, "{\"Warc Record\":[{\"rank\":%d}", cpt);
      ap_rprintf (req, ",{\"Offset\":%llu}",
                              (unsigned long long) WRecord_getOffset (wrecord) );

      ap_rprintf (req, ",{\"Compressed size\":%llu}",
                              (unsigned long long) WRecord_getCompressedSize (wrecord) );

      ap_rprintf (req, ",{\"fields\":[{\"Warc ID\":\"%s\"}",
                              (const char *) WRecord_getWarcId      (wrecord) );

      ap_rprintf (req, ",{\"Content-Length\":%u}",
                              WRecord_getContentLength  (wrecord) );

      ap_rprintf (req, ",{\"WARC-Type\":%u}",
                              WRecord_getRecordType  (wrecord) );

      ap_rprintf (req, ",{\"WARC-Date\":\"%s\"}",
                              (const char *) WRecord_getDate (wrecord) );

      ap_rprintf (req, ",{\"WARC-Record-ID\":\"%s\"}",
                              (const char *) WRecord_getRecordId    (wrecord) );

      ap_rprintf (req, ",{\"Other Fields\":[");

      string = WRecord_getContentType (wrecord);
      if (string)
         {
         ap_rprintf (req, "%s{\"Content-Type\":\"%s\"}",
                              (const char *) comma, (const char *) string);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }

      string = WRecord_getConcurrentTo (wrecord);
      if (string)
         {
         ap_rprintf (req, "%s{\"WARC-Concurrent-To\":\"%s\"}",
                              (const char *) comma, (const char *) string);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }

      string = WRecord_getBlockDigest (wrecord);
      if (string)
         {
         ap_rprintf (req, "%s{\"WARC-Block-Digest\":\"%s\"}",
                              (const char *) comma, (const char *) string);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }

      string = WRecord_getPayloadDigest (wrecord);
      if (string)
         {
         ap_rprintf (req, "%s{\"WARC-Payload-Digest\":\"%s\"}",
                              (const char *) comma, (const char *) string);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }

      string = WRecord_getIpAddress (wrecord);
      if (string)
         {
         ap_rprintf (req, "%s{\"WARC-Ip-Address\":\"%s\"}",
                              (const char *) comma, (const char *) string);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }

      string = WRecord_getRefersTo (wrecord);
      if (string)
         {
         ap_rprintf (req, "%s{\"WARC-Refers-To\":\"%s\"}",
                              (const char *) comma, (const char *) string);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }

      string = WRecord_getTargetUri (wrecord);
      if (string)
         {
         ap_rprintf (req, "%s{\"WARC-Target-URI\":\"%s\"}",
                              (const char *) comma, (const char *) string);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }

      string = WRecord_getTruncated (wrecord);
      if (string)
         {
         ap_rprintf (req, "%s{\"WARC-Truncated\":\"%s\"}",
                              (const char *) comma, (const char *) string);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }

      string = WRecord_getWarcInfoId (wrecord);
      if (string)
         {
         ap_rprintf (req, "%s{\"WARC-Warcinfo-ID\":\"%s\"}",
                              (const char *) comma, (const char *) string);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }

      string = WRecord_getFilename (wrecord);
      if (string)
         {
         ap_rprintf (req, "%s{\"WARC-Filename\":\"%s\"}",
                              (const char *) comma, (const char *) string);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }

      string = WRecord_getProfile (wrecord);
      if (string)
         {
         ap_rprintf (req, "%s{\"WARC-Profile\":\"%s\"}",
                              (const char *) comma, (const char *) string);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }

      string = WRecord_getPayloadType (wrecord);
      if (string)
         {
         ap_rprintf (req, "%s{\"WARC-Payload-Type\":\"%s\"}",
                              (const char *) comma, (const char *) string);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }

      string = WRecord_getSegmentOriginId (wrecord);
      if (string)
         {
         ap_rprintf (req, "%s{\"WARC-Segment-Origin-ID\":\"%s\"},",
                              (const char *) comma, (const char *) string);

         ap_rprintf (req, "{\"WARC-Segment-Number\":%d}",
                             WRecord_getSegmentNumber (wrecord));
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }

      tlen = WRecord_getSegTotalLength (wrecord);
      if (tlen)
         {
         ap_rprintf (req, "%s{\"WARC-Segment-Total-Length\":%d}",
                              (const char *) comma, tlen);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }

      ap_rprintf (req, "]}");


      /* dump ANVLs */

      if ((al = WRecord_getAnvl (wrecord) ))
        {
          warc_u32_t  i = 0;
          warc_u32_t  j = WList_size (al); /* how many ANVL are there? */
        
         if (j)
           {
           ap_rprintf (req, ",{\"Extra Fields\":[");

           first = WARC_TRUE;
           comma[0] = '\0';
       
           while ( i < j )
               {
                 const void  * a = WList_getElement (al, i); /* ANVL record */
   
                 /* we assume here that the ANVL value was in ASCII. */
                 /* use your own encoding to print it otherwise. */
                 ap_rprintf (req , "%s{\"%s\":\"%s\"}",
                                          (const char *) comma, (const char *) WAnvl_getKey (a), (const char *) WAnvl_getValue (a) );
                 if (first)
                   {
                   first = WARC_FALSE;
                   comma[0] = ',';
                   }

                 ++ i;
               }
           ap_rprintf (req, "]}");
           }

        }
      ap_rprintf (req, "]}]}");
      first = WARC_TRUE;
      comma[0] = '\0';
      destroy (wrecord);
    }

   ap_rprintf (req, "]}]}");

   req -> content_type = "text/plain";



  return (WARC_FALSE);
}



/**
 * @param fname: the name of the wanted file
 * @param prefix: the directory leading to the file location
 * @param offset: the offset from where the data will be sent
 * @param output: the output format
 * @param r: a request object
 * 
 * Sends a distant listing of the records in the warc file
 */

WPRIVATE warc_bool_t WSend_distantDumpResponse (const warc_u8_t * fname, const warc_u8_t * prefix, warc_u32_t offset, 
                                                const char* output, request_rec * r)
{
  void      *     w      = NIL;

   w = bless (WFile, fname, WARC_MAX_SIZE,
                WARC_FILE_READER, WARC_FILE_DETECT_COMPRESSION, prefix);

  
   unless (w)
      {
       r->content_type = "text/html";
      ap_rprintf (r, "<FONT SIZE = 4 COLOR = red>  File  not found  </FONT><br>");
      return (WARC_TRUE);
      }
   
   if (WFile_seek (w, offset))
      {
       destroy (w);
       r->content_type = "text/html";
       ap_rprintf (r, "<FONT SIZE = 4 COLOR = red>Bad offset</FONT><br>");
       return (WARC_TRUE);
      }
  unless (w_strcmp (output, "xml"))
    {
    r -> content_type = "text/xml";
    if (WBuildXmlOutput (fname,  w, r)) 
       {   
          r->content_type = "text/html";
          ap_rprintf (r, "<FONT SIZE = 4 COLOR = red>Can not display information about the file</FONT><br>");
          destroy (w);
          return (WARC_TRUE);
       }
     }
  else unless (w_strcmp (output, "html"))
       {
        r -> content_type = "text/html";
        if (WBuildHtmlOutput (fname,  w, r)) 
          {
            r->content_type = "text/html";
            ap_rprintf (r, "<FONT SIZE = 4 COLOR = red>Can not display information about the file</FONT><br>");
            destroy (w);
            return (WARC_TRUE);
          }
       }
  else unless (w_strcmp (output, "text"))
       {
        r -> content_type = "text/plain";
        if (WBuildTextOutput (fname,  w, r)) 
          {
            r->content_type = "text/html";
            ap_rprintf (r, "<FONT SIZE = 4 COLOR = red>Can not display information about the file</FONT><br>");
            destroy (w);
            return (WARC_TRUE);
          }
       }
  else unless (w_strcmp (output, "json"))
       {
        r -> content_type = "text/plain";
        if (WBuildJsonOutput (fname,  w, r)) 
          {
            r->content_type = "text/html";
            ap_rprintf (r, "<FONT SIZE = 4 COLOR = red>Can not display information about the file</FONT><br>");
            destroy (w);
            return (WARC_TRUE);
          }
       }

  return (WARC_FALSE);
}



static warc_i32_t warc_handler(request_rec * r)
{ 
    const char   *   reqnature;
    const char   *   fname ;
    const char   *   ofrec;
    void         *   objfile;
    void         *   objrec;
    void         *   objnature;
    warc_u32_t pos             = w_strlen (uS(WARC_VERSION)) + 1;
    warc_i32_t       offset    = -1;



   
   
    if (w_strcmp(r->handler, "warc"))
        return DECLINED;


    r->content_type = "text/html";
    objnature = recoverUriField (r-> uri, & pos);
    reqnature = WString_getText (objnature);

    if ( w_strcmp (reqnature, "file" ))
          if ( w_strcmp (reqnature, "record"))
               if (  w_strcmp (reqnature, "list"))
                  
                   if (  w_strcmp (reqnature, "filter"))
                      {
                      destroy (objnature);
                      ap_rprintf (r, "<FONT SIZE = 4 COLOR = blue> Nature of request not recognized </FONT>\n<br>");
                      return OK;
                      }

    
    
   objrec = recoverUriField (r -> uri, & pos);
   ofrec  = WString_getText(objrec);
   
   if ( strIsDigit (ofrec))
       offset = atoi(ofrec);
    else
      {
        destroy (objrec);
        destroy (objnature);
        ap_rprintf (r, "<FONT SIZE = 4 COLOR = %s> offset not digit </font>\n<br>");
        return OK;
      }

  unless (w_strcmp (reqnature, "filter"))
      {
       void         *   objfdfilter;
       void         *   objfilter;
       const char   *   fdfilter;
       const char   *   filter;
 
        
      objfdfilter =  recoverUriField (r -> uri, & pos);
      fdfilter = WString_getText (objfdfilter);

      if (w_strcmp (fdfilter, "uri"))
             if (w_strcmp (fdfilter, "contenttype"))
                    if (w_strcmp (fdfilter, "recordtype"))
                        {
                        ap_set_content_type (r, "text/html");
                        destroy (objrec);
                        destroy (objnature);
                        destroy (objfdfilter);
                        ap_rprintf (r, "<FONT SIZE = 4 COLOR = red> can not apply this filter: field to filter must be content type or uri or record type </font>\n<br>");
                        return OK;
                        }
             
      objfilter =  recoverUriField (r -> uri, & pos);
      filter = WString_getText (objfilter);
       
      objfile = recoverFName (r -> uri, & pos);
      fname =   WString_getText(objfile);

     
      if (WSend_filtredResponse (fname, dname, offset, fdfilter, filter, r))
         {
          r->content_type = "text/html";
          ap_rprintf (r, "<font color=red size=4>Filtred File sending aborted</font><br>");
         }      

      
     

      destroy (objfile);
      destroy (objrec);
      destroy (objnature);
      destroy (objfilter);
      destroy (objfdfilter);
      return OK;

      }
      
      

 /* we want here te recover a whole warc file */
  unless (w_strcmp (reqnature, "file"))
      {
         
      objfile = recoverFName (r -> uri, & pos);
      fname =   WString_getText(objfile);
      
      if (WSend_fullResponse (fname, dname, offset, r))
         {
          r->content_type = "text/html";
          ap_rprintf (r, "<font color=red size=4> File sending aborted</font><br>");
         }


  destroy (objfile);
  destroy (objrec);
  destroy (objnature);

  return OK;
  }


/* here, we wante only to send one record */
  unless (w_strcmp (reqnature, "record"))
    {
      objfile = recoverFName (r -> uri, & pos);
      fname =   WString_getText(objfile);

    if (WSend_record (fname, dname, offset, r))
         {
          r->content_type = "text/html";
          ap_rprintf (r, "<font color=red size=4> Record sending aborted</font><br>");
         }

    destroy (objfile);
    destroy (objrec);
    destroy (objnature);

  return OK;
  }

/* here, we want to list information about the warc file */
  unless ( w_strcmp (reqnature, "list"))
     {

     void       * objout = NIL;
     const char * output = NIL;

      objout =  recoverUriField (r -> uri, & pos);
      output = WString_getText (objout);

      if (w_strcmp (output, "xml"))
             if (w_strcmp (output, "html"))
                    if (w_strcmp (output, "text"))
                        if (w_strcmp (output, "json"))
                           {
                           ap_set_content_type (r, "text/html");
                           destroy (objrec);
                           destroy (objnature);
                           destroy (objout);
                           ap_rprintf (r, "<FONT SIZE = 4 COLOR = red> Output format \"%s\" not supported </font>\n<br>", output);
                           return OK;
                          }

      objfile = recoverFName (r -> uri, & pos);
      fname =   WString_getText(objfile);

      if (WSend_distantDumpResponse (fname, dname, offset, output, r))
         {
         r -> content_type = "text/html";
         ap_rprintf (r, "<font color=red size=4> Records listing aborted</font><br>");
         }


    destroy (objrec);
    destroy (objout);
    destroy (objfile);
    destroy (objnature);
    return OK;
    
   } 

 destroy (objrec);
 destroy (objfile);
 destroy (objnature);
 return OK;
}

static void *  create_warc_dir_config (apr_pool_t *p, char *path) 

{
  warc_dir_config *conf;

  conf = (warc_dir_config *)apr_pcalloc(p, sizeof(warc_dir_config));
  conf->warc_tmp_dir = NULL;

  return (void *)conf;
}

static void *   merge_warc_dir_configs(apr_pool_t *p, void *bp, void *ap) {
     warc_dir_config *base = (warc_dir_config *)bp;
     warc_dir_config *add = (warc_dir_config *)ap;
     warc_dir_config *conf;

     conf = (warc_dir_config *)apr_pcalloc(p, sizeof(warc_dir_config));
 
     conf->warc_tmp_dir = PTR_OR(add->warc_tmp_dir, base->warc_tmp_dir);

 
     return conf;
     }

static const char *set_warc_tmp_dir(cmd_parms *cmd,
                                               void *tmp, const char *arg)
{
  warc_dir_config *conf = (warc_dir_config *) tmp;
  conf->warc_tmp_dir = apr_pstrdup(cmd->pool, arg);
  strcpy (dname, conf->warc_tmp_dir);
  return NULL;
}



static const  command_rec warc_cmds[] = {

     AP_INIT_TAKE1("WarcTmpdir", set_warc_tmp_dir,
     NULL, OR_OPTIONS, "tmpdir used to create temporary file in the compressed case"),
     {NULL}
     };


static void warc_register_hooks(apr_pool_t *p)
{
    ap_hook_handler(warc_handler, NULL, NULL, APR_HOOK_MIDDLE);
}


/* Dispatch list for API hooks */
module AP_MODULE_DECLARE_DATA warc_module = {

    STANDARD20_MODULE_STUFF, 
    create_warc_dir_config,     /* create per-dir    config structures */
    merge_warc_dir_configs,     /* merge  per-dir    config structures */
    NULL,                       /* create per-server config structures */
    NULL,                       /* merge  per-server config structures */
    warc_cmds,                  /* table of config file commands       */
    warc_register_hooks         /* register hooks                      */
};

      
