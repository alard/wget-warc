
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
#include <afile.h>       /* AFile */
#include <arecord.h>     /* ARecord */

int init_suite1(void) {return 0; }
int clean_suite1(void) {return 0; }
int init_suite2(void) {return 0; }
int clean_suite2(void) {return 0; }
warc_bool_t callback (void * env, const char* buff, const warc_u32_t size)
{
  UNUSED (env);

  if (size)
    {
      warc_u32_t  i = 0;

      while (i < size)
        {
          fprintf (stdout, "%c", buff[i]);
          ++i;
        }

      fprintf (stdout, "\n");
    }

  return (WARC_TRUE); /* return WARC_FALSE to stop extraction */
}


void test1 (void)
{
 
  void        * a = bless (AFile, "./app/wdata/arc2warc/file.arc",
                           ARC_FILE_UNCOMPRESSED, ".");


  assert (a);
CU_ASSERT_PTR_NOT_EQUAL(a,NIL);

  while (AFile_hasMoreRecords (a) )
    {
      void * ar  = AFile_nextRecord (a);

      unless (ar)
      {
        destroy (a);
        return;
      }

     /* fprintf (stdout, "URL:  %s\n", ARecord_getUrl (ar) );
      fprintf (stdout, "Date: %s\n", ARecord_getCreationDate (ar) );
      fprintf (stdout, "Mime: %s\n", ARecord_getMimeType (ar) );
      fprintf (stdout, "IP:   %s\n", ARecord_getIpAddress (ar) );*/
        CU_ASSERT_STRING_EQUAL("filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc",ARecord_getUrl (ar));
        CU_ASSERT_STRING_EQUAL("20070524113301",ARecord_getCreationDate (ar));
        CU_ASSERT_STRING_EQUAL("text/plain",ARecord_getMimeType (ar));
        CU_ASSERT_STRING_EQUAL("0.0.0.0",ARecord_getIpAddress(ar));
      if (AFile_register (a, ar, callback, (void *) 0) )
        {
          CU_FAIL("unable to register the callbck");
          destroy (a);
          destroy (ar);
          return;
        }

      /*ARecord_getContent (ar);*/

      destroy (ar);
    }

  destroy (a);

 return;
}
void test2 (void)
{
  
  void        * a = bless (AFile, "./app/wdata/arc2warc/file.arc",
                           ARC_FILE_UNCOMPRESSED, ".");
  void        * a2  = bless (AFile, "./app/wdata/arc2warc/mrec.arc",
                             ARC_FILE_UNCOMPRESSED, ".");

  assert (a);
CU_ASSERT_PTR_NOT_EQUAL(a,NIL);
  while (AFile_hasMoreRecords (a) )
    {
      void * ar  = AFile_nextRecord (a);

      unless (ar)
      {
        destroy (a);
        return;
      }

     /* fprintf (stdout, "URL:  %s\n", ARecord_getUrl (ar) );
      fprintf (stdout, "Date: %s\n", ARecord_getCreationDate (ar) );
      fprintf (stdout, "Mime: %s\n", ARecord_getMimeType (ar) );
      fprintf (stdout, "IP:   %s\n", ARecord_getIpAddress (ar) );*/
        CU_ASSERT_STRING_EQUAL("filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc",ARecord_getUrl (ar));
        CU_ASSERT_STRING_EQUAL("20070524113301",ARecord_getCreationDate (ar));
        CU_ASSERT_STRING_EQUAL("text/plain",ARecord_getMimeType (ar));
        CU_ASSERT_STRING_EQUAL("0.0.0.0",ARecord_getIpAddress(ar));
      destroy (a);

      if (AFile_register (a2, ar, callback, (void *) 0) )
        {
          CU_PASS( "unable to register the callback");
          destroy (a2);
          destroy (ar);
          return;
        }

    /*  ARecord_getContent (ar);*/

      destroy (ar);
    }

  destroy (a2);

 return;
}

