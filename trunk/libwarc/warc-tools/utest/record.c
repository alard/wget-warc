
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
#include <Basic.h>
#include <Console.h>
#include <Automated.h>
#include <CUnit.h>
#include <CUError.h>
#include <TestDB.h>
#include <TestRun.h>
#include <menu.h>
#include <warc.h>

#include <whdline.h>
#include <wfsmhdl.h>
#include <wfsmanvl.h>
#include <wrecord.x> /* for WRecord_check */

#define makeS(s) ((warc_u8_t *) s), w_strlen((warc_u8_t *) (s))
#define makeU(s) (const warc_u8_t *) (s), (warc_u64_t) w_strlen((warc_u8_t *) (s))
int init_suite1(void) { return 0; }
int clean_suite1(void) { return 0; }
int init_suite2(void) { return 0; }
int clean_suite2(void) { return 0; }
int init_suite3(void) { return 0; }
int clean_suite3(void) { return 0; }
int init_suite4(void) { return 0; }
int clean_suite4(void) { return 0; }
int init_suite5(void) { return 0; }
int clean_suite5(void) { return 0; }
void test1 (void)
{
  
  void       * r   = bless (WRecord);


CU_ASSERT_PTR_NOT_EQUAL(r,NIL);

  if (r)
    {
      WRecord_setTargetUri   (r, makeS ("http://www.w3c.org") );
      WRecord_setDate (r, makeS ("12172007") );
      WRecord_setContentType  (r, makeS ("warcproject/testheaderline") );
      WRecord_setRecordId     (r, makeS ("id://warc-x584jz39") );
      WRecord_setRecordType   (r, WARC_INFO_RECORD);

      /*fprintf (stdout, "WarcId: %-20s\n",       WRecord_getWarcId      (r) );
     CU_FAIL( "ContentLength: %-20d\n",   WRecord_getContentLength  (r) );
     CU_FAIL( "RecordType: %-20d\n",   WRecord_getRecordType  (r) );
     CU_FAIL( "TargetUri: %-20s\n",   WRecord_getTargetUri  (r) );
     CU_FAIL( "CreationDate: %-20s\n", WRecord_getDate (r) );
     CU_FAIL( "ContentType: %-20s\n",  WRecord_getContentType (r) );
     CU_FAIL( "RecordId: %-20s\n",     WRecord_getRecordId    (r) );*/
        CU_ASSERT_STRING_EQUAL(WARC_VERSION,WRecord_getWarcId (r));
        CU_ASSERT(0== WRecord_getContentLength  (r)); 
        CU_ASSERT(1==WRecord_getRecordType  (r) ); 
        CU_ASSERT_STRING_EQUAL("http://www.w3c.org",WRecord_getTargetUri  (r) );
	CU_ASSERT_STRING_EQUAL("",WRecord_getDate (r)  );
	CU_ASSERT_STRING_EQUAL("warcproject/testheaderline",WRecord_getContentType (r) );
	CU_ASSERT_STRING_EQUAL("id://warc-x584jz39",WRecord_getRecordId    (r) ); 
      destroy (r);
    }




 return;
}


void test2 (void)
{
  
  void * r = bless (WRecord);
  void * u = NIL;

 
CU_ASSERT_PTR_NOT_EQUAL(r,NIL);

  if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
     CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);
      return;
    }

  if  (WRecord_setRecordType (r,WARC_REQUEST_RECORD) )
    {
     CU_FAIL( "Corrupted Record type\n");
      destroy (r);
      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
     CU_FAIL( "Corrupted date\n");
      destroy (r);
      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
     CU_FAIL( "Corrupted record ID\n");
      destroy (r);
      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
     CU_FAIL( "Corrupted content type\n");
      destroy (r);
      return;
    }

   if (WRecord_setConcurrentTo (r, makeS ( "concuuid://id1:warc-x584sld56" )))

    {
     CU_FAIL( "Corrupted concurrent to \n");
      destroy (r);
      return;
    }
  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
     CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);
      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
     CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);
      return;
     }

    if (WRecord_setTruncated (r, makeS("time")))
   
     {
     CU_FAIL( "Corrupted Truncated  \n");
      destroy (r);
      return;
     }
     
    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
     CU_FAIL( "Corrupted Profile  \n");
      destroy (r);
      return;
     } 

    if (WRecord_setPayloadType (r, makeS("text/html; charset=unicode-utf8")))

     
     {
     CU_FAIL( "Corrupted PayLoadType  \n");
      destroy (r);
      return;
     }

  WRecord_setContentFromFileName (r, "./version");
  
  unless (WRecord_check (r))
    {
     CU_FAIL( "inocmpatible fields with record type\n");
      destroy (r);
      return;
    }

 CU_PASS( "OK\n");

  destroy (r);

 return;
}

