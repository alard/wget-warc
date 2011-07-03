
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

#include <stdlib.h>
#include <warc.h>
#include <wfile.x>
#include <wmem.h>

#include <fcgi_stdio.h>
#define uS(s) (warc_u8_t *)(s)
#define makeS(s) uS(s),w_strlen(uS(s))
#define makeWString(s) bless (WString, uS(s), w_strlen((uS(s))))

#ifndef WARC_MAX_SIZE
#define WARC_MAX_SIZE 629145600
#define MAX_BUFFER_SIZE 512
#endif




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
               return (NIL);
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
  achar [0] ='/';
  WString_append (item, achar, 1);

  while ( uri [i] == '/')
        i ++ ;

  for (k = i; k < strlen (uri ); k++)
      {
      achar [0] = uri [k];
      if (WString_append (item, achar, 1))
             {
               destroy (item);
               return (NIL);
             }
      
      }
  * (position) = i ;

  return item;
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
 * @param fname: the name of the wanted file
 * @param offset: the offset from where the data will be sent
 * @param prefix: the directory to create temporary file
 *
 * @return False if succeed, True otherwise
 *
 * Sends a full file response to the client
 */

WPRIVATE warc_bool_t WSend_fullResponse (const warc_u8_t * fname, const warc_u8_t * prefix,warc_u32_t offset)
{
   FILE          *   f        = NIL;
   warc_u8_t     *   buffer   = NIL;
   warc_u32_t        fsize    = 0  ;
   warc_u32_t        filled   = 0  ;
   warc_u32_t        sentsize = 0  ;
   void          *    w = NIL;
   wfile_comp_t      cmode    = WARC_FILE_UNCOMPRESSED;

  buffer = wmalloc (MAX_BUFFER_SIZE);
  
  unless (buffer)
    {
    fprintf (stdout, "content-type: text/html\n\n");
    fprintf (stdout, "<html>\n<body>\n");
    fprintf (stdout, "<font color=red size = 4>Failed in allocation of memory for respponse sending </font><br>");
    
    return (WARC_TRUE);
    }

  f = w_fopen_rb ((char *) fname);

  unless (f)
    {
    fprintf (stdout, "content-type: text/html\n\n");
    fprintf (stdout, "<html>\n<body>\n"); 
    fprintf (stdout, "<font color=red size = 4>File not found</font><br>");
    wfree (buffer);
    
    return (WARC_TRUE);
    }

    w = bless (WFile, fname, WARC_MAX_SIZE,
               WARC_FILE_READER, WARC_FILE_DETECT_COMPRESSION, prefix);


    unless (w)
      {
      fprintf (stdout, "content-type: text/html\n\n");
      fprintf (stdout, "<html>\n<body>\n"); 
      fprintf (stdout, "<font color=red size = 4>File not found</font><br>");
      w_fclose (f);
      return (WARC_TRUE);
      }

   cmode = WFile_getCompressionMode (w);

   destroy (w);


   if (cmode == WARC_FILE_COMPRESSED_GZIP)
          fprintf (stdout, "content-type: application/x-gzip, \r\n\r\n");
          

    else
          fprintf (stdout, "content-type: text/random, \r\n\r\n");
          

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
    fwrite (buffer, filled, 1, stdout);
 
    }
 
  wfree (buffer);
  w_fclose (f);
  return (WARC_FALSE);

}

/**
 * @param fname: the name of the wanted file
 * @param prefix: the directory leading to the file location
 * @param offset: the offset from where the data will be sent
 *
 * @return False if succeed, True otherwise
 *
 * Sends a Warc Record Response
 */

