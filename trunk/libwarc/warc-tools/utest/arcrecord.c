
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
#include <afsmhdl.h>
#include <arecord.h>
#include <arecord.x>
#include <afile.h>


#define makeS(s) ((warc_u8_t *) s), w_strlen((warc_u8_t *) (s))

static void * wdir  = NIL;
int init_suite1(void) { return 0; }
int clean_suite1(void) { return 0; }
int init_suite2(void) { return 0; }
int clean_suite2(void) { return 0; }
int init_suite3(void) { return 0; }
int clean_suite3(void) { return 0; }

void test1 (void)
{
  void       * hl = bless (ARecord,
                           makeS ("http://www.w3c.org"),
                           makeS ("192.168.4.1"),
                           makeS ("12172007"),
                           makeS ("warcproject/testheaderline"),
                           12,
                           wdir);


  assert (hl);
 CU_ASSERT_PTR_NOT_EQUAL(hl,NIL);

 /* fprintf (stdout, "DataLength:   %d\n", ARecord_getDataLength  (hl) );
  fprintf (stdout, "MimeType:   %s\n", ARecord_getMimeType  (hl) );
  fprintf (stdout, "Url:   %s\n",        ARecord_getUrl  (hl) );
  fprintf (stdout, "CreationDate: %s\n", ARecord_getCreationDate (hl) );
  fprintf (stdout, "IP-adress:     %s\n", ARecord_getIpAddress    (hl) );*/
 CU_ASSERT(12==ARecord_getDataLength  (hl)  ); 
 CU_ASSERT_STRING_EQUAL("warcproject/testheaderline",ARecord_getMimeType  (hl)  );
 /* CU_ASSERT_STRING_EQUAL("www.w3c.org",ARecord_getUrl  (hl)  ); */
 CU_ASSERT_STRING_EQUAL("12172007",ARecord_getCreationDate (hl) ); 
 CU_ASSERT_STRING_EQUAL("192.168.4.1", ARecord_getIpAddress    (hl)  ); 


  destroy (hl);

  return ;
}


void test2 (void)
{


  void       * hl = bless (ARecord,
                           makeS ("http://www.w3c.org"),
                           makeS ("192.168.4.1"),
                           makeS ("12172007"),
                           makeS ("warcproject/testheaderline"),
                           12,
                           wdir);


  assert (hl);
 CU_ASSERT_PTR_NOT_EQUAL(hl,NIL);

  ARecord_setDataLength   (hl, 15);
  ARecord_setUrl   (hl, makeS ("http://www.iso.net") );
  ARecord_setCreationDate (hl, makeS ("01012999") );
  ARecord_setMimeType  (hl, makeS ("application/xpdf") );
  ARecord_setIpAddress     (hl, makeS ("168.12.0.0") );


  /*fprintf (stdout, "DataLength:   %d\n", ARecord_getDataLength  (hl) );
  fprintf (stdout, "MimeType:   %s\n", ARecord_getMimeType  (hl) );
  fprintf (stdout, "Url:   %s\n",        ARecord_getUrl  (hl) );
  fprintf (stdout, "CreationDate: %s\n", ARecord_getCreationDate (hl) );
  fprintf (stdout, "IP-adress:     %s\n", ARecord_getIpAddress    (hl) );*/
CU_ASSERT(15==ARecord_getDataLength  (hl)  ); 
 CU_ASSERT_STRING_EQUAL("application/xpdf",ARecord_getMimeType  (hl)  );
  CU_ASSERT_STRING_EQUAL("http://www.iso.net",ARecord_getUrl  (hl)  ); 
 CU_ASSERT_STRING_EQUAL("01012999",ARecord_getCreationDate (hl) ); 
 CU_ASSERT_STRING_EQUAL("168.12.0.0", ARecord_getIpAddress    (hl)  ); 

  destroy (hl);

  return ;
}


void test3 (void)
{
  const char * filename = "app/wdata/testarc/file.arc";
  void       * fin      = NIL;
  void       * hl       = NIL;
  void       * fsm      = NIL;

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
  return ;

  /* init HDL FSM */
  fsm = bless (AFsmHDL, fin, wdir);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (AFsmHDL_run (fsm) )
  {
    /* generate the WHDLine object from the FSM */
    hl = AFsmHDL_transform (fsm);


   /* fprintf (stdout, "DataLength:   %d\n", ARecord_getDataLength  (hl) );
    fprintf (stdout, "MimeType:   %s\n", ARecord_getMimeType  (hl) );
    fprintf (stdout, "Url:   %s\n",        ARecord_getUrl  (hl) );
    fprintf (stdout, "CreationDate: %s\n", ARecord_getCreationDate (hl) );
    fprintf (stdout, "IP-adress:     %s\n", ARecord_getIpAddress    (hl) );*/
CU_ASSERT(22566==ARecord_getDataLength  (hl)  ); 
 CU_ASSERT_STRING_EQUAL("text/plain",ARecord_getMimeType  (hl)  );
  CU_ASSERT_STRING_EQUAL("filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc",ARecord_getUrl  (hl)  ); 
 CU_ASSERT_STRING_EQUAL("20070524113301",ARecord_getCreationDate (hl) ); 
 CU_ASSERT_STRING_EQUAL("0.0.0.0", ARecord_getIpAddress    (hl)  ); 

    destroy (hl);
  }

  else
    {
      /* error when parsing the WARC header line */
      CU_FAIL(fprintf (stderr,  "error in FSM state address %p, at offset %ld in \"%s\"\n",
               AFsmHDL_state (fsm), ftell (fin), filename));
    }


  destroy (fsm);

  fclose  (fin);

  return ;
}

