
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
#define makeWString(s) (bless (WString, ((warc_u8_t *) s), w_strlen ((warc_u8_t *) (s))))
int init_suite1(void) { return 0; }
int clean_suite1(void) { return 0; }
int init_suite2(void) { return 0; }
int clean_suite2(void) { return 0; }
int init_suite3(void) { return 0; }
int clean_suite3(void) { return 0; }

void test1 (void)
{
 void       * l = bless (WList);
  
  CU_ASSERT_PTR_NOT_EQUAL(l,NIL);

  WList_push (l, makeWString ("AAAA") );
  WList_push (l, makeWString ("BBBB") );
  WList_push (l, makeWString ("CCCC") );
 /*printf (stdout, "%s> list size: %d\n", t, WList_size (l) );*/
  CU_ASSERT_EQUAL(3, WList_size (l));
  CU_ASSERT_NOT_EQUAL(0, WList_size (l));
  CU_ASSERT_NOT_EQUAL(4, WList_size (l));
  destroy (l);  
/*return 0;*/
}

void test2 (void)
{
  void       * l  = bless (WList);

  CU_ASSERT_PTR_NOT_EQUAL(l,NIL);

  WList_unshift (l, makeWString ("AAAA") );
  WList_unshift (l, makeWString ("BBB") );
  WList_unshift (l, makeWString ("CCCCC") );

/* fprintf (stdout, "%s> list size: %d\n", t, WList_size (l) );*/
  CU_ASSERT_EQUAL(3, WList_size (l));
  CU_ASSERT_NOT_EQUAL(4, WList_size (l));
  CU_ASSERT_NOT_EQUAL(2, WList_size (l));  
  destroy (l); 
/*return 0;*/
}

void test3 (void)
{
  void       * l = bless (WList);

  CU_ASSERT_PTR_NOT_EQUAL(l,NIL);

  WList_unshift (l, makeWString ("AAAA") );
  WList_unshift (l, makeWString ("BBB") );
  WList_unshift (l, makeWString ("CCCCC") );
  WList_unshift (l, makeWString ("DDDD") );

 /* fprintf (stdout, "%s> list size: %d\n", t, WList_size (l) );*/
  CU_ASSERT_EQUAL(4, WList_size (l));
  CU_ASSERT_NOT_EQUAL(5, WList_size (l));
  CU_ASSERT_NOT_EQUAL(3, WList_size (l));  
  destroy (l); 
/*return 0;*/
}

void test4 (void)
{
  void       * l = bless (WList);
  void       * s = NIL;

  warc_u32_t  i      = 1;
  CU_ASSERT_PTR_NOT_EQUAL(l,NIL);

  WList_unshift (l, makeWString ("AAAA") );
  WList_unshift (l, makeWString ("BBB") );
  WList_unshift (l, makeWString ("CCCCC") );
  WList_push    (l, makeWString ("DD") );
  
  CU_ASSERT_NOT_EQUAL(3, WList_size (l));
  CU_ASSERT_EQUAL(4, WList_size (l));
  CU_ASSERT_NOT_EQUAL(5, WList_size (l));
/*fprintf (stdout, "%s> list size: %d\n", t, WList_size (l) );*/

  while ( WList_size (l) )
    {
       s = WList_shift (l); /* WList_shift */
     /* fprintf (stdout, "%s\n", WString_getText (s) );*/
       if (i==1)
         {
	CU_ASSERT_STRING_EQUAL("CCCCC",WString_getText (s));
        CU_ASSERT_STRING_NOT_EQUAL("BBB",WString_getText (s));		
	CU_ASSERT_STRING_NOT_EQUAL("DD",WString_getText (s));
	CU_ASSERT_STRING_NOT_EQUAL("AAAA",WString_getText (s)); 
	}
      
       if (i==2)
         {
	CU_ASSERT_STRING_EQUAL("BBB",WString_getText (s));
        CU_ASSERT_STRING_NOT_EQUAL("AAAA",WString_getText (s));		
	CU_ASSERT_STRING_NOT_EQUAL("CCCCC",WString_getText (s));
	CU_ASSERT_STRING_NOT_EQUAL("DD",WString_getText (s)); 
	}   
    
       if (i==3)
         {
	CU_ASSERT_STRING_EQUAL("AAAA",WString_getText (s));
        CU_ASSERT_STRING_NOT_EQUAL("DD",WString_getText (s));		
	CU_ASSERT_STRING_NOT_EQUAL("CCCCC",WString_getText (s));
	CU_ASSERT_STRING_NOT_EQUAL("BBB",WString_getText (s)); 
	}  
      
	if (i==4)
         {
	CU_ASSERT_STRING_EQUAL("DD",WString_getText (s));
        CU_ASSERT_STRING_NOT_EQUAL("BBB",WString_getText (s));		
	CU_ASSERT_STRING_NOT_EQUAL("CCCCC",WString_getText (s));
	CU_ASSERT_STRING_NOT_EQUAL("AAAA",WString_getText (s)); 

	}
	i++;
      destroy (s);
    }
  destroy (l);

}