void test3 (void)
{
  
  void        * a = bless (AFile, "./app/wdata/arc2warc/mrec.arc",
                           ARC_FILE_UNCOMPRESSED, ".");


  assert (a);
CU_ASSERT_PTR_NOT_EQUAL(a,NIL);
  while (AFile_hasMoreRecords (a) )
    {
      void * ar  = AFile_nextRecord (a);

      unless (ar)
      {
        destroy (a);
        return;
      }

    /*  fprintf (stdout, "URL:  %s\n", ARecord_getUrl (ar) );
     fprintf (stdout, "Date: %s\n", ARecord_getCreationDate (ar) );
      fprintf (stdout, "Mime: %s\n", ARecord_getMimeType (ar) );
      fprintf (stdout, "IP:   %s\n", ARecord_getIpAddress (ar) );*/
CU_ASSERT_STRING_EQUAL("filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc",ARecord_getUrl (ar));
        CU_ASSERT_STRING_EQUAL("20070524113301",ARecord_getCreationDate (ar));
        CU_ASSERT_STRING_EQUAL("text/plain",ARecord_getMimeType (ar));
        CU_ASSERT_STRING_EQUAL("0.0.0.0",ARecord_getIpAddress(ar));

      if (AFile_register (a, ar, callback, (void *) 0) )
        {
          CU_FAIL ("unable to register the callback");
          destroy (a);
          destroy (ar);
          return;
        }

     /* ARecord_getContent (ar);*/

      destroy (ar);
    }

  destroy (a);

 return;
}

void test4 (void)
{
 
  void        * a = bless (AFile, "./app/wdata/arc2warc/err1.arc",
                           ARC_FILE_UNCOMPRESSED, ".");
fprintf (stdout, "/////////////////////////////////////test 4 /////////////////////////////////\n");
 

  assert (a);
CU_ASSERT_PTR_NOT_EQUAL(a,NIL);
  while (AFile_hasMoreRecords (a) )
    {
      void * ar  = AFile_nextRecord (a);

      unless (ar)
      {
        destroy (a);
        return;
      }

     /* fprintf (stdout, "URL:  %s\n", ARecord_getUrl (ar) );
     fprintf (stdout, "Date: %s\n", ARecord_getCreationDate (ar) );
      fprintf (stdout, "Mime: %s\n", ARecord_getMimeType (ar) );
      fprintf (stdout, "IP:   %s\n", ARecord_getIpAddress (ar) );*/
	CU_ASSERT_STRING_EQUAL("filedesc://BnF-elec2007-20061119183301-00236-atlas1.arc",ARecord_getUrl (ar));
        CU_ASSERT_STRING_EQUAL("20061119183301",ARecord_getCreationDate (ar));
        CU_ASSERT_STRING_EQUAL("text/plain",ARecord_getMimeType (ar));
        CU_ASSERT_STRING_EQUAL("0.0.0.0",ARecord_getIpAddress(ar));

      if (AFile_register (a, ar, callback, (void *) 0) )
        {
          CU_FAIL( "unable to register the callback");
          destroy (a);
          destroy (ar);
          return;
        }

      /*ARecord_getContent (ar);*/

      destroy (ar);
    }

  destroy (a);

 return;
}

void test5 (void)
{
  
  void        * a = bless (AFile, "./app/wdata/arc2warc/err2.arc",
                           ARC_FILE_UNCOMPRESSED, ".");
fprintf (stdout, "/////////////////////////////////////test 5 /////////////////////////////////\n");
 
  assert (a);
CU_ASSERT_PTR_NOT_EQUAL(a,NIL);
  while (AFile_hasMoreRecords (a) )
    {
      void * ar  = AFile_nextRecord (a);

      unless (ar)
      {
        destroy (a);
        return;
      }

   /*   fprintf (stdout, "URL:  %s\n", ARecord_getUrl (ar) );
    fprintf (stdout, "Date: %s\n", ARecord_getCreationDate (ar) );
      fprintf (stdout, "Mime: %s\n", ARecord_getMimeType (ar) );
      fprintf (stdout, "IP:   %s\n", ARecord_getIpAddress (ar) );*/
	CU_ASSERT_STRING_EQUAL("filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc",ARecord_getUrl (ar));
        CU_ASSERT_STRING_EQUAL("20070524113301",ARecord_getCreationDate (ar));
        CU_ASSERT_STRING_EQUAL("text/plain",ARecord_getMimeType (ar));
        CU_ASSERT_STRING_EQUAL("0.0.0.0",ARecord_getIpAddress(ar));

      if (AFile_register (a, ar, callback, (void *) 0) )
        {
          CU_FAIL("unable to register the callback");
          destroy (a);
          destroy (ar);
          return;
        }

/*      ARecord_getContent (ar);*/

      destroy (ar);
    }

  destroy (a);

 return;
}

