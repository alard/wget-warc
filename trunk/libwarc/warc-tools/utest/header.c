
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
#include <Basic.h>
#include <Console.h>
#include <Automated.h>
#include <CUnit.h>
#include <CUError.h>
#include <TestDB.h>
#include <TestRun.h>
#include <menu.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <warc.h>

#include <wheader.h>     /* WHeader */



#define makeS(s) ((warc_u8_t *) (s)), w_strlen((warc_u8_t *) (s))
int init_suite1(void) { return 0; }
int clean_suite1(void) { return 0; }

void test1 (void)
{
  
  void       * h = bless (WHeader,
                           WARC_INFO_RECORD,
                           makeS ("id://warc-x584jz39"),
                           makeS ("12172007") );

  assert (h);
 CU_ASSERT_PTR_NOT_EQUAL(h,NIL);
 /* fprintf (stdout, "WarcId:       %s, length: %d\n", WHeader_getWarcId (h), 
                    WHeader_getWarcIdLen (h));
  fprintf (stdout, "RecordType:   %d\n", WHeader_getRecordType(h));
  fprintf (stdout, "CreationDate: %s, length: %d\n", WHeader_getCreationDate (h), 
                    WHeader_getCreationDateLen (h));
  fprintf (stdout, "RecordId:     %s, length: %d\n", WHeader_getRecordId(h), 
                   WHeader_getRecordIdLen (h));*/

 CU_ASSERT_STRING_EQUAL(WARC_VERSION,WHeader_getWarcId (h)  );
CU_ASSERT(9==WHeader_getWarcIdLen (h)); 
  CU_ASSERT(1==WHeader_getRecordType(h)  ); 
 CU_ASSERT_STRING_EQUAL("12172007", WHeader_getCreationDate (h) ); 
 CU_ASSERT(8==WHeader_getCreationDateLen (h) ); 
 CU_ASSERT_STRING_EQUAL("id://warc-x584jz39", WHeader_getRecordId(h)); 
 CU_ASSERT(18== WHeader_getRecordIdLen (h)); 

  destroy (h);

  return ;
}


