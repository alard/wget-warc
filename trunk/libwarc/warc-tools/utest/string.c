
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
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <menu.h>
#include <warc.h>

#define makeS(s) ((warc_u8_t *) s), w_strlen((warc_u8_t *) (s))
#define makeWString(s) (bless (WString, ((warc_u8_t *)s), w_strlen((warc_u8_t *) (s))))


/*                 the suites                         */

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

int init_suite6(void) { return 0; }
int clean_suite6(void) { return 0; }

/*         les tests                                  */

void test11 (void)
{
  void       * s = NIL;
   s = makeWString ("");
   
   CU_ASSERT_PTR_NOT_EQUAL(s,NIL);
   destroy (s);
}

void test12 (void)
{
  void       * s = NIL;
  s = makeWString ("");
  CU_ASSERT_PTR_NOT_EQUAL(s,NIL);
  CU_ASSERT_STRING_EQUAL("", WString_getText   (s)  ); 
  CU_ASSERT_NOT_EQUAL(5, WString_getLength (s) );       			
  destroy (s);  
}

void test13 (void)
{
  void       * s = NIL;
  s = makeWString ("");
  CU_ASSERT_PTR_NOT_EQUAL(s,NIL);
  CU_ASSERT_STRING_EQUAL("",WString_getText   (s)  ); 
  CU_ASSERT_EQUAL(0,WString_getLength (s) );				
  destroy (s);
}
void test21 (void)
{
  void       * s = NIL;
   s = makeWString ("AABBCCDDEEF");
   CU_ASSERT_PTR_NOT_EQUAL(s,NIL);
   destroy (s);
}

void test22 (void)
{
  void       * s = NIL;
  s = makeWString ("AABBCCDDEEF");
  CU_ASSERT_PTR_NOT_EQUAL(s,NIL);
  CU_ASSERT_STRING_EQUAL("AABBCCDDEEF", WString_getText   (s)  ); 
  CU_ASSERT_EQUAL(11, WString_getLength (s) );				
  destroy (s);  
}
void test23 (void)
{
  void       * s = NIL;
  s = makeWString ("AABBCCDDEEF");
  CU_ASSERT_PTR_NOT_EQUAL(s,NIL);
  CU_ASSERT_STRING_NOT_EQUAL("AABBCCDDEEE",WString_getText   (s)  ); 
  CU_ASSERT_EQUAL(11,WString_getLength (s) );				
  destroy (s);
}

void test31 (void)
{
  void * s = NIL;

  s = makeWString ("AA");
  CU_ASSERT_PTR_NOT_EQUAL(s,NIL);
  CU_ASSERT_STRING_EQUAL("AA", WString_getText   (s)  ); 
  CU_ASSERT_EQUAL(2, WString_getLength (s) );
  
  WString_setText (s, makeS ("BBBBB") );
  
  CU_ASSERT_STRING_NOT_EQUAL("AA", WString_getText   (s)  ); 
  CU_ASSERT_EQUAL(5, WString_getLength (s) );
  WString_setText (s, makeS ("AABBCCDDEEF") );
  
  CU_ASSERT_STRING_EQUAL("AABBCCDDEEF", WString_getText   (s)  ); 
  CU_ASSERT_EQUAL(11, WString_getLength (s) );
  
  destroy (s);  
}



void test41 (void)
{
  void       * a = NIL;
  void       * b = NIL;
   a = makeWString ("ABCD");
 
   CU_ASSERT_PTR_NOT_EQUAL(a,NIL);
   b = makeWString ("EF");

   CU_ASSERT_PTR_NOT_EQUAL(b,NIL);
   destroy (a);
   destroy (b);
}

void test42 (void)
{
  void       * a = NIL;
  void       * b = NIL;
   a = makeWString ("ABCD");

   CU_ASSERT_PTR_NOT_EQUAL(a,NIL);
   b = makeWString ("EF");
   
        CU_ASSERT_PTR_NOT_EQUAL(b,NIL);
	CU_ASSERT_STRING_EQUAL("ABCD", WString_getText   (a)  ); 
        CU_ASSERT_EQUAL(4, WString_getLength (a) );
	CU_ASSERT_STRING_EQUAL("EF", WString_getText   (b)  ); 
        CU_ASSERT_EQUAL(2, WString_getLength (b) );
        assert (! WString_concat (a, b) );
	CU_ASSERT_STRING_EQUAL("ABCDEF", WString_getText   (a)  ); 
        CU_ASSERT_EQUAL(6, WString_getLength (a) );
	CU_ASSERT_STRING_NOT_EQUAL("ACDEF", WString_getText   (a)  ); 
        CU_ASSERT_NOT_EQUAL(5, WString_getLength (a) );				
        destroy (a);  
	destroy (b);
}

void test51 (void)
{
  const warc_u8_t * x = (unsigned char *) "better";
  const warc_u8_t * y = (unsigned char *) "c++";
  void                * s = NIL;

  s = makeWString ("warc is better than arc");
  CU_ASSERT_PTR_NOT_EQUAL(s,NIL);
  CU_ASSERT_STRING_EQUAL("warc is better than arc", WString_getText   (s)  ); 
  CU_ASSERT_STRING_EQUAL("better", x  ); 
  CU_ASSERT_EQUAL(8, WString_strstr (s, x) )
  CU_ASSERT_STRING_EQUAL("c++", y  ); 
  CU_ASSERT_EQUAL(-1, WString_strstr (s, y) )
  destroy (s);

}


void test61 (void)
{
  void       * s = NIL;
  s = makeWString ("XML is ... ");
  CU_ASSERT_PTR_NOT_EQUAL(s,NIL);
  CU_ASSERT_STRING_EQUAL("XML is ... ", WString_getText   (s)  ); 
  WString_append (s, makeS ("for the old") );
  CU_ASSERT_STRING_EQUAL("XML is ... for the old", WString_getText   (s)  ); 
  WString_append (s, makeS (" school. ") );
  CU_ASSERT_STRING_EQUAL("XML is ... for the old school. ", WString_getText   (s)  ); 
  WString_append (s, makeS ("JSON is better !!!") );
  CU_ASSERT_STRING_EQUAL("XML is ... for the old school. JSON is better !!!", WString_getText   (s)  );
  destroy (s);
}

int main(void)
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

   if ((NULL == CU_add_test(pSuite, "test_pointeur_NIL()", test11)) ||
       (NULL == CU_add_test(pSuite, "test_chaine1_vide()", test12))||
       (NULL == CU_add_test(pSuite, "test_chaine2_vide()", test13)))
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
 
   if ((NULL == CU_add_test(pSuite, "test_pointeur()", test21)) ||
       (NULL == CU_add_test(pSuite, "test_chaine1()", test22))||
       (NULL == CU_add_test(pSuite, "test_chaine2()", test23)))
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

   if (NULL == CU_add_test(pSuite, "test_pointeur()", test31))
       
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

   if ((NULL == CU_add_test(pSuite, "test_pointeur_a_b()", test41)) ||
       (NULL == CU_add_test(pSuite, "test_concatenation_a_b()", test42)))
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

   if ((NULL == CU_add_test(pSuite, "test_index()", test51)) )
      
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

   if ((NULL == CU_add_test(pSuite, "test_append_s()", test61)) )
      
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
    CU_set_output_filename("./utest/outputs/string");
    CU_set_output_filename("./utest/outputs/string" );
    CU_automated_run_tests();
    CU_list_tests_to_file();
 break; }
  }
/*CU_console_run_tests();*/
 CU_cleanup_registry();
 return CU_get_error();
}