void test6 (void)
{
 
  void        * a = bless (AFile, "./app/wdata/arc2warc/file.arc.gz",
                           ARC_FILE_COMPRESSED_GZIP, ".");



  assert (a);
CU_ASSERT_PTR_NOT_EQUAL(a,NIL);
  while (AFile_hasMoreRecords (a) )
    {
      void * ar  = AFile_nextRecord (a);

      unless (ar)
      {
        destroy (a);
        return;
      }

      /*fprintf (stdout, "URL:  %s\n", ARecord_getUrl (ar) );
     fprintf (stdout, "Date: %s\n", ARecord_getCreationDate (ar) );
      fprintf (stdout, "Mime: %s\n", ARecord_getMimeType (ar) );
      fprintf (stdout, "IP:   %s\n", ARecord_getIpAddress (ar) );*/
	CU_ASSERT_STRING_EQUAL("filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc",ARecord_getUrl (ar));
        CU_ASSERT_STRING_EQUAL("20070524113301",ARecord_getCreationDate (ar));
        CU_ASSERT_STRING_EQUAL("text/plain",ARecord_getMimeType (ar));
        CU_ASSERT_STRING_EQUAL("0.0.0.0",ARecord_getIpAddress(ar));

      if (AFile_register (a, ar, callback, (void *) 0) )
        {
          CU_FAIL( "unable to register the callback");
          destroy (a);
          destroy (ar);
          return;
        }

   /*   ARecord_getContent (ar);*/

      destroy (ar);
    }

  destroy (a);

 return;
}

void test7 (void)
{
 
  void        * a = bless (AFile, "./app/wdata/arc2warc/sfile.arc.gz",
                           ARC_FILE_COMPRESSED_GZIP, ".");

 

  assert (a);
CU_ASSERT_PTR_NOT_EQUAL(a,NIL);
  while (AFile_hasMoreRecords (a) )
    {
      void * ar  = AFile_nextRecord (a);

      unless (ar)
      {
        destroy (a);
        return;
      }

      /*fprintf (stdout, "URL:  %s\n", ARecord_getUrl (ar) );
      fprintf (stdout, "Date: %s\n", ARecord_getCreationDate (ar) );
      fprintf (stdout, "Mime: %s\n", ARecord_getMimeType (ar) );
      fprintf (stdout, "IP:   %s\n", ARecord_getIpAddress (ar) );*/
	CU_ASSERT_STRING_EQUAL("filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc",ARecord_getUrl (ar));
        CU_ASSERT_STRING_EQUAL("20070524113301",ARecord_getCreationDate (ar));
        CU_ASSERT_STRING_EQUAL("text/plain",ARecord_getMimeType (ar));
        CU_ASSERT_STRING_EQUAL("0.0.0.0",ARecord_getIpAddress(ar));

      if (AFile_register (a, ar, callback, (void *) 0) )
        {
          CU_FAIL( "unable to register the callback");
          destroy (a);
          destroy (ar);
          return;
        }

      /*ARecord_getContent (ar);*/

      destroy (ar);
    }

  destroy (a);

 return;
}