void test3 (void)
{

  void * r = bless (WRecord);
  void * u = NIL;


CU_ASSERT_PTR_NOT_EQUAL(r,NIL);

  if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
     CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);
      return;
    }

  if  (WRecord_setRecordType (r, WARC_RESOURCE_RECORD) )
    {
     CU_FAIL( "Corrupted Record type\n");
      destroy (r);

      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
     CU_FAIL( "Corrupted date\n");
      destroy (r);

      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
     CU_FAIL( "Corrupted record ID\n");
      destroy (r);

      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
     CU_FAIL( "Corrupted content type\n");
      destroy (r);

      return;
    }

   if (WRecord_setConcurrentTo (r, makeS ( "concuuid://id1:warc-x584sld56" )))

    {
     CU_FAIL( "Corrupted concurrent to \n");
      destroy (r);

      return;
    }
  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
     CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);

      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
     CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);

      return;
     }

    if (WRecord_setIpAddress (r, makeS ("192.165.1.0")))
 
     {
     CU_FAIL( "Corrupted Ip Adress Digest to \n");
      destroy (r);

      return;
     }

     
    if (WRecord_setWarcInfoId (r, makeS ("infuuid://id0:warc-x000sl076")))

     {
     CU_FAIL( "Corrupted InfoId  \n");
      destroy (r);

      return;
     }
   
  WRecord_setContentFromFileName (r, "./version");
  unless (WRecord_check (r))
    {
     CU_FAIL( "inocmpatible fields with record type\n");
      destroy (r);
      return;
    }


 CU_PASS( "OK\n");

  destroy (r);

 return;
}

void test4 (void)
{
  

  void * r = bless (WRecord);
  void * u = NIL;

CU_ASSERT_PTR_NOT_EQUAL(r,NIL);

  if  (WRecord_setRecordType (r, WARC_INFO_RECORD) )
    {
     CU_FAIL( "Corrupted Record type\n");
      destroy (r);

      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
     CU_FAIL( "Corrupted date\n");
      destroy (r);

      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
     CU_FAIL( "Corrupted record ID\n");
      destroy (r);

      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
     CU_FAIL( "Corrupted content type\n");
      destroy (r);

      return;
    }

   
  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
     CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);

      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
     CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);

      return;
     }  


    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
     CU_FAIL( "Corrupted Profile  \n");
      destroy (r);

      return;
     }
  WRecord_setContentFromFileName (r, "./version");
  unless (WRecord_check (r))
    {
     CU_FAIL( "inocmpatible fields with record type\n");
      destroy (r);
      return;
    }

 CU_PASS( "OK\n");

  destroy (r);


 return;
}


void test5 (void)
{
  
  void * r = bless (WRecord);
  void * u = NIL;


CU_ASSERT_PTR_NOT_EQUAL(r,NIL);
  if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
     CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);

      return;
    }

  if  (WRecord_setRecordType (r, WARC_REVISIT_RECORD) )
    {
     CU_FAIL( "Corrupted Record type\n");
      destroy (r);

      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
     CU_FAIL( "Corrupted date\n");
      destroy (r);

      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
     CU_FAIL( "Corrupted record ID\n");
      destroy (r);

      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
     CU_FAIL( "Corrupted content type\n");
      destroy (r);

      return;
    }

  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
     CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);

      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
     CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);

      return;
     }

    if (WRecord_setIpAddress (r, makeS ("192.165.1.0")))
 
     {
     CU_FAIL( "Corrupted Ip Adress Digest to \n");
      destroy (r);

      return;
     }

   if (WRecord_setRefersTo (r, makeS ("refuuid://id14:warc-x265sll76")))

     {
     CU_FAIL( "Corrupted Refers to \n");
      destroy (r);

      return;
     }

    if (WRecord_setTruncated (r, makeS("time")))
   
     {
     CU_FAIL( "Corrupted Truncated  \n");
      destroy (r);

      return;
     }
     
 

    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
     CU_FAIL( "Corrupted Profile  \n");
      destroy (r);

      return;
     }

    if (WRecord_setPayloadType (r, makeS("text/html; charset=unicode-utf8")))

     
     {
     CU_FAIL( "Corrupted PayLoadType  \n");
      destroy (r);

      return;
     }
  WRecord_setContentFromFileName (r, "./version");
  unless (WRecord_check (r))
    {
     CU_FAIL( "inocmpatible fields with record type\n");
      destroy (r);
      return;
    }

 CU_PASS( "OK\n");

  destroy (r);


 return;
}


