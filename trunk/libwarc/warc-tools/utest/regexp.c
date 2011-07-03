
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
#include <wcsafe.h>
#include <warc.h>
#include <menu.h>

#define makeS(s) (warc_u8_t *) (s), w_strlen ((warc_u8_t *) (s))


int init_suite1(void) { return 0; }
int clean_suite1(void) { return 0; }
int init_suite2(void) { return 0; }
int clean_suite2(void) { return 0; }

void test1 (void)
{
  void * rexp = NIL;
  warc_bool_t res;
 /*  warc_u8_t   string [20];*/


  rexp = bless (WRegexp, makeS (".*a+.*"));
  CU_ASSERT_PTR_NOT_EQUAL (rexp,NIL);

  res = WRegexp_match (rexp, makeS ("xdaaaaskokd"));
  CU_ASSERT_TRUE (res);
  if (res)
      fprintf (stdout, "matched %s\n", "xdaaaaskokd");

  res = WRegexp_match (rexp, makeS ("aaa"));
  CU_ASSERT_TRUE (res);
  if (res)
      fprintf (stdout, "matched %s\n", "aaa");

  res = WRegexp_match (rexp, makeS ("somethingaaa"));
  CU_ASSERT_TRUE (res);
  if (res)
      fprintf (stdout, "matched %s\n", "somethingaaa");

  destroy (rexp);

}

void test2 (void)
{
  void * rexp = NIL;
  warc_bool_t res;

/*  warc_u8_t   string [20];*/


  rexp = bless (WRegexp, makeS ("aaa"));
  CU_ASSERT_PTR_NOT_EQUAL (rexp,NIL);

  res = WRegexp_match (rexp, makeS ("xdskokd"));
  CU_ASSERT_FALSE (res);
  unless (res)
      fprintf (stdout, "not matched %s\n", "xdskokd");

  res = WRegexp_match (rexp, makeS ("aa"));
  CU_ASSERT_FALSE (res);
  unless (res)
      fprintf (stdout, "not matched %s\n", "aa");

  res = WRegexp_match (rexp, makeS ("a"));
  CU_ASSERT_FALSE (res);
  unless (res)
      fprintf (stdout, "not matched %s\n", "a");

  destroy (rexp);

}


void test3 (void)
{
  void * rexp = NIL;
  warc_bool_t res;
/*  warc_u8_t   string [20];*/


  rexp = bless (WRegexp, makeS ("a+.*"));
  CU_ASSERT_PTR_NOT_EQUAL (rexp,NIL);

  res = WRegexp_match (rexp, makeS ("axdskaokd"));
  CU_ASSERT_TRUE (res);
  if (res)
      fprintf (stdout, "matched %s\n", "axdskaokd");

  res = WRegexp_match (rexp, makeS ("aa"));
  CU_ASSERT_TRUE (res);
  if (res)
      fprintf (stdout, "matched %s\n", "aa");

  res = WRegexp_match (rexp, makeS ("p"));
  CU_ASSERT_FALSE (res);
  unless (res)
      fprintf (stdout, "not matched %s\n", "p");

  destroy (rexp);

}


void test4 (void)
{
  void * rexp = NIL;
  warc_bool_t res;
/*  warc_u8_t   string [20];*/



  rexp = bless (WRegexp, makeS ("aaa"));
  CU_ASSERT_PTR_NOT_EQUAL (rexp,NIL);

  res = WRegexp_search (rexp, makeS ("xdaaaaskokd"));
  CU_ASSERT_TRUE (res);
  if (res)
      fprintf (stdout, "found in %s\n", "xdaaaaskokd");

  res = WRegexp_search (rexp, makeS ("aaa"));
  CU_ASSERT_TRUE (res);
  if (res)
      fprintf (stdout, "found in  %s\n", "aaa");

  res = WRegexp_search (rexp, makeS ("somethingaaa"));
  CU_ASSERT_TRUE (res);
  if (res)
      fprintf (stdout, "found in %s\n", "somethingaaa");

  destroy (rexp);

}

void test5 (void)
{
  void * rexp = NIL;
  warc_bool_t res;
/*  warc_u8_t   string [20];*/


  rexp = bless (WRegexp, makeS ("aaa"));
  CU_ASSERT_PTR_NOT_EQUAL (rexp,NIL);

  res = WRegexp_search (rexp, makeS ("xdskokd"));
  CU_ASSERT_FALSE (res);
  unless (res)
      fprintf (stdout, "not found in %s\n", "xdskokd");

  res = WRegexp_search (rexp, makeS ("aa"));
  CU_ASSERT_FALSE (res);
  unless (res)
      fprintf (stdout, "not found in %s\n", "aa");

  res = WRegexp_search (rexp, makeS ("a"));
  CU_ASSERT_FALSE (res);
  unless (res)
      fprintf (stdout, "not found in %s\n", "a");

  destroy (rexp);

}


void test6 (void)
{
  void * rexp = NIL;
  warc_bool_t res;
/*  warc_u8_t   string [20];*/


  rexp = bless (WRegexp, makeS ("aa.*"));
  CU_ASSERT_PTR_NOT_EQUAL (rexp,NIL);

  res = WRegexp_match (rexp, makeS ("aaxdskaokd"));
  CU_ASSERT_TRUE (res)
  if (res)
      fprintf (stdout, "found in %s\n", "aaxdskaokd");

  res = WRegexp_match (rexp, makeS ("aa"));
  CU_ASSERT_TRUE (res)
  if (res)
      fprintf (stdout, "found in %s\n", "aa");

  res = WRegexp_match (rexp, makeS ("pppapppp"));
  CU_ASSERT_FALSE (res)
  unless (res)
      fprintf (stdout, "not found in %s\n", "pppapppp");

  destroy (rexp);

}



int main (void)
{
 
/*void (* tests []) () = { test1,  test2, test3};
  warc_u32_t  i      = 0;*/
  /* warc_u32_t  j      = 2;
  warc_u8_t  s[20];*/
  /*char       * ss = NIL;*/
 
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
  
 /* for (i = 0; i < ARRAY_LEN (tests); ++ i)
	{
         
         if (NULL == CU_add_test(pSuite,"ttttttttttttttt" ,  tests [i]))
	{
     	 CU_cleanup_registry();
      	return CU_get_error();
  	 }
    }*/
   /* add the tests to the suite */

   if ((NULL == CU_add_test(pSuite, "TEST 1: testing the matching function", test1)) ||
       (NULL == CU_add_test(pSuite, "TEST2: testing a no matching case", test2))||
       (NULL == CU_add_test(pSuite, "TEST3: matching a regular expression", test3)))
      
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

   if ((NULL == CU_add_test(pSuite, "TEST4: testing the searching function", test4)) ||
       (NULL == CU_add_test(pSuite, "TEST5: testing a no existance case", test5)) ||
       (NULL == CU_add_test(pSuite, "TEST6: searching for a regular expression", test6)))
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
                CU_set_output_filename("./utest/outputs/regexp");
    		CU_set_output_filename("./utest/outputs/regexp" );
  		CU_automated_run_tests();
   		CU_list_tests_to_file();
           	break;}
     
   }
   CU_cleanup_registry();
   return CU_get_error();
}
