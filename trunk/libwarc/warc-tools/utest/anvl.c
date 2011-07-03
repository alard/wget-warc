
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

#include <wfsmanvl.h>

#define makeS(s) ((warc_u8_t *) s), w_strlen((warc_u8_t *) (s))

int init_suite1(void) { return 0; }
int clean_suite1(void) { return 0; }
int init_suite2(void) { return 0; }
int clean_suite2(void) { return 0; }

void test1 (void)
{
  void       * a = bless (WAnvl, makeS ("key1"), makeS ("value1") );
  
  assert (a);
  CU_ASSERT_PTR_NOT_EQUAL(a,NIL);  
  CU_ASSERT_STRING_EQUAL("key1",WAnvl_getKey   (a) ); 
  CU_ASSERT_STRING_EQUAL("value1",WAnvl_getValue   (a) ); 
  destroy (a);
  return ;
}



void test2 (void)
{ 
  void       * a = bless (WAnvl, makeS ("key2"), makeS ("value2") );
  assert (a);
  
  CU_ASSERT_PTR_NOT_EQUAL(a,NIL);  
  WAnvl_setKey   (a, makeS ("CCCC") );
  WAnvl_setValue (a, makeS ("1234") );
  CU_ASSERT_STRING_EQUAL("CCCC",WAnvl_getKey   (a) ); 
  CU_ASSERT_STRING_EQUAL("1234",WAnvl_getValue   (a) ); 
  destroy (a);
  return ;
}


void test3 (void)
{
  
  const char * filename = "app/wdata/testanvl/anvl-1";
  void       * fin      = NIL;
  void       * afsm     = NIL;
  void       * anvl     = NIL;
  warc_bool_t  more     = WARC_TRUE;

  warc_u32_t  i      = 1;
  /* open a valid WARC header file */

  fin = fopen (filename, "r");
  unless (fin)
       {
  CU_FAIL("ERROR YOU CAN NOT OPEN FILE ");
  return ;
       }
  else CU_PASS("file ready to read ");

  do
    {
      /* init ANVL FSM */
      afsm = bless (WFsmANVL, fin);
      assert (afsm);

      unless (WFsmANVL_run (afsm) )
      {
        anvl = WFsmANVL_transform (afsm);

        if (anvl)
          {
         /* fprintf (stdout, "ANVL Key: %s\n",   WAnvl_getKey   (anvl) );*/
            /*fprintf (stdout, "ANVL Value: %s\n", WAnvl_getValue (anvl) );*/
	 if (i==1)
         {
	CU_ASSERT_STRING_EQUAL("color",WAnvl_getKey   (anvl));
       /*CU_ASSERT_STRING_EQUAL("red, yellow\x0d\x0a        green,white",WAnvl_getValue (anvl));
       fprintf (stdout, "red,yellow\x0d\x0a        green,white"	);*/	
	
	}
	if (i==2)
         {
	CU_ASSERT_STRING_EQUAL("height",WAnvl_getKey   (anvl));
       CU_ASSERT_STRING_EQUAL(" 200",WAnvl_getValue (anvl));		
	
	}
	if (i==3)
         {
	CU_ASSERT_STRING_EQUAL("width",WAnvl_getKey   (anvl));
       CU_ASSERT_STRING_EQUAL("300",WAnvl_getValue (anvl));		
	
	}
	if (i==5)
         {
	CU_ASSERT_STRING_EQUAL("ip",WAnvl_getKey   (anvl));
       CU_ASSERT_STRING_EQUAL("adress:122.12.34",WAnvl_getValue (anvl));		
	
	}
            
            destroy (anvl);
          }

        else
          more = WARC_FALSE;

      }

      else
        {
          /* error when parsing the WARC header line */
       CU_FAIL(   fprintf (stderr,  "error in FSM state address %p, at offset %ld in \"%s\"\n",
                   WFsmANVL_state (afsm), ftell (fin), filename));
          more = WARC_FALSE;
        }

      destroy (afsm);
i++;
    }

  while (more);



  fclose  (fin);

  return ;
}


void test4 (void)
{
  const char * filename = "app/wdata/testanvl/anvl-err";
  void       * fin      = NIL;
  void       * afsm     = NIL;
  void       * anvl     = NIL;
  warc_bool_t  more     = WARC_TRUE;
  warc_u32_t  i      = 1; 

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
   {
CU_FAIL("ERROR YOU CAN NOT OPEN FILE ");

  return ;}
else CU_PASS("file ready to read ");


  do
    {
      /* init ANVL FSM */
      afsm = bless (WFsmANVL, fin);
      assert (afsm);

      unless (WFsmANVL_run (afsm) )
      {
        anvl = WFsmANVL_transform (afsm);

        if (anvl)
          {
            if (i==2)
         {
	CU_ASSERT_STRING_EQUAL("text",WAnvl_getKey   (anvl));
       CU_ASSERT_STRING_EQUAL(" cliq",WAnvl_getValue (anvl));	
	
	}
	if (i==3)
         {
	CU_ASSERT_STRING_EQUAL("kloipmllkjj",WAnvl_getKey   (anvl));
       CU_ASSERT_STRING_EQUAL("nbhjklpmoiuytre",WAnvl_getValue (anvl));		
	
	}
	
	

            destroy (anvl);
          }

        else
          destroy (anvl);
      }

      else
        {
          /* error when parsing the WARC header line */
        
       /* CU_ASSERT_STRING_EQUAL("ipadress",WAnvl_getKey   (anvl));
         CU_ASSERT_STRING_EQUAL("0.0.0.0",WAnvl_getValue (anvl));	*/	
	
        CU_PASS( error in FSM state address 0x804cc00 at offset 117 in "app/wdata/testanvl/anvl-err" );

          more = WARC_FALSE;
        }


      destroy (afsm);
i++;
    }

  while (more);

  fclose  (fin);

  return ;
}