void test2 (void)
{

  void       * h = bless (WHeader,
                           WARC_INFO_RECORD,
                           makeS ("id://warc-x584jz39"),
                           makeS ("12172007") );

  assert (h);
 CU_ASSERT_PTR_NOT_EQUAL(h,NIL);
 


    WHeader_setContentLength (h, 12);
    WHeader_setSubjectUri (h, makeS("http://www.fromhome.hom/f1/path"));
    WHeader_setContentType (h, makeS("appliaction/something"));
    WHeader_setConcurrentTo (h, makeS("concuuid://id1:warc-x584sld56"));
    WHeader_setBlockDigest (h, makeS("sha1:0451-9645-AM35P-12LL"));
    WHeader_setPayloadDigest (h, makeS("sha2:0121-KJ45-1111-A001L"));
    WHeader_setIpAddress (h, makeS("192.165.1.0"));
    WHeader_setRefersTo (h, makeS("refuuid://id14:warc-x265sll76"));
    WHeader_setTruncated (h, makeS("timeout:14s"));
    WHeader_setInfoId (h, makeS("infuuid://id0:warc-x000sl076"));
    WHeader_setFilename (h, makeS("/home/user/warcfile.warc"));
    WHeader_setProfile (h, makeS("Compressed:gzip:best speed"));
    WHeader_setPayloadType (h, makeS("text/html; charset=unicode-utf8"));
    WHeader_setSegmentOriginId (h, makeS("orgid://w45-pp78-4574"));
    WHeader_setSegmentNumber (h, 1);
    WHeader_setSegTotalLength (h, 145500);

    /*fprintf (stdout, "Content Langth: %d\n", WHeader_getContentLength (h));*/
	CU_ASSERT(12==WHeader_getContentLength (h)); 

   /* fprintf (stdout, "Subject Uri: %s, length: %d\n", WHeader_getSubjectUri (h),
                     WHeader_getSubjectUriLen (h));*/
           CU_ASSERT_STRING_EQUAL("http://www.fromhome.hom/f1/path",WHeader_getSubjectUri (h) );
            CU_ASSERT(31==WHeader_getSubjectUriLen (h)); 

   /* fprintf (stdout, "Content type: %s, length: %d\n", WHeader_getContentType (h),
                     WHeader_getContentTypeLen (h));*/
 	CU_ASSERT_STRING_EQUAL("appliaction/something",WHeader_getContentType (h) );
            CU_ASSERT(21== WHeader_getContentTypeLen (h)); 

    /*fprintf (stdout, "Concurrent Id: %s, length: %d\n", WHeader_getConcurrentTo (h),
                     WHeader_getConcurrentToLen (h));*/
	CU_ASSERT_STRING_EQUAL("concuuid://id1:warc-x584sld56",WHeader_getConcurrentTo (h) );
            CU_ASSERT(29== WHeader_getConcurrentToLen (h)); 

   /* fprintf (stdout, "Block Digest: %s, length: %d\n", WHeader_getBlockDigest (h),
                     WHeader_getBlockDigestLen (h));*/
CU_ASSERT_STRING_EQUAL("sha1:0451-9645-AM35P-12LL",WHeader_getBlockDigest (h));
            CU_ASSERT(25== WHeader_getBlockDigestLen (h)); 

  /*  fprintf (stdout, "Payload Digest: %s, length %d\n", WHeader_getPayloadDigest (h),
                     WHeader_getPayloadDigestLen (h));*/
CU_ASSERT_STRING_EQUAL("sha2:0121-KJ45-1111-A001L", WHeader_getPayloadDigest (h));
            CU_ASSERT(25==  WHeader_getPayloadDigestLen (h)); 

    /*fprintf (stdout, "Ip Address: %s, length: %d\n", WHeader_getIpAddress (h),
                     WHeader_getIpAddressLen (h));*/
CU_ASSERT_STRING_EQUAL("192.165.1.0",WHeader_getIpAddress (h));
            CU_ASSERT(11== WHeader_getIpAddressLen (h)); 

  /*  fprintf (stdout, "Refers To: %s, length: %d\n", WHeader_getRefers (h),
                     WHeader_getRefersToLen (h));*/
CU_ASSERT_STRING_EQUAL("refuuid://id14:warc-x265sll76",WHeader_getRefers (h));
            CU_ASSERT(29== WHeader_getRefersToLen (h)); 

    /*fprintf (stdout, "Trucation: %s, length: %d\n", WHeader_getTruncated (h),
                     WHeader_getTruncatedLen (h));*/
CU_ASSERT_STRING_EQUAL("timeout:14s",WHeader_getTruncated (h) );
            CU_ASSERT(11==  WHeader_getTruncatedLen (h)); 

   /* fprintf (stdout, "Warc Info Id: %s, length: %d\n", WHeader_getInfoId (h),
                     WHeader_getInfoIdLen (h));*/
CU_ASSERT_STRING_EQUAL("infuuid://id0:warc-x000sl076",WHeader_getInfoId (h));
            CU_ASSERT(28== WHeader_getInfoIdLen (h)); 

 /*   fprintf (stdout, "Profile: %s, length: %d\n", WHeader_getProfile (h),
                     WHeader_getProfileLen (h));*/
CU_ASSERT_STRING_EQUAL("Compressed:gzip:best speed",WHeader_getProfile (h));
            CU_ASSERT(26== WHeader_getProfileLen (h)); 

  /*  fprintf (stdout, "Filename: %s, length: %d\n", WHeader_getFilename (h),
                     WHeader_getFilenameLen (h));*/
CU_ASSERT_STRING_EQUAL("/home/user/warcfile.warc",WHeader_getFilename (h));
            CU_ASSERT(24==  WHeader_getFilenameLen (h)); 

  /*  fprintf (stdout, "Payload Type: %s, length: %d\n", WHeader_getPayloadType (h),
                     WHeader_getPayloadTypeLen (h));*/
CU_ASSERT_STRING_EQUAL("text/html; charset=unicode-utf8",WHeader_getPayloadType (h));
            CU_ASSERT(31==WHeader_getPayloadTypeLen (h)); 

   /* fprintf (stdout, "Origin Segment Id: %s, length :%d\n", WHeader_getSegmentOriginId (h),
                    WHeader_getSegmentOriginIdLen (h));*/
CU_ASSERT_STRING_EQUAL("orgid://w45-pp78-4574",WHeader_getSegmentOriginId (h) );
            CU_ASSERT(21==  WHeader_getSegmentOriginIdLen (h)); 

    /*fprintf (stdout, "Segment number: %d\n", WHeader_getSegmentNumber (h));*/
            CU_ASSERT(1== WHeader_getSegmentNumber (h)); 

  /*  fprintf (stdout, "Segments Total Size: %d\n", WHeader_getSegmentNumber (h));*/
 CU_ASSERT(145500==WHeader_getSegTotalLength (h)); 

  destroy (h);

  return ;
}


