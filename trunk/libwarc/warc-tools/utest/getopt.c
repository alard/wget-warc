
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

#define makeS(s) ((warc_u8_t *) s), w_strlen((warc_u8_t *) (s))
int init_suite1(void) { return 0; }
int clean_suite1(void) { return 0; }
int init_suite2(void) { return 0; }
int clean_suite2(void) { return 0; }
void test1 (void)
{
  void       * g     = NIL;
  const warc_u8_t * flags = (unsigned char *) "abc:";
  const char * _av[] = { "test1", "-a", "-b" };
  warc_i32_t   ac    = ARRAY_LEN (_av);
  const char ** av   = _av;
  warc_i32_t   c;
  warc_i32_t   i;
  warc_i32_t   j;

  g = bless (WGetOpt, makeS (flags) );
  assert (g);
  CU_ASSERT_PTR_NOT_NULL(g);
    CU_ASSERT_STRING_EQUAL("abc:",flags);
 
  j=1;
  for (i = 0; i < ac; ++ i)
    {
	if(j==1)  CU_ASSERT_STRING_EQUAL("test1",av[i]);
	if(j==2)  CU_ASSERT_STRING_EQUAL("-a",av[i]);
	if(j==3)  CU_ASSERT_STRING_EQUAL("-b",av[i]);  
   	 j++;    
	/*fprintf (stdout, "%s ", av [i]);*/
    }

  /* print flags */
  j=1;
  while ( (c = WGetOpt_parse (g, ac, av) ) != -1)
    {
      if (c == '?') /* illegal option or missing argument */
        {
          destroy (g);
        return;
        }
      if (j==1)
	{  
           CU_ASSERT_PTR_NULL((const char *)WGetOpt_argument (g));
           CU_ASSERT_EQUAL('a',c);
 	}  
	if (j==2)
	{
           CU_ASSERT_PTR_NULL((const char *)WGetOpt_argument (g));
           CU_ASSERT_EQUAL('b',c);
 	}  
  
	j++;   
}

  /* end of flags; print the rest of options. */

  i = WGetOpt_indice (g);

  if (i < ac)
    {
      for (; i < ac; ++ i)
	CU_FAIL(fprintf (stdout, "%s dans for ", * (av + i) ));
    }
   

  destroy (g);

return;
}

void test2 (void)
{
  
  void       * g     = NIL;
  const warc_u8_t * flags = (unsigned char *) "a:";
  const char * _av[] = { "test2", "-a", "15" };
  warc_i32_t   ac    = ARRAY_LEN (_av);
  const char ** av   = _av;
  warc_i32_t   c;
  warc_i32_t   i;
  warc_i32_t   j;



  g = bless (WGetOpt, makeS (flags) );
  assert (g);
  CU_ASSERT_PTR_NOT_NULL(g);
 
  j=1;
  for (i = 0; i < ac; ++ i)
    {
     /* fprintf (stdout, "%s ", av [i]);*/
	if(j==1)  CU_ASSERT_STRING_EQUAL("test2",av[i]);
	if(j==2)  CU_ASSERT_STRING_EQUAL("-a",av[i]);
	if(j==3)  CU_ASSERT_STRING_EQUAL("15",av[i]);  
   	 j++;
    }

 
  /* print flags */
  j=1;
  while ( (c = WGetOpt_parse (g, ac, av) ) != -1)
    {
      if (c == '?') /* illegal option or missing argument */
        {
          destroy (g);
        return;
        }

      if (j==1)
	{  /*fprintf (stdout, " pointeur ====== %s ",(const char *)WGetOpt_argument (g) );*/
           CU_ASSERT_PTR_EQUAL("15",(const char *)WGetOpt_argument (g));
           CU_ASSERT_EQUAL('a',c);
 	}  
	
	j++; 
    }

  i = WGetOpt_indice (g);

  if (i < ac)
    {
      for (; i < ac; ++ i)
        CU_FAIL(fprintf (stdout, " %s ", * (av + i) ));
    }

 

  destroy (g);

return;
}


