
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
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <warc.h>

#include <wgzip.h>       /* WGzip */

#define makeString(m) (#m)

int init_suite1(void) { return 0; }
int clean_suite1(void) { return 0; }

int init_suite2(void) { return 0; }
int clean_suite2(void) { return 0; }


/* uncompress everything */
warc_u32_t callback (const warc_u8_t * buffer,
                     const warc_u32_t nbr, void * env)
{
  FILE * out = (FILE *) env;

  /* copy the uncompressed 'nbr' bytes to out */
  fwrite (buffer, 1, nbr, out);


  return (Z_CONTINUE_DECODING);
}

FILE * openReading (const char * fin)
{
  FILE * in;
  unless (in = fopen (fin, "r+b") )
  {
    fprintf (stderr, "error: cannot open file \"%s\" for reading\n",
             fin);
 
    return NIL;
  }

  return in;
}

FILE * openWriting (const char * fout)
{
  FILE * out;
  unless (out = fopen (fout, "w+b") )
  {
    fprintf (stderr, "error: cannot open file \"%s\" for binary writing\n",
             fout);
    return NIL;
  }

  return out;
}

const char * fin = "app/wdata/testwfile/warcfile.warc";

void test1 (void)
{
  const char * fout  = "compress_none.gz";
  FILE       * in    = NIL;
  FILE       * out   = NIL;
  void       * g     = NIL; /* WGzip object */
  warc_i32_t   ret   = 0;
  warc_u64_t   csize = 0;   /* compressed file size */
/*const char * f = "app/wdata/testwfile/warcfile.warc";*/

  /* empty string */
  g = bless (WGzip);
  assert (g);

  in = openReading (fin);
  assert (in);

  out = openWriting (fout);
  assert (out);
 CU_ASSERT_PTR_NOT_EQUAL(g,NIL);
 CU_ASSERT_PTR_NOT_EQUAL(in,NIL);
 CU_ASSERT_PTR_NOT_EQUAL(out,NIL);
  ret = WGzip_compress (g, in, out, WARC_GZIP_NO_COMPRESSION, & csize);

  assert (!ret);
 CU_ASSERT_EQUAL(ret,0);
/*  fprintf (stderr, "\"%s\" compressed to \"%s\" [size: %llu] [mode: %s]\n",
           fin, fout, (unsigned long long) csize,
           makeString (WARC_GZIP_NO_COMPRESSION) );*/
CU_ASSERT_STRING_EQUAL(fin,"app/wdata/testwfile/warcfile.warc");
CU_ASSERT_STRING_EQUAL(fout,"compress_none.gz");
CU_ASSERT(3626==csize);
CU_ASSERT_STRING_EQUAL("WARC_GZIP_NO_COMPRESSION",makeString (WARC_GZIP_NO_COMPRESSION));
  destroy (g);
  fclose (out);
  fclose (in);

  return ;
}


void test2 (void)
{
  const char * fout  = "compress_default.gz";
  FILE       * in    = NIL;
  FILE       * out   = NIL;
  void       * g     = NIL; /* WGzip object */
  warc_i32_t   ret   = 0;
  warc_u64_t   csize = 0;   /* compressed file size */


  /* empty string */
  g = bless (WGzip);
  assert (g);

  in = openReading (fin);
  assert (in);

  out = openWriting (fout);
  assert (out);
 CU_ASSERT_PTR_NOT_EQUAL(g,NIL);
 CU_ASSERT_PTR_NOT_EQUAL(in,NIL);
 CU_ASSERT_PTR_NOT_EQUAL(out,NIL);
  ret = WGzip_compress (g, in, out, WARC_GZIP_DEFAULT_COMPRESSION, & csize);

  assert (! ret);
 CU_ASSERT_EQUAL(ret,0);
 
/*
  fprintf (stdout, "\"%s\" compressed to \"%s\" [size: %llu] [mode: %s]\n",
           fin, fout, (unsigned long long) csize,
           makeString (WARC_GZIP_DEFAULT_COMPRESSION) );*/
CU_ASSERT_STRING_EQUAL(fin,"app/wdata/testwfile/warcfile.warc");
CU_ASSERT_STRING_EQUAL(fout,"compress_default.gz");
CU_ASSERT(1433==csize);
CU_ASSERT_STRING_EQUAL("WARC_GZIP_DEFAULT_COMPRESSION",makeString (WARC_GZIP_DEFAULT_COMPRESSION));

  destroy (g);
  fclose (out);
  fclose (in);

  return ;
}

void test3 (void)
{
  const char * fout  = "compress_speed.gz";
  FILE       * in    = NIL;
  FILE       * out   = NIL;
  void       * g     = NIL; /* WGzip object */
  warc_u64_t   csize = 0;   /* compressed file size */

 /* empty string */
  g = bless (WGzip);
  assert (g);

  in = openReading (fin);
  assert (in);

  out = openWriting (fout);
  assert (out);
 CU_ASSERT_PTR_NOT_EQUAL(g,NIL);
 CU_ASSERT_PTR_NOT_EQUAL(in,NIL);
 CU_ASSERT_PTR_NOT_EQUAL(out,NIL);

  WGzip_compress (g, in, out, WARC_GZIP_BEST_SPEED, & csize);
 /* fprintf (stderr, "\"%s\" compressed to \"%s\" [size: %llu] [mode: %s]\n",
           fin, fout, (unsigned long long) csize,
           makeString (WARC_GZIP_BEST_SPEED) );*/
CU_ASSERT_STRING_EQUAL(fin,"app/wdata/testwfile/warcfile.warc");
CU_ASSERT_STRING_EQUAL(fout,"compress_speed.gz");
CU_ASSERT(1511==csize);
CU_ASSERT_STRING_EQUAL("WARC_GZIP_BEST_SPEED",makeString (WARC_GZIP_BEST_SPEED));

  destroy (g);
  fclose (out);
  fclose (in);

  return ;
}