void test6 (void)
{
  

  void * r = bless (WRecord);
  void * u = NIL;


CU_ASSERT_PTR_NOT_EQUAL(r,NIL);

  if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
     CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);

      return;
    }

  if  (WRecord_setRecordType (r, WARC_METADATA_RECORD) )
    {
     CU_FAIL( "Corrupted Record type\n");
      destroy (r);

      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
     CU_FAIL( "Corrupted date\n");
      destroy (r);

      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
     CU_FAIL( "Corrupted record ID\n");
      destroy (r);

      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
     CU_FAIL( "Corrupted content type\n");
      destroy (r);

      return;
    }

   if (WRecord_setConcurrentTo (r, makeS ( "concuuid://id1:warc-x584sld56" )))

    {
     CU_FAIL( "Corrupted concurrent to \n");
      destroy (r);

      return;
    }
  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
     CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);

      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
     CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);

      return;
     }

    if (WRecord_setIpAddress (r, makeS ("192.165.1.0")))
 
     {
     CU_FAIL( "Corrupted Ip Adress Digest to \n");
      destroy (r);

      return;
     }

   if (WRecord_setRefersTo (r, makeS ("refuuid://id14:warc-x265sll76")))

     {
     CU_FAIL( "Corrupted Refers to \n");
      destroy (r);

      return;
     }

     
    if (WRecord_setWarcInfoId (r, makeS ("infuuid://id0:warc-x000sl076")))

     {
     CU_FAIL( "Corrupted InfoId  \n");
      destroy (r);

      return;
     }
   


    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
     CU_FAIL( "Corrupted Profile  \n");
      destroy (r);

      return;
     }
  WRecord_setContentFromFileName (r, "./version");
  unless (WRecord_check (r))
    {
     CU_FAIL( "inocmpatible fields with record type\n");
      destroy (r);
      return;
    }

 CU_PASS( "OK\n");

  destroy (r);


 return;
}

void test7 (void)
{
 

  void * r = bless (WRecord);
  void * u = NIL;

  
CU_ASSERT_PTR_NOT_EQUAL(r,NIL);


  if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
     CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);

      return;
    }

  if  (WRecord_setRecordType (r, WARC_CONVERSION_RECORD) )
    {
     CU_FAIL( "Corrupted Record type\n");
      destroy (r);

      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
     CU_FAIL( "Corrupted date\n");
      destroy (r);

      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
     CU_FAIL( "Corrupted record ID\n");
      destroy (r);

      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
     CU_FAIL( "Corrupted content type\n");
      destroy (r);

      return;
    }
  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
     CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);

      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
     CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);

      return;
     }

    

   if (WRecord_setRefersTo (r, makeS ("refuuid://id14:warc-x265sll76")))

     {
     CU_FAIL( "Corrupted Refers to \n");
      destroy (r);

      return;
     }


     
    if (WRecord_setWarcInfoId (r, makeS ("infuuid://id0:warc-x000sl076")))

     {
     CU_FAIL( "Corrupted InfoId  \n");
      destroy (r);

      return;
     }
   


    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
     CU_FAIL( "Corrupted Profile  \n");
      destroy (r);

      return;
     }
  WRecord_setContentFromFileName (r, "./version");
  unless (WRecord_check (r))
    {
     CU_FAIL( "inocmpatible fields with record type\n");
      destroy (r);
      return;
    }

 CU_PASS( "OK\n");

  destroy (r);


 return;
}


