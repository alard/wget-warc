
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
#include <assert.h>
#include <Basic.h>
#include <Console.h>
#include <Automated.h>
#include <CUnit.h>
#include <CUError.h>
#include <TestDB.h>
#include <TestRun.h>
#include <menu.h>

#include <warc.h>

#ifndef WARC_MAX_SIZE
/* 16 Go by default */
#define WARC_MAX_SIZE 17179869184ULL
#endif

#define makeS(s) ((warc_u8_t *) s), w_strlen((warc_u8_t *) (s))
#define makeU(s) (const warc_u8_t *) (s), (warc_u64_t) w_strlen((warc_u8_t *) (s))
int init_suite1(void) {return 0; }
int clean_suite1(void) {return 0; }
int init_suite2(void) {return 0; }
int clean_suite2(void) {return 0; }
int init_suite3(void) {return 0; }
int clean_suite3(void) {return 0; }
int init_suite4(void) {return 0; }
int clean_suite4(void) {return 0; }
int init_suite5(void) {return 0; }
int clean_suite5(void) {return 0; }
int init_suite6(void) {return 0; }
int clean_suite6(void) {return 0; }


warc_bool_t callback (void * env, const char* buff, const warc_u32_t size)
{
  UNUSED (env);
UNUSED (buff);
  if (size)
    {
    /*  warc_u32_t  i = 0;*/

    /*  while (i < size)
        {
          fprintf(stdout, "%c", buff[i]);
          ++i;
        }

      fprintf(stdout, "\n");*/
    }

  return (WARC_TRUE); /* return WARC_FALSE to stop extraction */
}


