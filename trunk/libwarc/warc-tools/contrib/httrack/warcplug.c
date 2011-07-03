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

#include <sys/time.h>  /* strftime */
#include <stdio.h>     /* sprintf */

#include <warc.h>      /* WARC headers */
#include <wmem.h>      /* wcalloc, wfree */

#include <warcplug.h>
#include <warctime.h>  /* warcTimeStamp */

#define makeS(s) (s), w_strlen(s)
#define uS(s) ((warc_u8_t *) (s))

struct WPlugin {
  void * w;
  void * ip;
  void * url;
  void * mime;
  void * prefix;
  warc_u32_t cnt;
};


WPUBLIC warc_bool_t makeNewWARC (void * _wst)
{
  struct WPlugin * wst  = (struct WPlugin *) _wst;
  char           * name = NIL;

  /* check for integer overflow */
  if (wst -> cnt + 1 <= wst -> cnt)
    return (WARC_TRUE);
  
  /* ... + 10 because any 32bits can be coded on 10 positions at max */
  /* ... + 8  because of ".warc.gz" */
  name = wmalloc (sizeof(char) * WString_getLength(wst -> prefix) + 10 + 1 + 8);
  unless (name)
    return (WARC_TRUE);
  
  sprintf(name, "%s%d.warc.gz", WString_getText(wst -> prefix), wst -> cnt);

  wst -> cnt ++;

  /* make a WFile object */
  if (wst -> w) /* close the old one if any */
    destroy (wst -> w);

  WarcDebugMsg("creating a new WARC file");

  wst -> w = bless (WFile, name, HTTRACK_WARC_SIZE, WARC_FILE_WRITER, 
                    WARC_FILE_COMPRESSED_GZIP, HTTRACK_DEFAULT_TMPDIR);

  wfree(name), name = NIL;

  unless (wst -> w)
      return (WARC_TRUE);

  return (WARC_FALSE);
}

WPUBLIC void * blessWARC (const char * prefix)
{
  struct WPlugin * wst  = NIL;
  
  /* check WARC name prefix */
  unless (prefix)
    return (NIL);
  
  wst = wmalloc (sizeof(struct WPlugin));
  unless (wst)
    return (NIL);
  
  wst -> cnt = 0;

  /* this "bless" need to be before the "bless (WFile, ...)" below */
  wst -> prefix = bless (WString, makeS(uS(prefix)));
  unless (wst -> prefix)
    {
     destroyWARC (wst);
     return (NIL);
    }
  
  /* make WARC name */
  wst -> w = NIL;
  if (makeNewWARC (wst))
  {
    destroyWARC (wst);
    return (NIL);
  }
  
  wst -> url = bless (WString, makeS(uS("")));
  unless (wst -> url)
    {
      destroyWARC (wst);
      return (NIL);
    }
  
  wst -> mime = bless (WString, makeS(uS("")));
  unless (wst -> mime)
    {
      destroyWARC (wst);
      return (NIL);
    }
  
  wst -> ip = bless (WString, makeS(uS("")));
  unless (wst -> ip)
    {
      destroyWARC (wst);
      return (NIL);
    }

  return (wst);
}


WPUBLIC void setIP (const char * ip, void * _wst)
{
  struct WPlugin * wst = (struct WPlugin *) _wst;
  
  WString_setText(wst -> ip, makeS(uS(ip)));
}


WPUBLIC void setMIME (const char * mime, void * _wst)
{
  struct WPlugin * wst = (struct WPlugin *) _wst;
  
  WString_setText(wst -> mime, makeS(uS(mime)));
}


WPUBLIC void setURL (const char * domain, const char * file, void * _wst)
{
  struct WPlugin * wst = (struct WPlugin *) _wst;
  
  WString_setText(wst -> url, makeS(uS(domain)));
  WString_append (wst -> url, makeS(uS(file)));
}


WPUBLIC void  destroyWARC (void * _wst)
{
  struct WPlugin * wst = (struct WPlugin *) _wst;

  unless (wst)
    return;

  if (wst -> ip)
    destroy (wst -> ip);

  if (wst -> url)
    destroy (wst -> url);

  if (wst -> mime)
    destroy (wst -> mime);

  if (wst -> prefix)
    destroy (wst -> prefix);

  if (wst -> w)
    destroy (wst -> w);

  wfree (wst), wst = NIL;
}


WPRIVATE warc_bool_t writeRecord (void * _wst, void * r)
{
  struct WPlugin * wst = (struct WPlugin *) _wst;

  /* try to store the WARC record */
  if (WFile_storeRecord (wst -> w, r))
    {
      /* serious issue, WARC isn't full and record couldn't be store */
      if ( ! WFile_isFull(wst ->w) ) 
        return (WARC_TRUE);
    }
  else
    return (WARC_FALSE);

  /* try to save the WARC record in a new WARC file */
  if (makeNewWARC (wst) || WFile_storeRecord (wst -> w, r))
    return (WARC_TRUE);

  return (WARC_FALSE);
}