void test8 (void)
{
 
  void * r = bless (WRecord);
  void * u = NIL;

  
CU_ASSERT_PTR_NOT_EQUAL(r,NIL);

  if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
     CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);

      return;
    }

  if  (WRecord_setRecordType (r, WARC_CONTINUATION_RECORD) )
    {
     CU_FAIL( "Corrupted Record type\n");
      destroy (r);

      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
     CU_FAIL( "Corrupted date\n");
      destroy (r);

      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
     CU_FAIL( "Corrupted record ID\n");
      destroy (r);

      destroy (u);
      return;
    }

  destroy (u);

 
  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
     CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);

      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
     CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);

      return;
     }

  
    if (WRecord_setSegmentOriginId (r, makeS ("orgid://w45-pp78-4574")))

     {
     CU_FAIL( "Corrupted Segment Oringin Id  \n");
      destroy (r);

      return;
     }

    if (WRecord_setSegmentNumber (r, 1))
   
     {
     CU_FAIL( "Corrupted SegmentNumber  \n");
      destroy (r);

      return;
     }

    if (WRecord_setSegTotalLength (r, 145500))

     {
     CU_FAIL( "Corrupted TotalLength  \n");
      destroy (r);

      return;
     }  
  WRecord_setContentFromFileName (r, "./version");
  unless (WRecord_check (r))
    {
     CU_FAIL( "inocmpatible fields with record type\n");
      destroy (r);
      return;
    }

 CU_PASS( "OK\n");

  destroy (r);


 return;
}

void test9 (void)
{
  


  void * r = bless (WRecord);
  void * u = NIL;


CU_ASSERT_PTR_NOT_EQUAL(r,NIL);

  if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
     CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);

      return;
    }

  if  (WRecord_setRecordType (r,WARC_REQUEST_RECORD) )
    {
     CU_FAIL( "Corrupted Record type\n");
      destroy (r);

      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
     CU_FAIL( "Corrupted date\n");
      destroy (r);

      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
     CU_FAIL( "Corrupted record ID\n");
      destroy (r);

      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
     CU_FAIL( "Corrupted content type\n");
      destroy (r);

      return;
    }

   if (WRecord_setConcurrentTo (r, makeS ( "concuuid://id1:warc-x584sld56" )))

    {
     CU_FAIL( "Corrupted concurrent to \n");
      destroy (r);

      return;
    }
  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
     CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);

      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
     CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);

      return;
     }


    if (WRecord_setTruncated (r, makeS("time")))
   
     {
     CU_FAIL( "Corrupted Truncated  \n");
      destroy (r);

      return;
     }

   
     if (WRecord_setFilename (r, makeS ("/home/user/warcfile.warc")))

     {
     CU_FAIL( "Corrupted Filename  \n");
      destroy (r);

      return;
     } 

    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
     CU_FAIL( "Corrupted Profile  \n");
      destroy (r);

      return;
     } 

    if (WRecord_setPayloadType (r, makeS("text/html; charset=unicode-utf8")))

     
     {
     CU_FAIL( "Corrupted PayLoadType  \n");
      destroy (r);

      return;
     }


  WRecord_setContentFromFileName (r, "./version");
  unless (WRecord_check (r))
    {
     CU_PASS( "inocmpatible fields with record type\n");
      destroy (r);
      return;
    }

 CU_PASS( "OK\n");

  destroy (r);


 return;
}

void test10 (void)
{
  
 

  void * r = bless (WRecord);
  void * u = NIL;

 
CU_ASSERT_PTR_NOT_EQUAL(r,NIL);

  if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
     CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);

      return;
    }

  if  (WRecord_setRecordType (r, WARC_RESOURCE_RECORD) )
    {
     CU_FAIL( "Corrupted Record type\n");
      destroy (r);

      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
     CU_FAIL( "Corrupted date\n");
      destroy (r);

      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
     CU_FAIL( "Corrupted record ID\n");
      destroy (r);

      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
     CU_FAIL( "Corrupted content type\n");
      destroy (r);

      return;
    }

   if (WRecord_setConcurrentTo (r, makeS ( "concuuid://id1:warc-x584sld56" )))

    {
     CU_FAIL( "Corrupted concurrent to \n");
      destroy (r);

      return;
    }
  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
     CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);

      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
     CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);

      return;
     }

    if (WRecord_setIpAddress (r, makeS ("192.165.1.0")))
 
     {
     CU_FAIL( "Corrupted Ip Adress Digest to \n");
      destroy (r);

      return;
     }

    if (WRecord_setRefersTo (r, makeS ("refuuid://id14:warc-x265sll76")))

     {
     CU_FAIL( "Corrupted Refers to \n");
      destroy (r);

      return;
     }

     
    if (WRecord_setWarcInfoId (r, makeS ("infuuid://id0:warc-x000sl076")))

     {
     CU_FAIL( "Corrupted InfoId  \n");
      destroy (r);

      return;
     }
   

  WRecord_setContentFromFileName (r, "./version");
  unless (WRecord_check (r))
    {
     CU_PASS( "inocmpatible fields with record type\n");
      destroy (r);
      return;
    }

 CU_PASS( "OK\n");

  destroy (r);


 return;
}