void test3 (void)
{
 
  void       * g     = NIL;
  const warc_u8_t * flags = (unsigned char *) "ab:";
  const char * _av[] = { "test3", "-a", "15", "-b", "true" };
  warc_i32_t   ac    = ARRAY_LEN (_av);
  const char ** av   = _av;
  warc_i32_t   c;
  warc_i32_t   i;
  warc_i32_t   j;
 


  g = bless (WGetOpt, makeS (flags) );
  assert (g);
  CU_ASSERT_PTR_NOT_NULL(g);

j=1;
  for (i = 0; i < ac; ++ i)
    {
     /* fprintf (stdout, "%s ", av [i]);*/
	if(j==1)  CU_ASSERT_STRING_EQUAL("test3",av[i]);
	if(j==2)  CU_ASSERT_STRING_EQUAL("-a",av[i]);
	if(j==3)  CU_ASSERT_STRING_EQUAL("15",av[i]); 
	if(j==4)  CU_ASSERT_STRING_EQUAL("-b",av[i]);
	if(j==5)  CU_ASSERT_STRING_EQUAL("true",av[i]);   
   	 j++;
    }



  /* print flags */
  j=1;
  while ( (c = WGetOpt_parse (g, ac, av) ) != -1)
    {
      if (c == '?') /* illegal option or missing argument */
        {
          destroy (g);
        return;
        }
  	if (j==1)
	{  /*fprintf (stdout, " pointeur ====== %s ",(const char *)WGetOpt_argument (g) );*/
           CU_ASSERT_PTR_NULL((const char *)WGetOpt_argument (g));
           CU_ASSERT_EQUAL('a',c);
 	}  
	
	j++; 

      /*fprintf (stdout, "dans whilec== -%c s==  %s ", c,

               w_index (flags, c) [1] == ':' ? WGetOpt_argument (g) : "");*/
    }

  /* end of flags; print the rest of options. */
/*  fprintf (stdout, "-- ");*/

  i = WGetOpt_indice (g);
   j=1;
  if (i < ac)
    {
      for (; i < ac; ++ i)
        {
       /* fprintf (stdout, "dans for %s ", * (av + i) );*/
         if (j==1)
	{  /*fprintf (stdout, " pointeur ====== %s ",(const char *)WGetOpt_argument (g) );*/
           CU_ASSERT_PTR_EQUAL("15",* (av + i));
 	}  
       if (j==2)
	{  /*fprintf (stdout, " pointeur ====== %s ",(const char *)WGetOpt_argument (g) );*/
           CU_ASSERT_PTR_EQUAL("-b",* (av + i));
 	}  
	if (j==3)
	{  /*fprintf (stdout, " pointeur ====== %s ",(const char *)WGetOpt_argument (g) );*/
           CU_ASSERT_PTR_EQUAL("true",* (av + i));
 	}  
	
	
	j++; 
        }
    }



  destroy (g);

return;
}


void test4 (void)
{

  void       * g     = NIL;
  const warc_u8_t * flags = (unsigned char *) "ab";
  const char * _av[] = { "test4", "-ab" };
  warc_i32_t   ac    = ARRAY_LEN (_av);
  const char ** av   = _av;
  warc_i32_t   c;
  warc_i32_t   i;
  warc_i32_t   j;



  g = bless (WGetOpt, makeS (flags) );
  assert (g);
  CU_ASSERT_PTR_NOT_NULL(g);
j=1;
  for (i = 0; i < ac; ++ i)
    {
      /*fprintf (stdout, "%s ", av [i]);*/
	if(j==1)  CU_ASSERT_STRING_EQUAL("test4",av[i]);
	if(j==2)  CU_ASSERT_STRING_EQUAL("-ab",av[i]); 
   	 j++;
    }


  /* print flags */
  j=1;
  while ( (c = WGetOpt_parse (g, ac, av) ) != -1)
    {
      if (c == '?') /* illegal option or missing argument */
        {
          destroy (g);
        return;
        }

     if (j==1)
	{  
           CU_ASSERT_PTR_NULL((const char *)WGetOpt_argument (g));
           CU_ASSERT_EQUAL('a',c);
 	}  
	if (j==2)
	{
           CU_ASSERT_PTR_NULL((const char *)WGetOpt_argument (g));
           CU_ASSERT_EQUAL('b',c);
 	}  
  
	j++;   
    }

  /* end of flags; print the rest of options. */
 /* fprintf (stdout, "-- ");*/

  i = WGetOpt_indice (g);

  if (i < ac)
    {
      for (; i < ac; ++ i)
       CU_FAIL( fprintf (stdout, "dans for %s ", * (av + i) ));
    }

  destroy (g);

return;
}

