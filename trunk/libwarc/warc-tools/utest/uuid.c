
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
#include <assert.h>
#include <stdio.h>
#include <menu.h>
#include <warc.h>

#define makeU(s) (const warc_u8_t *) (s), (warc_u64_t) w_strlen((warc_u8_t *) (s))
int init_suite1(void) { return 0; }
int clean_suite1(void) { return 0; }
 
void test1 (void)
{
  void       * u = NIL;
  const char * s = NIL;
  /* empty string */
  u = bless (WUUID);
  CU_ASSERT_PTR_NOT_EQUAL(u,NIL);
  WUUID_hash (u, makeU ("") );
  s = WUUID_text (u);
  CU_ASSERT_STRING_EQUAL(s, "uuid:24F0130C63AC933216166E76B1BB925FF373DE2D49584E7A"); 
  /*fprintf (stdout, "%s\n", s);*/
  WUUID_reinit (u); /* re-initialize the Tiger buffer */
  WUUID_hash (u, makeU ("Hello World") );
  s = WUUID_text (u);
  CU_ASSERT_STRING_EQUAL(s, "uuid:C01CED32B823AB2B24299F5A8E8B495455AA2AA2352F04D4"); 
 /* fprintf (stdout, "%s\n", s);*/

  destroy (u);

}

void test2 (void)
{

  void       * u = NIL;
  const char * s = NIL;
  /* empty string */
  u = bless (WUUID);
  CU_ASSERT_PTR_NOT_EQUAL(u,NIL);
  WUUID_hash (u, makeU ("Tiger Hashing") );
  s = WUUID_text (u);
/* fprintf (stdout, "%s\n", s);*/
  CU_ASSERT_STRING_EQUAL(s, "uuid:857E9A95798EFA1164240BC483EE63FC7EEC1898B2F0A245"); 
  WUUID_reinit (u); /* re-initialize the Tiger buffer */
  s = WUUID_text (u);
  /*fprintf (stdout, "%s\n", s);*/
  CU_ASSERT_STRING_EQUAL(s, "uuid:0123456789ABCDEFFEDCBA9876543210F096A5B4C3B2E187"); 
  destroy (u);

}

void test3 (void)
{

  void       * u = NIL;
  const char * s = NIL;

  /* empty string */
  u = bless (WUUID);
  CU_ASSERT_PTR_NOT_EQUAL(u,NIL);
  WUUID_hash (u, makeU ("Hello") );
  WUUID_hash (u, makeU ("World") );
  s = WUUID_text (u);
  /*fprintf (stdout, "%s\n", s);*/
  CU_ASSERT_STRING_EQUAL(s, "uuid:73CB4D1F4675C95E0B9E4A0B8974AD14CD1DFDB91E01BCD1"); 
  WUUID_reinit (u); /* re-initialize the Tiger buffer */
  WUUID_hash (u, makeU ("Hello World") );
  s = WUUID_text (u);
  /*fprintf (stdout, "%s\n", s);*/
  CU_ASSERT_STRING_EQUAL(s, "uuid:C01CED32B823AB2B24299F5A8E8B495455AA2AA2352F04D4"); 
  destroy (u);

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
   if ((NULL == CU_add_test(pSuite, "TEST 1: ", test1)) ||
       (NULL == CU_add_test(pSuite, "TEST 2:  ", test2))||
       (NULL == CU_add_test(pSuite, "TEST 3:  ", test3 )))
      
   {
      CU_cleanup_registry();
      return CU_get_error();
   }
 /* Run all tests using the automated interface*/ 
    switch (menu()) 
  {
        case 1: {CU_console_run_tests();break;} 
	case 2:  {
                       
                           case 21: {CU_basic_set_mode(CU_BRM_NORMAL); CU_basic_run_tests(); break;}
                            case 22:{CU_basic_set_mode(CU_BRM_VERBOSE ); CU_basic_run_tests(); break;}
                             case 23:{CU_basic_set_mode(CU_BRM_SILENT); CU_basic_run_tests(); break;}  

                              }
        case 3:{
                CU_set_output_filename("./utest/outputs/uuid");
    		CU_set_output_filename("./utest/outputs/uuid" );
  		CU_automated_run_tests();
   		CU_list_tests_to_file();
           	break;}
   }
   CU_cleanup_registry();
   return CU_get_error();

}