void test11 (void)
{
  

  void * r = bless (WRecord);
  void * u = NIL;

CU_ASSERT_PTR_NOT_EQUAL(r,NIL);
  if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
     CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);

      return;
    }

  if  (WRecord_setRecordType (r, WARC_INFO_RECORD) )
    {
     CU_FAIL( "Corrupted Record type\n");
      destroy (r);

      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
     CU_FAIL( "Corrupted date\n");
      destroy (r);

      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
     CU_FAIL( "Corrupted record ID\n");
      destroy (r);

      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
     CU_FAIL( "Corrupted content type\n");
      destroy (r);

      return;
    }


  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
     CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);

      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
     CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);

      return;
     }

   
    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
     CU_FAIL( "Corrupted Profile  \n");
      destroy (r);

      return;
     }
  WRecord_setContentFromFileName (r, "./version");
  unless (WRecord_check (r))
    {
     CU_PASS( "inocmpatible fields with record type\n");
      destroy (r);
      return;
    }

 CU_FAIL( "OK\n");

  destroy (r);


 return;
}

void test12 (void)
{
   

  void * r = bless (WRecord);
  void * u = NIL;


CU_ASSERT_PTR_NOT_EQUAL(r,NIL); 

  if  (WRecord_setRecordType (r, WARC_INFO_RECORD) )
    {
     CU_FAIL( "Corrupted Record type\n");
      destroy (r);

      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
     CU_FAIL( "Corrupted date\n");
      destroy (r);

      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
     CU_FAIL( "Corrupted record ID\n");
      destroy (r);

      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
     CU_FAIL( "Corrupted content type\n");
      destroy (r);

      return;
    }
  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
     CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);

      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
     CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);

      return;
     }

   
    if (WRecord_setWarcInfoId (r, makeS ("infuuid://id0:warc-x000sl076")))

     {
     CU_FAIL( "Corrupted InfoId  \n");
      destroy (r);

      return;
     }
   
    if (WRecord_setFilename (r, makeS ("/home/user/warcfile.warc")))

     {
     CU_FAIL( "Corrupted Filename  \n");
      destroy (r);

      return;
     }

    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
     CU_FAIL( "Corrupted Profile  \n");
      destroy (r);

      return;
     }

  WRecord_setContentFromFileName (r, "./version");
  unless (WRecord_check (r))
    {
     CU_PASS( "inocmpatible fields with record type\n");
      destroy (r);
      return;
    }

 CU_PASS( "OK\n");

  destroy (r);


 return;
}


void test13 (void)
{
 
  void * r = bless (WRecord);
  void * u = NIL;

  
CU_ASSERT_PTR_NOT_EQUAL(r,NIL);

  if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
     CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);

      return;
    }

  if  (WRecord_setRecordType (r, WARC_REVISIT_RECORD) )
    {
     CU_FAIL( "Corrupted Record type\n");
      destroy (r);

      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
     CU_FAIL( "Corrupted date\n");
      destroy (r);

      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
     CU_FAIL( "Corrupted record ID\n");
      destroy (r);

      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
     CU_FAIL( "Corrupted content type\n");
      destroy (r);

      return;
    }


  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
     CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);

      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
     CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);

      return;
     }

    if (WRecord_setIpAddress (r, makeS ("192.165.1.0")))
 
     {
     CU_FAIL( "Corrupted Ip Adress Digest to \n");
      destroy (r);

      return;
     }

   if (WRecord_setRefersTo (r, makeS ("refuuid://id14:warc-x265sll76")))

     {
     CU_FAIL( "Corrupted Refers to \n");
      destroy (r);

      return;
     }

    if (WRecord_setTruncated (r, makeS("time")))
   
     {
     CU_FAIL( "Corrupted Truncated  \n");
      destroy (r);

      return;
     }
     
  

  

    if (WRecord_setPayloadType (r, makeS("text/html; charset=unicode-utf8")))

     
     {
     CU_FAIL( "Corrupted PayLoadType  \n");
      destroy (r);

      return;
     }

  WRecord_setContentFromFileName (r, "./version");
  unless (WRecord_check (r))
    {
     CU_PASS( "inocmpatible fields with record type\n");
      destroy (r);
      return;
    }

 CU_PASS( "OK\n");

  destroy (r);


 return;
}