void test4 (void)
{
  const char * filename = "app/wdata/testarc/err1.arc";
  void       * fin      = NIL;
  void       * hl       = NIL;
  void       * fsm      = NIL;

  /* open a valid WARC header file */
 
fin = fopen (filename, "r");
  unless (fin)
  return ;

  /* init HDL FSM */
  fsm = bless (AFsmHDL, fin, wdir);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (AFsmHDL_run (fsm) )
  {
    /* generate the WHDLine object from the FSM */
    hl = AFsmHDL_transform (fsm);


    /*fprintf (stdout, "DataLength:   %d\n", ARecord_getDataLength  (hl) );
    fprintf (stdout, "MimeType:   %s\n", ARecord_getMimeType  (hl) );
    fprintf (stdout, "Url:   %s\n",        ARecord_getUrl  (hl) );
    fprintf (stdout, "CreationDate: %s\n", ARecord_getCreationDate (hl) );
    fprintf (stdout, "IP-adress:     %s\n", ARecord_getIpAddress    (hl) );*/
CU_ASSERT(1489==ARecord_getDataLength  (hl)  ); 
 CU_ASSERT_STRING_EQUAL("text/plain",ARecord_getMimeType  (hl)  );
  CU_ASSERT_STRING_EQUAL("filedesc:BnF-elec2007-20070524113301-00040-heritrix1.arc",ARecord_getUrl  (hl)  ); 
 CU_ASSERT_STRING_EQUAL("20070524113301",ARecord_getCreationDate (hl) ); 
 CU_ASSERT_STRING_EQUAL("0.0.0.0", ARecord_getIpAddress    (hl)  ); 


    destroy (hl);
  }

  else
    {
      /* error when parsing the WARC header line */
      CU_FAIL(fprintf (stderr,  "error in FSM state address %p, at offset %ld in \"%s\"\n",
               AFsmHDL_state (fsm), ftell (fin), filename));
    }


  destroy (fsm);

  fclose  (fin);

  return ;
}


void test5 (void)
{

  const char * filename = "app/wdata/testarc/err2.arc";
  void       * fin      = NIL;
  void       * hl       = NIL;
  void       * fsm      = NIL;
fprintf(stdout,"////////// test 5//////////\n");

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
  return ;

  /* init HDL FSM */
  fsm = bless (AFsmHDL, fin, wdir);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (AFsmHDL_run (fsm) )
  {
    /* generate the WHDLine object from the FSM */
    hl = AFsmHDL_transform (fsm);


   CU_FAIL( fprintf (stdout, "DataLength:   %d\n", ARecord_getDataLength  (hl) ));
   CU_FAIL( fprintf (stdout, "MimeType:   %s\n", ARecord_getMimeType  (hl) ));
    CU_FAIL(fprintf (stdout, "Url:   %s\n",        ARecord_getUrl  (hl) ));
    CU_FAIL(fprintf (stdout, "CreationDate: %s\n", ARecord_getCreationDate (hl)) );
    CU_FAIL(fprintf (stdout, "IP-adress:     %s\n", ARecord_getIpAddress    (hl)) );

    destroy (hl);
  }

  else
    {
      /* error when parsing the WARC header line */
    CU_PASS( expecting a valid creation date error in FSM state address 0x80652c0 at offset 73 in "app/wdata/testarc/err2.arc");
    }


  destroy (fsm);

  fclose  (fin);

  return ;
}