WPRIVATE warc_bool_t WSend_record (const warc_u8_t * fname, const warc_u8_t * prefix, warc_u32_t offset)
{
    void        *     w         = NIL;
    void        *     record    = NIL;
    warc_u8_t   *     buffer    = NIL;
    warc_u32_t        rsize     = 0  ;
    warc_u32_t        sentsize  = 0  ;
    warc_i32_t        filled    = 0  ;
    wfile_comp_t      cmode    = WARC_FILE_UNCOMPRESSED;


    w = bless (WFile, fname, WARC_MAX_SIZE,
               WARC_FILE_READER, WARC_FILE_DETECT_COMPRESSION, prefix);

    unless (w)
      {
      fprintf (stdout, "content-type: text/html\n\n");
      fprintf (stdout, "<html>\n<body>\n");
      fprintf (stdout,  "<font color=red size = 4>File not found</font><br>");
      return (WARC_TRUE);
      }

    if (WFile_seek (w, offset))
       {
        fprintf (stdout, "content-type: text/html\n\n");
        fprintf (stdout, "<html>\n<body>\n");
        fprintf (stdout, "<font color = red size = 4 > Can not reach the reclaimed offset </font><br>");
        destroy (w);
        return (WARC_TRUE);
       }

    record = WFile_nextRecord (w);

    unless (record)
       {
        fprintf (stdout, "content-type: text/html\n\n");
        fprintf (stdout, "<html>\n<body>\n");
        fprintf (stdout, "<font color = red size = 4 > No records at this offset</font><br>");
        
        destroy (w);
        return (WARC_TRUE);
       } 

    WFile_seek (w, offset);

    buffer = wmalloc (MAX_BUFFER_SIZE);
 
    unless (buffer)
     {
      fprintf (stdout, "content-type: text/html\n\n");
      fprintf (stdout, "<html>\n<body>\n");
      fprintf (stdout, "<font color = red size = 4 >Failed in allocation of memory for respponse sending </font><br>");
      destroy (w);
      destroy (record);
      return (WARC_TRUE);
     }
    
    cmode = WFile_getCompressionMode (w);
    
    if (cmode == WARC_FILE_COMPRESSED_GZIP)
          fprintf (stdout, "content-type: application/x-gzip, \r\n\r\n");
          

    else
          fprintf (stdout, "content-type: text/random, \r\n\r\n");

   
    

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
      WFile_fillBuffer  (w, (char *) buffer, sentsize, & filled);
      fwrite (buffer, filled, 1, stdout);
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
 *
 * @return False if succeed, True otherwise
 *
 * Sends a filtred Warc file Response
 */


WPRIVATE warc_bool_t WSend_filtredResponse (const warc_u8_t * fname, const warc_u8_t * prefix, warc_u32_t offset,
                                const warc_u8_t * fdfilter,const warc_u8_t * filter)
{
  void         *    w         = NIL;
  void         *    record    = NIL;
  void         *    rexpr     = NIL;
  warc_u8_t    *    buffer    = NIL;
  const warc_u8_t    *    hofld     = NIL;
  warc_u32_t        rsize     = 0  ;
  warc_u32_t        sentsize  = 0  ;
  warc_i32_t        filled    = 0  ;
  warc_u32_t        rectype = 0;
  wfile_comp_t      cmode    = WARC_FILE_UNCOMPRESSED;
 
  w = bless (WFile, fname, WARC_MAX_SIZE,
               WARC_FILE_READER, WARC_FILE_DETECT_COMPRESSION, prefix);

  unless (w)
      {
      fprintf (stdout, "content-type: text/html\n\n");
      fprintf (stdout, "<html>\n<body>\n");
      fprintf (stdout,  "<font color=red size = 4>File not found</font><br>");
      return (WARC_TRUE);
      }

  rexpr = bless (WRegexp, makeS (filter));
  unless (rexpr)
      {
      fprintf (stdout, "content-type: text/html\n\n");
      fprintf (stdout, "<html>\n<body>\n");
      fprintf (stdout,  "<font color=red size = 4>Bad filter string</font><br>");
      destroy (w);
      return (WARC_TRUE);
      }

   if (WFile_seek (w, offset))
      {
       fprintf (stdout, "content-type: text/html\n\n");
       fprintf (stdout, "<html>\n<body>\n");
       fprintf (stdout, "<font color = red size = 4 > Can not reach the reclaimed offset </font><br>");
       destroy (w);
       destroy (rexpr);
       return (WARC_TRUE);
      }
   
   cmode = WFile_getCompressionMode (w);

   if (cmode == WARC_FILE_COMPRESSED_GZIP)
               fprintf (stdout, "content-type: application/x-gzip, \r\n\r\n");
          
   else
               fprintf (stdout, "content-type: text/random, \r\n\r\n");

   buffer = wmalloc (MAX_BUFFER_SIZE);
    
   while (WFile_hasMoreRecords (w) )
      {
       unless ( (record = WFile_nextRecord (w) ) )
         {
          fprintf (stdout, "content-type: text/html\n\n");
          fprintf (stdout, "<html>\n<body>\n");
          fprintf (stdout, "<FONT SIZE = 4 COLOR = red>Bad record found</FONT><br>");
          wfree (buffer);
          destroy (w);
          destroy (rexpr);
          return (WARC_TRUE);
         }

       rsize = WRecord_getCompressedSize (record);

       unless (w_strcmp (fdfilter, uS ("uri")))
              hofld =   WRecord_getTargetUri  (record);
       else
         {
         unless (w_strcmp(fdfilter, uS ("contenttype")))
            hofld =   WRecord_getContentType (record);
         else
            {
             rectype =   WRecord_getRecordType (record);
             hofld = uS ("");

             switch (rectype)
                {
                case 0:
                     hofld = uS ("unknownrecord");
                     break;

                case 1:
                     hofld = uS ("warcinfo");
                     break;

                case 2:
                     hofld = uS ("response");
                     break;

                case 3:
                     hofld = uS ("request");
                     break;

                case 4:
                     hofld = uS ("metadata");
                     break;

                case 5:
                     hofld = uS ("revisit");
                     break;
 
                case 6:
                     hofld = uS ("conversion");
                     break;
 
                case 7:
                     hofld = uS ("contunuation");
                     break;

                case 8:
                     hofld = uS ("resource");
                     break;

                default: break;

             }
          }
       }


       if ( (hofld) && (WRegexp_search (rexpr, makeS (hofld))))
          {
           WFile_seek (w, offset);
           offset += rsize;          
 
          unless (buffer)
                 {
                 fprintf (stdout, "content-type: text/html\n\n");
                 fprintf (stdout, "<html>\n<body>\n");
                 fprintf (stdout,  "<font color = red size = 4 >Failed in allocation of memory for respponse sending </font><br>");
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
                WFile_fillBuffer  (w, (char *) buffer, sentsize, & filled);
                fwrite (buffer, filled, 1, stdout);
                }
          }
       else
          offset += rsize;

    destroy (record);
    }

  wfree (buffer);
  destroy (w);
  destroy (rexpr);
  return (WARC_FALSE);
}



/**
 * @param fname: the name of the wanted file
 * @param prefix: the directory leading to the file location
 * @param offset: the offset from where the data will be sent

 * 
 * Sends a distant listing of the records in the warc file
 */

WPRIVATE warc_bool_t WSend_distantHDumpResponse (const warc_u8_t * fname, const warc_u8_t * prefix,
                                                                                     warc_u32_t offset)
{
  void      *     w      = NIL;
  void      *     record = NIL;


  fprintf (stdout, "content-type: text/html\n\n");
  fprintf (stdout, "<html>\n<body>\n");
  fprintf (stdout, "<center> <H2> <FONT COLOR = red>  extract  warc records from  warc file %s </FONT> </H2> </center><br>", fname); 
 

   w = bless (WFile, fname, WARC_MAX_SIZE,
                WARC_FILE_READER, WARC_FILE_DETECT_COMPRESSION, prefix);

  
   unless (w)
      {
      fprintf (stdout, "<FONT SIZE = 4 COLOR = red>  File  not found  </FONT><br>");
      fprintf (stdout, "</body>\n</html>\n");
      return 1;     
      return (WARC_TRUE);
      }
   
   if (WFile_seek (w, offset))
      {
       destroy (w);
      
       fprintf (stdout, "<FONT SIZE = 4 COLOR = red>Bad offset</FONT><br>");
       fprintf (stdout, "</body>\n</html>\n");
       return (WARC_TRUE);
      }
     
    
   while (WFile_hasMoreRecords (w) )
      {
       const void * al = NIL;
       const warc_u8_t * string = NIL;
       warc_u32_t  tlen   = 0  ;
       warc_bool_t m      = WARC_FALSE;

       unless ( (record = WFile_nextRecord (w) ) )
         {
          
          fprintf (stdout, "<FONT SIZE = 4 COLOR = red>Bad record found</FONT><br>");
          fprintf (stdout, "</body>\n</html>\n");
          destroy (w);
          return (WARC_TRUE);
         }



      /*print WHDLine object for this WRecord */
      fprintf (stdout, "<I> <B> <FONT SIZE = 5  COLOR = blue> Warc record that begin at offset  %10d  fields : </FONT> </B> </I>\n\n",
                       (int) WRecord_getOffset (record) );

      fprintf (stdout, "<p> <FONT SIZE = 4 COLOR = purple >  Header line field </FONT>  <p>\n");
      fprintf (stdout, "<BLOCKQUOTE> <TABLE BORDER = 2   BGCOLOR = gray>");
      fprintf (stdout, " <TR>  <TD> <B>  Warc ID  </B> </TD> <TD> %-20s </TD> </TR> \n",        WRecord_getWarcId      (record) );
      fprintf (stdout, "<TR> <TD> <B>  Content-Length  </B> </TD> <TD> %-20d </TD> </TR>\n",        WRecord_getContentLength  (record) );
      fprintf (stdout, "<TR> <TD> <B>  WARC-Type  </B> </TD> <TD> %-20d </TD> </TR>\n",       WRecord_getRecordType  (record) );
      fprintf (stdout, "<TR> <TD> <B>  WARC-Record-ID   </B>   </TD> <TD> %-20s </TD> </TR> \n",     WRecord_getRecordId    (record) );
      fprintf (stdout, "<TR> <TD> <B>  WARC-Date  </B> </TD> <TD> %-20s </TD> </TR>\n",     WRecord_getDate (record));
      fprintf (stdout, "</TABLE> </BLOCKQUOTE>");

      fprintf (stdout, "<font color = darkgreen size 3= > Other fields </font><br>");

      fprintf (stdout, "<BLOCKQUOTE> <TABLE BORDER = 2   BGCOLOR = gray>");

      string = WRecord_getContentType (record);
      if (string)
        {
         fprintf (stdout, " <TR> <TD> <B> Content-Type </B> </TD> <TD> %-20s </TD> </TR>\n", string);
         m = WARC_TRUE;
        }

      string = WRecord_getConcurrentTo (record);
      if (string)
        {
         fprintf (stdout, " <TR> <TD> <B> WARC-Concurrent-To </B> </TD> <TD> %-20s </TD> </TR>\n", string);
         m = WARC_TRUE;
        }

      string = WRecord_getBlockDigest (record);
      if (string)
        {
         fprintf (stdout, " <TR> <TD> <B> WARC-Block-Digest </B> </TD> <TD> %-20s </TD> </TR>\n", string);
         m = WARC_TRUE;
        }

      string = WRecord_getPayloadDigest (record);
      if (string)
        {
         fprintf (stdout, " <TR> <TD> <B> WARC-Payload-Digest </B> </TD> <TD> %-20s </TD> </TR>\n", string);
         m = WARC_TRUE;
        }

      string = WRecord_getIpAddress (record);
      if (string)
        {
         fprintf (stdout, " <TR> <TD> <B> WARC-IP-Address </B> </TD> <TD> %-20s </TD> </TR>\n", string);
         m = WARC_TRUE;
        }

      string = WRecord_getRefersTo (record);
      if (string)
        {
         fprintf (stdout, " <TR> <TD> <B> WARC-Refers-To </B> </TD> <TD> %-20s </TD> </TR>\n", string);
         m = WARC_TRUE;
        }

      string = WRecord_getTargetUri (record);
      if (string)
        {
         fprintf (stdout, "<TR> <TD> <B>  WARC-target-URI  </B> </TD> <TD> %-20s </TD> </TR>\n", string);
         m = WARC_TRUE;
        }

      string = WRecord_getTruncated (record);
      if (string)
        {
         fprintf (stdout, " <TR> <TD> <B> WARC-Truncated </B> </TD> <TD> %-20s </TD> </TR>\n", string);
         m = WARC_TRUE;
        }

      string = WRecord_getWarcInfoId (record);
      if (string)
        {
         fprintf (stdout, " <TR> <TD> <B> WARC-Warcinfo-ID </B> </TD> <TD> %-20s </TD> </TR>\n", string);
         m = WARC_TRUE;
        }

      string = WRecord_getFilename (record);
      if (string)
        {
         fprintf (stdout, " <TR> <TD> <B> WARC-Filename </B> </TD> <TD> %-20s </TD> </TR>\n", string);
         m = WARC_TRUE;
        }

      string = WRecord_getProfile (record);
      if (string)
        {
         fprintf (stdout, " <TR> <TD> <B> WARC-Profile </B> </TD> <TD> %-20s </TD> </TR>\n", string);
         m = WARC_TRUE;
        }

      string = WRecord_getPayloadType (record);
      if (string)
        {
         fprintf (stdout, " <TR> <TD> <B> WARC-Identified-Payload-Type </B> </TD> <TD> %-20s </TD> </TR>\n", string);
         m = WARC_TRUE;
        }

      string = WRecord_getSegmentOriginId (record);
      if (string)
        {
         fprintf (stdout, " <TR> <TD> <B> WARC-Segment-Origin-ID </B> </TD> <TD> %-20s </TD> </TR>\n", string);

         fprintf (stdout, " <TR> <TD> <B> WARC-Segment-Number </B> </TD> <TD> %-20d </TD> </TR>\n",
                     WRecord_getSegmentNumber (record));

         m = WARC_TRUE;
        }

      tlen = WRecord_getSegTotalLength (record);
      if (tlen)
        {
         fprintf (stdout, " <TR> <TD> <B> WARC-Segment-Total-Length </B> </TD> <TD> %-20d </TD> </TR>\n", tlen);

         m = WARC_TRUE;
        }

      unless (m)
         fprintf (stdout, " <TR><TD> --No One-- </TD> </TR>\n");

         fprintf (stdout,  "</TABLE> </BLOCKQUOTE>");
      

          
      /* dump ANVLs */

      if ((al = WRecord_getAnvl (record) ))
         {
          warc_u32_t  i = 0;
          warc_u32_t  j = WList_size (al); /* how many ANVL are there? */
        
         if (j)
           {
            fprintf (stdout, "<font color=darkred size = 4> <b> More informations </b> </font>");
            fprintf (stdout, "<blockquote> <table BORDER = 2 bgcolor=lightgray>");
       
            while ( i < j )
               {
                 const void  * a = WList_getElement (al, i); /* ANVL record */
   
                 /* we assume here that the ANVL value was in ASCII. */
                /* use your own encoding to print it otherwise. */
                fprintf (stdout, "<tr><td><font color=darkgreen size = 3> %s </font></td><td>%s</td></tr>",
                                             (const char *) WAnvl_getKey (a), (const char *) WAnvl_getValue (a) );

                ++ i;
               }
            fprintf (stdout, "</table></blockquote><br><br>");
           }
        }

     destroy (record);
    }

  return (WARC_FALSE);
}

/**
 * @param fname: the name of the wanted file
 * @param prefix: the directory leading to the file location
 * @param offset: the offset from where the data will be sent

 * 
 * Sends a distant listing of the records in the warc file in xml format
 */

WPRIVATE warc_bool_t WSend_distantXDumpResponse (const warc_u8_t * fname, const warc_u8_t * prefix,
                                                                                     warc_u32_t offset)
{
  void      *     w      = NIL;
  void      *     record = NIL;
  warc_u8_t        xmlstr[256];
  warc_u32_t cpt     =  0;
 

   w = bless (WFile, fname, WARC_MAX_SIZE,
                WARC_FILE_READER,  WARC_FILE_DETECT_COMPRESSION, prefix);

  

  
   unless (w)
      {
      fprintf (stdout, "content-type: text/html\n\n");
      fprintf (stdout, "<html>\n<body>\n");
      fprintf (stdout, "<FONT SIZE = 4 COLOR = red>  File  not found  </FONT><br>");
      return (WARC_TRUE);
      }
   

 

   if (WFile_seek (w, offset))
      {
       destroy (w);
       fprintf (stdout, "content-type: text/html\n\n");
       fprintf (stdout, "<html>\n<body>\n");
       fprintf (stdout, "<FONT SIZE = 4 COLOR = red>Bad offset</FONT><br>");
       return (WARC_TRUE);
      }

 

  fprintf (stdout, "content-type : text/xml, \r\n\r\n");
  fprintf (stdout, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
  
  fprintf (stdout, "<warcfile name=\"%s\" size=\"%d\">\r\n", (const char *) fname, (int)WFile_getFileSize (w) );   
   
    
   while (WFile_hasMoreRecords (w) )
      {
       const void * al = NIL;
       const warc_u8_t * string = NIL;
       warc_u32_t  tlen   = 0  ;
       warc_bool_t m      = WARC_FALSE;

       unless ( (record = WFile_nextRecord (w) ) )
         {
          fprintf (stdout, "content-type: text/html\n\n");
          fprintf (stdout, "<html>\n<body>\n");   
          fprintf (stdout, "<FONT SIZE = 4 COLOR = red>Bad record found</FONT><br>");
          destroy (w);
          return (WARC_TRUE);
         }
      cpt ++;
      fprintf ( stdout, "<warc-record rank=\"%d\" ", cpt );
      fprintf (stdout, "offset=\"%d\" ", (int) WRecord_getOffset (record) );
      fprintf (stdout,  "compressed-size=\"%d\" ", (int) WRecord_getCompressedSize (record) );
      fprintf (stdout, "uncompressed-size=\"%d\" ", (int) WRecord_getUncompressedSize (record) );
      xml_recode (makeS (WRecord_getWarcId (record)), xmlstr);
      fprintf (stdout, "warc-id=\"%s\" ", (const char *) xmlstr);
      fprintf (stdout, "content-length=\"%u\" ", WRecord_getContentLength  (record) );
      fprintf (stdout,  "warc-type=\"%u\" ", WRecord_getRecordType  (record) );
      xml_recode (makeS (WRecord_getDate (record)), xmlstr);
      fprintf (stdout, "warc-date=\"%s\" ", (const char *) xmlstr );
      xml_recode ( makeS (WRecord_getRecordId (record)), xmlstr);
      fprintf (stdout, "warc-record-id=\"%s\">\r\n", (const char *) xmlstr);
      
      string = WRecord_getContentType (record);
      if (string)
        {
         xml_recode ( makeS (string), xmlstr);
         fprintf (stdout, "<content-type value=\"%s\"/>\r\n", (const char *) xmlstr);
         m = WARC_TRUE;
        }

      string = WRecord_getConcurrentTo (record);
      if (string)
        {
         xml_recode ( makeS (string), xmlstr);
         fprintf (stdout,  "<warc-concurrent-to value=\"%s\"/>\r\n", (const char *) xmlstr);
         m = WARC_TRUE;
        }

      string = WRecord_getBlockDigest (record);
      if (string)
        {
         xml_recode ( makeS (string), xmlstr);
         fprintf (stdout, "<warc-block-digest value=\"%s\"/>\r\n", (const char *) xmlstr);
         m = WARC_TRUE;
        }

      string = WRecord_getPayloadDigest (record);
      if (string)
        {
         xml_recode ( makeS (string), xmlstr);
         fprintf (stdout, "<warc-payload-digest value=\"%s\"/>\r\n", (const char *) xmlstr);
         m = WARC_TRUE;
        }

      string = WRecord_getIpAddress (record);
      if (string)
        {
         xml_recode ( makeS (string), xmlstr);
         fprintf (stdout,  "<warc-ip-address value=\"%s\"/>\r\n", (const char *) xmlstr);
         m = WARC_TRUE;
        }

      string = WRecord_getRefersTo (record);
      if (string)
        {
         xml_recode ( makeS (string), xmlstr);
         fprintf (stdout,  "<warc-refers-to value=\"%s\"/>\r\n", (const char *) xmlstr);
         m = WARC_TRUE;
        }

      string = WRecord_getTargetUri (record);
      if (string)
        {
         xml_recode ( makeS (string), xmlstr);
         fprintf (stdout, "<warc-taget-uri value=\"%s\"/>\r\n", (const char *) xmlstr );
         m = WARC_TRUE;
        }

      string = WRecord_getTruncated (record);
      if (string)
        {
         xml_recode ( makeS (string), xmlstr);
         fprintf (stdout, "<warc-truncated value=\"%s\"/>\r\n", (const char *) xmlstr);
         m = WARC_TRUE; 
        }

      string = WRecord_getWarcInfoId (record);
      if (string)
        {
         xml_recode ( makeS (string), xmlstr);
         fprintf (stdout, "<warc-filename value=\"%s\"/>\r\n", (const char *) xmlstr);
         m = WARC_TRUE;
        }

      string = WRecord_getFilename (record);
      if (string)
        {
         xml_recode ( makeS (string), xmlstr);
         fprintf (stdout,  "<warc-filename value=\"%s\"/>\r\n", (const char *) xmlstr);
         m = WARC_TRUE;
        }

      string = WRecord_getProfile (record);
      if (string)
        {
         xml_recode ( makeS (string), xmlstr); 
         fprintf (stdout, "<warc-profile value=\"%s\"/>\r\n", (const char *) xmlstr);
         m = WARC_TRUE;
        }

      string = WRecord_getPayloadType (record);
      if (string)
        {
         xml_recode ( makeS (string), xmlstr); 
         fprintf (stdout, "<warc-identrified-payload-type value=\"%s\"/>\r\n", (const char *) xmlstr);
         m = WARC_TRUE;
        }

      string = WRecord_getSegmentOriginId (record);
      if (string)
        {
         xml_recode ( makeS (string), xmlstr); 
         fprintf (stdout, "<warc-segment-origin-id value=\"%s\"/>\r\n", (const char *) xmlstr);

         fprintf (stdout, "<warc-segment-nummber value=\"%d\"/>\r\n",  WRecord_getSegmentNumber (record));

         m = WARC_TRUE;
        }

      tlen = WRecord_getSegTotalLength (record);
      if (tlen)
        {
         fprintf (stdout, "<warc-segment-total-length value=\"%d\"/>\r\n", tlen);

         m = WARC_TRUE;
        }

     
      

          
      /* dump ANVLs */

      if ((al = WRecord_getAnvl (record) ))
         {
          warc_u32_t  i = 0;
          warc_u32_t  j = WList_size (al); /* how many ANVL are there? */
        
         if (j)
           {
            fprintf(stdout, "<extra-fields>\r\n");
            while ( i < j )
               {
                 const void  * a = WList_getElement (al, i); /* ANVL record */
   
                 /* we assume here that the ANVL value was in ASCII. */
                /* use your own encoding to print it otherwise. */
                fprintf (stdout, "<field key=\"%s\" value=\"%s\"/>", (char *) xml_recode (makeS (WAnvl_getKey (a)), xmlstr), 
                                      (char *) xml_recode (makeS (WAnvl_getValue (a)), xmlstr) );

                ++ i;
               }
            fprintf (stdout, "</extra-fields>\r\n");
           }
        }

     destroy (record);
     fprintf (stdout, "</warc-record>\r\n");
    }
  fprintf (stdout, "</warcfile>\r\n");
  return (WARC_FALSE);
}

/**
 * @param fname: the name of the wanted file
 * @param prefix: the directory leading to the file location
 * @param offset: the offset from where the data will be sent

 * 
 * Sends a distant listing of the records in the warc file in text format
 */

WPRIVATE warc_bool_t WSend_distantTDumpResponse (const warc_u8_t * fname, const warc_u8_t * prefix,
                                                                                     warc_u32_t offset)
{
  void      *     w      = NIL;
  void      *     record = NIL;
 
  warc_u32_t cpt     =  0;
 

   w = bless (WFile, fname, WARC_MAX_SIZE,
                WARC_FILE_READER,  WARC_FILE_DETECT_COMPRESSION, prefix);

  

  
   unless (w)
      {
      fprintf (stdout, "content-type: text/html\n\n");
      fprintf (stdout, "<html>\n<body>\n");
      fprintf (stdout, "<FONT SIZE = 4 COLOR = red>  File  not found  </FONT><br>");
      return (WARC_TRUE);
      }
   

 

   if (WFile_seek (w, offset))
      {
       destroy (w);
       fprintf (stdout, "content-type: text/html\n\n");
       fprintf (stdout, "<html>\n<body>\n");
       fprintf (stdout, "<FONT SIZE = 4 COLOR = red>Bad offset</FONT><br>");
       return (WARC_TRUE);
      }

 

  fprintf (stdout, "content-type : text/plain, \r\n\r\n");
  fprintf (stdout, "warcfile : name=\"%s\" size=\"%d\" \r\n", (const char *) fname , (int)WFile_getFileSize (w) );   
   
    
   while (WFile_hasMoreRecords (w) )
      {
       const void * al = NIL;
       const warc_u8_t * string = NIL;
       warc_u32_t  tlen   = 0  ;
       warc_bool_t m      = WARC_FALSE;

       unless ( (record = WFile_nextRecord (w) ) )
         {
          fprintf (stdout, "content-type: text/html\n\n");
          fprintf (stdout, "<html>\n<body>\n");   
          fprintf (stdout, "<FONT SIZE = 4 COLOR = red>Bad record found</FONT><br>");
          destroy (w);
          return (WARC_TRUE);
         }
      cpt ++;
      fprintf ( stdout, "- Warc Record: %d\r\n", cpt);
      fprintf (stdout,  "\t* Offset: %d\r\n" , (int) WRecord_getOffset (record) );
      fprintf (stdout,  "\t* Compressed size: %d\r\n", (int) WRecord_getCompressedSize (record) );
      fprintf (stdout,  "\t* Warc ID: %s\r\n", (const char *)  WRecord_getWarcId (record));
      fprintf (stdout,  "\t* Content-Length: %u\r\n", WRecord_getContentLength  (record) );
      fprintf (stdout,  "\t* WARC-Type: %u\r\n", WRecord_getRecordType  (record) );
      fprintf (stdout,  "\t* WARC-Date: %s\r\n", (const char *)  WRecord_getDate (record) );
      fprintf (stdout,  "\t* WARC-Record-ID: %s\r\n", (const char *) WRecord_getRecordId (record)  );
      
      string = WRecord_getContentType (record);
      if (string)
        {
         fprintf (stdout, "\t\t@ Content-Type: %s\r\n", (const char *) string);
         m = WARC_TRUE;
        }

      string = WRecord_getConcurrentTo (record);
      if (string)
        {
         fprintf (stdout,  "\t\t@ WARC-Concurrent-To: %s\r\n", (const char *) string);
         m = WARC_TRUE;
        }

      string = WRecord_getBlockDigest (record);
      if (string)
        {
         fprintf (stdout,  "\t\t@ WARC-Block-Digest: %s\r\n", (const char *) string);
         m = WARC_TRUE;
        }

      string = WRecord_getPayloadDigest (record);
      if (string)
        {
         fprintf (stdout,"\t\t@ WARC-Block-Digest: %s\r\n" , (const char *) string);
         m = WARC_TRUE;
        }

      string = WRecord_getIpAddress (record);
      if (string)
        {
         fprintf (stdout, "\t\t@ WARC-Payload-Digest: %s\r\n"  , (const char *) string);
         m = WARC_TRUE;
        }

      string = WRecord_getRefersTo (record);
      if (string)
        {
         fprintf (stdout,  "\t\t@ WARC-Refers-To: %s\r\n", (const char *) string);
         m = WARC_TRUE;
        }

      string = WRecord_getTargetUri (record);
      if (string)
        {
         fprintf (stdout, "\t\t@ WARC-Target-URI: %s\r\n", (const char *) string );
         m = WARC_TRUE;
        }

      string = WRecord_getTruncated (record);
      if (string)
        {
         fprintf (stdout,  "\t\t@ WARC-Truncated: %s\r\n", (const char *) string);
         m = WARC_TRUE; 
        }

      string = WRecord_getWarcInfoId (record);
      if (string)
        {
         fprintf (stdout, "\t\t@ WARC-Warcinfo-ID: %s\r\n", (const char *) string);
         m = WARC_TRUE;
        }

      string = WRecord_getFilename (record);
      if (string)
        {
         fprintf (stdout,  "\t\t@ WARC-Filename: %s\r\n", (const char *) string);
         m = WARC_TRUE;
        }

      string = WRecord_getProfile (record);
      if (string)
        {
         fprintf (stdout, "\t\t@ WARC-Profile: %s\r\n", (const char *) string);
         m = WARC_TRUE;
        }

      string = WRecord_getPayloadType (record);
      if (string)
        {
         fprintf (stdout,  "\t\t@ WARC-Identified-Payload-Type: %s\r\n", (const char *) string);
         m = WARC_TRUE;
        }

      string = WRecord_getSegmentOriginId (record);
      if (string)
        {
         fprintf (stdout, "\t\t@ WARC-Segment-Origin-ID: %s\r\n", (const char *) string);

         fprintf (stdout, "\t\t@ WARC-Segment-Number: %d\r\n",  WRecord_getSegmentNumber (record));

         m = WARC_TRUE;
        }

      tlen = WRecord_getSegTotalLength (record);
      if (tlen)
        {
         fprintf (stdout, "\t\t@ WARC-Segment-Total-Length: %d\r\n", tlen);

         m = WARC_TRUE;
        }
      
      unless (m)
         fprintf (stdout,  "---No One---\r\n");

          
      /* dump ANVLs */

      if ((al = WRecord_getAnvl (record) ))
         {
          warc_u32_t  i = 0;
          warc_u32_t  j = WList_size (al); /* how many ANVL are there? */
        
         if (j)
           {
            fprintf (stdout, "\r\n* More information:\r\n");
            while ( i < j )
               {
                const void  * a = WList_getElement (al, i); /* ANVL record */
   
                 /* we assume here that the ANVL value was in ASCII. */
                /* use your own encoding to print it otherwise. */
                fprintf (stdout , "\t\t@ %s: %s\r\n",
                                          (const char *) WAnvl_getKey (a), (const char *) WAnvl_getValue (a) );
                ++ i;
               }
           
           }
        }

     destroy (record);
     
    }
  return (WARC_FALSE);
}

/**
 * @param fname: the name of the wanted file
 * @param prefix: the directory leading to the file location
 * @param offset: the offset from where the data will be sent

 * 
 * Sends a distant listing of the records in the warc file in json format
 */

WPRIVATE warc_bool_t WSend_distantJDumpResponse (const warc_u8_t * fname, const warc_u8_t * prefix,
                                                                                     warc_u32_t offset)
{
  void      *     w      = NIL;
  void      *     record = NIL;
  warc_u8_t      comma[2]    = {'\0','\0'};
  warc_bool_t    first       = WARC_TRUE;
  warc_bool_t    firstr      = WARC_TRUE;
  warc_u32_t cpt     =  0;
 

   w = bless (WFile, fname, WARC_MAX_SIZE,
                WARC_FILE_READER,  WARC_FILE_DETECT_COMPRESSION, prefix);

  

  
   unless (w)
      {
      fprintf (stdout, "content-type: text/html\n\n");
      fprintf (stdout, "<html>\n<body>\n");
      fprintf (stdout, "<FONT SIZE = 4 COLOR = red>  File  not found  </FONT><br>");
      return (WARC_TRUE);
      }
   

 

   if (WFile_seek (w, offset))
      {
       destroy (w);
       fprintf (stdout, "content-type: text/html\n\n");
       fprintf (stdout, "<html>\n<body>\n");
       fprintf (stdout, "<FONT SIZE = 4 COLOR = red>Bad offset</FONT><br>");
       return (WARC_TRUE);
      }

 

  fprintf (stdout, "content-type : text/json\r\n\r\n");   
  fprintf (stdout, "{\"warcfile\":[{\"name\":\"%s\"},{\"size\":\"%d\"},{\"records\":[", (const char *) (fname), 
                                                                 (int)WFile_getFileSize (w)); 
    
   while (WFile_hasMoreRecords (w) )
      {
       const void * al = NIL;
       const warc_u8_t * string = NIL;
       warc_u32_t  tlen   = 0  ;

       unless ( (record = WFile_nextRecord (w) ) )
         {
          fprintf (stdout, "content-type: text/html\n\n");
          fprintf (stdout, "<html>\n<body>\n");   
          fprintf (stdout, "<FONT SIZE = 4 COLOR = red>Bad record found</FONT><br>");
          destroy (w);
          return (WARC_TRUE);
         }
      cpt ++;
      if (!firstr)
         fprintf (stdout, ",");
      else
         firstr = WARC_FALSE;
      fprintf (stdout, "{\"Warc Record\":[{\"rank\":%d}", cpt);
      fprintf (stdout,  ",{\"Offset\":%d}" , (int) WRecord_getOffset (record) );
      fprintf (stdout,  ",{\"Compressed size\":%d}", (int) WRecord_getCompressedSize (record) );
      fprintf (stdout,  ",{\"fields\":[{\"Warc ID\":\"%s\"}", (const char *)  WRecord_getWarcId (record));
      fprintf (stdout,  ",{\"Content-Length\":%u}", WRecord_getContentLength  (record) );
      fprintf (stdout,  ",{\"WARC-Type\":%u}", WRecord_getRecordType  (record) );
      fprintf (stdout,  ",{\"WARC-Date\":\"%s\"}", (const char *)  WRecord_getDate (record) );
      fprintf (stdout,  ",{\"WARC-Record-ID\":\"%s\"}", (const char *) WRecord_getRecordId (record)  );
      fprintf (stdout,  ",{\"Other Fields\":[");
      string = WRecord_getContentType (record);
      
      if (string)
         {
         fprintf (stdout, "%s{\"Content-Type\":\"%s\"}",
                              (const char *) comma, (const char *) string);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }


      string = WRecord_getConcurrentTo (record);
      
      if (string)
         {
         fprintf (stdout, "%s{\"WARC-Concurrent-To\":\"%s\"}",
                              (const char *) comma, (const char *) string);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }

      string = WRecord_getBlockDigest (record);
      if (string)
         {
         fprintf (stdout, "%s{\"WARC-Block-Digest\":\"%s\"}",
                              (const char *) comma, (const char *) string);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }

      string = WRecord_getPayloadDigest (record);
       if (string)
         {
         fprintf (stdout, "%s{\"WARC-Payload-Digest\":\"%s\"}",
                              (const char *) comma, (const char *) string);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }

      string = WRecord_getIpAddress (record);
      if (string)
         {
         fprintf (stdout,"%s{\"WARC-Ip-Address\":\"%s\"}" ,
                              (const char *) comma, (const char *) string);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }

      string = WRecord_getRefersTo (record);
       if (string)
         {
         fprintf (stdout, "%s{\"WARC-Refers-To\":\"%s\"}",
                              (const char *) comma, (const char *) string);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }

      string = WRecord_getTargetUri (record);
      if (string)
         {
         fprintf (stdout, "%s{\"WARC-Target-URI\":\"%s\"}",
                              (const char *) comma, (const char *) string);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }

      string = WRecord_getTruncated (record);
      if (string)
         {
         fprintf (stdout, "%s{\"WARC-Truncated\":\"%s\"}",
                              (const char *) comma, (const char *) string);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }

      string = WRecord_getWarcInfoId (record);
      if (string)
         {
         fprintf (stdout, "%s{\"WARC-Warcinfo-ID\":\"%s\"}",
                              (const char *) comma, (const char *) string);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }

      string = WRecord_getFilename (record);
      if (string)
         {
         fprintf (stdout, "%s{\"WARC-Filename\":\"%s\"}",
                              (const char *) comma, (const char *) string);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }
      string = WRecord_getProfile (record);
       if (string)
         {
         fprintf (stdout, "%s{\"WARC-Profile\":\"%s\"}",
                              (const char *) comma, (const char *) string);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }

     string = WRecord_getPayloadType (record);
     if (string)
         {
         fprintf (stdout, "%s{\"WARC-Payload-Type\":\"%s\"}",
                              (const char *) comma, (const char *) string);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }


     string = WRecord_getSegmentOriginId (record);
     if (string)
         {
         fprintf (stdout, "%s{\"WARC-Segment-Origin-ID\":\"%s\"},",
                              (const char *) comma, (const char *) string);

         fprintf (stdout, "{\"WARC-Segment-Number\":%d}",
                             WRecord_getSegmentNumber (record));
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         } 
        

      tlen = WRecord_getSegTotalLength (record);
      if (tlen)
         {
         fprintf (stdout, "%s{\"WARC-Segment-Total-Length\":%d}",
                              (const char *) comma, tlen);
         if (first)
            {
            first = WARC_FALSE;
            comma [0] = ',';
            }
         }
      
       fprintf (stdout, "]}");

          
      /* dump ANVLs */

      if ((al = WRecord_getAnvl (record) ))
         {
          warc_u32_t  i = 0;
          warc_u32_t  j = WList_size (al); /* how many ANVL are there? */
        
         if (j)
           {
            fprintf (stdout, ",{\"Extra Fields\":[");
            first = WARC_TRUE;
             comma[0] = '\0';

            while ( i < j )
               {
                const void  * a = WList_getElement (al, i); /* ANVL record */
   
                 /* we assume here that the ANVL value was in ASCII. */
                /* use your own encoding to print it otherwise. */
                 fprintf (stdout , "%s{\"%s\":\"%s\"}",
                                          (const char *) comma, (const char *) WAnvl_getKey (a), (const char *) WAnvl_getValue (a) );

                if (first)
                   {
                   first = WARC_FALSE;
                   comma[0] = ',';
                   }
                ++ i;
               }
             fprintf (stdout, "]}");
           }
        }
     fprintf (stdout, "]}]}");
     first = WARC_TRUE;
     comma[0] = '\0';
     
     destroy (record);
     
    }
  fprintf (stdout, "]}]}");
  return (WARC_FALSE);
}





int main()
{

    const warc_u8_t   *   reqnature;
    const warc_u8_t   *   fname ;
    const warc_u8_t   *   ofrec;
    void         *   objfile;
    void         *   objrec;
    void         *   objnature;

    void            *  objwrc;
    const warc_u8_t *  wrc;
    void            *  objver;
    const warc_u8_t *  version;
   
    warc_i32_t       offset    = -1;

    while(FCGI_Accept() >= 0) {

    warc_u32_t   pos             = w_strlen (uS (getenv ("SCRIPT_NAME"))) + 1;
    warc_u8_t         *   prefix    = uS (getenv ("DOCUMENT_ROOT"));
    const    char  * r = getenv ("REQUEST_URI");

    objwrc   = recoverUriField (r, & pos);
    wrc      = WString_getText (objwrc);
    if ( w_strcmp (wrc, uS ("WARC")))
       {
          fprintf (stdout, "content-type: text/html\n\n");
          fprintf (stdout, "<html>\n<body>\n");
          fprintf (stdout, "<FONT SIZE = 4 COLOR = blue> The first parameter of request must be constant 'WARC' </FONT>\n<br>");
          fprintf (stdout, "</body>\n</html>\n");
          return 1;
       }
    destroy (objwrc);

    objver   = recoverUriField (r, & pos);
    version      = WString_getText (objver);
    if ( w_strcmp (version, uS ("0.18")))
       {
          fprintf (stdout, "content-type: text/html\n\n");
          fprintf (stdout, "<html>\n<body>\n");
          fprintf (stdout, "<FONT SIZE = 4 COLOR = blue> The second parameter of request must be a constant '0.18' actual version of warc format </FONT>\n<br>");
          fprintf (stdout, "</body>\n</html>\n");
          return 1;
       }
    destroy (objver);
    
   
    objnature = recoverUriField (r, & pos);
    reqnature = WString_getText (objnature);
    
    if ( w_strcmp (reqnature, uS ("file")))
          if ( w_strcmp (reqnature, uS ("record")))
               if (  w_strcmp (reqnature, uS ("list")))
                  
                   if (  w_strcmp (reqnature, uS ("filter")))
                      {
                      destroy (objnature);
                      fprintf (stdout, "content-type: text/html\n\n");
                      fprintf (stdout, "<html>\n<body>\n");
                      fprintf (stdout, "<FONT SIZE = 4 COLOR = blue> Nature of request not recognized </FONT>\n<br>");
                      fprintf (stdout, "</body>\n</html>\n");
                      return 1;
                      }
   
         
   objrec = recoverUriField (r, & pos);
   ofrec  = WString_getText(objrec);
    
   if ( w_atou(ofrec, w_strlen (ofrec), (warc_u32_t *) & offset))
      {
        destroy (objrec);
        destroy (objnature);
        fprintf (stdout, "content-type: text/html\n\n");
        fprintf (stdout, "<html>\n<body>\n");
        fprintf (stdout, "<FONT SIZE = 4 COLOR = red> offset not digit </font>\n<br>");
        fprintf (stdout, "</body>\n</html>\n");
        return 1;
      }

   unless (w_strcmp (reqnature, uS ("filter")))
       {
        void         *   objfdfilter;
        void         *   objfilter;
        const warc_u8_t   *   fdfilter;
        const warc_u8_t   *   filter;    
 
 
        
        objfdfilter =  recoverUriField (r, & pos);
        fdfilter = WString_getText (objfdfilter);

        if (w_strcmp (fdfilter, uS ("uri")))
              if (w_strcmp (fdfilter, uS ("contenttype")))
                    if (w_strcmp (fdfilter, uS ("recordtype")))
                        {
                        fprintf (stdout, "content-type: text/html\n\n");
                        fprintf (stdout, "<html>\n<body>\n");
                        destroy (objrec);
                        destroy (objnature);
                        destroy (objfdfilter);
                        fprintf (stdout, "<FONT SIZE = 4 COLOR = red> can not apply this filter: field to filter must be content type or uri or record type </font>\n<br>");
                        fprintf (stdout, "</body>\n</html>\n");
                        return 1;
                        }
             
        objfilter =  recoverUriField (r, & pos);
        filter = WString_getText (objfilter);
       
        objfile = recoverFName (r, & pos);
        fname =   WString_getText(objfile);

     
      if (WSend_filtredResponse (fname, prefix, offset, fdfilter, filter))
         {
         fprintf (stdout, "<font color=red size=4>Filtred File sending aborted</font><br>");
         fprintf (stdout, "</body>\n</html>\n");
         }      

      
     

      destroy (objfile);
      destroy (objrec);
      destroy (objnature);
      destroy (objfilter);
      destroy (objfdfilter);
      return 0;

      }  
   unless ( w_strcmp (reqnature, uS ("list")))
      {
     
      void         *   objoutpout;
      const warc_u8_t   *   outpout;

      objoutpout =  recoverUriField (r, & pos);
      outpout =     WString_getText (objoutpout);

        if (w_strcmp (outpout, uS ("text")))
              if (w_strcmp (outpout, uS ("html")))
                    if (w_strcmp (outpout, uS ("xml")))
                        if (w_strcmp (outpout, uS ("json")))
                             {
                             fprintf (stdout, "content-type: text/html\n\n");
                             fprintf (stdout, "<html>\n<body>\n");
                      
                             destroy (objrec);
                             destroy (objnature);
                             destroy (objoutpout);
                             fprintf (stdout, "<FONT SIZE = 4 COLOR = red> The format of outpout is text, html, json or xml </font>\n<br>");
                             fprintf (stdout, "</body>\n</html>\n");
                             return 1;
                             }
             
       
        objfile = recoverFName (r, & pos);
        fname =   WString_getText(objfile);
      unless ( w_strcmp (outpout, uS ("html")))
         {

         if (WSend_distantHDumpResponse (fname, prefix, offset))

           {
              fprintf (stdout, "<font color=red size=4> Records listing aborted</font><br>");
              fprintf (stdout, "</body>\n</html>\n");
           }
          }


      unless (w_strcmp (outpout, uS ("text")))
         {
          if (WSend_distantTDumpResponse (fname, prefix, offset))

           {
              fprintf (stdout, "<font color=red size=4> Records listing aborted</font><br>");
              fprintf (stdout, "</body>\n</html>\n");
           }
         
         }

       unless (w_strcmp (outpout, uS ("xml")))
         {
          if (WSend_distantXDumpResponse (fname, prefix, offset))

           {
              fprintf (stdout, "<font color=red size=4> Records listing aborted</font><br>");
              fprintf (stdout, "</body>\n</html>\n");
           }
         
         }

       unless (w_strcmp (outpout, uS ("json")))
         {
          if (WSend_distantJDumpResponse (fname, prefix, offset))

           {
              fprintf (stdout, "<font color=red size=4> Records listing aborted</font><br>");
              fprintf (stdout, "</body>\n</html>\n");
           }
         
         }


     destroy (objrec);
   
     destroy (objfile);
     destroy (objnature);
     return 1;
    
       } 
 
  objfile = recoverFName (r , & pos);
  fname =   WString_getText(objfile);
 
  

 /* we want here te recover a whole warc file */
  unless (w_strcmp (reqnature, uS ("file")))
      {
         
     
      
      if (WSend_fullResponse (fname, prefix, offset))
         {
          
          fprintf (stdout, "<font color=red size=4> File sending aborted</font><br>");
          fprintf (stdout, "</body>\n</html>\n");
         }

    destroy (objrec);
    destroy (objfile);
    destroy (objnature);
    return 1;
    
   } 


  /* here, we wante only to send one record */
  unless (w_strcmp (reqnature, uS ("record")))
    {
     

    if (WSend_record (fname, prefix, offset))
         {

          fprintf (stdout, "<font color=red size=4> Record sending aborted</font><br>");
          fprintf (stdout, "</body>\n</html>\n");
          return 0;
         }

    destroy (objfile);
    destroy (objrec);
    destroy (objnature);

  return 0;
  }


  

 destroy (objrec);
 destroy (objfile);
 destroy (objnature); 

}
 return 0;
}