void test1 (void)
{
 const char * file1 = "./app/wdata/testwfile/unidesc.html";

  void * r = bless (WRecord);
  void * w = bless (WFile, "./awanvl.warc", WARC_MAX_SIZE, 
                    WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED, ".");
  void * u = NIL;

  unless (w)
  {
    fprintf(stderr, "\n+++++++++++++ 1 ptr: %p\n", w);
    exit (15);
  }


  if  (WRecord_setRecordType (r, WARC_INFO_RECORD) )
    {
    CU_FAIL( "Corrupted Record type\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
      CU_FAIL( "Corrupted date\n");
      destroy (r);
      destroy (w);
      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
      CU_FAIL( "Corrupted record ID\n");
      destroy (r);
      destroy (w);
      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
      CU_FAIL( "Corrupted content type\n");
      destroy (r);
      destroy (w);
      return;
    }

    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
      CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
      CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }


    if (WRecord_setTruncated (r, makeS("time")))
   
     {
      CU_FAIL( "Corrupted Truncated  \n");
      destroy (r);
      destroy (w);
      return;
     }

   
    if (WRecord_setFilename (r, makeS ("/home/user/warcfile.warc")))

     {
      CU_FAIL( "Corrupted Filename  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
      CU_FAIL( "Corrupted Profile  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadType (r, makeS("text/html; charset=unicode-utf8")))

     
     {
      CU_FAIL( "Corrupted PayLoadType  \n");
      destroy (r);
      destroy (w);
      return;
     }


  if (WRecord_setContentFromFileName (r, file1) )
    {
      CU_FAIL( "record not filled\n");
      destroy (r);
      destroy (w);
      return;
    }

  if (WFile_storeRecord (w, r) )
    {
      CU_FAIL( "record not stored\n");
      destroy (r);
      destroy (w);
      return;
    }

  CU_PASS( "OK\n");

  destroy (r);
  destroy (w);

  return;
}

void test2 (void)
{

  
  const char * file1 = "./app/wdata/testwfile/unidesc.html";

  void * r = bless (WRecord);
  void * w = bless (WFile, "./awarcavl.warc", WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED, ".");
  void * u = NIL;
fprintf(stdout,"////////////////////////////test 2 /////////////////////////////////\n");


  if  (WRecord_setRecordType (r, WARC_INFO_RECORD) )
    {
      CU_FAIL( "Corrupted Record type\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
      CU_FAIL( "Corrupted date\n");
      destroy (r);
      destroy (w);
      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
      CU_FAIL( "Corrupted record ID\n");
      destroy (r);
      destroy (w);
      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
      CU_FAIL( "Corrupted content type\n");
      destroy (r);
      destroy (w);
      return;
    }

  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
      CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
      CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setTruncated (r, makeS("time")))
   
     {
      CU_FAIL( "Corrupted Truncated  \n");
      destroy (r);
      destroy (w);
      return;
     }
     
    if (WRecord_setWarcInfoId (r, makeS ("infuuid://id0:warc-x000sl076")))

     {
      CU_FAIL( "Corrupted InfoId  \n");
      destroy (r);
      destroy (w);
      return;
     }
   
    if (WRecord_setFilename (r, makeS ("/home/user/warcfile.warc")))

     {
      CU_FAIL( "Corrupted Filename  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
      CU_FAIL( "Corrupted Profile  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadType (r, makeS("text/html; charset=unicode-utf8")))

     
     {
      CU_FAIL( "Corrupted PayLoadType  \n");
      destroy (r);
      destroy (w);
      return;
     }



    if  (WRecord_addAnvl (r, makeS ("key1"), makeS ("val1") ) )
     {
       CU_FAIL( "Corrupted anvl\n");
       destroy (r);
       destroy (w);
       return;
     }  

  if (WRecord_setContentFromFileName (r, file1) )
    {
      CU_FAIL( "record not filled\n");
      destroy (r);
      destroy (w);
      return;
    }

  if (WFile_storeRecord (w, r) )
    {
      CU_PASS( "record not stored\n");
      destroy (r);
      destroy (w);
      return;
    }

  CU_PASS( "OK\n");

  destroy (r);
  destroy (w);

  return;
}

void test3 (void)
{
 
  const char * file1 = "./app/wdata/testwfile/unidesc.html";
  const char * file2 = "./app/wdata/testwfile/anvlcom";

  void * r = bless (WRecord);
  void * w = bless (WFile, "./awarcmlp.warc", WARC_MAX_SIZE,  WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED, ".");
  void * u = NIL;

 


  if  (WRecord_setRecordType (r, WARC_INFO_RECORD) )
    {
      CU_FAIL( "Corrupted Record type\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
      CU_FAIL( "Corrupted date\n");
      destroy (r);
      destroy (w);
      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
      CU_FAIL( "Corrupted record ID\n");
      destroy (r);
      destroy (w);
      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
      CU_FAIL( "Corrupted content type\n");
      destroy (r);
      destroy (w);
      return;
    }

  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
      CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
      CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setTruncated (r, makeS("time")))
   
     {
      CU_FAIL( "Corrupted Truncated  \n");
      destroy (r);
      destroy (w);
      return;
     }
     
  
    if (WRecord_setFilename (r, makeS ("/home/user/warcfile.warc")))

     {
      CU_FAIL( "Corrupted Filename  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
      CU_FAIL( "Corrupted Profile  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadType (r, makeS("text/html; charset=unicode-utf8")))

     
     {
      CU_FAIL( "Corrupted PayLoadType  \n");
      destroy (r);
      destroy (w);
      return;
     }


    if  (WRecord_addAnvl (r, makeS ("key1"), makeS ("val1") ) )
     {
       CU_FAIL( "Corrupted anvl\n");
       destroy (r);
       destroy (w);
       return;
     }  

  if (WRecord_setContentFromFileName (r, file1) )
    {
      CU_FAIL( "record not filled\n");
      destroy (r);
      destroy (w);
      return;
    }

  if (WFile_storeRecord (w, r) )
    {
      CU_FAIL( "record not stored\n");
      destroy (r);
      destroy (w);
      return;
    }

  CU_PASS( "OK\n");

  destroy (r);
  
  r = bless (WRecord);



  if  (WRecord_setRecordType (r, WARC_INFO_RECORD) )
    {
      CU_FAIL( "Corrupted Record type\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
      CU_FAIL( "Corrupted date\n");
      destroy (r);
      destroy (w);
      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
      CU_FAIL( "Corrupted record ID\n");
      destroy (r);
      destroy (w);
      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
      CU_FAIL( "Corrupted content type\n");
      destroy (r);
      destroy (w);
      return;
    }

  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
      CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
      CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setTruncated (r, makeS("time")))
   
     {
      CU_FAIL( "Corrupted Truncated  \n");
      destroy (r);
      destroy (w);
      return;
     }
     
   
    if (WRecord_setFilename (r, makeS ("/home/user/warcfile.warc")))

     {
      CU_FAIL( "Corrupted Filename  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
      CU_FAIL( "Corrupted Profile  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadType (r, makeS("text/html; charset=unicode-utf8")))

     
     {
      CU_FAIL( "Corrupted PayLoadType  \n");
      destroy (r);
      destroy (w);
      return;
     }


    if  (WRecord_addAnvl (r, makeS ("key1"), makeS ("val1") ) )
     {
       CU_FAIL( "Corrupted anvl\n");
       destroy (r);
       destroy (w);
       return;
     }  

  if (WRecord_setContentFromFileName (r, file2) )
    {
      CU_FAIL( "record not filled\n");
      destroy (r);
      destroy (w);
      return;
    }

  if (WFile_storeRecord (w, r) )
    {
      CU_FAIL( "record not stored\n");
      destroy (r);
      destroy (w);
      return;
    }

  CU_PASS( "OK\n");

  destroy (r);
  destroy (w);

  return;
}

void test4 (void)
{
 
  const char * file1 = "./app/wdata/testwfile/unidesc.html";

  void * r = bless (WRecord);
  void * w = bless (WFile, "./awarfdt.warc", WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED, ".");
  void * u = NIL;

 

  if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
      CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setRecordType (r, WARC_INFO_RECORD) )
    {
      CU_FAIL( "Corrupted Record type\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setDate (r, makeS ("200f8-04-0i:55Z") ) )
    {
      CU_PASS( "Corrupted date\n");
      destroy (r);
      destroy (w);
      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
      CU_FAIL( "Corrupted record ID\n");
      destroy (r);
      destroy (w);
      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
      CU_FAIL( "Corrupted content type\n");
      destroy (r);
      destroy (w);
      return;
    }
   if (WRecord_setConcurrentTo (r, makeS ( "concuuid://id1:warc-x584sld56" )))

    {
      CU_FAIL( "Corrupted concurrent to \n");
      destroy (r);
      destroy (w);
      return;
    }
  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
      CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
      CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setIpAddress (r, makeS ("192.165.1.0")))
 
     {
      CU_FAIL( "Corrupted Ip Adress Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setRefersTo (r, makeS ("refuuid://id14:warc-x265sll76")))

     {
      CU_FAIL( "Corrupted Refers to \n");
      destroy (r);
      destroy (w);
      return;
     }
    if (WRecord_setTruncated (r, makeS("time")))
   
     {
      CU_FAIL( "Corrupted Truncated  \n");
      destroy (r);
      destroy (w);
      return;
     }
     
    if (WRecord_setWarcInfoId (r, makeS ("infuuid://id0:warc-x000sl076")))

     {
      CU_FAIL( "Corrupted InfoId  \n");
      destroy (r);
      destroy (w);
      return;
     }
   
    if (WRecord_setFilename (r, makeS ("/home/user/warcfile.warc")))

     {
      CU_FAIL( "Corrupted Filename  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
      CU_FAIL( "Corrupted Profile  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadType (r, makeS("text/html; charset=unicode-utf8")))

     
     {
      CU_FAIL( "Corrupted PayLoadType  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegmentOriginId (r, makeS ("orgid://w45-pp78-4574")))

     {
      CU_FAIL( "Corrupted Segment Oringin Id  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegmentNumber (r, 1))
   
     {
      CU_FAIL( "Corrupted SegmentNumber  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegTotalLength (r, 145500))

     {
      CU_FAIL( "Corrupted TotalLength  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if  (WRecord_addAnvl (r, makeS ("key1"), makeS ("val1") ) )
     {
       CU_FAIL( "Corrupted anvl\n");
       destroy (r);
       destroy (w);
       return;
     }  

  if (WRecord_setContentFromFileName (r, file1) )
    {
      CU_FAIL( "record not filled\n");
      destroy (r);
      destroy (w);
      return;
    }

  if (WFile_storeRecord (w, r) )
    {
      CU_FAIL( "record not stored\n");
      destroy (r);
      destroy (w);
      return;
    }

  CU_FAIL( "OK\n");

  destroy (r);
  
  r = bless (WRecord);

   if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
      CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setRecordType (r, WARC_INFO_RECORD) )
    {
      CU_FAIL( "Corrupted Record type\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
      CU_FAIL( "Corrupted date\n");
      destroy (r);
      destroy (w);
      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
      CU_FAIL( "Corrupted record ID\n");
      destroy (r);
      destroy (w);
      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
      CU_FAIL( "Corrupted content type\n");
      destroy (r);
      destroy (w);
      return;
    }
   if (WRecord_setConcurrentTo (r, makeS ( "concuuid://id1:warc-x584sld56" )))

    {
      CU_FAIL( "Corrupted concurrent to \n");
      destroy (r);
      destroy (w);
      return;
    }
  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
      CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
      CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setIpAddress (r, makeS ("192.165.1.0")))
 
     {
      CU_FAIL( "Corrupted Ip Adress Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setRefersTo (r, makeS ("refuuid://id14:warc-x265sll76")))

     {
      CU_FAIL( "Corrupted Refers to \n");
      destroy (r);
      destroy (w);
      return;
     }
    if (WRecord_setTruncated (r, makeS("time")))
   
     {
      CU_FAIL( "Corrupted Truncated  \n");
      destroy (r);
      destroy (w);
      return;
     }
     
    if (WRecord_setWarcInfoId (r, makeS ("infuuid://id0:warc-x000sl076")))

     {
      CU_FAIL( "Corrupted InfoId  \n");
      destroy (r);
      destroy (w);
      return;
     }
   
    if (WRecord_setFilename (r, makeS ("/home/user/warcfile.warc")))

     {
      CU_FAIL( "Corrupted Filename  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
      CU_FAIL( "Corrupted Profile  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadType (r, makeS("text/html; charset=unicode-utf8")))

     
     {
      CU_FAIL( "Corrupted PayLoadType  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegmentOriginId (r, makeS ("orgid://w45-pp78-4574")))

     {
      CU_FAIL( "Corrupted Segment Oringin Id  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegmentNumber (r, 1))
   
     {
      CU_FAIL( "Corrupted SegmentNumber  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegTotalLength (r, 145500))

     {
      CU_FAIL( "Corrupted TotalLength  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if  (WRecord_addAnvl (r, makeS ("key1"), makeS ("val1") ) )
     {
       CU_FAIL( "Corrupted anvl\n");
       destroy (r);
       destroy (w);
       return;
     }  

  if (WRecord_setContentFromFileName (r, file1) )
    {
      CU_FAIL( "record not filled\n");
      destroy (r);
      destroy (w);
      return;
    }

  if (WFile_storeRecord (w, r) )
    {
      CU_FAIL( "record not stored\n");
      destroy (r);
      destroy (w);
      return;
    }

  CU_FAIL( "OK\n");

  destroy (r);
  destroy (w);

  return;
}

void test5 (void)
{
  
  const char * file1 = "./app/wdata/testwfile/unidesc.html";

  void * r = bless (WRecord);
  void * w = bless (WFile, "./awarcfdt.warc", WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED, ".");
  void * u = NIL;

  if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
      CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setRecordType (r, 12) )
    {
      CU_PASS( "Corrupted Record type\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-0:55Z") ) )
    {
      CU_FAIL( "Corrupted date\n");
      destroy (r);
      destroy (w);
      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
      CU_FAIL( "Corrupted record ID\n");
      destroy (r);
      destroy (w);
      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
      CU_FAIL( "Corrupted content type\n");
      destroy (r);
      destroy (w);
      return;
    }
   if (WRecord_setConcurrentTo (r, makeS ( "concuuid://id1:warc-x584sld56" )))

    {
      CU_FAIL( "Corrupted concurrent to \n");
      destroy (r);
      destroy (w);
      return;
    }
  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
      CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
      CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setIpAddress (r, makeS ("192.165.1.0")))
 
     {
      CU_FAIL( "Corrupted Ip Adress Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setRefersTo (r, makeS ("refuuid://id14:warc-x265sll76")))

     {
      CU_FAIL( "Corrupted Refers to \n");
      destroy (r);
      destroy (w);
      return;
     }
    if (WRecord_setTruncated (r, makeS("time")))
   
     {
      CU_FAIL( "Corrupted Truncated  \n");
      destroy (r);
      destroy (w);
      return;
     }
     
    if (WRecord_setWarcInfoId (r, makeS ("infuuid://id0:warc-x000sl076")))

     {
      CU_FAIL( "Corrupted InfoId  \n");
      destroy (r);
      destroy (w);
      return;
     }
   
    if (WRecord_setFilename (r, makeS ("/home/user/warcfile.warc")))

     {
      CU_FAIL( "Corrupted Filename  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
      CU_FAIL( "Corrupted Profile  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadType (r, makeS("text/html; charset=unicode-utf8")))

     
     {
      CU_FAIL( "Corrupted PayLoadType  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegmentOriginId (r, makeS ("orgid://w45-pp78-4574")))

     {
      CU_FAIL( "Corrupted Segment Oringin Id  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegmentNumber (r, 1))
   
     {
      CU_FAIL( "Corrupted SegmentNumber  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegTotalLength (r, 145500))

     {
      CU_FAIL( "Corrupted TotalLength  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if  (WRecord_addAnvl (r, makeS ("key1"), makeS ("val1") ) )
     {
       CU_FAIL( "Corrupted anvl\n");
       destroy (r);
       destroy (w);
       return;
     }  

  if (WRecord_setContentFromFileName (r, file1) )
    {
      CU_FAIL( "record not filled\n");
      destroy (r);
      destroy (w);
      return;
    }

  if (WFile_storeRecord (w, r) )
    {
      CU_FAIL( "record not stored\n");
      destroy (r);
      destroy (w);
      return;
    }

  CU_FAIL( "OK\n");

  destroy (r);
  
  r = bless (WRecord);

   if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
      CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setRecordType (r, WARC_INFO_RECORD) )
    {
      CU_FAIL( "Corrupted Record type\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
      CU_FAIL( "Corrupted date\n");
      destroy (r);
      destroy (w);
      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
      CU_FAIL( "Corrupted record ID\n");
      destroy (r);
      destroy (w);
      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
      CU_FAIL( "Corrupted content type\n");
      destroy (r);
      destroy (w);
      return;
    }
   if (WRecord_setConcurrentTo (r, makeS ( "concuuid://id1:warc-x584sld56" )))

    {
      CU_FAIL( "Corrupted concurrent to \n");
      destroy (r);
      destroy (w);
      return;
    }
  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
      CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
      CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setIpAddress (r, makeS ("192.165.1.0")))
 
     {
      CU_FAIL( "Corrupted Ip Adress Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setRefersTo (r, makeS ("refuuid://id14:warc-x265sll76")))

     {
      CU_FAIL( "Corrupted Refers to \n");
      destroy (r);
      destroy (w);
      return;
     }
    if (WRecord_setTruncated (r, makeS("time")))
   
     {
      CU_FAIL( "Corrupted Truncated  \n");
      destroy (r);
      destroy (w);
      return;
     }
     
    if (WRecord_setWarcInfoId (r, makeS ("infuuid://id0:warc-x000sl076")))

     {
      CU_FAIL( "Corrupted InfoId  \n");
      destroy (r);
      destroy (w);
      return;
     }
   
    if (WRecord_setFilename (r, makeS ("/home/user/warcfile.warc")))

     {
      CU_FAIL( "Corrupted Filename  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
      CU_FAIL( "Corrupted Profile  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadType (r, makeS("text/html; charset=unicode-utf8")))

     
     {
      CU_FAIL( "Corrupted PayLoadType  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegmentOriginId (r, makeS ("orgid://w45-pp78-4574")))

     {
      CU_FAIL( "Corrupted Segment Oringin Id  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegmentNumber (r, 1))
   
     {
      CU_FAIL( "Corrupted SegmentNumber  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegTotalLength (r, 145500))

     {
      CU_FAIL( "Corrupted TotalLength  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if  (WRecord_addAnvl (r, makeS ("key1"), makeS ("val1") ) )
     {
       CU_FAIL( "Corrupted anvl\n");
       destroy (r);
       destroy (w);
       return;
     }  

  if (WRecord_setContentFromFileName (r, file1) )
    {
      CU_FAIL( "record not filled\n");
      destroy (r);
      destroy (w);
      return;
    }

  if (WFile_storeRecord (w, r) )
    {
      CU_FAIL( "record not stored\n");
      destroy (r);
      destroy (w);
      return;
    }

  CU_FAIL( "OK\n");

  destroy (r);
  destroy (w);

  return;
}

void test6 (void)
{
  
  const char * file1 = "./app/wdata/testwfile/unidesc.html";

  void * r = bless (WRecord);
  void * w = bless (WFile, "./awarfldt.warc", WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED, ".");
  void * u = NIL;



  if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
      CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setRecordType (r, WARC_INFO_RECORD) )
    {
      CU_FAIL( "Corrupted Record type\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
      CU_FAIL( "Corrupted date\n");
      destroy (r);
      destroy (w);
      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
      CU_FAIL( "Corrupted record ID\n");
      destroy (r);
      destroy (w);
      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
      CU_FAIL( "Corrupted content type\n");
      destroy (r);
      destroy (w);
      return;
    }
   if (WRecord_setConcurrentTo (r, makeS ( "concuuid://id1:warc-x584sld56" )))

    {
      CU_FAIL( "Corrupted concurrent to \n");
      destroy (r);
      destroy (w);
      return;
    }
  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
      CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
      CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setIpAddress (r, makeS ("192.165.1.0")))
 
     {
      CU_FAIL( "Corrupted Ip Adress Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setRefersTo (r, makeS ("refuuid://id14:warc-x265sll76")))

     {
      CU_FAIL( "Corrupted Refers to \n");
      destroy (r);
      destroy (w);
      return;
     }
    if (WRecord_setTruncated (r, makeS("time")))
   
     {
      CU_FAIL( "Corrupted Truncated  \n");
      destroy (r);
      destroy (w);
      return;
     }
     
    if (WRecord_setWarcInfoId (r, makeS ("infuuid://id0:warc-x000sl076")))

     {
      CU_FAIL( "Corrupted InfoId  \n");
      destroy (r);
      destroy (w);
      return;
     }
   
    if (WRecord_setFilename (r, makeS ("/home/user/warcfile.warc")))

     {
      CU_FAIL( "Corrupted Filename  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
      CU_FAIL( "Corrupted Profile  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadType (r, makeS("text/html; charset=unicode-utf8")))

     
     {
      CU_FAIL( "Corrupted PayLoadType  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegmentOriginId (r, makeS ("orgid://w45-pp78-4574")))

     {
      CU_FAIL( "Corrupted Segment Oringin Id  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegmentNumber (r, 1))
   
     {
      CU_FAIL( "Corrupted SegmentNumber  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegTotalLength (r, 145500))

     {
      CU_FAIL( "Corrupted TotalLength  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if  (WRecord_addAnvl (r, makeS ("key 1"), makeS ("val1") ) )
     {
       CU_PASS( "Corrupted anvl\n");
       destroy (r);
       destroy (w);
       return;
     }  

  if (WRecord_setContentFromFileName (r, file1) )
    {
      CU_FAIL( "record not filled\n");
      destroy (r);
      destroy (w);
      return;
    }

  if (WFile_storeRecord (w, r) )
    {
      CU_FAIL( "record not stored\n");
      destroy (r);
      destroy (w);
      return;
    }

  CU_FAIL( "OK\n");

  destroy (r);
  destroy (w);

  return;
}

void test7 (void)
{
  
  const char * file1 = "./app/wdata/testwfile/unidesc.html";

  void * r = bless (WRecord);
  void * w = bless (WFile, "./awarfldt.warc", WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED, ".");
  void * u = NIL;


  if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
      CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setRecordType (r, WARC_INFO_RECORD) )
    {
      CU_FAIL( "Corrupted Record type\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
      CU_FAIL( "Corrupted date\n");
      destroy (r);
      destroy (w);
      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
      CU_FAIL( "Corrupted record ID\n");
      destroy (r);
      destroy (w);
      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
      CU_FAIL( "Corrupted content type\n");
      destroy (r);
      destroy (w);
      return;
    }
   if (WRecord_setConcurrentTo (r, makeS ( "concuuid://id1:warc-x584sld56" )))

    {
      CU_FAIL( "Corrupted concurrent to \n");
      destroy (r);
      destroy (w);
      return;
    }
  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
      CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
      CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setIpAddress (r, makeS ("192.165.1.0")))
 
     {
      CU_FAIL( "Corrupted Ip Adress Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setRefersTo (r, makeS ("refuuid://id14:warc-x265sll76")))

     {
      CU_FAIL( "Corrupted Refers to \n");
      destroy (r);
      destroy (w);
      return;
     }
    if (WRecord_setTruncated (r, makeS("timeout")))
   
     {
      CU_PASS( "Corrupted Truncated  \n");
      destroy (r);
      destroy (w);
      return;
     }
     
    if (WRecord_setWarcInfoId (r, makeS ("infuuid://id0:warc-x000sl076")))

     {
      CU_FAIL( "Corrupted InfoId  \n");
      destroy (r);
      destroy (w);
      return;
     }
   
    if (WRecord_setFilename (r, makeS ("/home/user/warcfile.warc")))

     {
      CU_FAIL( "Corrupted Filename  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
      CU_FAIL( "Corrupted Profile  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadType (r, makeS("text/html; charset=unicode-utf8")))

     
     {
      CU_FAIL( "Corrupted PayLoadType  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegmentOriginId (r, makeS ("orgid://w45-pp78-4574")))

     {
      CU_FAIL( "Corrupted Segment Oringin Id  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegmentNumber (r, 1))
   
     {
      CU_FAIL( "Corrupted SegmentNumber  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegTotalLength (r, 145500))

     {
      CU_FAIL( "Corrupted TotalLength  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if  (WRecord_addAnvl (r, makeS ("key1"), makeS ("val1") ) )
     {
       CU_FAIL( "Corrupted anvl\n");
       destroy (r);
       destroy (w);
       return;
     }  

  if (WRecord_setContentFromFileName (r, file1) )
    {
      CU_FAIL( "record not filled\n");
      destroy (r);
      destroy (w);
      return;
    }

  if (WFile_storeRecord (w, r) )
    {
      CU_FAIL( "record not stored\n");
      destroy (r);
      destroy (w);
      return;
    }

  CU_FAIL( "OK\n");

  destroy (r);
  destroy (w);

  return;
}

void test8 (void)
{
  
  const char * file1 = "./app/wdata/testwfile/unidesc.html";

  void * r = bless (WRecord);
  void * w = bless (WFile, "./awarfldt.warc", WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED, ".");
  void * u = NIL;
fprintf(stdout,"////////////////////////////test 8 /////////////////////////////////\n");
 
  if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
      CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setRecordType (r, WARC_INFO_RECORD) )
    {
      CU_FAIL( "Corrupted Record type\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
      CU_FAIL( "Corrupted date\n");
      destroy (r);
      destroy (w);
      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
      CU_FAIL( "Corrupted record ID\n");
      destroy (r);
      destroy (w);
      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
      CU_FAIL( "Corrupted content type\n");
      destroy (r);
      destroy (w);
      return;
    }
   if (WRecord_setConcurrentTo (r, makeS ( "concuuid://id1:warc-x584sld56" )))

    {
      CU_FAIL( "Corrupted concurrent to \n");
      destroy (r);
      destroy (w);
      return;
    }
  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
      CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
      CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setIpAddress (r, makeS ("192.165.1..r0")))
 
     {
      CU_PASS( "Corrupted Ip Adress  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setRefersTo (r, makeS ("refuuid://id14:warc-x265sll76")))

     {
      CU_FAIL( "Corrupted Refers to \n");
      destroy (r);
      destroy (w);
      return;
     }
    if (WRecord_setTruncated (r, makeS("time")))
   
     {
      CU_FAIL( "Corrupted Truncated  \n");
      destroy (r);
      destroy (w);
      return;
     }
     
    if (WRecord_setWarcInfoId (r, makeS ("infuuid://id0:warc-x000sl076")))

     {
      CU_FAIL( "Corrupted InfoId  \n");
      destroy (r);
      destroy (w);
      return;
     }
   
    if (WRecord_setFilename (r, makeS ("/home/user/warcfile.warc")))

     {
      CU_FAIL( "Corrupted Filename  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
      CU_FAIL( "Corrupted Profile  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadType (r, makeS("text/html; charset=unicode-utf8")))

     
     {
      CU_FAIL( "Corrupted PayLoadType  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegmentOriginId (r, makeS ("orgid://w45-pp78-4574")))

     {
      CU_FAIL( "Corrupted Segment Oringin Id  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegmentNumber (r, 1))
   
     {
      CU_FAIL( "Corrupted SegmentNumber  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegTotalLength (r, 145500))

     {
      CU_FAIL( "Corrupted TotalLength  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if  (WRecord_addAnvl (r, makeS ("key1"), makeS ("val1") ) )
     {
       CU_FAIL( "Corrupted anvl\n");
       destroy (r);
       destroy (w);
       return;
     }  

  if (WRecord_setContentFromFileName (r, file1) )
    {
      CU_FAIL( "record not filled\n");
      destroy (r);
      destroy (w);
      return;
    }

  if (WFile_storeRecord (w, r) )
    {
      CU_FAIL( "record not stored\n");
      destroy (r);
      destroy (w);
      return;
    }

  CU_FAIL( "OK\n");

  destroy (r);
  destroy (w);

  return;
}

void test9 (void)
{
  
  const char * file1 = "./app/wdata/testwfile/unidesc.html";

  void * r = bless (WRecord);
  void * w = bless (WFile, "./awarfldt.warc", WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED, ".");
  void * u = NIL;

 

  if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
      CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setRecordType (r, WARC_INFO_RECORD) )
    {
      CU_FAIL( "Corrupted Record type\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
      CU_FAIL( "Corrupted date\n");
      destroy (r);
      destroy (w);
      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
      CU_FAIL( "Corrupted record ID\n");
      destroy (r);
      destroy (w);
      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
      CU_FAIL( "Corrupted content type\n");
      destroy (r);
      destroy (w);
      return;
    }
   if (WRecord_setConcurrentTo (r, makeS ( "concuuid://id1:warc-x584sld56" )))

    {
      CU_FAIL( "Corrupted concurrent to \n");
      destroy (r);
      destroy (w);
      return;
    }
  
    if (WRecord_setBlockDigest (r, makeS ("sha10451-9645-AM35P-12LL")))

     {
      CU_PASS( "Corrupted Bloc Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
      CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setIpAddress (r, makeS ("192.165.1.0")))
 
     {
      CU_FAIL( "Corrupted Ip Adress  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setRefersTo (r, makeS ("refuuid://id14:warc-x265sll76")))

     {
      CU_FAIL( "Corrupted Refers to \n");
      destroy (r);
      destroy (w);
      return;
     }
    if (WRecord_setTruncated (r, makeS("time")))
   
     {
      CU_FAIL( "Corrupted Truncated  \n");
      destroy (r);
      destroy (w);
      return;
     }
     
    if (WRecord_setWarcInfoId (r, makeS ("infuuid://id0:warc-x000sl076")))

     {
      CU_FAIL( "Corrupted InfoId  \n");
      destroy (r);
      destroy (w);
      return;
     }
   
    if (WRecord_setFilename (r, makeS ("/home/user/warcfile.warc")))

     {
      CU_FAIL( "Corrupted Filename  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
      CU_FAIL( "Corrupted Profile  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadType (r, makeS("text/html; charset=unicode-utf8")))

     
     {
      CU_FAIL( "Corrupted PayLoadType  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegmentOriginId (r, makeS ("orgid://w45-pp78-4574")))

     {
      CU_FAIL( "Corrupted Segment Oringin Id  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegmentNumber (r, 1))
   
     {
      CU_FAIL( "Corrupted SegmentNumber  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegTotalLength (r, 145500))

     {
      CU_FAIL( "Corrupted TotalLength  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if  (WRecord_addAnvl (r, makeS ("key1"), makeS ("val1") ) )
     {
       CU_FAIL( "Corrupted anvl\n");
       destroy (r);
       destroy (w);
       return;
     }  

  if (WRecord_setContentFromFileName (r, file1) )
    {
      CU_FAIL( "record not filled\n");
      destroy (r);
      destroy (w);
      return;
    }

  if (WFile_storeRecord (w, r) )
    {
      CU_FAIL( "record not stored\n");
      destroy (r);
      destroy (w);
      return;
    }

  CU_FAIL( "OK\n");

  destroy (r);
  destroy (w);

  return;
}

void test10 (void)
{
  
  const char * file1 = "./app/wdata/testwfile/unidesc.html";

  void * r = bless (WRecord);
  void * w = bless (WFile, "./awarfldt.warc", WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED, ".");
  void * u = NIL;

 

  if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
      CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setRecordType (r, WARC_INFO_RECORD) )
    {
      CU_FAIL( "Corrupted Record type\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
      CU_FAIL( "Corrupted date\n");
      destroy (r);
      destroy (w);
      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
      CU_FAIL( "Corrupted record ID\n");
      destroy (r);
      destroy (w);
      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
      CU_FAIL( "Corrupted content type\n");
      destroy (r);
      destroy (w);
      return;
    }
   if (WRecord_setConcurrentTo (r, makeS ( "concuuid://id1:warc-x584sld56" )))

    {
      CU_FAIL( "Corrupted concurrent to \n");
      destroy (r);
      destroy (w);
      return;
    }
  
    if (WRecord_setBlockDigest (r, makeS ("sha:10451-9645-AM35P-12LL")))

     {
      CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha20121-KJ45-1111-A001L")))

     {
      CU_PASS( "Corrupted Pay Load Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setIpAddress (r, makeS ("192.165.1.0")))
 
     {
      CU_FAIL( "Corrupted Ip Adress  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setRefersTo (r, makeS ("refuuid://id14:warc-x265sll76")))

     {
      CU_FAIL( "Corrupted Refers to \n");
      destroy (r);
      destroy (w);
      return;
     }
    if (WRecord_setTruncated (r, makeS("time")))
   
     {
      CU_FAIL( "Corrupted Truncated  \n");
      destroy (r);
      destroy (w);
      return;
     }
     
    if (WRecord_setWarcInfoId (r, makeS ("infuuid://id0:warc-x000sl076")))

     {
      CU_FAIL( "Corrupted InfoId  \n");
      destroy (r);
      destroy (w);
      return;
     }
   
    if (WRecord_setFilename (r, makeS ("/home/user/warcfile.warc")))

     {
      CU_FAIL( "Corrupted Filename  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
      CU_FAIL( "Corrupted Profile  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadType (r, makeS("text/html; charset=unicode-utf8")))

     
     {
      CU_FAIL( "Corrupted PayLoadType  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegmentOriginId (r, makeS ("orgid://w45-pp78-4574")))

     {
      CU_FAIL( "Corrupted Segment Oringin Id  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegmentNumber (r, 1))
   
     {
      CU_FAIL( "Corrupted SegmentNumber  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegTotalLength (r, 145500))

     {
      CU_FAIL( "Corrupted TotalLength  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if  (WRecord_addAnvl (r, makeS ("key1"), makeS ("val1") ) )
     {
       CU_FAIL( "Corrupted anvl\n");
       destroy (r);
       destroy (w);
       return;
     }  

  if (WRecord_setContentFromFileName (r, file1) )
    {
      CU_FAIL( "record not filled\n");
      destroy (r);
      destroy (w);
      return;
    }

  if (WFile_storeRecord (w, r) )
    {
      CU_FAIL( "record not stored\n");
      destroy (r);
      destroy (w);
      return;
    }

  CU_FAIL( "OK\n");

  destroy (r);
  destroy (w);

  return;
}

void test11 (void)
{
  
  const char * file1 = "./app/wdata/testwfile/unidesc.html";

  void * r = bless (WRecord);
  void * w = bless (WFile, "./awarfldt.warc", WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED, ".");
  void * u = NIL;

 
  if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
      CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setRecordType (r, WARC_INFO_RECORD) )
    {
      CU_FAIL( "Corrupted Record type\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
      CU_FAIL( "Corrupted date\n");
      destroy (r);
      destroy (w);
      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
      CU_FAIL( "Corrupted record ID\n");
      destroy (r);
      destroy (w);
      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
      CU_FAIL( "Corrupted content type\n");
      destroy (r);
      destroy (w);
      return;
    }
   if (WRecord_setConcurrentTo (r, makeS ( "concuuid://id1:warc-x584sld56" )))

    {
      CU_FAIL( "Corrupted concurrent to \n");
      destroy (r);
      destroy (w);
      return;
    }
  
    if (WRecord_setBlockDigest (r, makeS ("sha:10451-9645-AM35P-12LL")))

     {
      CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha20121-KJ45-1111-A001L")))

     {
      CU_PASS( "Corrupted Pay Load Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setIpAddress (r, makeS ("192.165.1.0")))
 
     {
      CU_FAIL( "Corrupted Ip Adress  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setRefersTo (r, makeS ("refuuid://id14:warc-x265sll76")))

     {
      CU_FAIL( "Corrupted Refers to \n");
      destroy (r);
      destroy (w);
      return;
     }
    if (WRecord_setTruncated (r, makeS("time")))
   
     {
      CU_FAIL( "Corrupted Truncated  \n");
      destroy (r);
      destroy (w);
      return;
     }
     
    if (WRecord_setWarcInfoId (r, makeS ("infuuid://id0:warc-x000sl076")))

     {
      CU_FAIL( "Corrupted InfoId  \n");
      destroy (r);
      destroy (w);
      return;
     }
   
    if (WRecord_setFilename (r, makeS ("/home/user/\x0Dwarcfile.warc")))

     {
      CU_FAIL( "Corrupted Filename  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
      CU_FAIL( "Corrupted Profile  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadType (r, makeS("text/html; charset=unicode-utf8")))

     
     {
      CU_FAIL( "Corrupted PayLoadType  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegmentOriginId (r, makeS ("orgid://w45-pp78-4574")))

     {
      CU_FAIL( "Corrupted Segment Oringin Id  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegmentNumber (r, 1))
   
     {
      CU_FAIL( "Corrupted SegmentNumber  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegTotalLength (r, 145500))

     {
      CU_FAIL( "Corrupted TotalLength  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if  (WRecord_addAnvl (r, makeS ("key1"), makeS ("val1") ) )
     {
       CU_FAIL( "Corrupted anvl\n");
       destroy (r);
       destroy (w);
       return;
     }  

  if (WRecord_setContentFromFileName (r, file1) )
    {
      CU_FAIL( "record not filled\n");
      destroy (r);
      destroy (w);
      return;
    }

  if (WFile_storeRecord (w, r) )
    {
      CU_FAIL( "record not stored\n");
      destroy (r);
      destroy (w);
      return;
    }

  CU_FAIL( "OK\n");

  destroy (r);
  destroy (w);

  return;
}


void test12 (void)
{
  
  const char * file1 = "./app/wdata/testwfile/unidesc.html";

  void * r = bless (WRecord);
  void * w = bless (WFile, "./awarfldt.warc", WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED, ".");
  void * u = NIL;

  

  if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
      CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setRecordType (r,  WARC_CONTINUATION_RECORD) )
    {
      CU_FAIL( "Corrupted Record type\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
      CU_FAIL( "Corrupted date\n");
      destroy (r);
      destroy (w);
      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
      CU_FAIL( "Corrupted record ID\n");
      destroy (r);
      destroy (w);
      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
      CU_FAIL( "Corrupted content type\n");
      destroy (r);
      destroy (w);
      return;
    }
   if (WRecord_setConcurrentTo (r, makeS ( "concuuid://id1:warc-x584sld56" )))

    {
      CU_FAIL( "Corrupted concurrent to \n");
      destroy (r);
      destroy (w);
      return;
    }
  
    if (WRecord_setBlockDigest (r, makeS ("sha:10451-9645-AM35P-12LL")))

     {
      CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha20121-KJ45-1111-A001L")))

     {
      CU_PASS( "Corrupted Pay Load Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setIpAddress (r, makeS ("192.165.1.0")))
 
     {
      CU_FAIL( "Corrupted Ip Adress  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setRefersTo (r, makeS ("refuuid://id14:warc-x265sll76")))

     {
      CU_FAIL( "Corrupted Refers to \n");
      destroy (r);
      destroy (w);
      return;
     }
    if (WRecord_setTruncated (r, makeS("time")))
   
     {
      CU_FAIL( "Corrupted Truncated  \n");
      destroy (r);
      destroy (w);
      return;
     }
     
    if (WRecord_setWarcInfoId (r, makeS ("infuuid://id0:warc-x000sl076")))

     {
      CU_FAIL( "Corrupted InfoId  \n");
      destroy (r);
      destroy (w);
      return;
     }
   
    if (WRecord_setFilename (r, makeS ("/home/user/warcfile.warc")))

     {
      CU_FAIL( "Corrupted Filename  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
      CU_FAIL( "Corrupted Profile  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadType (r, makeS("text/html; charset=unicode-utf8")))

     
     {
      CU_FAIL( "Corrupted PayLoadType  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegmentOriginId (r, makeS ("orgid://w45-pp78-4574")))

     {
      CU_FAIL( "Corrupted Segment Oringin Id  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegmentNumber (r, -36))
   
     {
      CU_FAIL( "Corrupted SegmentNumber  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegTotalLength (r, 145500))

     {
      CU_FAIL( "Corrupted TotalLength  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if  (WRecord_addAnvl (r, makeS ("key1"), makeS ("val1") ) )
     {
       CU_FAIL( "Corrupted anvl\n");
       destroy (r);
       destroy (w);
       return;
     }  

  if (WRecord_setContentFromFileName (r, file1) )
    {
      CU_FAIL( "record not filled\n");
      destroy (r);
      destroy (w);
      return;
    }

  if (WFile_storeRecord (w, r) )
    {
      CU_FAIL( "record not stored\n");
      destroy (r);
      destroy (w);
      return;
    }

  CU_FAIL( "OK\n");

  destroy (r);
  destroy (w);

  return;
}

void test13 (void)
{
  
  const char * file1 = "./app/wdata/testwfile/unidesc.html";

  void * r = bless (WRecord);
  void * w = bless (WFile, "./awarfldt.warc", WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED, ".");
  void * u = NIL;

  

  if (WRecord_setTargetUri (r, makeS ("test://anunknownplace") ) )
    {
      CU_FAIL( "Corrupted Target Uri\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setRecordType (r,  WARC_CONTINUATION_RECORD) )
    {
      CU_FAIL( "Corrupted Record type\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
      CU_FAIL( "Corrupted date\n");
      destroy (r);
      destroy (w);
      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
      CU_FAIL( "Corrupted record ID\n");
      destroy (r);
      destroy (w);
      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
      CU_FAIL( "Corrupted content type\n");
      destroy (r);
      destroy (w);
      return;
    }
   if (WRecord_setConcurrentTo (r, makeS ( "concuuid://id1:warc-x584sld56" )))

    {
      CU_FAIL( "Corrupted concurrent to \n");
      destroy (r);
      destroy (w);
      return;
    }
  
    if (WRecord_setBlockDigest (r, makeS ("sha:10451-9645-AM35P-12LL")))

     {
      CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha20121-KJ45-1111-A001L")))

     {
      CU_PASS( "Corrupted Pay Load Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setIpAddress (r, makeS ("192.165.1.0")))
 
     {
      CU_FAIL( "Corrupted Ip Adress  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setRefersTo (r, makeS ("refuuid://id14:warc-x265sll76")))

     {
      CU_FAIL( "Corrupted Refers to \n");
      destroy (r);
      destroy (w);
      return;
     }
    if (WRecord_setTruncated (r, makeS("time")))
   
     {
      CU_FAIL( "Corrupted Truncated  \n");
      destroy (r);
      destroy (w);
      return;
     }
     
    if (WRecord_setWarcInfoId (r, makeS ("infuuid://id0:warc-x000sl076")))

     {
      CU_FAIL( "Corrupted InfoId  \n");
      destroy (r);
      destroy (w);
      return;
     }
   
    if (WRecord_setFilename (r, makeS ("/home/user/warcfile.warc")))

     {
      CU_FAIL( "Corrupted Filename  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
      CU_FAIL( "Corrupted Profile  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadType (r, makeS("text/html; charset=unicode-utf8")))

     
     {
      CU_FAIL( "Corrupted PayLoadType  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegmentOriginId (r, makeS ("orgid://w45-pp78-4574")))

     {
      CU_FAIL( "Corrupted Segment Oringin Id  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegmentNumber (r, 36))
   
     {
      CU_FAIL( "Corrupted SegmentNumber  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setSegTotalLength (r, -1532))

     {
      CU_FAIL( "Corrupted TotalLength  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if  (WRecord_addAnvl (r, makeS ("key1"), makeS ("val1") ) )
     {
       CU_FAIL( "Corrupted anvl\n");
       destroy (r);
       destroy (w);
       return;
     }  

  if (WRecord_setContentFromFileName (r, file1) )
    {
      CU_FAIL( "record not filled\n");
      destroy (r);
      destroy (w);
      return;
    }

  if (WFile_storeRecord (w, r) )
    {
      CU_FAIL( "record not stored\n");
      destroy (r);
      destroy (w);
      return;
    }

  CU_FAIL( "OK\n");

  destroy (r);
  destroy (w);

  return;
}
  
void test14 (void)
{
  
  const char * file1 = "./app/wdata/testwfile/unidesc.html";


  void * r = NIL;
  void * w = NIL;
  void * u = NIL;

  

  r = bless (WRecord);
  w = bless (WFile, "./cwarc.warc.gz", WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_COMPRESSED_GZIP_BEST_SPEED, ".");


  if  (WRecord_setRecordType (r, WARC_INFO_RECORD) )
    {
      CU_FAIL( "Corrupted Record type\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
      CU_FAIL( "Corrupted date\n");
      destroy (r);
      destroy (w);
      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
      CU_FAIL( "Corrupted record ID\n");
      destroy (r);
      destroy (w);
      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
      CU_FAIL( "Corrupted content type\n");
      destroy (r);
      destroy (w);
      return;
    }

  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
      CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
      CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }


    if (WRecord_setTruncated (r, makeS("time")))
   
     {
      CU_FAIL( "Corrupted Truncated  \n");
      destroy (r);
      destroy (w);
      return;
     }

   
    if (WRecord_setFilename (r, makeS ("/home/user/warcfile.warc")))

     {
      CU_FAIL( "Corrupted Filename  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
      CU_FAIL( "Corrupted Profile  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadType (r, makeS("text/html; charset=unicode-utf8")))

     
     {
      CU_FAIL( "Corrupted PayLoadType  \n");
      destroy (r);
      destroy (w);
      return;
     }


    

  if (WRecord_setContentFromFileName (r, file1) )
    {
      CU_FAIL( "record not filled\n");
      destroy (r);
      destroy (w);
      return;
    }

  if (WFile_storeRecord (w, r) )
    {
      CU_FAIL( "record not stored\n");
      destroy (r);
      destroy (w);
      return;
    }

  CU_PASS( "OK first.\n");

  destroy (r);


  destroy (w);

  return;
}

void test15 (void)
{
  
  const char * file1 = "./app/wdata/testwfile/unidesc.html";
  const char * file2 = "./app/wdata/testwfile/anvlcom";

  void * r = NIL;
  void * w = NIL;
  void * u = NIL;



  r = bless (WRecord);
  w = bless (WFile, "./2cwarc.warc.gz", WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_COMPRESSED_GZIP_BEST_SPEED, ".");



  if  (WRecord_setRecordType (r, WARC_INFO_RECORD) )
    {
      CU_FAIL( "Corrupted Record type\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
      CU_FAIL( "Corrupted date\n");
      destroy (r);
      destroy (w);
      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
      CU_FAIL( "Corrupted record ID\n");
      destroy (r);
      destroy (w);
      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
      CU_FAIL( "Corrupted content type\n");
      destroy (r);
      destroy (w);
      return;
    }


  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
      CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
      CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }


    if (WRecord_setTruncated (r, makeS("time")))
   
     {
      CU_FAIL( "Corrupted Truncated  \n");
      destroy (r);
      destroy (w);
      return;
     }

   
    if (WRecord_setFilename (r, makeS ("/home/user/warcfile.warc")))

     {
      CU_FAIL( "Corrupted Filename  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
      CU_FAIL( "Corrupted Profile  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadType (r, makeS("text/html; charset=unicode-utf8")))

     
     {
      CU_FAIL( "Corrupted PayLoadType  \n");
      destroy (r);
      destroy (w);
      return;
     }

    

  if (WRecord_setContentFromFileName (r, file1) )
    {
      CU_FAIL( "record not filled\n");
      destroy (r);
      destroy (w);
      return;
    }

  if (WFile_storeRecord (w, r) )
    {
      CU_FAIL( "record not stored\n");
      destroy (r);
      destroy (w);
      return;
    }

  CU_PASS( "OK first.\n");

  destroy (r);
  
  r = bless (WRecord);

  if  (WRecord_setRecordType (r, WARC_INFO_RECORD) )
    {
      CU_FAIL( "Corrupted Record type\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
      CU_FAIL( "Corrupted date\n");
      destroy (r);
      destroy (w);
      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
      CU_FAIL( "Corrupted record ID\n");
      destroy (r);
      destroy (w);
      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
      CU_FAIL( "Corrupted content type\n");
      destroy (r);
      destroy (w);
      return;
    }

  
    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
      CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
      CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }



    if (WRecord_setTruncated (r, makeS("time")))
   
     {
      CU_FAIL( "Corrupted Truncated  \n");
      destroy (r);
      destroy (w);
      return;
     }

   
    if (WRecord_setFilename (r, makeS ("/home/user/warcfile.warc")))

     {
      CU_FAIL( "Corrupted Filename  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
      CU_FAIL( "Corrupted Profile  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadType (r, makeS("text/html; charset=unicode-utf8")))

     
     {
      CU_FAIL( "Corrupted PayLoadType  \n");
      destroy (r);
      destroy (w);
      return;
     }


  if (WRecord_setContentFromFileName (r, file2) )
    {
      CU_FAIL( "record not filled\n");
      destroy (r);
      destroy (w);
      return;
    }

  if (WFile_storeRecord (w, r) )
    {
      CU_FAIL( "record not stored\n");
      destroy (r);
      destroy (w);
      return;
    }

  CU_PASS( "OK\n");

  destroy (r);
  destroy (w);

  return;
}

void test16 (void)
{

 
  void * w =  NIL;  /*warc file object */
  void * r = NIL;  /*to recover records */
  char  env[20];



  w = bless (WFile, "./awanvl.warc", WARC_MAX_SIZE,
             WARC_FILE_READER, WARC_FILE_UNCOMPRESSED, ".");
  unless (w)
  return;

  while (WFile_hasMoreRecords (w) )
    {

      unless ( (r = WFile_nextRecord (w) ) )
      {
        destroy (w);
        return;
      }



     /*fprintf(stdout, "==============================================================================\n");
     
      
    fprintf(stdout, "Content Langth: %d\n", WRecord_getContentLength (r));
     fprintf(stdout, "Target Uri: %s\n", WRecord_getTargetUri (r));
     fprintf(stdout, "Content type: %s\n", WRecord_getContentType (r));
     fprintf(stdout, "Concurrent Id: %s\n", WRecord_getConcurrentTo (r));
     fprintf(stdout, "Block Digest: %s\n", WRecord_getBlockDigest (r));
    fprintf(stdout,"Payload Digest: %s\n", WRecord_getPayloadDigest (r));
    fprintf(stdout, "Ip Address: %s\n", WRecord_getIpAddress (r));
    fprintf(stdout, "Refers To: %s \n", WRecord_getRefersTo (r));
    fprintf(stdout, "Trucation: %s\n", WRecord_getTruncated (r));
    fprintf(stdout, "Warc Info Id: %s\n", WRecord_getWarcInfoId (r));
    fprintf(stdout, "Profile: %s\n", WRecord_getProfile (r));
    fprintf(stdout, "Filename: %s\n", WRecord_getFilename (r));
    fprintf(stdout, "Payload Type: %s\n", WRecord_getPayloadType (r));
    fprintf(stdout, "Origin Segment Id: %s\n", WRecord_getSegmentOriginId (r));
    fprintf(stdout, "Segment number: %d\n", WRecord_getSegmentNumber (r));
    fprintf(stdout, "Segments Total Size: %d\n", WRecord_getSegTotalLength (r));


      fprintf(stdout, "--------------------------------------------------------------------------\n");
*/


      CU_ASSERT(WRecord_getContentLength (r)==9739);
     /* CU_ASSERT_STRING_EQUAL("(null)",WRecord_getTargetUri (r));*/
CU_ASSERT_STRING_EQUAL("Text/random",WRecord_getContentType (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getConcurrentTo (r));*/
CU_ASSERT_STRING_EQUAL("sha1:0451-9645-AM35P-12LL",WRecord_getBlockDigest (r));
CU_ASSERT_STRING_EQUAL("sha2:0121-KJ45-1111-A001L", WRecord_getPayloadDigest (r));

/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getIpAddress (r));*/
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getRefersTo (r));*/
CU_ASSERT_STRING_EQUAL("time", WRecord_getTruncated (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getWarcInfoId (r));*/

CU_ASSERT_STRING_EQUAL("Compressed:gzip:best speed",WRecord_getProfile (r));
CU_ASSERT_STRING_EQUAL("/home/user/warcfile.warc",WRecord_getFilename (r));
CU_ASSERT_STRING_EQUAL("text/html; charset=unicode-utf8",WRecord_getPayloadType (r));
/*CU_ASSERT_STRING_EQUAL("(null)", WRecord_getSegmentOriginId (r));*/

CU_ASSERT(WRecord_getSegmentNumber (r)==0);
CU_ASSERT(WRecord_getSegTotalLength (r)==0);

      WFile_register (w, r, callback, (void *) env);
      
        CU_ASSERT_FALSE(WRecord_getContent (r));

      destroy (r);
    }

  destroy (w);

  return;
}

void test17 (void)
{

 
  void * w =  NIL;  /*warc file object */
  void * r = NIL;  /*to recover records */
 warc_u32_t  i      = 1;

  w = bless (WFile, "./awarcmlp.warc", WARC_MAX_SIZE,
             WARC_FILE_READER, WARC_FILE_UNCOMPRESSED, ".");
  unless (w)
  return;

  while (WFile_hasMoreRecords (w) )
    {

      unless ( (r = WFile_nextRecord (w) ) )
      {
        destroy (w);
        return;
      }


/*
      fprintf(stdout, "==============================================================================\n");
      
      
    fprintf(stdout, "Content Langth: %d\n", WRecord_getContentLength (r));
    fprintf(stdout, "Target Uri: %s\n", WRecord_getTargetUri (r));
    fprintf(stdout, "Content type: %s\n", WRecord_getContentType (r));
    fprintf(stdout, "Concurrent Id: %s\n", WRecord_getConcurrentTo (r));
    fprintf(stdout, "Block Digest: %s\n", WRecord_getBlockDigest (r));
    fprintf(stdout, "Payload Digest: %s\n", WRecord_getPayloadDigest (r));
    fprintf(stdout, "Ip Address: %s\n", WRecord_getIpAddress (r));
    fprintf(stdout, "Refers To: %s \n", WRecord_getRefersTo (r));
    fprintf(stdout, "Trucation: %s\n", WRecord_getTruncated (r));
    fprintf(stdout, "Warc Info Id: %s\n", WRecord_getWarcInfoId (r));
    fprintf(stdout, "Profile: %s\n", WRecord_getProfile (r));
    fprintf(stdout, "Filename: %s\n", WRecord_getFilename (r));
    fprintf(stdout, "Payload Type: %s\n", WRecord_getPayloadType (r));
    fprintf(stdout, "Origin Segment Id: %s\n", WRecord_getSegmentOriginId (r));
    fprintf(stdout, "Segment number: %d\n", WRecord_getSegmentNumber (r));
    fprintf(stdout, "Segments Total Size: %d\n", WRecord_getSegTotalLength (r));


      fprintf(stdout, "--------------------------------------------------------------------------\n");

      fprintf(stdout, "\n\n");*/

   if (i==1)
       {


CU_ASSERT(WRecord_getContentLength (r)==9739);
     /* CU_ASSERT_STRING_EQUAL("(null)",WRecord_getTargetUri (r));*/
CU_ASSERT_STRING_EQUAL("Text/random",WRecord_getContentType (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getConcurrentTo (r));*/
CU_ASSERT_STRING_EQUAL("sha1:0451-9645-AM35P-12LL",WRecord_getBlockDigest (r));
CU_ASSERT_STRING_EQUAL("sha2:0121-KJ45-1111-A001L", WRecord_getPayloadDigest (r));

/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getIpAddress (r));*/
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getRefersTo (r));*/
CU_ASSERT_STRING_EQUAL("time", WRecord_getTruncated (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getWarcInfoId (r));*/
CU_ASSERT_STRING_EQUAL("Compressed:gzip:best speed",WRecord_getProfile (r));
CU_ASSERT_STRING_EQUAL("/home/user/warcfile.warc",WRecord_getFilename (r));
CU_ASSERT_STRING_EQUAL("text/html; charset=unicode-utf8",WRecord_getPayloadType (r));
/*CU_ASSERT_STRING_EQUAL("(null)", WRecord_getSegmentOriginId (r));*/

CU_ASSERT(WRecord_getSegmentNumber (r)==0);
CU_ASSERT(WRecord_getSegTotalLength (r)==0);

        }
if (i==2)
       {


CU_ASSERT(WRecord_getContentLength (r)==8329);
     /* CU_ASSERT_STRING_EQUAL("(null)",WRecord_getTargetUri (r));*/
CU_ASSERT_STRING_EQUAL("Text/random",WRecord_getContentType (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getConcurrentTo (r));*/
CU_ASSERT_STRING_EQUAL("sha1:0451-9645-AM35P-12LL",WRecord_getBlockDigest (r));
CU_ASSERT_STRING_EQUAL("sha2:0121-KJ45-1111-A001L", WRecord_getPayloadDigest (r));

/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getIpAddress (r));*/
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getRefersTo (r));*/
CU_ASSERT_STRING_EQUAL("time", WRecord_getTruncated (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getWarcInfoId (r));*/
CU_ASSERT_STRING_EQUAL("Compressed:gzip:best speed",WRecord_getProfile (r));
CU_ASSERT_STRING_EQUAL("/home/user/warcfile.warc",WRecord_getFilename (r));
CU_ASSERT_STRING_EQUAL("text/html; charset=unicode-utf8",WRecord_getPayloadType (r));
/*CU_ASSERT_STRING_EQUAL("(null)", WRecord_getSegmentOriginId (r));*/

CU_ASSERT(WRecord_getSegmentNumber (r)==0);
CU_ASSERT(WRecord_getSegTotalLength (r)==0);

        }
if (i==3)
       {

CU_ASSERT(WRecord_getContentLength (r)==9739);
     /* CU_ASSERT_STRING_EQUAL("(null)",WRecord_getTargetUri (r));*/
CU_ASSERT_STRING_EQUAL("Text/random",WRecord_getContentType (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getConcurrentTo (r));*/
CU_ASSERT_STRING_EQUAL("sha1:0451-9645-AM35P-12LL",WRecord_getBlockDigest (r));
CU_ASSERT_STRING_EQUAL("sha2:0121-KJ45-1111-A001L", WRecord_getPayloadDigest (r));

/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getIpAddress (r));*/
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getRefersTo (r));*/
CU_ASSERT_STRING_EQUAL("time", WRecord_getTruncated (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getWarcInfoId (r));*/
CU_ASSERT_STRING_EQUAL("Compressed:gzip:best speed",WRecord_getProfile (r));
CU_ASSERT_STRING_EQUAL("/home/user/warcfile.warc",WRecord_getFilename (r));
CU_ASSERT_STRING_EQUAL("text/html; charset=unicode-utf8",WRecord_getPayloadType (r));
/*CU_ASSERT_STRING_EQUAL("(null)", WRecord_getSegmentOriginId (r));*/

CU_ASSERT(WRecord_getSegmentNumber (r)==0);
CU_ASSERT(WRecord_getSegTotalLength (r)==0);

        }
if (i==4)
       {


CU_ASSERT(WRecord_getContentLength (r)==8329);
     /* CU_ASSERT_STRING_EQUAL("(null)",WRecord_getTargetUri (r));*/
CU_ASSERT_STRING_EQUAL("Text/random",WRecord_getContentType (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getConcurrentTo (r));*/
CU_ASSERT_STRING_EQUAL("sha1:0451-9645-AM35P-12LL",WRecord_getBlockDigest (r));
CU_ASSERT_STRING_EQUAL("sha2:0121-KJ45-1111-A001L", WRecord_getPayloadDigest (r));

/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getIpAddress (r));*/
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getRefersTo (r));*/
CU_ASSERT_STRING_EQUAL("time", WRecord_getTruncated (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getWarcInfoId (r));*/
CU_ASSERT_STRING_EQUAL("Compressed:gzip:best speed",WRecord_getProfile (r));
CU_ASSERT_STRING_EQUAL("/home/user/warcfile.warc",WRecord_getFilename (r));
CU_ASSERT_STRING_EQUAL("text/html; charset=unicode-utf8",WRecord_getPayloadType (r));
/*CU_ASSERT_STRING_EQUAL("(null)", WRecord_getSegmentOriginId (r));*/

CU_ASSERT(WRecord_getSegmentNumber (r)==0);
CU_ASSERT(WRecord_getSegTotalLength (r)==0);

        }
if (i==5)
       {


CU_ASSERT(WRecord_getContentLength (r)==9739);
     /* CU_ASSERT_STRING_EQUAL("(null)",WRecord_getTargetUri (r));*/
CU_ASSERT_STRING_EQUAL("Text/random",WRecord_getContentType (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getConcurrentTo (r));*/
CU_ASSERT_STRING_EQUAL("sha1:0451-9645-AM35P-12LL",WRecord_getBlockDigest (r));
CU_ASSERT_STRING_EQUAL("sha2:0121-KJ45-1111-A001L", WRecord_getPayloadDigest (r));

/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getIpAddress (r));*/
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getRefersTo (r));*/
CU_ASSERT_STRING_EQUAL("time", WRecord_getTruncated (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getWarcInfoId (r));*/
CU_ASSERT_STRING_EQUAL("Compressed:gzip:best speed",WRecord_getProfile (r));
CU_ASSERT_STRING_EQUAL("/home/user/warcfile.warc",WRecord_getFilename (r));
CU_ASSERT_STRING_EQUAL("text/html; charset=unicode-utf8",WRecord_getPayloadType (r));
/*CU_ASSERT_STRING_EQUAL("(null)", WRecord_getSegmentOriginId (r));*/

CU_ASSERT(WRecord_getSegmentNumber (r)==0);
CU_ASSERT(WRecord_getSegTotalLength (r)==0);

        }
if (i==6)
       {
CU_ASSERT(WRecord_getContentLength (r)==8329);
     /* CU_ASSERT_STRING_EQUAL("(null)",WRecord_getTargetUri (r));*/
CU_ASSERT_STRING_EQUAL("Text/random",WRecord_getContentType (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getConcurrentTo (r));*/
CU_ASSERT_STRING_EQUAL("sha1:0451-9645-AM35P-12LL",WRecord_getBlockDigest (r));
CU_ASSERT_STRING_EQUAL("sha2:0121-KJ45-1111-A001L", WRecord_getPayloadDigest (r));

/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getIpAddress (r));*/
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getRefersTo (r));*/
CU_ASSERT_STRING_EQUAL("time", WRecord_getTruncated (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getWarcInfoId (r));*/
CU_ASSERT_STRING_EQUAL("Compressed:gzip:best speed",WRecord_getProfile (r));
CU_ASSERT_STRING_EQUAL("/home/user/warcfile.warc",WRecord_getFilename (r));
CU_ASSERT_STRING_EQUAL("text/html; charset=unicode-utf8",WRecord_getPayloadType (r));
/*CU_ASSERT_STRING_EQUAL("(null)", WRecord_getSegmentOriginId (r));*/

CU_ASSERT(WRecord_getSegmentNumber (r)==0);
CU_ASSERT(WRecord_getSegTotalLength (r)==0);

        }
i++;
      destroy (r);
    }

  destroy (w);

  return;
}

void test18 (void)
{

  
  void * w =  NIL;  /*warc file object */
  void * r = NIL;  /*to recover records */
  char  env[20]; 
warc_u32_t  i      = 1;


  w = bless (WFile, "./2cwarc.warc.gz", WARC_MAX_SIZE,
             WARC_FILE_READER, WARC_FILE_COMPRESSED_GZIP, ".");
  unless (w)
  return;

  while (WFile_hasMoreRecords (w) )
    {
      unless ( (r = WFile_nextRecord (w) ) )
      {
        destroy (w);
        return;
      }



    /*  fprintf(stdout, "==============================================================================\n");
      fprintf(stdout, "Content Langth: %d\n", WRecord_getContentLength (r));
      fprintf(stdout, "Target Uri: %s\n", WRecord_getTargetUri (r));
      fprintf(stdout, "Content type: %s\n", WRecord_getContentType (r));
      fprintf(stdout, "Concurrent Id: %s\n", WRecord_getConcurrentTo (r));
      fprintf(stdout, "Block Digest: %s\n", WRecord_getBlockDigest (r));
      fprintf(stdout, "Payload Digest: %s\n", WRecord_getPayloadDigest (r));
      fprintf(stdout, "Ip Address: %s\n", WRecord_getIpAddress (r));
      fprintf(stdout, "Refers To: %s \n", WRecord_getRefersTo (r));
      fprintf(stdout, "Trucation: %s\n", WRecord_getTruncated (r));
      fprintf(stdout, "Warc Info Id: %s\n", WRecord_getWarcInfoId (r));
      fprintf(stdout, "Profile: %s\n", WRecord_getProfile (r));
      fprintf(stdout, "Filename: %s\n", WRecord_getFilename (r));
      fprintf(stdout, "Payload Type: %s\n", WRecord_getPayloadType (r));
      fprintf(stdout, "Origin Segment Id: %s\n", WRecord_getSegmentOriginId (r));
      fprintf(stdout, "Segment number: %d\n", WRecord_getSegmentNumber (r));
      fprintf(stdout, "Segments Total Size: %d\n", WRecord_getSegTotalLength (r));
      

      fprintf(stdout, "--------------------------------------------------------------------------\n");

      fprintf(stdout, "\n\n");*/

       WFile_register (w, r, callback, (void *) env);
       CU_ASSERT_FALSE(WRecord_getContent (r)); 

 if (i==1)
       {

CU_ASSERT(WRecord_getContentLength (r)==9739);
     /* CU_ASSERT_STRING_EQUAL("(null)",WRecord_getTargetUri (r));*/
CU_ASSERT_STRING_EQUAL("Text/random",WRecord_getContentType (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getConcurrentTo (r));*/
CU_ASSERT_STRING_EQUAL("sha1:0451-9645-AM35P-12LL",WRecord_getBlockDigest (r));
CU_ASSERT_STRING_EQUAL("sha2:0121-KJ45-1111-A001L", WRecord_getPayloadDigest (r));

/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getIpAddress (r));*/
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getRefersTo (r));*/
CU_ASSERT_STRING_EQUAL("time", WRecord_getTruncated (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getWarcInfoId (r));*/
CU_ASSERT_STRING_EQUAL("Compressed:gzip:best speed",WRecord_getProfile (r));
CU_ASSERT_STRING_EQUAL("/home/user/warcfile.warc",WRecord_getFilename (r));
CU_ASSERT_STRING_EQUAL("text/html; charset=unicode-utf8",WRecord_getPayloadType (r));
/*CU_ASSERT_STRING_EQUAL("(null)", WRecord_getSegmentOriginId (r));*/

CU_ASSERT(WRecord_getSegmentNumber (r)==0);
CU_ASSERT(WRecord_getSegTotalLength (r)==0);

        }
if (i==2)
       {

CU_ASSERT(WRecord_getContentLength (r)==8329);
     /* CU_ASSERT_STRING_EQUAL("(null)",WRecord_getTargetUri (r));*/
CU_ASSERT_STRING_EQUAL("Text/random",WRecord_getContentType (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getConcurrentTo (r));*/
CU_ASSERT_STRING_EQUAL("sha1:0451-9645-AM35P-12LL",WRecord_getBlockDigest (r));
CU_ASSERT_STRING_EQUAL("sha2:0121-KJ45-1111-A001L", WRecord_getPayloadDigest (r));

/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getIpAddress (r));*/
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getRefersTo (r));*/
CU_ASSERT_STRING_EQUAL("time", WRecord_getTruncated (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getWarcInfoId (r));*/
CU_ASSERT_STRING_EQUAL("Compressed:gzip:best speed",WRecord_getProfile (r));
CU_ASSERT_STRING_EQUAL("/home/user/warcfile.warc",WRecord_getFilename (r));
CU_ASSERT_STRING_EQUAL("text/html; charset=unicode-utf8",WRecord_getPayloadType (r));
/*CU_ASSERT_STRING_EQUAL("(null)", WRecord_getSegmentOriginId (r));*/

CU_ASSERT(WRecord_getSegmentNumber (r)==0);
CU_ASSERT(WRecord_getSegTotalLength (r)==0);

        }
if (i==3)
       {

CU_ASSERT(WRecord_getContentLength (r)==9739);
     /* CU_ASSERT_STRING_EQUAL("(null)",WRecord_getTargetUri (r));*/
CU_ASSERT_STRING_EQUAL("Text/random",WRecord_getContentType (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getConcurrentTo (r));*/
CU_ASSERT_STRING_EQUAL("sha1:0451-9645-AM35P-12LL",WRecord_getBlockDigest (r));
CU_ASSERT_STRING_EQUAL("sha2:0121-KJ45-1111-A001L", WRecord_getPayloadDigest (r));

/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getIpAddress (r));*/
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getRefersTo (r));*/
CU_ASSERT_STRING_EQUAL("time", WRecord_getTruncated (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getWarcInfoId (r));*/
CU_ASSERT_STRING_EQUAL("Compressed:gzip:best speed",WRecord_getProfile (r));
CU_ASSERT_STRING_EQUAL("/home/user/warcfile.warc",WRecord_getFilename (r));
CU_ASSERT_STRING_EQUAL("text/html; charset=unicode-utf8",WRecord_getPayloadType (r));
/*CU_ASSERT_STRING_EQUAL("(null)", WRecord_getSegmentOriginId (r));*/

CU_ASSERT(WRecord_getSegmentNumber (r)==0);
CU_ASSERT(WRecord_getSegTotalLength (r)==0);

        }
if (i==4)
       {
CU_ASSERT(WRecord_getContentLength (r)==8329);
     /* CU_ASSERT_STRING_EQUAL("(null)",WRecord_getTargetUri (r));*/
CU_ASSERT_STRING_EQUAL("Text/random",WRecord_getContentType (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getConcurrentTo (r));*/
CU_ASSERT_STRING_EQUAL("sha1:0451-9645-AM35P-12LL",WRecord_getBlockDigest (r));
CU_ASSERT_STRING_EQUAL("sha2:0121-KJ45-1111-A001L", WRecord_getPayloadDigest (r));

/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getIpAddress (r));*/
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getRefersTo (r));*/
CU_ASSERT_STRING_EQUAL("time", WRecord_getTruncated (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getWarcInfoId (r));*/
CU_ASSERT_STRING_EQUAL("Compressed:gzip:best speed",WRecord_getProfile (r));
CU_ASSERT_STRING_EQUAL("/home/user/warcfile.warc",WRecord_getFilename (r));
CU_ASSERT_STRING_EQUAL("text/html; charset=unicode-utf8",WRecord_getPayloadType (r));
/*CU_ASSERT_STRING_EQUAL("(null)", WRecord_getSegmentOriginId (r));*/

CU_ASSERT(WRecord_getSegmentNumber (r)==0);
CU_ASSERT(WRecord_getSegTotalLength (r)==0);

        }
if (i==5)
       {

CU_ASSERT(WRecord_getContentLength (r)==9739);
     /* CU_ASSERT_STRING_EQUAL("(null)",WRecord_getTargetUri (r));*/
CU_ASSERT_STRING_EQUAL("Text/random",WRecord_getContentType (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getConcurrentTo (r));*/
CU_ASSERT_STRING_EQUAL("sha1:0451-9645-AM35P-12LL",WRecord_getBlockDigest (r));
CU_ASSERT_STRING_EQUAL("sha2:0121-KJ45-1111-A001L", WRecord_getPayloadDigest (r));

/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getIpAddress (r));*/
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getRefersTo (r));*/
CU_ASSERT_STRING_EQUAL("time", WRecord_getTruncated (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getWarcInfoId (r));*/
CU_ASSERT_STRING_EQUAL("Compressed:gzip:best speed",WRecord_getProfile (r));
CU_ASSERT_STRING_EQUAL("/home/user/warcfile.warc",WRecord_getFilename (r));
CU_ASSERT_STRING_EQUAL("text/html; charset=unicode-utf8",WRecord_getPayloadType (r));
/*CU_ASSERT_STRING_EQUAL("(null)", WRecord_getSegmentOriginId (r));*/

CU_ASSERT(WRecord_getSegmentNumber (r)==0);
CU_ASSERT(WRecord_getSegTotalLength (r)==0);

        }
if (i==6)
       {
CU_ASSERT(WRecord_getContentLength (r)==8329);
     /* CU_ASSERT_STRING_EQUAL("(null)",WRecord_getTargetUri (r));*/
CU_ASSERT_STRING_EQUAL("Text/random",WRecord_getContentType (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getConcurrentTo (r));*/
CU_ASSERT_STRING_EQUAL("sha1:0451-9645-AM35P-12LL",WRecord_getBlockDigest (r));
CU_ASSERT_STRING_EQUAL("sha2:0121-KJ45-1111-A001L", WRecord_getPayloadDigest (r));

/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getIpAddress (r));*/
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getRefersTo (r));*/
CU_ASSERT_STRING_EQUAL("time", WRecord_getTruncated (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getWarcInfoId (r));*/
CU_ASSERT_STRING_EQUAL("Compressed:gzip:best speed",WRecord_getProfile (r));
CU_ASSERT_STRING_EQUAL("/home/user/warcfile.warc",WRecord_getFilename (r));
CU_ASSERT_STRING_EQUAL("text/html; charset=unicode-utf8",WRecord_getPayloadType (r));
/*CU_ASSERT_STRING_EQUAL("(null)", WRecord_getSegmentOriginId (r));*/

CU_ASSERT(WRecord_getSegmentNumber (r)==0);
CU_ASSERT(WRecord_getSegTotalLength (r)==0);

        }
i++;

      destroy (r);
    }

  destroy (w);

  return;
}






void test19 (void)
{

  
  void * w =  NIL;  /*warc file object */
  void * r = NIL;  /*to recover records */
  char  env[20]; 

  

  w = bless (WFile, "../app/wdata/testwfile/ero1.warc", WARC_MAX_SIZE,
             WARC_FILE_READER, WARC_FILE_UNCOMPRESSED, ".");
  unless (w)
  return;

  while (WFile_hasMoreRecords (w) )
    {
      unless ( (r = WFile_nextRecord (w) ) )
      {
        destroy (w);
        return;
      }

      CU_FAIL( "Content Langth: %d\n WRecord_getContentLength (r)");
      CU_FAIL("Target Uri: %s\n WRecord_getTargetUri (r)");
      CU_FAIL("Content type: %s\n  WRecord_getContentType (r)");
      CU_FAIL("Concurrent Id: %s\n WRecord_getConcurrentTo (r)");
      CU_FAIL("Block Digest: %s\n WRecord_getBlockDigest (r)");
      CU_FAIL("Payload Digest: %s\n WRecord_getPayloadDigest (r)");
      CU_FAIL("Ip Address: %s\n WRecord_getIpAddress (r)");
      CU_FAIL("Refers To: %s \n WRecord_getRefersTo (r)");
      CU_FAIL("Trucation: %s\n WRecord_getTruncated (r)");
      CU_FAIL("Warc Info Id: %s\n WRecord_getWarcInfoId (r)");
      CU_FAIL("Profile: %s\n WRecord_getProfile (r)");
      CU_FAIL( "Filename: %s\n WRecord_getFilename (r)");
      CU_FAIL("Payload Type: %s\n WRecord_getPayloadType (r)");
      CU_FAIL("Origin Segment Id: %s\n WRecord_getSegmentOriginId (r)");
      CU_FAIL( "Segment number: %d\n WRecord_getSegmentNumber (r)");
      CU_FAIL("Segments Total Size: %d\n WRecord_getSegTotalLength (r)");
      

      fprintf(stdout, "--------------------------------------------------------------------------\n");

      fprintf(stdout, "\n\n");

       WFile_register (w, r, callback, (void *) env);
       CU_ASSERT_FALSE(WRecord_getContent (r)); 

      destroy (r);
    }

  destroy (w);

  return;
}

void test20 (void)
{


  void * w =  NIL;  /*warc file object */
  void * r = NIL;  /*to recover records */
  char  env[20]; 

  

  w = bless (WFile, "../app/wdata/testwfile/ero2.warc", WARC_MAX_SIZE,
             WARC_FILE_READER, WARC_FILE_UNCOMPRESSED, ".");
  unless (w)
  return;

  while (WFile_hasMoreRecords (w) )
    {
      unless ( (r = WFile_nextRecord (w) ) )
      {
        destroy (w);
        return;
      }



      CU_FAIL( "Content Langth: %d\n WRecord_getContentLength (r)");
      CU_FAIL("Target Uri: %s\n WRecord_getTargetUri (r)");
      CU_FAIL("Content type: %s\n  WRecord_getContentType (r)");
      CU_FAIL("Concurrent Id: %s\n WRecord_getConcurrentTo (r)");
      CU_FAIL("Block Digest: %s\n WRecord_getBlockDigest (r)");
      CU_FAIL("Payload Digest: %s\n WRecord_getPayloadDigest (r)");
      CU_FAIL("Ip Address: %s\n WRecord_getIpAddress (r)");
      CU_FAIL("Refers To: %s \n WRecord_getRefersTo (r)");
      CU_FAIL("Trucation: %s\n WRecord_getTruncated (r)");
      CU_FAIL("Warc Info Id: %s\n WRecord_getWarcInfoId (r)");
      CU_FAIL("Profile: %s\n WRecord_getProfile (r)");
      CU_FAIL( "Filename: %s\n WRecord_getFilename (r)");
      CU_FAIL("Payload Type: %s\n WRecord_getPayloadType (r)");
      CU_FAIL("Origin Segment Id: %s\n WRecord_getSegmentOriginId (r)");
      CU_FAIL( "Segment number: %d\n WRecord_getSegmentNumber (r)");
      CU_FAIL("Segments Total Size: %d\n WRecord_getSegTotalLength (r)");
      

    
       WFile_register (w, r, callback, (void *) env);
       CU_ASSERT_FALSE(WRecord_getContent (r)); 

      destroy (r);
    }

  destroy (w);

  return;
}

void test21 (void)
{

  void * w =  NIL;  /* warc file object */
  void * r = NIL;  /*to recover records */
  char  env[20]; 

 

  w = bless (WFile, "../app/wdata/testwfile/ero3.warc", WARC_MAX_SIZE,
             WARC_FILE_READER, WARC_FILE_UNCOMPRESSED, ".");
  unless (w)
  return;

  while (WFile_hasMoreRecords (w) )
    {
      unless ( (r = WFile_nextRecord (w) ) )
      {
        destroy (w);
        return;
      }



      CU_FAIL( "Content Langth: %d\n WRecord_getContentLength (r)");
      CU_FAIL("Target Uri: %s\n WRecord_getTargetUri (r)");
      CU_FAIL("Content type: %s\n  WRecord_getContentType (r)");
      CU_FAIL("Concurrent Id: %s\n WRecord_getConcurrentTo (r)");
      CU_FAIL("Block Digest: %s\n WRecord_getBlockDigest (r)");
      CU_FAIL("Payload Digest: %s\n WRecord_getPayloadDigest (r)");
      CU_FAIL("Ip Address: %s\n WRecord_getIpAddress (r)");
      CU_FAIL("Refers To: %s \n WRecord_getRefersTo (r)");
      CU_FAIL("Trucation: %s\n WRecord_getTruncated (r)");
      CU_FAIL("Warc Info Id: %s\n WRecord_getWarcInfoId (r)");
      CU_FAIL("Profile: %s\n WRecord_getProfile (r)");
      CU_FAIL( "Filename: %s\n WRecord_getFilename (r)");
      CU_FAIL("Payload Type: %s\n WRecord_getPayloadType (r)");
      CU_FAIL("Origin Segment Id: %s\n WRecord_getSegmentOriginId (r)");
      CU_FAIL( "Segment number: %d\n WRecord_getSegmentNumber (r)");
      CU_FAIL("Segments Total Size: %d\n WRecord_getSegTotalLength (r)");

       WFile_register (w, r, callback, (void *) env);
       CU_ASSERT_FALSE(WRecord_getContent (r)); 

      destroy (r);
    }

  destroy (w);

  return;
}


void test22 (void)
{

  
  void * w =  NIL;  /* warc file object */
  void * r =  NIL;  /* to recover records */
  void * r2 = NIL; /* the false record */


  char  env[20];

  w = bless (WFile, "./awanvl.warc", WARC_MAX_SIZE,
             WARC_FILE_READER, WARC_FILE_UNCOMPRESSED, ".");

  unless (w)
  return;

  while (WFile_hasMoreRecords (w) )
    {

      unless ( (r = WFile_nextRecord (w) ) )
      {
        destroy (w);
        return;
      }


CU_ASSERT(WRecord_getContentLength (r)==9739);
     /* CU_ASSERT_STRING_EQUAL("(null)",WRecord_getTargetUri (r));*/
CU_ASSERT_STRING_EQUAL("Text/random",WRecord_getContentType (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getConcurrentTo (r));*/
CU_ASSERT_STRING_EQUAL("sha1:0451-9645-AM35P-12LL",WRecord_getBlockDigest (r));
CU_ASSERT_STRING_EQUAL("sha2:0121-KJ45-1111-A001L", WRecord_getPayloadDigest (r));

/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getIpAddress (r));*/
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getRefersTo (r));*/
CU_ASSERT_STRING_EQUAL("time", WRecord_getTruncated (r));
/*CU_ASSERT_STRING_EQUAL("(null)",WRecord_getWarcInfoId (r));*/
CU_ASSERT_STRING_EQUAL("Compressed:gzip:best speed",WRecord_getProfile (r));
CU_ASSERT_STRING_EQUAL("/home/user/warcfile.warc",WRecord_getFilename (r));
CU_ASSERT_STRING_EQUAL("text/html; charset=unicode-utf8",WRecord_getPayloadType (r));
/*CU_ASSERT_STRING_EQUAL("(null)", WRecord_getSegmentOriginId (r));*/

CU_ASSERT(WRecord_getSegmentNumber (r)==0);
CU_ASSERT(WRecord_getSegTotalLength (r)==0);

      destroy (r);
      r2 = bless (WRecord);

      if (WFile_register (w, r2, callback, (void *) env) )
        {
          CU_PASS( "unable to register the callback");
          destroy (w);
          destroy (r2);
          return;
        }

      WRecord_getContent (r2);

      destroy (r2);
    }

  destroy (w);

  return;
}


void test23 (void)
{

 
  void * w =  NIL;  /*warc file object */
  void * r = NIL;  /*to recover records */
  void * w2 = NIL; /* the false wfile object */
  char  env[20];


  w = bless (WFile, "./awanvl.warc", WARC_MAX_SIZE,
             WARC_FILE_READER, WARC_FILE_UNCOMPRESSED, ".");

  unless (w)
  return;

  while (WFile_hasMoreRecords (w) )
    {
      unless ( (r = WFile_nextRecord (w) ) )
      {
        destroy (w);
        return;
      }
/*CU_ASSERT(WRecord_getContentLength (r)==9739);*/
/*CU_ASSERT_STRING_EQUAL("Text/random",WRecord_getContentType (r));*/
/*CU_ASSERT_STRING_EQUAL("sha1:0451-9645-AM35P-12LL",WRecord_getBlockDigest (r));
CU_ASSERT_STRING_EQUAL("sha2:0121-KJ45-1111-A001L", WRecord_getPayloadDigest (r));
CU_ASSERT_STRING_EQUAL("time", WRecord_getTruncated (r));
CU_ASSERT_STRING_EQUAL("Compressed:gzip:best speed",WRecord_getProfile (r));
CU_ASSERT_STRING_EQUAL("/home/user/warcfile.warc",WRecord_getFilename (r));
CU_ASSERT_STRING_EQUAL("text/html; charset=unicode-utf8",WRecord_getPayloadType (r));*/
/*CU_ASSERT(WRecord_getSegmentNumber (r)==0);
CU_ASSERT(WRecord_getSegTotalLength (r)==0);*/


      w2 = bless (WFile, "./app/wdata/testwfile/warcfile.warc", WARC_MAX_SIZE,
                  WARC_FILE_READER, WARC_FILE_UNCOMPRESSED);

      if (WFile_register (w2, r, callback, (void *) env) )
        {
          CU_PASS("unable to register the callback");
          destroy (w2);
          destroy (r);
          destroy (w);
          return;
        }


      CU_ASSERT_FALSE(WRecord_getContent (r));

      destroy (r);
    }

  destroy (w2);

  destroy (w);
  return;
}





void test24 (void)
{
  

  /*const char * file1 = "./app/wdata/testwfile/unidesc.html";*/

  void * r = bless (WRecord);
  void * w = bless (WFile, "./strings.warc", WARC_MAX_SIZE,  WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED, ".");
  void * u = NIL;

  


  if  (WRecord_setRecordType (r, WARC_INFO_RECORD) )
    {
    CU_FAIL( "Corrupted Record type\n");
      destroy (r);
      destroy (w);
      return;
    }

  if  (WRecord_setDate (r, makeS ("2008-04-03T02:59:55Z") ) )
    {
      CU_FAIL( "Corrupted date\n");
      destroy (r);
      destroy (w);
      return;
    }

  u = bless (WUUID);

  WUUID_hash (u, makeU ("record1://anunknownplace") );

  if (WRecord_setRecordId (r, makeS (WUUID_text (u) ) ) )
    {
      CU_FAIL( "Corrupted record ID\n");
      destroy (r);
      destroy (w);
      destroy (u);
      return;
    }

  destroy (u);

  if  (WRecord_setContentType (r, makeS ("Text/random") ) )
    {
      CU_FAIL( "Corrupted content type\n");
      destroy (r);
      destroy (w);
      return;
    }

    if (WRecord_setBlockDigest (r, makeS ("sha1:0451-9645-AM35P-12LL")))

     {
      CU_FAIL( "Corrupted Bloc Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadDigest (r, makeS ("sha2:0121-KJ45-1111-A001L")))

     {
      CU_FAIL( "Corrupted Pay Load Digest to \n");
      destroy (r);
      destroy (w);
      return;
     }


    if (WRecord_setTruncated (r, makeS("time")))
   
     {
      CU_FAIL( "Corrupted Truncated  \n");
      destroy (r);
      destroy (w);
      return;
     }

   
    if (WRecord_setFilename (r, makeS ("/home/user/warcfile.warc")))

     {
      CU_FAIL( "Corrupted Filename  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setProfile (r, makeS ("Compressed:gzip:best speed")))

     {
      CU_FAIL( "Corrupted Profile  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setPayloadType (r, makeS("text/html; charset=unicode-utf8")))

     
     {
      CU_FAIL( "Corrupted PayLoadType  \n");
      destroy (r);
      destroy (w);
      return;
     }

    if (WRecord_setContentFromString (r, makeS ("some text here\r\nto test if the header string works\r\nlets hope that it will work\r\n")))
       {
        CU_FAIL ("bad string\n");
        destroy (r);
        destroy (w);
        return;
       }

    if (WRecord_setContentFromStringConcat (r, makeS ("an extra text to test with it\r\nlets hope that this will also work\r\n")))
       {
        CU_FAIL ("bad string\n");
        destroy (r);
        destroy (w);
        return;
       }

    if (WRecord_setContentFromStringConcat (r, makeS ("an extra text to test with it\r\nlets hope that this will also work\r\n")))
       {
        CU_FAIL ("bad string\n");
        destroy (r);
        destroy (w);
        return;
       }

    if (WRecord_setContentFromStringConcat (r, makeS ("an extra text to test with it\r\nlets hope that this will also work\r\n")))
       {
        CU_FAIL ("bad string\n");
        destroy (r);
        destroy (w);
        return;
       }

    if (WRecord_setContentFromStringConcat (r, makeS ("an extra text to test with it\r\nlets hope that this will also work\r\n")))
       {
        CU_FAIL ("bad string\n");
        destroy (r);
        destroy (w);
        return;
       }


/*  if (WRecord_setContentFromFileName (r, file1) )
    {
      CU_FAIL( "record not filled\n");
      destroy (r);
      destroy (w);
      return;
    }*/

  if (WFile_storeRecord (w, r) )
    {
      CU_FAIL( "record not stored\n");
      destroy (r);
      destroy (w);
      return;
    }

  CU_PASS( "OK\n");

  destroy (r);
  destroy (w);

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
   if (NULL == pSuite) 
     {
       CU_cleanup_registry();
       return CU_get_error();
     }
   
   
   /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite, "TEST 1:creating Warc File './awanvl.warc' with one record an no anvl field", test1)) ||
       (NULL == CU_add_test(pSuite, "TEST 2:creating a Warc File './awarcavl.warc' with one record containing one anvl field this recond will not be created (presence of ip-adress or refers-to in warc info record)", test2))||
       (NULL == CU_add_test(pSuite, "TEST 3:Creating a Warc File '/awarcmlp.warc' with two Records ", test3))||
       (NULL == CU_add_test(pSuite, "TEST 4:Trying to create a Warc File with a Warc record having a corrupted Creation Date field Corrupted date", test4)))
     {
       CU_cleanup_registry();
       return CU_get_error();
     } 
   
   /* add a suite to the registry */
   pSuite = CU_add_suite("Suite2", init_suite2, clean_suite2);
   if (NULL == pSuite) 
     {
       CU_cleanup_registry();
       return CU_get_error();
     }
   
   /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite, "TEST 5:Trying to create a Warc File with a Warc record having a corrupted Record Type field Corrupted Record type  ", test5)) ||
       (NULL == CU_add_test(pSuite, "TEST 6:Trying to create a Warc File with a Warc record having a corrupted anvl field Corrupted anvl", test6))||
       (NULL == CU_add_test(pSuite, "TEST 7:Trying to create a Warc File with a Warc record having a corrupted truncated field Corrupted Truncated  ", test7))||
       (NULL == CU_add_test(pSuite, "TEST 8: Trying to create a Warc File with a Warc record having a corrupted ip adress field", test8))||
	(NULL == CU_add_test(pSuite, "TEST 9:  Trying to create a Warc File with a Warc record having a corrupted bloc digest field Corrupted Bloc Digest to  ", test9)))
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

   if ((NULL == CU_add_test(pSuite, "TEST 10:Trying to create a Warc File with a Warc record having a corrupted payload digest field Corrupted Pay Load Digest to ", test10)) ||
       (NULL == CU_add_test(pSuite, "TEST 11:Trying to create a Warc File with a Warc record having a corrupted FileName Corrupted Pay Load Digest to ", test11))||
       (NULL == CU_add_test(pSuite, "TEST 12:Trying to create a Warc File with a Warc record  having a corrupted segment number Corrupted Pay Load Digest to   ", test12))||
       (NULL == CU_add_test(pSuite, "TEST 13: Trying to create a Warc File with a Warc record having a corrupted segment Total length Corrupted Pay Load Digest to ", test13))||
	(NULL == CU_add_test(pSuite, "TEST 14:  trying to create compressed warc file  ", test14)))
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

   if ((NULL == CU_add_test(pSuite, "TEST 15:trying to create compressed warc file with two record ", test15)) ||
       (NULL == CU_add_test(pSuite, "TEST 16:reading a warc file who contain one record>", test16))||
       (NULL == CU_add_test(pSuite, "TEST 17:reading a warc file who contain two record (we not read) the bloc of data> ", test17))||
       (NULL == CU_add_test(pSuite, "TEST 18: redaing a warc file compressed who contains 2 record>", test18))||
	(NULL == CU_add_test(pSuite, "TEST 19:reading a corrupter warc file (error in record) type > ", test19)))
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

   if ((NULL == CU_add_test(pSuite, "TEST 20:reading a second corrupter warc file (error in bloc digest) > ", test20)) ||
       (NULL == CU_add_test(pSuite, "TEST 21:reading a third corrupter warc file ( error in ip adress)> ", test21))||
       (NULL == CU_add_test(pSuite, "TEST 22:trying to give another record  the read one in the registration phase>  ", test22))||
       (NULL == CU_add_test(pSuite, "TEST 23: trying to give to register the record in another WFile object> ", test23)))
	
             {
      CU_cleanup_registry();
      return CU_get_error();
             }

/* add a suite to the registry */
   pSuite = CU_add_suite("Suite6", init_suite6, clean_suite6);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
                        }

   /* add the tests to the suite */

   if ((NULL == CU_add_test(pSuite, "TEST 24:creatig a warcfile with a content filled from strings > ", test24)))	
             {
      CU_cleanup_registry();
      return CU_get_error();
             }


switch (menu()) 
  {
        case 1: {CU_console_run_tests(); break;} 
	case 2:  {
                       
                           case 21: {CU_basic_set_mode(CU_BRM_NORMAL); CU_basic_run_tests(); break;}
                            case 22:{CU_basic_set_mode(CU_BRM_VERBOSE ); CU_basic_run_tests(); break;}
                             case 23:{CU_basic_set_mode(CU_BRM_SILENT); CU_basic_run_tests(); break;}  

                              }
        case 3:{
                CU_set_output_filename("./utest/outputs/file");
    		CU_set_output_filename("./utest/outputs/file" );
  		CU_automated_run_tests();
   		CU_list_tests_to_file();
           	break;}
     
   }
   CU_cleanup_registry();
   return CU_get_error();
}