void test14 (void)
{
 
  void * r = bless (WRecord);
  void * u = NIL;

 
CU_ASSERT_PTR_NOT_EQUAL(r,NIL);

  if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
     CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);

      return;
    }

  if  (WRecord_setRecordType (r, WARC_METADATA_RECORD) )
    {
     CU_FAIL( "Corrupted Record type\n");
      destroy (r);

      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
     CU_FAIL( "Corrupted date\n");
      destroy (r);

      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
     CU_FAIL( "Corrupted record ID\n");
      destroy (r);

      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
     CU_FAIL( "Corrupted content type\n");
      destroy (r);

      return;
    }

   if (WRecord_setConcurrentTo (r, makeS ( "concuuid://id1:warc-x584sld56" )))

    {
     CU_FAIL( "Corrupted concurrent to \n");
      destroy (r);

      return;
    }
  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
     CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);

      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
     CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);

      return;
     }

    if (WRecord_setIpAddress (r, makeS ("192.165.1.0")))
 
     {
     CU_FAIL( "Corrupted Ip Adress Digest to \n");
      destroy (r);

      return;
     }

   if (WRecord_setRefersTo (r, makeS ("refuuid://id14:warc-x265sll76")))

     {
     CU_FAIL( "Corrupted Refers to \n");
      destroy (r);

      return;
     }


     
    if (WRecord_setWarcInfoId (r, makeS ("infuuid://id0:warc-x000sl076")))

     {
     CU_FAIL( "Corrupted InfoId  \n");
      destroy (r);

      return;
     }
   


    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
     CU_FAIL( "Corrupted Profile  \n");
      destroy (r);

      return;
     }

    if (WRecord_setPayloadType (r, makeS("text/html; charset=unicode-utf8")))

     
     {
     CU_FAIL( "Corrupted PayLoadType  \n");
      destroy (r);

      return;
     }

    if (WRecord_setSegmentOriginId (r, makeS ("orgid://w45-pp78-4574")))

     {
     CU_FAIL( "Corrupted Segment Oringin Id  \n");
      destroy (r);

      return;
     }

   
  WRecord_setContentFromFileName (r, "./version");
  unless (WRecord_check (r))
    {
     CU_PASS( "inocmpatible fields with record type\n");
      destroy (r);
      return;
    }

 CU_PASS( "OK\n");

  destroy (r);


 return;
}

void test15 (void)
{

  void * r = bless (WRecord);
  void * u = NIL;

  
CU_ASSERT_PTR_NOT_EQUAL(r,NIL);

  if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
     CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);

      return;
    }

  if  (WRecord_setRecordType (r, WARC_CONVERSION_RECORD) )
    {
     CU_FAIL( "Corrupted Record type\n");
      destroy (r);

      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
     CU_FAIL( "Corrupted date\n");
      destroy (r);

      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
     CU_FAIL( "Corrupted record ID\n");
      destroy (r);

      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
     CU_FAIL( "Corrupted content type\n");
      destroy (r);

      return;
    }
  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
     CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);

      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
     CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);

      return;
     }

    if (WRecord_setIpAddress (r, makeS ("192.165.1.0")))
 
     {
     CU_FAIL( "Corrupted Ip Adress Digest to \n");
      destroy (r);

      return;
     }

   if (WRecord_setRefersTo (r, makeS ("refuuid://id14:warc-x265sll76")))

     {
     CU_FAIL( "Corrupted Refers to \n");
      destroy (r);

      return;
     }


    if (WRecord_setWarcInfoId (r, makeS ("infuuid://id0:warc-x000sl076")))

     {
     CU_FAIL( "Corrupted InfoId  \n");
      destroy (r);

      return;
     }
   


    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
     CU_FAIL( "Corrupted Profile  \n");
      destroy (r);

      return;
     }

  WRecord_setContentFromFileName (r, "./version");
  unless (WRecord_check (r))
    {
     CU_PASS( "inocmpatible fields with record type\n");
      destroy (r);
      return;
    }

 CU_PASS( "OK\n");

  destroy (r);


 return;
}