void test4 (void)
{
  const char * fout  = "compress_size.gz";
  FILE       * in    = NIL;
  FILE       * out   = NIL;
  void       * g     = NIL; /* WGzip object */
  warc_i32_t   ret   = 0;
  warc_u64_t   csize = 0;   /* compressed file size */

  /* empty string */
  g = bless (WGzip);
  assert (g);

  in = openReading (fin);
  assert (in);

  out = openWriting (fout);
  assert (out);
 CU_ASSERT_PTR_NOT_EQUAL(g,NIL);
 CU_ASSERT_PTR_NOT_EQUAL(in,NIL);
 CU_ASSERT_PTR_NOT_EQUAL(out,NIL);

  ret = WGzip_compress (g, in, out, WARC_GZIP_BEST_COMPRESSION, & csize);
  assert (! ret);
 CU_ASSERT_EQUAL(ret,0);
 /* fprintf (stderr, "\"%s\" compressed to \"%s\" [size: %llu] [mode: %s]\n",
           fin, fout, (unsigned long long) csize,
           makeString (WARC_GZIP_BEST_COMPRESSION) );*/
CU_ASSERT_STRING_EQUAL(fin,"app/wdata/testwfile/warcfile.warc");
CU_ASSERT_STRING_EQUAL(fout,"compress_size.gz");
CU_ASSERT(1433==csize);
CU_ASSERT_STRING_EQUAL("WARC_GZIP_BEST_COMPRESSION",makeString (WARC_GZIP_BEST_COMPRESSION));


  destroy (g);
  fclose (out);
  fclose (in);

  return ;
}


void test5 (void)
{
  
  const char * fout  = "compress.gz";
  FILE       * in    = NIL;
  FILE       * out   = NIL;
  void       * g     = NIL; /* WGzip object */
  warc_i32_t   ret   = 0;
  warc_u64_t   csize = 0;   /* compressed file size */


  /* empty string */
  g = bless (WGzip);
  assert (g);

  in = openReading (fin);
  assert (in);

  out = openWriting (fout);
  assert (out);
 CU_ASSERT_PTR_NOT_EQUAL(g,NIL);
 CU_ASSERT_PTR_NOT_EQUAL(in,NIL);
 CU_ASSERT_PTR_NOT_EQUAL(out,NIL);

  ret = WGzip_compress (g, in, out, WARC_GZIP_BEST_COMPRESSION, & csize);
  assert (! ret);
 CU_ASSERT_EQUAL(ret,0);
 /* fprintf (stderr, "\"%s\" compressed to \"%s\" [size: %llu] [mode: %s]\n",
           fin, fout, (unsigned long long) csize,
           makeString (WARC_GZIP_BEST_COMPRESSION) );*/
CU_ASSERT_STRING_EQUAL(fin,"app/wdata/testwfile/warcfile.warc");
CU_ASSERT_STRING_EQUAL(fout,"compress.gz");
CU_ASSERT(1433==csize);
CU_ASSERT_STRING_EQUAL("WARC_GZIP_BEST_COMPRESSION",makeString (WARC_GZIP_BEST_COMPRESSION));


  destroy (g);
  fclose (out);
  fclose (in);

  return ;
}


void test6 (void)
{
  const char * fin   = "compress.gz";
  FILE       * in    = NIL;
  void       * g     = NIL; /* WGzip object */
  warc_u64_t   usize = 0;   /* uncompressed file size */
  warc_u64_t   csize = 0;   /* compressed file size */
  warc_bool_t  ret;

 /* unused = 0;*/


  /* empty string */
  g = bless (WGzip);
  assert (g);

  in = openReading (fin);
  assert (in);
 CU_ASSERT_PTR_NOT_EQUAL(g,NIL);
 CU_ASSERT_PTR_NOT_EQUAL(in,NIL);
  /* try to extract compressed and uncompressed size from GZIP header */
  ret = WGzip_analyzeHeader (g, in, 0, & usize, & csize);

  unless (ret)
 { CU_PASS("Found data in the GZIP header:compressed : 1433  uncompressed : 3591");}
  else
    CU_FAIL("GZIP header contains no information. Sorry !!!");

  fclose (in);

  destroy (g);
}




int main(void)
{
 CU_pSuite pSuite = NULL;
  
  /*UNUSED (argc);
  UNUSED (argv);*/
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

   if ((NULL == CU_add_test(pSuite, "test 1", test1)) ||
       (NULL == CU_add_test(pSuite, "test 2", test2))||
       (NULL == CU_add_test(pSuite, "test 3", test3)))
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
 
   if ((NULL == CU_add_test(pSuite, "test 4", test4)) ||
       (NULL == CU_add_test(pSuite, "test 5", test5))||
       (NULL == CU_add_test(pSuite, "test 6", test6)))
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
                CU_set_output_filename("./utest/outputs/warcgzip");
    		CU_set_output_filename("./utest/outputs/warcgzip" );
  		CU_automated_run_tests();
   		CU_list_tests_to_file();
           	break;}
   }
/*CU_console_run_tests();*/
 CU_cleanup_registry();
   return CU_get_error();
}