void test6 (void)
{
  const char * filename = "app/wdata/testarc/err3.arc";
  void       * fin      = NIL;
  void       * hl       = NIL;
  void       * fsm      = NIL;

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
  return ;

  /* init HDL FSM */
  fsm = bless (AFsmHDL, fin, wdir);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (AFsmHDL_run (fsm) )
  {
    /* generate the WHDLine object from the FSM */
    hl = AFsmHDL_transform (fsm);


/*    fprintf (stdout, "DataLength:   %d\n", ARecord_getDataLength  (hl) );
    fprintf (stdout, "MimeType:   %s\n", ARecord_getMimeType  (hl) );
    fprintf (stdout, "Url:   %s\n",        ARecord_getUrl  (hl) );
    fprintf (stdout, "CreationDate: %s\n", ARecord_getCreationDate (hl) );
    fprintf (stdout, "IP-adress:     %s\n", ARecord_getIpAddress    (hl) );*/
CU_ASSERT(1489==ARecord_getDataLength  (hl)  ); 
 CU_ASSERT_STRING_EQUAL("textplain",ARecord_getMimeType  (hl)  );
  CU_ASSERT_STRING_EQUAL("filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc",ARecord_getUrl  (hl)  ); 
 CU_ASSERT_STRING_EQUAL("20070524113301",ARecord_getCreationDate (hl) ); 
 CU_ASSERT_STRING_EQUAL("0.0.0.0", ARecord_getIpAddress    (hl)  ); 

    destroy (hl);
  }

  else
    {
      /* error when parsing the WARC header line */
      CU_FAIL(fprintf (stderr,  "error in FSM state address %p, at offset %ld in \"%s\"\n",
               AFsmHDL_state (fsm), ftell (fin), filename));
    }


  destroy (fsm);

  fclose  (fin);

  return ;
}
void test7 (void)
{

  const char * filename = "app/wdata/testarc/err4.arc";
  void       * fin      = NIL;
  void       * hl       = NIL;
  void       * fsm      = NIL;

fprintf(stdout,"////////// test 7//////////\n");
  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
  return ;

  /* init HDL FSM */
  fsm = bless (AFsmHDL, fin, wdir);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (AFsmHDL_run (fsm) )
  {
    /* generate the WHDLine object from the FSM */
    hl = AFsmHDL_transform (fsm);


   CU_FAIL( fprintf (stdout, "DataLength:   %d\n", ARecord_getDataLength  (hl) ));
   CU_FAIL( fprintf (stdout, "MimeType:   %s\n", ARecord_getMimeType  (hl) ));
   CU_FAIL( fprintf (stdout, "Url:   %s\n",        ARecord_getUrl  (hl) ));
    CU_FAIL(fprintf (stdout, "CreationDate: %s\n", ARecord_getCreationDate (hl) ));
    CU_FAIL(fprintf (stdout, "IP-adress:     %s\n", ARecord_getIpAddress    (hl) ));

    destroy (hl);
  }

  else
    {
      /* error when parsing the WARC header line */
     CU_PASS( invalid data length: mm9 error in FSM state address 0x80653c0 at offset 101 in "app/wdata/testarc/err4.arc");
    }


  destroy (fsm);

  fclose  (fin);

  return ;
}


void test8 (void)
{
  const char * filename = "app/wdata/testarc/err5.arc";
  void       * fin      = NIL;
  void       * hl       = NIL;
  void       * fsm      = NIL;
fprintf(stdout,"////////// test 8//////////\n");

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
  return ;

  /* init HDL FSM */
  fsm = bless (AFsmHDL, fin, wdir);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (AFsmHDL_run (fsm) )
  {
    /* generate the WHDLine object from the FSM */
    hl = AFsmHDL_transform (fsm);


    CU_FAIL(fprintf (stdout, "DataLength:   %d\n", ARecord_getDataLength  (hl)) );
    CU_FAIL( fprintf (stdout, "MimeType:   %s\n", ARecord_getMimeType  (hl)) );
    CU_FAIL( fprintf (stdout, "Url:   %s\n",        ARecord_getUrl  (hl) ));
    CU_FAIL( fprintf (stdout, "CreationDate: %s\n", ARecord_getCreationDate (hl) ));
    CU_FAIL( fprintf (stdout, "IP-adress:     %s\n", ARecord_getIpAddress    (hl) ));

    destroy (hl);
  }

  else
    {
      /* error when parsing the WARC header line */
      CU_PASS(found IP address: 0.0.00. error in FSM state address 0x8065240 at offset 66 in app/wdata/testarc/err5.arc);
    }


  destroy (fsm);

  fclose  (fin);

  return ;
}