void test5 (void)
{
  void       * l = bless (WList);
  void       * s = NIL;

  warc_u32_t  i      = 1;
  CU_ASSERT_PTR_NOT_EQUAL(l,NIL);

  WList_push (l, makeWString ("AAAA") );
  WList_push (l, makeWString ("BBBB") );
  WList_push (l, makeWString ("CCCC") );
  WList_push (l, makeWString ("DDDD") );

  /* fprintf (stdout, "%s> list size: %d\n", t, WList_size (l) );*/
  CU_ASSERT_EQUAL(4, WList_size (l));
  CU_ASSERT_NOT_EQUAL(5, WList_size (l));
  CU_ASSERT_NOT_EQUAL(3, WList_size (l));
 
while ( WList_size (l) )
    {
      s = WList_pop (l); /* WList_pop */
     /* fprintf (stdout, "%s\n", WString_getText (s) );*/
       if (i==1)
         {
         CU_ASSERT_STRING_EQUAL("DDDD",WString_getText (s));		
        CU_ASSERT_STRING_NOT_EQUAL("AAAA",WString_getText (s));		
	CU_ASSERT_STRING_NOT_EQUAL("CCCC",WString_getText (s));
	CU_ASSERT_STRING_NOT_EQUAL("BBBB",WString_getText (s)); 
	}
     
       if (i==2)
         {
	CU_ASSERT_STRING_EQUAL("CCCC",WString_getText (s));
        CU_ASSERT_STRING_NOT_EQUAL("BBBB",WString_getText (s));		
	CU_ASSERT_STRING_NOT_EQUAL("AAAA",WString_getText (s));
	CU_ASSERT_STRING_NOT_EQUAL("DDDD",WString_getText (s)); 
	}   

      if (i==3)
         {
	CU_ASSERT_STRING_EQUAL("BBBB",WString_getText (s));
        CU_ASSERT_STRING_NOT_EQUAL("DDDD",WString_getText (s));		
	CU_ASSERT_STRING_NOT_EQUAL("CCCC",WString_getText (s));
	CU_ASSERT_STRING_NOT_EQUAL("AAAA",WString_getText (s)); 
	}  

      if (i==4)
         {
	CU_ASSERT_STRING_EQUAL("AAAA",WString_getText (s));
        CU_ASSERT_STRING_NOT_EQUAL("BBBB",WString_getText (s));		
	CU_ASSERT_STRING_NOT_EQUAL("DDDD",WString_getText (s));
	CU_ASSERT_STRING_NOT_EQUAL("CCCC",WString_getText (s)); 
	}
      destroy (s);
      i++;
    }
  destroy (l);
}


void test6 (void)
{
  void       * l = bless (WList);
  void       * s = NIL;
  warc_u32_t   i;

  CU_ASSERT_PTR_NOT_EQUAL(l,NIL);

  WList_push (l, makeWString ("AAAA") );
  WList_push (l, makeWString ("BBBB") );
  WList_push (l, makeWString ("CCCC") );

  /*fprintf (stdout, "%s> list size: %d\n", t, WList_size (l) );*/
  CU_ASSERT_EQUAL(3, WList_size (l));
  CU_ASSERT_NOT_EQUAL(4, WList_size (l));
  CU_ASSERT_NOT_EQUAL(2, WList_size (l));
  i = 1;
  s = (void *) WList_getElement (l, i);
  /*fprintf (stdout, "list index %d is : %s\n", i, WString_getText (s) );*/
  CU_ASSERT_STRING_EQUAL("BBBB",WString_getText (s));
  
  i = 0;
  s = (void *) WList_getElement (l, i);
 /*fprintf (stdout, "list index %d is : %s\n", i, WString_getText (s) );*/
  CU_ASSERT_STRING_EQUAL("AAAA",WString_getText (s));

  i = 2;
  s = (void *) WList_getElement (l, i);
 /* fprintf (stdout, "list index %d is : %s\n", i, WString_getText (s) );*/
  CU_ASSERT_STRING_EQUAL("CCCC",WString_getText (s));
  destroy (l);

}