void test3 (void)
{
 
  void       * h = bless (WHeader,
                           WARC_UNKNOWN_RECORD,
                           makeS (" "),
                           makeS (" "));
 
  const char * fname = "./app/wdata/testheader/header1.warc";
  FILE       * file = NIL;
fprintf(stdout,"//////////test3/////////\n");
  assert (h);
 CU_ASSERT_PTR_NOT_EQUAL(h,NIL);
  
  file = fopen (fname, "rb");
    unless (file)
      {
      CU_FAIL("No file Found");
      destroy (h);
      return ;
      }

  if (WHeader_extractFromWarcFile (h, file))
     {
     CU_PASS ("This is not a valid warc header");
     fclose (file);
     destroy (h);
     return ;
     }

  fclose (file);

 CU_FAIL( (stdout, "Content Langth: %d\n", WHeader_getContentLength (h)));
	

    CU_FAIL(fprintf (stdout, "Subject Uri: %s, length: %d\n", WHeader_getSubjectUri (h),
                     WHeader_getSubjectUriLen (h)));
          
    CU_FAIL( fprintf (stdout, "Content type: %s, length: %d\n", WHeader_getContentType (h),
                     WHeader_getContentTypeLen (h)));
 	
    CU_FAIL( fprintf (stdout, "Concurrent Id: %s, length: %d\n", WHeader_getConcurrentTo (h),
                     WHeader_getConcurrentToLen (h)));
	
    CU_FAIL( fprintf (stdout, "Block Digest: %s, length: %d\n", WHeader_getBlockDigest (h),
                     WHeader_getBlockDigestLen (h)));


    CU_FAIL(fprintf (stdout, "Payload Digest: %s, length %d\n", WHeader_getPayloadDigest (h),
                     WHeader_getPayloadDigestLen (h)));


    CU_FAIL( fprintf (stdout, "Ip Address: %s, length: %d\n", WHeader_getIpAddress (h),
                     WHeader_getIpAddressLen (h)));


   CU_FAIL(fprintf (stdout, "Refers To: %s, length: %d\n", WHeader_getRefers (h),
                     WHeader_getRefersToLen (h)));

     CU_FAIL(fprintf (stdout, "Trucation: %s, length: %d\n", WHeader_getTruncated (h),
                     WHeader_getTruncatedLen (h)));


     CU_FAIL(fprintf (stdout, "Warc Info Id: %s, length: %d\n", WHeader_getInfoId (h),
                     WHeader_getInfoIdLen (h)));


     CU_FAIL(fprintf (stdout, "Profile: %s, length: %d\n", WHeader_getProfile (h),
                     WHeader_getProfileLen (h)));

    CU_FAIL(fprintf (stdout, "Filename: %s, length: %d\n", WHeader_getFilename (h),
                     WHeader_getFilenameLen (h)));

     CU_FAIL(fprintf (stdout, "Payload Type: %s, length: %d\n", WHeader_getPayloadType (h),
                     WHeader_getPayloadTypeLen (h)));


     CU_FAIL(fprintf (stdout, "Origin Segment Id: %s, length :%d\n", WHeader_getSegmentOriginId (h),
                    WHeader_getSegmentOriginIdLen (h)));


    CU_FAIL( fprintf (stdout, "Segment number: %d\n", WHeader_getSegmentNumber (h)));
          

 CU_FAIL( fprintf (stdout, "Segments Total Size: %d\n", WHeader_getSegmentNumber (h)));

  destroy (h);

  return ;
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

   if ((NULL == CU_add_test(pSuite, " 1: ", test1)) ||
       (NULL == CU_add_test(pSuite, " 2: ", test2))||
       (NULL == CU_add_test(pSuite, " 3:  ", test3)))      
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
                CU_set_output_filename("./utest/outputs/header");
    		CU_set_output_filename("./utest/outputs/header" );
  		CU_automated_run_tests();
   		CU_list_tests_to_file();
           	break;}
     
   }
   CU_cleanup_registry();
   return CU_get_error();  
}