void test8 (void)
{
 
  void        * a = bless (AFile, "./app/wdata/arc2warc/mfile.arc.gz",
                           ARC_FILE_COMPRESSED_GZIP, ".");
fprintf (stdout, "/////////////////////////////////////test 8 /////////////////////////////////\n");
 

  assert (a);
CU_ASSERT_PTR_NOT_EQUAL(a,NIL);
  while (AFile_hasMoreRecords (a) )
    {
      void * ar  = AFile_nextRecord (a);

      unless (ar)
      {
        destroy (a);
        return;
      }

      /*fprintf (stdout, "URL:  %s\n", ARecord_getUrl (ar) );
      fprintf (stdout, "Date: %s\n", ARecord_getCreationDate (ar) );
      fprintf (stdout, "Mime: %s\n", ARecord_getMimeType (ar) );
      fprintf (stdout, "IP:   %s\n", ARecord_getIpAddress (ar) );*/
	CU_ASSERT_STRING_EQUAL("filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc",ARecord_getUrl (ar));
        CU_ASSERT_STRING_EQUAL("20070524113301",ARecord_getCreationDate (ar));
        CU_ASSERT_STRING_EQUAL("text/plain",ARecord_getMimeType (ar));
        CU_ASSERT_STRING_EQUAL("0.0.0.0",ARecord_getIpAddress(ar));

      if (AFile_register (a, ar, callback, (void *) 0) )
        {
          CU_FAIL("unable to register the callback");
          destroy (a);
          destroy (ar);
          return;
        }

     /* ARecord_getContent (ar);*/

      destroy (ar);
    }

  destroy (a);

 return;
}

void test9 (void)
{
  
  void        * a = bless (AFile, "./app/wdata/arc2warc/mmfile.arc.gz",
                           ARC_FILE_COMPRESSED_GZIP, ".");

 fprintf (stdout, "/////////////////////////////////////test 9 /////////////////////////////////\n"); 

  assert (a);
CU_ASSERT_PTR_NOT_EQUAL(a,NIL);
  while (AFile_hasMoreRecords (a) )
    {
      void * ar  = AFile_nextRecord (a);

      unless (ar)
      {
        destroy (a);
        return;
      }

   /*   fprintf (stdout, "URL:  %s\n", ARecord_getUrl (ar) );
     fprintf (stdout, "Date: %s\n", ARecord_getCreationDate (ar) );
      fprintf (stdout, "Mime: %s\n", ARecord_getMimeType (ar) );
      fprintf (stdout, "IP:   %s\n", ARecord_getIpAddress (ar) );*/
	CU_ASSERT_STRING_EQUAL("filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc",ARecord_getUrl (ar));
        CU_ASSERT_STRING_EQUAL("20070524113301",ARecord_getCreationDate (ar));
        CU_ASSERT_STRING_EQUAL("text/plain",ARecord_getMimeType (ar));
        CU_ASSERT_STRING_EQUAL("0.0.0.0",ARecord_getIpAddress(ar));

      if (AFile_register (a, ar, callback, (void *) 0) )
        {
          CU_FAIL("unable to register the callback");
          destroy (a);
          destroy (ar);
          return;
        }

     /* ARecord_getContent (ar);*/

      destroy (ar);
    }

  destroy (a);

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

   if ((NULL == CU_add_test(pSuite, "TEST 1:uncompressed ARC file Which contains a single record ", test1)) ||
       (NULL == CU_add_test(pSuite, "TEST 2:uncompressed ARC file try to register another record in afile either the read one", test2))||
       (NULL == CU_add_test(pSuite, "TEST 3:uncompressed  ARC file Which contains several records ", test3))||
       (NULL == CU_add_test(pSuite, "TEST 4:uncompressed ARC file Which contains several records ( the second record are corrupted )", test4)))
      
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

   if ((NULL == CU_add_test(pSuite, "TEST 5:uncompressed ARC file Which contains several records ( the first record are corrupted )  ", test5)) ||
       (NULL == CU_add_test(pSuite, "TEST 6:compressed ARC file  Which contains a single record ", test6))||
       (NULL == CU_add_test(pSuite, "TEST 7:compressed ARC file Which contains several records  ", test7))||
       (NULL == CU_add_test(pSuite, "TEST 8: compressed ARC file Which contains several records ( the second record are corrupted )", test8))||
	(NULL == CU_add_test(pSuite, "TEST 9: compressed ARC file Which contains several records ( the first record are corrupted ) ", test9)))
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
                CU_set_output_filename("./utest/outputs/arcfile");
    		CU_set_output_filename("./utest/outputs/arcfile" );
  		CU_automated_run_tests();
   		CU_list_tests_to_file();
           	break;}
     
   }
   CU_cleanup_registry();
   return CU_get_error();
}