void test7 (void)
{
  
  void       * l = bless (WList);
  void       * s = NIL;
  void       * o = makeWString ("ZZZZZZZ");
  warc_u32_t   i = 1;
  CU_ASSERT_PTR_NOT_EQUAL(l,NIL);

  WList_push (l, makeWString ("AAAA") );
  WList_push (l, makeWString ("BBBB") );
  WList_push (l, makeWString ("CCCC") );

  CU_ASSERT_EQUAL(3, WList_size (l));
  CU_ASSERT_NOT_EQUAL(2, WList_size (l));
  CU_ASSERT_NOT_EQUAL(4, WList_size (l));
 /* fprintf (stdout, "%s> list size: %d\n", t, WList_size (l) );*/

  s = (void *) WList_setElement (l, i, o);
/* fprintf (stdout, "replace %s at index %d by %s\n",*/
  /*         WString_getText (s), i, WString_getText (o) );*/
  CU_ASSERT_STRING_EQUAL("ZZZZZZZ", WString_getText (o));
  CU_ASSERT_STRING_EQUAL("BBBB", WString_getText (s));
  CU_ASSERT_STRING_NOT_EQUAL("BBBB", WString_getText (o));
  CU_ASSERT_STRING_NOT_EQUAL("ZZZZZZZ", WString_getText (s));
  destroy (s);
  destroy (l);
}


void test8 (void)
{ 
  void       * l = bless (WList);
  void       * s = NIL;
  warc_u32_t   i;
  CU_ASSERT_PTR_NOT_EQUAL(l,NIL);

  WList_push (l, makeWString ("AAAA") );
  WList_push (l, makeWString ("BBBB") );
  WList_push (l, makeWString ("CCCC") );
  WList_push (l, makeWString ("DDDD") );

  CU_ASSERT_EQUAL(4, WList_size (l));
  CU_ASSERT_NOT_EQUAL(5, WList_size (l));
  CU_ASSERT_NOT_EQUAL(3, WList_size (l));
  /*fprintf (stdout, "%s> list size: %d\n", t, WList_size (l) );*/

  i = 2;
  s = WList_remove (l, i);
  /*fprintf (stdout, "remove %s from index %d\n",
          WString_getText (s), i);*/
  CU_ASSERT_STRING_EQUAL("CCCC", WString_getText (s));
  destroy (s);
  /*assurer que CCCC est supprimé*/
   i = 2;
  s = (void *) WList_getElement (l, i);
  CU_ASSERT_STRING_NOT_EQUAL("CCCC",WString_getText (s));
  CU_ASSERT_STRING_EQUAL("DDDD",WString_getText (s));

  i = 0;
  s = WList_remove (l, i);
  /*fprintf (stdout, "remove %s from index %d\n",
           WString_getText (s), i);*/
  CU_ASSERT_STRING_EQUAL("AAAA", WString_getText (s));

  destroy (s);
  destroy (l);
}


void test9 (void)
{
  void       * l = bless (WList);

  CU_ASSERT_PTR_NOT_EQUAL(l,NIL);
  CU_ASSERT_EQUAL(0, WList_size (l));
  CU_ASSERT_NOT_EQUAL(3, WList_size (l));
/*fprintf (stdout, "%s> list size: %d\n", t, WList_size (l) );*/
  WList_pop (l);
  destroy (l);

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

   if ((NULL == CU_add_test(pSuite, "TEST 1:size l1 (younes) ", test1)) ||
       (NULL == CU_add_test(pSuite, "TEST 2: size l2 ", test2))||
       (NULL == CU_add_test(pSuite, "TEST 3: size l3 ", test3)))
      
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

   if ((NULL == CU_add_test(pSuite, "TEST 4:contenu l1 ", test4)) ||
       (NULL == CU_add_test(pSuite, "TEST 5: contenu l2 ", test5)))   
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

   if ((NULL == CU_add_test(pSuite, "TEST 6: INDEX ", test6)) ||
       (NULL == CU_add_test(pSuite, "TEST 7: REMPLACE ", test7))||
       (NULL == CU_add_test(pSuite, "TEST 8: REMOVE ", test8))||
       (NULL == CU_add_test(pSuite, "TEST 9: LIST VIDE ", test9)))
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
                CU_set_output_filename("./utest/outputs/list");
    		CU_set_output_filename("./utest/outputs/list" );
  		CU_automated_run_tests();
   		CU_list_tests_to_file();
           	break;}
     
   }
   CU_cleanup_registry();
   return CU_get_error();
}
