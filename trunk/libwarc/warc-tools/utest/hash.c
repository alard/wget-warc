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


#define makeS(s) s, strlen(s)
#define makeI(i) &i, sizeof(warc_u32_t)
#define makeWString(s) (bless (WString, ((warc_u8_t *)s), w_strlen((warc_u8_t *) (s))))
/*                 les suites                         */



int init_suite1(void) { return 0; }
int clean_suite1(void) { return 0; }



/* les tests */
void test1 (void)
{
  void       * h = NIL;
  void       * s = NIL;
  char       * k = NIL;
  const void * v = NIL;

  h = bless (WHash, 101);
  CU_ASSERT_PTR_NOT_EQUAL(h,NIL);

  s = makeWString ("arc");  
  WHash_insert (h, WString_getText (s), WString_getLength (s), s);

  s = makeWString ("warc");
  CU_ASSERT_PTR_NOT_EQUAL(s,NIL);
  WHash_insert (h, WString_getText (s), WString_getLength (s), s);

  s = makeWString ("iipc");
  CU_ASSERT_PTR_NOT_EQUAL(s,NIL);
  WHash_insert (h, WString_getText (s), WString_getLength (s), s);


  k = "warc";
  v = WHash_get (h, makeS (k) );
  CU_ASSERT_PTR_EQUAL(v,WHash_get (h, makeS (k)));
   if (v)
 {  CU_PASS ( "found key:");}
  else
    CU_FAIL ( "no key   :");

  k = "arc";
  v = WHash_get (h, makeS (k)) ;
  CU_ASSERT_PTR_EQUAL(v,WHash_get (h, makeS (k)));
 if (v)
 {  CU_PASS ( "found key:");}
  else
    CU_FAIL ( "no key   :");

  k = "ziipc";
  v = WHash_get (h, makeS (k) );
  CU_ASSERT_PTR_EQUAL(v,WHash_get (h, makeS (k)));
  if (v)
   CU_FAIL ( "found key:")
  else
    CU_PASS( "no key   :");

  destroy (h);

}



void test2 (void)
{
  void       * h = NIL;
  void       * s = NIL;
  char       * k = NIL;
  const void * v = NIL;

  h = bless (WHash, 101);
  CU_ASSERT_PTR_NOT_EQUAL(h,NIL);

  s = makeWString ("arc");
  CU_ASSERT_PTR_NOT_EQUAL(s,NIL);
  WHash_insert (h, WString_getText (s), WString_getLength (s), s);

  s = makeWString ("warc");
  CU_ASSERT_PTR_NOT_EQUAL(s,NIL);
  WHash_insert (h, WString_getText (s), WString_getLength (s), s);

  k = "warc";
  v = WHash_get (h, makeS (k) );
  CU_ASSERT_PTR_EQUAL(v,WHash_get (h, makeS (k)));
   if (v)
 {  CU_PASS ( "found key:");}
  else
    CU_FAIL ( "no key   :");

  k = "arc";
  v = WHash_get (h, makeS (k) );
  CU_ASSERT_PTR_EQUAL(v,WHash_get (h, makeS (k)));
  if (v)
 {  CU_PASS ( "found key:");}
  else
    CU_FAIL ( "no key   :");

  k = "warc";
  s = WHash_delete (h, makeS (k) );
  destroy (s); /* need to explicitly free the deleted object */
  v = WHash_get (h, makeS (k) );
  CU_ASSERT_PTR_EQUAL(v,WHash_get (h, makeS (k)));
 if (v)
 {  CU_FAIL ( "found key:");}
  else
    CU_PASS ( "no key   :");

  destroy (h);

 
}


void test3 (void)
{
  void       * h = NIL;
  void       * s = NIL;
  const void * v = NIL;
  warc_u32_t   k;

  h = bless (WHash, 101);
  CU_ASSERT_PTR_NOT_EQUAL(h,NIL);

  s = makeWString ("arc");
  CU_ASSERT_PTR_NOT_EQUAL(s,NIL);
  k = 1;
  WHash_insert (h, makeI (k), s);

  s = makeWString ("iipc");
  CU_ASSERT_PTR_NOT_EQUAL(s,NIL);
  k = 2;
  WHash_insert (h, makeI (k), s);

  k = 1;
  v = WHash_get (h, makeI (k) );
  CU_ASSERT_PTR_EQUAL(v,WHash_get (h, makeI (k)));
  if (v)
 {  CU_PASS ( "found key:");}
  else
    CU_FAIL ( "no key   :");


  k = 7777;
  v = WHash_get (h, makeI (k) );
  CU_ASSERT_PTR_EQUAL(v,WHash_get (h, makeI (k))) ; 
   if (v)
 {  CU_FAIL ( "found key:");}
  else
    CU_PASS ( "no key   :");
  destroy (h);

  
}



void test4 (void)
{
  
  void       * h = NIL;
  void       * s = NIL;
  char       * k = NIL;
  const void * v = NIL;

  h = bless (WHash, 101);
  CU_ASSERT_PTR_NOT_EQUAL(h,NIL);

  s = makeWString ("arc");
  CU_ASSERT_PTR_NOT_EQUAL(s,NIL);
  WHash_insert (h, WString_getText (s), WString_getLength (s), s);
 k = "arc";
  v = WHash_get (h, makeS (k) );

  CU_ASSERT_PTR_EQUAL(v,WHash_get (h, makeS (k)));

  if (v)
 {  CU_PASS ( "found key:");}
  else
    CU_FAIL ( "no key   :");

 s = makeWString ("arc");
  WHash_insert (h, WString_getText (s), WString_getLength (s), s);
 k = "arc";
  v = WHash_get (h, makeS (k) );
  CU_ASSERT_PTR_EQUAL(v,WHash_get (h, makeS (k)));
  if (v)
 {  CU_PASS ( "found key:");}
  else
    CU_FAIL ( "no key   :");

  destroy (h);



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
       (NULL == CU_add_test(pSuite, "TEST 2: ", test2))||
       (NULL == CU_add_test(pSuite, "TEST 3: ", test3))||
       (NULL == CU_add_test(pSuite,"TEST 4: ", test4)))
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
                CU_set_output_filename("./utest/outputs/hash");
    		CU_set_output_filename("./utest/outputs/hash" );
  		CU_automated_run_tests();
   		CU_list_tests_to_file();
           	break;}
     
   }


 /*CU_console_run_tests();*/
   CU_cleanup_registry();
   return CU_get_error();


}