void test9 (void)
{
  const char * filename = "app/wdata/testarc/err6.arc";
  void       * fin      = NIL;
  void       * hl       = NIL;
  void       * fsm      = NIL;
fprintf(stdout,"////////// test 9//////////\n");

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
  return ;

  /* init HDL FSM */
  fsm = bless (AFsmHDL, fin, wdir);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (AFsmHDL_run (fsm) )
  {
    /* generate the WHDLine object from the FSM */
    hl = AFsmHDL_transform (fsm);


    CU_FAIL(fprintf (stdout, "DataLength:   %d\n", ARecord_getDataLength  (hl)) );
    CU_FAIL( fprintf (stdout, "MimeType:   %s\n", ARecord_getMimeType  (hl)) );
    CU_FAIL( fprintf (stdout, "Url:   %s\n",        ARecord_getUrl  (hl) ));
    CU_FAIL( fprintf (stdout, "CreationDate: %s\n", ARecord_getCreationDate (hl) ));
    CU_FAIL( fprintf (stdout, "IP-adress:     %s\n", ARecord_getIpAddress    (hl) ));
    destroy (hl);
  }

  else
    {
      /* error when parsing the WARC header line */
    CU_PASS(  ERROR:found IP address: 0.00.0);
    }


  destroy (fsm);

  fclose  (fin);

  return ;
}
void test10 (void)
{

  const char * filename = "app/wdata/testarc/err7.arc";
  void       * fin      = NIL;
  void       * hl       = NIL;
  void       * fsm      = NIL;

fprintf(stdout,"////////// test 10//////////\n");
  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
  return ;

  /* init HDL FSM */
  fsm = bless (AFsmHDL, fin, wdir);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (AFsmHDL_run (fsm) )
  {
    /* generate the WHDLine object from the FSM */
    hl = AFsmHDL_transform (fsm);


   CU_FAIL(fprintf (stdout, "DataLength:   %d\n", ARecord_getDataLength  (hl)) );
    CU_FAIL( fprintf (stdout, "MimeType:   %s\n", ARecord_getMimeType  (hl)) );
    CU_FAIL( fprintf (stdout, "Url:   %s\n",        ARecord_getUrl  (hl) ));
    CU_FAIL( fprintf (stdout, "CreationDate: %s\n", ARecord_getCreationDate (hl) ));
    CU_FAIL( fprintf (stdout, "IP-adress:     %s\n", ARecord_getIpAddress    (hl) ));
    destroy (hl);
  }

  else
    {
      /* error when parsing the WARC header line */
      CU_PASS( ERROR :found IP address: .10.40.3);
    }


  destroy (fsm);

  fclose  (fin);

  return ;
}


void test11 (void)
{
  const char * filename = "app/wdata/testarc/err8.arc";
  void       * fin      = NIL;
  void       * hl       = NIL;
  void       * fsm      = NIL;

fprintf(stdout,"////////// test 11//////////\n");
  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
  return ;

  /* init HDL FSM */
  fsm = bless (AFsmHDL, fin, wdir);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (AFsmHDL_run (fsm) )
  {
    /* generate the WHDLine object from the FSM */
    hl = AFsmHDL_transform (fsm);

    CU_FAIL(fprintf (stdout, "DataLength:   %d\n", ARecord_getDataLength  (hl)) );
    CU_FAIL( fprintf (stdout, "MimeType:   %s\n", ARecord_getMimeType  (hl)) );
    CU_FAIL( fprintf (stdout, "Url:   %s\n",        ARecord_getUrl  (hl) ));
    CU_FAIL( fprintf (stdout, "CreationDate: %s\n", ARecord_getCreationDate (hl) ));
    CU_FAIL( fprintf (stdout, "IP-adress:     %s\n", ARecord_getIpAddress    (hl) ));
    destroy (hl);
  }

  else
    {
      /* error when parsing the WARC header line */
      CU_PASS (ERROR: found IP address: 109.407.184.356);
    }


  destroy (fsm);

  fclose  (fin);

  return ;
}

int main (void)
{
     CU_pSuite pSuite = NULL; 
wdir = bless (WString, ".", 1);
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
       (NULL == CU_add_test(pSuite, " 3:  ", test3))||
	(NULL == CU_add_test(pSuite, " 4: ", test4)))
      
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

   if ((NULL == CU_add_test(pSuite, " 5: ", test5)) ||
       (NULL == CU_add_test(pSuite, " 6:   ", test6))||
 (NULL == CU_add_test(pSuite, " 7: ERROR invalid data length: mm9", test7))||
 (NULL == CU_add_test(pSuite, " 8: found IP address: 0.0.00", test8)))   
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

   if ((NULL == CU_add_test(pSuite,  " 9: ERROR:found IP address: 0.00.0", test9)) ||
       (NULL == CU_add_test(pSuite, " 10: ERROR :found IP address: .10.40.3", test10))||
       (NULL == CU_add_test(pSuite, " 11: ERROR: found IP address: 109.407.184.356", test11)))
      
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
                CU_set_output_filename("./utest/outputs/arcrecord");
    		CU_set_output_filename("./utest/outputs/arcrecord" );
  		CU_automated_run_tests();
   		CU_list_tests_to_file();
           	break;}
     
   }
   CU_cleanup_registry();
   return CU_get_error(); 
 destroy (wdir);

}