void test16 (void)
{
 

  void * r = bless (WRecord);
  void * u = NIL;


CU_ASSERT_PTR_NOT_EQUAL(r,NIL);
  if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
     CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);

      return;
    }

  if  (WRecord_setRecordType (r, WARC_CONTINUATION_RECORD) )
    {
     CU_FAIL( "Corrupted Record type\n");
      destroy (r);

      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
     CU_FAIL( "Corrupted date\n");
      destroy (r);

      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
     CU_FAIL( "Corrupted record ID\n");
      destroy (r);

      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
     CU_FAIL( "Corrupted content type\n");
      destroy (r);

      return;
    }
  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
     CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);

      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
     CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);

      return;
     }

  

    if (WRecord_setSegmentOriginId (r, makeS ("orgid://w45-pp78-4574")))

     {
     CU_FAIL( "Corrupted Segment Oringin Id  \n");
      destroy (r);

      return;
     }

    if (WRecord_setSegmentNumber (r, 1))
   
     {
     CU_FAIL( "Corrupted SegmentNumber  \n");
      destroy (r);

      return;
     }

    if (WRecord_setSegTotalLength (r, 145500))

     {
     CU_FAIL( "Corrupted TotalLength  \n");
      destroy (r);

      return;
     }  
  WRecord_setContentFromFileName (r, "./version");
  unless (WRecord_check (r))
    {
     CU_PASS( "inocmpatible fields with record type\n");
      destroy (r);
      return;
    }

 CU_PASS( "OK\n");

  destroy (r);


 return;
}

void test17 (void)
{
 

  void * r = bless (WRecord);
  void * u = NIL;


CU_ASSERT_PTR_NOT_EQUAL(r,NIL);
  if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
     CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);

      return;
    }

  if  (WRecord_setRecordType (r, WARC_CONTINUATION_RECORD) )
    {
     CU_FAIL( "Corrupted Record type\n");
      destroy (r);

      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
     CU_FAIL( "Corrupted date\n");
      destroy (r);

      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
     CU_FAIL( "Corrupted record ID\n");
      destroy (r);

      destroy (u);
      return;
    }

  destroy (u);

 
  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
     CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);

      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
     CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);

      return;
     }


    if (WRecord_setSegmentNumber (r, 1))
   
     {
     CU_FAIL( "Corrupted SegmentNumber  \n");
      destroy (r);

      return;
     }

    if (WRecord_setSegTotalLength (r, 145500))

     {
     CU_FAIL( "Corrupted TotalLength  \n");
      destroy (r);

      return;
     }  
  WRecord_setContentFromFileName (r, "./version");
  unless (WRecord_check (r))
    {
     CU_PASS( "inocmpatible fields with record type\n");
      destroy (r);
      return;
    }

 CU_PASS( "OK\n");

  destroy (r);


 return;
}



void test18 (void)
{
 

  void * r = bless (WRecord);
  warc_u32_t  i = 0;
  warc_u32_t n = 0;
  const warc_u8_t *  key = NIL;
  const warc_u8_t * value = NIL;


CU_ASSERT_PTR_NOT_EQUAL(r,NIL);

  CU_ASSERT_FALSE (WRecord_addAnvl (r, makeS ("Host-Name"), makeS ("the-name")));
  CU_ASSERT_FALSE (WRecord_addAnvl (r, makeS ("Owner"), makeS ("Only-me")));
  CU_ASSERT_FALSE (WRecord_addAnvl (r, makeS ("Exit-Condition"), makeS ("EOF")));
  CU_ASSERT_FALSE (WRecord_addAnvl (r, makeS ("Host-Location"), makeS ("Somwhere-Anywhere")));

  n = WRecord_getAnvlFieldsNumber (r);
 
  CU_ASSERT_EQUAL (n,4);

  while (i < n)
    {
     CU_ASSERT_FALSE (WRecord_getAnvlField (r ,i, & key, & value));
     fprintf (stdout, "Key is: %s, ", (const char *) key);
     fprintf (stdout, "Value is: %s\n", (const char *) value);

    i++;
    }

CU_PASS( "OK\n");

destroy (r);


 return;
}