WPUBLIC warc_bool_t writeResource (const char * mime, const char * ip,
                                   const char * file, void * _wst)
{
  struct WPlugin * wst = (struct WPlugin *) _wst;

  void * r = NIL;
  void * u = NIL;

  warc_u8_t timestamp [21]; /* ex. 2008-09-12T17:45:39Z + '\0' */

  r = bless (WRecord);
  unless (r)
    return (WARC_TRUE);

  u = bless (WUUID);

  unless (u)
    {
     destroy (r);
     return (WARC_TRUE);
    }
  
  /* use fallback values for MIME, IP ... */
  unless (WString_getLength(wst -> url))
    return (WARC_TRUE);

  unless (WString_getLength(wst -> mime))
      WString_setText(wst -> mime, makeS(uS(mime)));

  UNUSED (ip);
/*   unless (WString_getLength(wst -> ip)) */
/*       WString_setText(wst -> ip, makeS(uS(ip))); */

  warcTimeStamp (& (timestamp [0]));

  WRecord_setRecordType  (r, WARC_RESOURCE_RECORD);
  WRecord_setTargetUri   (r, makeS (uS(WString_getText(wst -> url))));
  WRecord_setDate        (r, makeS (uS(timestamp)));
  WRecord_setContentType (r, makeS (uS(WString_getText(wst -> mime))));
 /*  WRecord_setIpAddress   (r, makeS (uS(WString_getText(wst -> ip)))); */

  WUUID_hash (u, makeS(uS(WString_getText(wst -> url))));
  WUUID_hash (u, makeS(uS(timestamp)));

  WRecord_setRecordId(r, makeS (uS(WUUID_text (u))));

  destroy (u);

  if(WRecord_setContentFromFileName (r, file))
    {
      destroy (r);
      return (WARC_TRUE);
    }

  /* attempt to store the WARC record */
  if (writeRecord (wst, r))
    {
      destroy (r);
      destroyWARC (wst);
      return (WARC_TRUE);
    }

  destroy (r);

  return (WARC_FALSE);
}


WPUBLIC warc_bool_t writeRequest (const char * ip, const char * headers, 
                                  void * _wst)
{
  struct WPlugin * wst = (struct WPlugin *) _wst;

  void * r = NIL;
  void * u = NIL;

  warc_u8_t timestamp [21]; /* ex. 2008-09-12T17:45:39Z + '\0' */

  unless (headers)
  {
    /* throw an error, no headers in the request */
    return (WARC_TRUE);
  }
  
  r = bless (WRecord);
  unless (r)
    return (WARC_TRUE);

  u = bless (WUUID);

  unless (u)
  {
    destroy (r);
    return (WARC_TRUE);
  }
  
  /* use fallback values for MIME, IP ... */
  unless (WString_getLength(wst -> url))
    return (WARC_TRUE);

  UNUSED(ip);
/*   unless (WString_getLength(wst -> ip)) */
/*       WString_setText(wst -> ip, makeS(uS(ip))); */

  if (WFile_isFull(wst ->w))
    makeNewWARC (wst);

  warcTimeStamp (& (timestamp [0]));

  WRecord_setRecordType  (r, WARC_REQUEST_RECORD);
  WRecord_setTargetUri   (r, makeS (uS(WString_getText(wst -> url))));
  WRecord_setDate        (r, makeS (uS(timestamp)));
  WRecord_setContentType (r, makeS (uS(WString_getText(wst -> mime))));
 /*  WRecord_setIpAddress   (r, makeS (uS(WString_getText(wst -> ip)))); */

  WUUID_hash (u, makeS(uS(WString_getText(wst -> url))));
  WUUID_hash (u, makeS(uS(timestamp)));

  WRecord_setRecordId(r, makeS (uS(WUUID_text (u))));

  destroy (u);


  WRecord_setContentFromString(r, makeS (uS(headers)));

  /* attempt to store the WARC record */
  if (writeRecord (wst, r))
    {
      destroy (r);
      destroyWARC (wst);
      return (WARC_TRUE);
    }

  destroy (r);

  return (WARC_FALSE);
}


WPUBLIC warc_bool_t writeResponse (const char * ip, const char * headers, 
                                   void * _wst)
{
  struct WPlugin * wst = (struct WPlugin *) _wst;

  void * r = NIL;
  void * u = NIL;

  warc_u8_t timestamp [21]; /* ex. 2008-09-12T17:45:39Z + '\0' */

  unless (headers)
  {
    /* throw an error, no headers in the request */
    return (WARC_TRUE);
  }
  
  r = bless (WRecord);
  unless (r)
    return (WARC_TRUE);

  u = bless (WUUID);

  unless (u)
  {
    destroy (r);
    return (WARC_TRUE);
  }
  
  /* use fallback values for MIME, IP ... */
  unless (WString_getLength(wst -> url))
    return (WARC_TRUE);

  UNUSED(ip);
/*   unless (WString_getLength(wst -> ip)) */
/*       WString_setText(wst -> ip, makeS(uS(ip))); */

  if (WFile_isFull(wst ->w))
    makeNewWARC (wst);

  warcTimeStamp (& (timestamp [0]));

  WRecord_setRecordType  (r, WARC_RESPONSE_RECORD);
  WRecord_setTargetUri   (r, makeS (uS(WString_getText(wst -> url))));
  WRecord_setDate        (r, makeS (uS(timestamp)));
  WRecord_setContentType (r, makeS (uS(WString_getText(wst -> mime))));
 /*  WRecord_setIpAddress   (r, makeS (uS(WString_getText(wst -> ip)))); */

  WUUID_hash (u, makeS(uS(WString_getText(wst -> url))));
  WUUID_hash (u, makeS(uS(timestamp)));

  WRecord_setRecordId(r, makeS (uS(WUUID_text (u))));

  destroy (u);

  WRecord_setContentFromString(r, makeS (uS(headers)));

  /* attempt to store the WARC record */
  if (writeRecord (wst, r))
    {
      destroy (r);
      destroyWARC (wst);
      return (WARC_TRUE);
    }

  destroy (r);

  return (WARC_FALSE);
}