void test5 (void)
{
  void       * g     = NIL;
  const warc_u8_t * flags = (unsigned char *) "ab:";
  const char * _av[] = { "test5", "-ab" };
  warc_i32_t   ac    = ARRAY_LEN (_av);
  const char ** av   = _av;
  warc_i32_t   c;
  warc_i32_t   i;
  warc_i32_t   j;
   fprintf (stdout, "/////////////// test5 /////////////////////////\n");

  g = bless (WGetOpt, makeS (flags) );
  assert (g);
  CU_ASSERT_PTR_NOT_NULL(g);
j=1;
  for (i = 0; i < ac; ++ i)
    {
     /* fprintf (stdout, "%s ", av [i]);*/
	if(j==1)  CU_ASSERT_STRING_EQUAL("test5",av[i]);
	if(j==2)  CU_ASSERT_STRING_EQUAL("-ab",av[i]); 
   	 j++;
    }

  
  /* print flags */
   j =1;
  while ( (c = WGetOpt_parse (g, ac, av) ) != -1)
    {
      if (c == '?') /* illegal option or missing argument */
        {
          destroy (g);
        return;
        }
	 if (j==1)
	{  
           CU_ASSERT_PTR_NULL((const char *)WGetOpt_argument (g));
           CU_ASSERT_EQUAL('a',c);
 	}  
  
        j++;
      /*fprintf (stdout, "dans while C==-%c S == %s ", c,

               w_index (flags, c) [1] == ':' ? WGetOpt_argument (g) : "");*/
    }

  /* end of flags; print the rest of options. */


  i = WGetOpt_indice (g);

  if (i < ac)
    {
      for (; i < ac; ++ i)
       CU_FAIL( fprintf (stdout, " dans for %s ", * (av + i) ));
    }

  fprintf (stdout, "\n");

  destroy (g);

return;
}

void test6 (void)
{
  
  void       * g     = NIL;
  const warc_u8_t * flags = (unsigned char *) "ab";
  const char * _av[] = { "test6", "-a", "--b" };
  warc_i32_t   ac    = ARRAY_LEN (_av);
  const char ** av   = _av;
  warc_i32_t   c;
  warc_i32_t   i;
  warc_i32_t   j;
  fprintf (stdout, "///////////// TEST 6 /////////////////\n");

  g = bless (WGetOpt, makeS (flags) );
  assert (g);
  CU_ASSERT_PTR_NOT_NULL(g);
   j=1;
  for (i = 0; i < ac; ++ i)
    {
     /* fprintf (stdout, "%s ", av [i]);*/
	if(j==1)  CU_ASSERT_STRING_EQUAL("test6",av[i]);
	if(j==2)  CU_ASSERT_STRING_EQUAL("-a",av[i]);
	if(j==3)  CU_ASSERT_STRING_EQUAL("--b",av[i]);  
   	 j++;
    }
  /* print flags */
  j = 1;
  while ( (c = WGetOpt_parse (g, ac, av) ) != -1)
    {
      if (c == '?') /* illegal option or missing argument */
        {
          destroy (g);
        return;
        }

       if (j==1)
	{  
           CU_ASSERT_PTR_NULL((const char *)WGetOpt_argument (g));
           CU_ASSERT_EQUAL('a',c);
 	}  
       
        j++;
    }

  /* end of flags; print the rest of options. */
 

  i = WGetOpt_indice (g);

  if (i < ac)
    {
      for (; i < ac; ++ i)
       CU_FAIL( fprintf (stdout, "dans for %s ", * (av + i) ));
    }

  destroy (g);

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

   if ((NULL == CU_add_test(pSuite, "TEST 1: ", test1)) ||
       (NULL == CU_add_test(pSuite, "TEST 2:  ", test2))||
       (NULL == CU_add_test(pSuite, "TEST 3:", test3)))
      
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

   if ((NULL == CU_add_test(pSuite, "TEST 4: ", test4)) ||
       (NULL == CU_add_test(pSuite, "TEST 5:  ", test5))||
       (NULL == CU_add_test(pSuite, "TEST 6: ", test6)))   
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
                CU_set_output_filename("./utest/outputs/getopt");
    		CU_set_output_filename("./utest/outputs/getopt" );
  		CU_automated_run_tests();
   		CU_list_tests_to_file();
           	break;}
     
   }
   CU_cleanup_registry();
   return CU_get_error();
  
}