int main (void)
{
  CU_pSuite pSuite = NULL; 

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("Suite1", init_suite1, clean_suite1);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
                        }

   /* add the tests to the suite */

  if ((NULL == CU_add_test(pSuite, "TEST 1:", test1)) ||
       (NULL == CU_add_test(pSuite, " TEST 2: Filling a record with one valide request record which \
contains all mandatory field and target uri mandatory for \
request, esource, responce and revisit", test2))||
       (NULL == CU_add_test(pSuite, "TEST 3:Filling a valid resource record ", test3))||
	(NULL == CU_add_test(pSuite, "TEST 4:Filline a valid WARC_INFO record having no anvl fields", test4)))
      
   {
      CU_cleanup_registry();
      return CU_get_error();
   } 
 /* add a suite to the registry */
   pSuite = CU_add_suite("Suite2", init_suite2, clean_suite2);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
                        }

   /* add the tests to the suite */

  if ((NULL == CU_add_test(pSuite, "TEST 5:Filling valid revisit record havinng no anvl fields", test5)) ||
       (NULL == CU_add_test(pSuite, "TEST 6:Filling a valid metadata record having no anvl fields ", test6))||
       (NULL == CU_add_test(pSuite, "TEST 7: Filling a valid conversion record ", test7))||
	(NULL == CU_add_test(pSuite, "TEST 8:Filling a valid continuation record", test8)))
      
   {
      CU_cleanup_registry();
      return CU_get_error();
   } 
 /* add a suite to the registry */
   pSuite = CU_add_suite("Suite3", init_suite3, clean_suite3);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
                        }

   /* add the tests to the suite */

  if ((NULL == CU_add_test(pSuite, "TEST 9:Filling a request record sith bad fields", test9)) ||
       (NULL == CU_add_test(pSuite, "TEST 10:Filling a resource record with bad fields ", test10))||
       (NULL == CU_add_test(pSuite, "TEST 11:Filling a WARC_INFO with no WarcInfoId field", test11))||
	(NULL == CU_add_test(pSuite, "TEST 12:Filling a WARC_INFO record with innapropriate fields", test12)))
      
   {
      CU_cleanup_registry();
      return CU_get_error();
   }
 /* add a suite to the registry */
   pSuite = CU_add_suite("Suite4", init_suite4, clean_suite4);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
                        }

   /* add the tests to the suite */

  if ((NULL == CU_add_test(pSuite, "TEST 13:Filling a revisit record with innapropriate fields", test13)) ||
       (NULL == CU_add_test(pSuite, "TEST 14:Filling a metadata record with innapropriate fields ", test14))||
       (NULL == CU_add_test(pSuite, "TEST 15:Filling a conversion record with innapropraite fields", test15))||
	(NULL == CU_add_test(pSuite, "TEST 16:Filling a continuation record with innapropriate fields", test16))||
	(NULL == CU_add_test(pSuite, "TEST 17:Fillng a continuation record an other innapropriate field", test17)))
      
   {
      CU_cleanup_registry();
      return CU_get_error();
   }  

/* add a suite to the registry */
   pSuite = CU_add_suite("Suite5", init_suite5, clean_suite5);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
                        }

   /* add the tests to the suite */

  if ((NULL == CU_add_test(pSuite, "TEST 18: Testing the Anvl fields recovering function", test18)))
      
   {
      CU_cleanup_registry();
      return CU_get_error();
   }  


switch (menu()) 
  {
        case 1: {CU_console_run_tests();break;} 
	case 2:  {
                       
                           case 21: {CU_basic_set_mode(CU_BRM_NORMAL); CU_basic_run_tests(); break;}
                            case 22:{CU_basic_set_mode(CU_BRM_VERBOSE ); CU_basic_run_tests(); break;}
                             case 23:{CU_basic_set_mode(CU_BRM_SILENT); CU_basic_run_tests(); break;}  

                              }
        case 3:{
                CU_set_output_filename("./utest/outputs/record");
    		CU_set_output_filename("./utest/outputs/record" );
  		CU_automated_run_tests();
   		CU_list_tests_to_file();
           	break;}
     
   }
   CU_cleanup_registry();
   return CU_get_error();  
}