void test5 (void)
{
  const char * filename = "app/wdata/testanvl/anvl-2";
  void       * fin      = NIL;
  void       * afsm     = NIL;
  void       * anvl     = NIL;
  warc_bool_t  more     = WARC_TRUE;
warc_u32_t  i      = 1; 

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
  {
CU_FAIL("ERROR YOU CAN NOT OPEN FILE ");

  return ;}
else CU_PASS("file ready to read ");

  do
    {
      /* init ANVL FSM */
      afsm = bless (WFsmANVL, fin);
      assert (afsm);

      unless (WFsmANVL_run (afsm) )
      {
        anvl = WFsmANVL_transform (afsm);

        if (anvl)
          {
           if (i==2)
         {
	CU_ASSERT_STRING_EQUAL("tool",WAnvl_getKey   (anvl));
       CU_ASSERT_STRING_EQUAL(" guitar",WAnvl_getValue (anvl));	
	
	}
	if (i==3)
         {
	CU_ASSERT_STRING_EQUAL("ipadress",WAnvl_getKey   (anvl));
       CU_ASSERT_STRING_EQUAL("192.168.4.1",WAnvl_getValue (anvl));		
	
	}
            destroy (anvl);
          }

        else
          more = WARC_FALSE;
      }

      else
        {
          /* error when parsing the WARC header line */
         CU_FAIL( fprintf (stderr,
                   "error in FSM state address %p, at offset %ld in \"%s\"\n",
                   WFsmANVL_state (afsm), ftell (fin), filename));

          more = WARC_FALSE;
        }


      destroy (afsm);
i++;
    }

  while (more);


  fclose (fin);

  return ;
}


void test6 (void)
{
  
  const char * filename = "app/wdata/testanvl/anvl-ctlkey.anvl";
  void       * fin      = NIL;
  void       * afsm     = NIL;
  void       * anvl     = NIL;
  warc_bool_t  more     = WARC_TRUE;

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
   {
CU_FAIL("ERROR YOU CAN NOT OPEN FILE ");

  return ;}
else CU_PASS("file ready to read ");
  do
    {
      /* init ANVL FSM */
      afsm = bless (WFsmANVL, fin);
      assert (afsm);

      unless (WFsmANVL_run (afsm) )
      {
        anvl = WFsmANVL_transform (afsm);

        if (anvl)
          {
          CU_FAIL(  fprintf (stdout, "ANVL Key: %s\n",   WAnvl_getKey   (anvl) ));
            CU_FAIL(fprintf (stdout, "ANVL Value: %s\n", WAnvl_getValue (anvl) ));

            destroy (anvl);
          }

        else
          more = WARC_FALSE;
      }

      else
        {
          /* error when parsing the WARC header line */
       CU_PASS(   error in FSM state address 0x804c980 at offset 3 in "app/wdata/testanvl/anvl-ctlkey.anvl");

          more = WARC_FALSE;
        }


      destroy (afsm);

    }

  while (more);


  fclose (fin);

  return ;
}


void test7 (void)
{
  void       * a = bless (WAnvl, makeS ("key2"), makeS ("value2") );

  assert (a);
CU_ASSERT_PTR_NOT_EQUAL(a,NIL);
  if (WAnvl_setKey (a, makeS ("C\x7F") ) )
    {
      CU_PASS(Invalid key);
      destroy (a);
      return ;
    }

  WAnvl_setValue (a, makeS ("DDDD") );

CU_FAIL(  fprintf (stdout, "key  : %s\n", WAnvl_getKey   (a) ));
  CU_FAIL(fprintf (stdout, "value: %s\n", WAnvl_getValue (a) ));

  destroy (a);

  return ;
}


void test8 (void)
{

  void       * a = bless (WAnvl, makeS ("key2"), makeS ("value2") );

  assert (a);
CU_ASSERT_PTR_NOT_EQUAL(a,NIL);
  if (WAnvl_setKey (a, makeS ("C\x0Ausing") ) )
    {
     CU_PASS( An other invalid key);
      destroy (a);
      return ;
    }

  WAnvl_setValue (a, makeS ("DDDD") );

CU_FAIL(  fprintf (stdout, "key  : %s\n", WAnvl_getKey   (a) ));
 CU_FAIL( fprintf (stdout, "value: %s\n", WAnvl_getValue (a) ));

  destroy (a);

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
       (NULL == CU_add_test(pSuite, " 3: a valid anvl", test3))||
	(NULL == CU_add_test(pSuite, " 4: a corrupted anvl", test4)))
      
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

   if ((NULL == CU_add_test(pSuite, " 5: a valid anvl", test5)) ||
       (NULL == CU_add_test(pSuite, " 6:   ", test6))||
 (NULL == CU_add_test(pSuite, " 7: Invalid key ", test7))||
(NULL == CU_add_test(pSuite, " 8: n other invalid key ", test8)))
 
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
                CU_set_output_filename("./utest/outputs/anvl");
    		CU_set_output_filename("./utest/outputs/anvl" );
  		CU_automated_run_tests();
   		CU_list_tests_to_file();
           	break;}
     
   }
   CU_cleanup_registry();
   return CU_get_error();
}
