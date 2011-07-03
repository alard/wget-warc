
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

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <Basic.h>
#include <Console.h>
#include <Automated.h>
#include <CUnit.h>
#include <CUError.h>
#include <TestDB.h>
#include <TestRun.h>
#include <warc.h>
#include <menu.h>
#include <afile.h>
#include <arecord.h>
#include <wversion.h>


#define WARC_MAX_SIZE 629145600LLU

#define makeS(s) ((warc_u8_t *) s), w_strlen((warc_u8_t *) (s))
int init_suite1(void) { return 0; }
int clean_suite1(void) { return 0; }
int init_suite2(void) { return 0; }
int clean_suite2(void) { return 0; }
int init_suite3(void) { return 0; }
int clean_suite3(void) { return 0; }
int init_suite4(void) { return 0; }
int clean_suite4(void) { return 0; }

void test1 (void)
{
  
  void  * a = bless (AFile, "./app/wdata/arc2warc/file.arc", ARC_FILE_UNCOMPRESSED, ".");
  void  * w = bless (WFile, "file.warc",
                     WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED, ".");

  CU_ASSERT_PTR_NOT_EQUAL(a,NIL);
  CU_ASSERT_PTR_NOT_EQUAL(w,NIL);
  assert(a);
 assert(w);
  while (AFile_hasMoreRecords (a) )
    {
      void * ar   = AFile_nextRecord (a);
      void * wr   = NIL;


      unless (ar)
      {
       CU_FAIL ("unable to retrieve the ArcRecord object");
        destroy (w);	
        destroy (a);
	
       return ;
      }
      else CU_PASS (" retrieve the ArcRecord object");

      wr = bless (WRecord);
      unless (wr)
      {
        CU_FAIL( "unable to create the WRecord object");
        destroy (w);
        destroy (a);
        destroy (ar);
	return ;
      }
          else CU_PASS (" retrieve the ArcRecord object");

      WRecord_setTargetUri (wr, makeS (ARecord_getUrl (ar) ) );

      WRecord_setRecordType (wr, WARC_RESOURCE_RECORD);

      WRecord_setDateFromArc (wr, makeS (ARecord_getCreationDate (ar) ) );

      WRecord_setContentType (wr, makeS (ARecord_getMimeType (ar) ) );

      WRecord_setRecordId (wr, makeS (ARecord_getUrl (ar) ) );

      WRecord_setIpAddress (wr, makeS (ARecord_getIpAddress (ar) ) );


      if (ARecord_transferContent (ar, wr, a) )
        {
          CU_FAIL (
                   "Unable to pass content to the Warc Record");
          destroy (w);
          destroy (a);
          destroy (wr);
          destroy (ar);
          return ;
        }
        else CU_PASS (" retrieve the ArcRecord object");

      if (WFile_storeRecord (w, wr) )
       { CU_FAIL( "failed to write the WRecord");}
    else CU_PASS (" retrieve the ArcRecord object");

      destroy (wr);
      destroy (ar);

    } /* end of while */
  destroy (w);
  destroy (a);

}



void test2 (void)
{
  void * w =  NIL; /* warc file object */
  void * r = NIL;  /* to recover records */
  warc_u32_t  i      = 1;
  w = bless (WFile, "file.warc", WARC_MAX_SIZE, WARC_FILE_READER, WARC_FILE_UNCOMPRESSED, ".");
  
  CU_ASSERT_PTR_NOT_EQUAL(w,NIL);
  assert(w);

  while (WFile_hasMoreRecords (w) )
    {
      unless ( (r = WFile_nextRecord (w) ) )
      {
        destroy (w);
 	 return ;
      }

/*
     fprintf (stdout, "WarcId: %-20s\n",         WRecord_getWarcId      (r) );
      fprintf (stdout, "Content Length: %-20d\n", WRecord_getContentLength  (r) );
      fprintf (stdout, "RecordType: %-20d\n",     WRecord_getRecordType  (r) );
      fprintf (stdout, "TargetUri: %-20s\n",     WRecord_getTargetUri  (r) );
      fprintf (stdout, "CreationDate: %-20s\n",   WRecord_getDate (r) );
      fprintf (stdout, "ContentType: %-20s\n",    WRecord_getContentType (r) );
      fprintf (stdout, "Ip Address: %-20s\n",     WRecord_getIpAddress (r));
      fprintf (stdout, "RecordId: %-20s\n",       WRecord_getRecordId    (r) );*/

	if(i==1)
	{
     
	CU_ASSERT_STRING_EQUAL(WRecord_getWarcId      (r), WARC_VERSION); 
	CU_ASSERT( WRecord_getContentLength  (r) == 22566);
	CU_ASSERT(WRecord_getRecordType  (r)==8); 
	CU_ASSERT_STRING_EQUAL(WRecord_getTargetUri  (r) , "filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getDate (r) , "2007-05-24T11:33:01Z"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getContentType (r) , "text/plain"); 
	CU_ASSERT_STRING_EQUAL( WRecord_getIpAddress (r), "0.0.0.0"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getRecordId    (r), "filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc"); 
	}
	if(i==2)
	{
	CU_ASSERT_STRING_EQUAL(WRecord_getWarcId      (r), WARC_VERSION); 
	CU_ASSERT( WRecord_getContentLength  (r) == 22566);
	CU_ASSERT(WRecord_getRecordType  (r)==8); 
	CU_ASSERT_STRING_EQUAL(WRecord_getTargetUri  (r) , "filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getDate (r) , "2007-05-24T11:33:01Z"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getContentType (r) , "text/plain"); 
	CU_ASSERT_STRING_EQUAL( WRecord_getIpAddress (r), "0.0.0.0"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getRecordId    (r), "filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc"); 
	}
	if(i==3)
	{
	CU_ASSERT_STRING_EQUAL(WRecord_getWarcId      (r), WARC_VERSION); 
	CU_ASSERT( WRecord_getContentLength  (r) == 22566);
	CU_ASSERT(WRecord_getRecordType  (r)==8); 
	CU_ASSERT_STRING_EQUAL(WRecord_getTargetUri  (r) , "filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getDate (r) , "2007-05-24T11:33:01Z"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getContentType (r) , "text/plain"); 
	CU_ASSERT_STRING_EQUAL( WRecord_getIpAddress (r), "0.0.0.0"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getRecordId    (r), "filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc"); 
	}
	i++;
      destroy (r);
    }

  destroy (w);
}

void test3 (void)
{
  void  * a = bless (AFile, "./app/wdata/arc2warc/mrec.arc", ARC_FILE_UNCOMPRESSED, ".");
  void  * w = bless (WFile, "mrec.warc",
                     WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED, ".");
 
  assert(a);
  assert(w);
  CU_ASSERT_PTR_NOT_EQUAL(a,NIL);
  CU_ASSERT_PTR_NOT_EQUAL(w,NIL);

  while (AFile_hasMoreRecords (a) )
    {
      void * ar   = AFile_nextRecord (a);
      void * wr   = NIL;

      unless (ar)
      {
        CU_FAIL("unable to retrieve the ArcRecord object");
        destroy (w);
        destroy (a);
       return ;
      }
	/*else CU_PASS("retrieve the ArcRecord object");*/

      wr = bless (WRecord);
      unless (wr)
      {
        CU_FAIL("unable to create the WRecord object");
        destroy (w);
        destroy (a);
        destroy (ar);
      return ;
      }
     /*else CU_FAIL("create the WRecord object");*/
      WRecord_setTargetUri (wr, makeS (ARecord_getUrl (ar) ) );

      WRecord_setRecordType (wr, WARC_RESOURCE_RECORD);

      WRecord_setDateFromArc (wr, makeS (ARecord_getCreationDate (ar) ) );

      WRecord_setContentType (wr, makeS (ARecord_getMimeType (ar) ) );

      WRecord_setRecordId (wr, makeS (ARecord_getUrl (ar) ) );

      WRecord_setIpAddress (wr, makeS (ARecord_getIpAddress (ar) ) );


      if (ARecord_transferContent (ar, wr, a) )
        {
        CU_FAIL(
                   "Unable to pass content to the Warc Record");
          destroy (w);
          destroy (a);
          destroy (wr);
          destroy (ar);
	return ;
        }
  else CU_PASS(" pass content to the Warc Record");


      if (WFile_storeRecord (w, wr) )
        {CU_FAIL("failed to write the WRecord");}
      else CU_PASS("write the WRecord");
      destroy (wr);

      destroy (ar);

    } /* end of while */
  destroy (w);

  destroy (a);


}



void test4 (void)
{
  void  * w =  NIL; /* warc file object */
  void  * r = NIL;  /* to recover records */
 warc_u32_t  i      = 1;
  w = bless (WFile, "mrec.warc", WARC_MAX_SIZE, WARC_FILE_READER, WARC_FILE_UNCOMPRESSED, ".");
  CU_ASSERT_PTR_NOT_EQUAL(w,NIL);
 assert(w);
  while (WFile_hasMoreRecords (w) )
    {
      unless ( (r = WFile_nextRecord (w) ) )
      {
        destroy (w);
    return ;
      }

      if(i==1)
	{
	CU_ASSERT_STRING_EQUAL(WRecord_getWarcId      (r), WARC_VERSION); 
	CU_ASSERT( WRecord_getContentLength  (r) == 22566);
	CU_ASSERT(WRecord_getRecordType  (r)==8); 
	CU_ASSERT_STRING_EQUAL(WRecord_getTargetUri  (r) , "filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getDate (r) , "2007-05-24T11:33:01Z"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getContentType (r) , "text/plain"); 
	CU_ASSERT_STRING_EQUAL( WRecord_getIpAddress (r), "0.0.0.0"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getRecordId    (r), "filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc"); 
	}
	if(i==2)
	{
	CU_ASSERT_STRING_EQUAL(WRecord_getWarcId      (r), WARC_VERSION); 
	CU_ASSERT( WRecord_getContentLength  (r) == 22566);
	CU_ASSERT(WRecord_getRecordType  (r)==8); 
	CU_ASSERT_STRING_EQUAL(WRecord_getTargetUri  (r) , "filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getDate (r) , "2007-05-24T11:33:01Z"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getContentType (r) , "text/plain"); 
	CU_ASSERT_STRING_EQUAL( WRecord_getIpAddress (r), "0.0.0.0"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getRecordId    (r), "filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc"); 
	}
	if(i==3)
	{
	CU_ASSERT_STRING_EQUAL(WRecord_getWarcId      (r), WARC_VERSION); 
	CU_ASSERT( WRecord_getContentLength  (r) == 22566);
	CU_ASSERT(WRecord_getRecordType  (r)==8); 
	CU_ASSERT_STRING_EQUAL(WRecord_getTargetUri  (r) , "filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getDate (r) , "2007-05-24T11:33:01Z"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getContentType (r) , "text/plain"); 
	CU_ASSERT_STRING_EQUAL( WRecord_getIpAddress (r), "0.0.0.0"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getRecordId    (r), "filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc"); 
	}
      if(i==4)
	{
	CU_ASSERT_STRING_EQUAL(WRecord_getWarcId      (r), WARC_VERSION); 
	CU_ASSERT( WRecord_getContentLength  (r) == 22566);
	CU_ASSERT(WRecord_getRecordType  (r)==8); 
	CU_ASSERT_STRING_EQUAL(WRecord_getTargetUri  (r) , "filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getDate (r) , "2007-05-24T11:33:01Z"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getContentType (r) , "text/plain"); 
	CU_ASSERT_STRING_EQUAL( WRecord_getIpAddress (r), "0.0.0.0"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getRecordId    (r), "filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc"); 
	}
	if(i==5)
	{
	CU_ASSERT_STRING_EQUAL(WRecord_getWarcId      (r), WARC_VERSION); 
	CU_ASSERT( WRecord_getContentLength  (r) == 22566);
	CU_ASSERT(WRecord_getRecordType  (r)==8); 
	CU_ASSERT_STRING_EQUAL(WRecord_getTargetUri  (r) , "filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getDate (r) , "2007-05-24T11:33:01Z"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getContentType (r) , "text/plain"); 
	CU_ASSERT_STRING_EQUAL( WRecord_getIpAddress (r), "0.0.0.0"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getRecordId    (r), "filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc"); 
	}
	if(i==6)
	{
	CU_ASSERT_STRING_EQUAL(WRecord_getWarcId      (r), WARC_VERSION); 
	CU_ASSERT( WRecord_getContentLength  (r) == 22566);
	CU_ASSERT(WRecord_getRecordType  (r)==8); 
	CU_ASSERT_STRING_EQUAL(WRecord_getTargetUri  (r) , "filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getDate (r) , "2007-05-24T11:33:01Z"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getContentType (r) , "text/plain"); 
	CU_ASSERT_STRING_EQUAL( WRecord_getIpAddress (r), "0.0.0.0"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getRecordId    (r), "filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc"); 
	}
	i++;

     destroy (r);
    }
  destroy (w);

}


void test5 (void)
{
  void  * a = bless (AFile, "./app/wdata/arc2warc/test1.arc", ARC_FILE_UNCOMPRESSED, ".");
  void  * w = bless (WFile, "test1.warc",
                     WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED, ".");


 CU_ASSERT_PTR_NOT_EQUAL(a,NIL);
CU_ASSERT_PTR_NOT_EQUAL(w,NIL);
  assert(a);
 assert(w);
  while (AFile_hasMoreRecords (a) )
    {
      void * ar   = AFile_nextRecord (a);
      void * wr   = NIL;


      unless (ar)
      {
        CU_PASS ("unable to retrieve the ArcRecord object");
        destroy (w);
        destroy (a);
      return ;
      }
/*  else CU_FAIL ("retrieve the ArcRecord object");*/
      wr = bless (WRecord);
      unless (wr)
      {
        CU_PASS ("unable to create the WRecord object");
        destroy (w);
        destroy (a);
        destroy (ar);
        return ;
      }
/*else  CU_FAIL ("create the WRecord object");*/

      WRecord_setTargetUri (wr, makeS (ARecord_getUrl (ar) ) );

      WRecord_setRecordType (wr, WARC_RESOURCE_RECORD);

      WRecord_setDateFromArc (wr, makeS (ARecord_getCreationDate (ar) ) );

      WRecord_setContentType (wr, makeS (ARecord_getMimeType (ar) ) );

      WRecord_setRecordId (wr, makeS (ARecord_getUrl (ar) ) );

      WRecord_setIpAddress (wr, makeS (ARecord_getIpAddress (ar) ) );


      if (ARecord_transferContent (ar, wr, a) )
        {
          CU_FAIL(
                   "Unable to pass content to the Warc Record");
          destroy (w);
          destroy (a);
          destroy (wr);
          destroy (ar);
         return ;
        }
else CU_PASS( "pass content to the Warc Record");

      if (WFile_storeRecord (w, wr) )
{       CU_FAIL("failed to write the WRecord");}
else  CU_PASS("write the WRecord");

      destroy (wr);

      destroy (ar);

    } /* end of while */



  destroy (w);

  destroy (a);

}


void test6 (void)
{
  void * w =  NIL; /* warc file object */
  void * r = NIL;  /* to recover records */
 warc_u32_t  i      = 1;


  w = bless (WFile, "test1.warc", WARC_MAX_SIZE, WARC_FILE_READER, WARC_FILE_UNCOMPRESSED, ".");
CU_ASSERT_PTR_NOT_EQUAL(w,NIL);
 assert(w);
  while (WFile_hasMoreRecords (w) )
    {
      unless ( (r = WFile_nextRecord (w) ) )
      {
        destroy (w);
      return ;
      }

     /* fprintf (stdout, "===========================================================================\n");*/
      /*print WHDLine object for this WRecord */

   /*   fprintf (stdout, "WarcId: %-20s\n",        WRecord_getWarcId      (r) );
      fprintf (stdout, "ContentLength: %-20d\n", WRecord_getContentLength  (r) );
      fprintf (stdout, "RecordType: %-20d\n",    WRecord_getRecordType  (r) );
      fprintf (stdout, "TargetUri: %-20s\n",    WRecord_getTargetUri  (r) );
      fprintf (stdout, "CreationDate: %-20s\n",  WRecord_getDate (r) );
      fprintf (stdout, "ContentType: %-20s\n",   WRecord_getContentType (r) );
      fprintf (stdout, "RecordId: %-20s\n",      WRecord_getRecordId    (r) );
      fprintf (stdout, "Ip Address: %-20s\n",    WRecord_getIpAddress (r));*/
	if(i==1)
	{
 
	CU_ASSERT_STRING_EQUAL(WRecord_getWarcId      (r), WARC_VERSION); 
	CU_ASSERT( WRecord_getContentLength  (r) == 1363);
	CU_ASSERT(WRecord_getRecordType  (r)==8); 
	CU_ASSERT_STRING_EQUAL(WRecord_getTargetUri  (r) , "filedesc://BnF-elec2007-20061119183301-00236-atlas1.arc"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getDate (r) , "2006-11-19T18:33:01Z"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getContentType (r) , "text/plain"); 
	CU_ASSERT_STRING_EQUAL( WRecord_getIpAddress (r), "12.68.40.0"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getRecordId    (r), "filedesc://BnF-elec2007-20061119183301-00236-atlas1.arc"); 
	}
	if(i==2)
	{
	CU_ASSERT_STRING_EQUAL(WRecord_getWarcId      (r), WARC_VERSION); 
	CU_ASSERT( WRecord_getContentLength  (r) == 1363);
	CU_ASSERT(WRecord_getRecordType  (r)==8); 
	CU_ASSERT_STRING_EQUAL(WRecord_getTargetUri  (r) , "iledesc://BnF-elec2007-20061119183301-00236-atlas1.arc"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getDate (r) , "2006-11-19T18:33:01Z"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getContentType (r) , "text/plain"); 
	CU_ASSERT_STRING_EQUAL( WRecord_getIpAddress (r), "25.35.75.11"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getRecordId    (r), "iledesc://BnF-elec2007-20061119183301-00236-atlas1.arc"); 
	}
	if(i==3)
	{
	CU_ASSERT_STRING_EQUAL(WRecord_getWarcId      (r), WARC_VERSION); 
	CU_ASSERT( WRecord_getContentLength  (r) == 1363);
	CU_ASSERT(WRecord_getRecordType  (r)==8); 
	CU_ASSERT_STRING_EQUAL(WRecord_getTargetUri  (r) , "iledesc://BnF-elec2007-20061119183301-00236-atlas1.arc"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getDate (r) , "2006-11-19T18:33:01Z"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getContentType (r) , "text/plain"); 
	CU_ASSERT_STRING_EQUAL( WRecord_getIpAddress (r), "11.18.68.48"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getRecordId    (r), "iledesc://BnF-elec2007-20061119183301-00236-atlas1.arc"); 
	}
	if(i==4)
	{
	CU_ASSERT_STRING_EQUAL(WRecord_getWarcId      (r), WARC_VERSION); 
	CU_ASSERT( WRecord_getContentLength  (r) == 1363);
	CU_ASSERT(WRecord_getRecordType  (r)==8); 
	CU_ASSERT_STRING_EQUAL(WRecord_getTargetUri  (r) , "filedesc://BnF-elec2007-20061119183301-00236-atlas1.arc"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getDate (r) , "2006-11-19T18:33:01Z"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getContentType (r) , "text/plain"); 
	CU_ASSERT_STRING_EQUAL( WRecord_getIpAddress (r), "12.68.40.0"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getRecordId    (r), "filedesc://BnF-elec2007-20061119183301-00236-atlas1.arc"); 
	}
	if(i==5)
	{
	CU_ASSERT_STRING_EQUAL(WRecord_getWarcId      (r), WARC_VERSION); 
	CU_ASSERT( WRecord_getContentLength  (r) == 1363);
	CU_ASSERT(WRecord_getRecordType  (r)==8); 
	CU_ASSERT_STRING_EQUAL(WRecord_getTargetUri  (r) , "iledesc://BnF-elec2007-20061119183301-00236-atlas1.arc"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getDate (r) , "2006-11-19T18:33:01Z"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getContentType (r) , "text/plain"); 
	CU_ASSERT_STRING_EQUAL( WRecord_getIpAddress (r), "25.35.75.11"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getRecordId    (r), "iledesc://BnF-elec2007-20061119183301-00236-atlas1.arc"); 
	}
	if(i==6)
	{
	CU_ASSERT_STRING_EQUAL(WRecord_getWarcId      (r), WARC_VERSION); 
	CU_ASSERT( WRecord_getContentLength  (r) == 1363);
	CU_ASSERT(WRecord_getRecordType  (r)==8); 
	CU_ASSERT_STRING_EQUAL(WRecord_getTargetUri  (r) , "iledesc://BnF-elec2007-20061119183301-00236-atlas1.arc"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getDate (r) , "2006-11-19T18:33:01Z"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getContentType (r) , "text/plain"); 
	CU_ASSERT_STRING_EQUAL( WRecord_getIpAddress (r), "11.18.68.48"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getRecordId    (r), "iledesc://BnF-elec2007-20061119183301-00236-atlas1.arc"); 
	}
	if(i==7)
	{
	CU_ASSERT_STRING_EQUAL(WRecord_getWarcId      (r), WARC_VERSION); 
	CU_ASSERT( WRecord_getContentLength  (r) == 1363);
	CU_ASSERT(WRecord_getRecordType  (r)==8); 
	CU_ASSERT_STRING_EQUAL(WRecord_getTargetUri  (r) , "filedesc://BnF-elec2007-20061119183301-00236-atlas1.arc"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getDate (r) , "2006-11-19T18:33:01Z"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getContentType (r) , "text/plain"); 
	CU_ASSERT_STRING_EQUAL( WRecord_getIpAddress (r), "12.68.40.0"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getRecordId    (r), "filedesc://BnF-elec2007-20061119183301-00236-atlas1.arc"); 
	}
	if(i==8)
	{
	CU_ASSERT_STRING_EQUAL(WRecord_getWarcId      (r), WARC_VERSION); 
	CU_ASSERT( WRecord_getContentLength  (r) == 1363);
	CU_ASSERT(WRecord_getRecordType  (r)==8); 
	CU_ASSERT_STRING_EQUAL(WRecord_getTargetUri  (r) , "iledesc://BnF-elec2007-20061119183301-00236-atlas1.arc"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getDate (r) , "2006-11-19T18:33:01Z"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getContentType (r) , "text/plain"); 
	CU_ASSERT_STRING_EQUAL( WRecord_getIpAddress (r), "25.35.75.11"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getRecordId    (r), "iledesc://BnF-elec2007-20061119183301-00236-atlas1.arc"); 
	}
	if(i==9)
	{
	CU_ASSERT_STRING_EQUAL(WRecord_getWarcId      (r), WARC_VERSION); 
	CU_ASSERT( WRecord_getContentLength  (r) == 1363);
	CU_ASSERT(WRecord_getRecordType  (r)==8); 
	CU_ASSERT_STRING_EQUAL(WRecord_getTargetUri  (r) , "iledesc://BnF-elec2007-20061119183301-00236-atlas1.arc"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getDate (r) , "2006-11-19T18:33:01Z"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getContentType (r) , "text/plain"); 
	CU_ASSERT_STRING_EQUAL( WRecord_getIpAddress (r), "11.18.68.48"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getRecordId    (r), "iledesc://BnF-elec2007-20061119183301-00236-atlas1.arc"); 
	}
	i++;
      destroy (r);
    }

  destroy (w);


}



void test7 (void)
{
  

  void  * a = bless (AFile, "./app/wdata/arc2warc/err1.arc", WARC_FILE_UNCOMPRESSED, ".");
  void  * w = bless (WFile, "err1.warc",
                     WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED, ".");

fprintf(stdout,"****************** Test 7:******************\n");
  CU_ASSERT_PTR_NOT_EQUAL(a,NIL);
CU_ASSERT_PTR_NOT_EQUAL(w,NIL);
  assert(a);
 assert(w);
  while (AFile_hasMoreRecords (a) )
    {
      void * ar   = AFile_nextRecord (a);
      void * wr   = NIL;


      unless (ar)
      {
        CU_PASS ("unable to retrieve the ArcRecord object");
        destroy (w);
        destroy (a);
    return;
      }
    
      wr = bless (WRecord);
      unless (wr)
      {
        CU_PASS ( "unable to create the WRecord object");
        destroy (w);
        destroy (a);
        destroy (ar);
       return ;
      }


      WRecord_setTargetUri (wr, makeS (ARecord_getUrl (ar) ) );

      WRecord_setRecordType (wr, WARC_RESOURCE_RECORD);

      WRecord_setDateFromArc (wr, makeS (ARecord_getCreationDate (ar) ) );

      WRecord_setContentType (wr, makeS (ARecord_getMimeType (ar) ) );

      WRecord_setRecordId (wr, makeS (ARecord_getUrl (ar) ) );

      WRecord_setIpAddress (wr, makeS (ARecord_getIpAddress (ar) ) );


      if (ARecord_transferContent (ar, wr, a) )
        {
          CU_FAIL ("Unable to pass content to the Warc Record");
          destroy (w);
          destroy (a);
          destroy (wr);
          destroy (ar);
     return ;
        }
else CU_PASS ("pass content to the Warc Record");

      if (WFile_storeRecord (w, wr) )
      { CU_FAIL("failed to write the WRecord");}
else CU_PASS(" write the WRecord");
      destroy (wr);

      destroy (ar);

    } /* end of while */



  destroy (w);

  destroy (a);

}
void test8 (void)
{
  

  void  * a = bless (AFile, "./app/wdata/arc2warc/err2.arc", ARC_FILE_UNCOMPRESSED, ".");
  void  * w = bless (WFile, "err2.warc",
                     WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED, ".");

fprintf(stdout,"****************** Test 8:******************\n");
  CU_ASSERT_PTR_NOT_EQUAL(a,NIL);
CU_ASSERT_PTR_NOT_EQUAL(w,NIL);
  assert(a);
 assert(w);
  while (AFile_hasMoreRecords (a) )
    {
      void * ar   = AFile_nextRecord (a);
      void * wr   = NIL;


      unless (ar)
      {
        CU_PASS("unable to retrieve the ArcRecord object");
        destroy (w);
        destroy (a);
       return ;
      }
  else  CU_FAIL(" retrieve the ArcRecord object");

      wr = bless (WRecord);
      unless (wr)
      {
        CU_PASS("unable to create the WRecord object");
        destroy (w);
        destroy (a);
        destroy (ar);
       return ;
      }
	else  CU_FAIL("create the WRecord object");

      WRecord_setTargetUri (wr, makeS (ARecord_getUrl (ar) ) );

      WRecord_setRecordType (wr, WARC_RESOURCE_RECORD);

      WRecord_setDateFromArc (wr, makeS (ARecord_getCreationDate (ar) ) );

      WRecord_setContentType (wr, makeS (ARecord_getMimeType (ar) ) );

      WRecord_setRecordId (wr, makeS (ARecord_getUrl (ar) ) );

      WRecord_setIpAddress (wr, makeS (ARecord_getIpAddress (ar) ) );


      if (ARecord_transferContent (ar, wr, a) )
        {
          CU_PASS ("Unable to pass content to the Warc Record");
          destroy (w);
          destroy (a);
          destroy (wr);
          destroy (ar);
     return ;
        }
	else CU_FAIL (" pass content to the Warc Record");


      if (WFile_storeRecord (w, wr) )
       { CU_PASS("failed to write the WRecord");}
    else CU_FAIL(" write the WRecord");

      destroy (wr);

      destroy (ar);

    } /* end of while */



  destroy (w);

  destroy (a);

}
void test9 (void)
{
 
  void  * a = bless (AFile, "./app/wdata/arc2warc/test2.arc", ARC_FILE_UNCOMPRESSED, ".");
  void  * w = bless (WFile, "test2.warc",
                     WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_UNCOMPRESSED, ".");


  CU_ASSERT_PTR_NOT_EQUAL(a,NIL);
CU_ASSERT_PTR_NOT_EQUAL(w,NIL);
  assert (a);
  assert (w);
  while (AFile_hasMoreRecords (a) )
    {
      void * ar   = AFile_nextRecord (a);
      void * wr   = NIL;


      unless (ar)
      {
         CU_PASS("unable to retrieve the ArcRecord object");
        destroy (w);
        destroy (a);
     return ;
      }


      wr = bless (WRecord);
      unless (wr)
      {
        CU_PASS("unable to create the WRecord object");
        destroy (w);
        destroy (a);
        destroy (ar);
      return ;
      }	

      WRecord_setTargetUri (wr, makeS (ARecord_getUrl (ar) ) );

      WRecord_setRecordType (wr, WARC_RESOURCE_RECORD);

      WRecord_setDateFromArc (wr, makeS (ARecord_getCreationDate (ar) ) );

      WRecord_setContentType (wr, makeS (ARecord_getMimeType (ar) ) );

      WRecord_setRecordId (wr, makeS (ARecord_getUrl (ar) ) );

      WRecord_setIpAddress (wr, makeS (ARecord_getIpAddress (ar) ) );


      if (ARecord_transferContent (ar, wr, a) )
        {
          CU_FAIL (
                   "Unable to pass content to the Warc Record");
          destroy (w);
          destroy (a);
          destroy (wr);
          destroy (ar);
         return ;
        }
      else  CU_PASS (
                   " pass content to the Warc Record");

      if (WFile_storeRecord (w, wr) )
       { CU_FAIL ("failed to write the WRecord");}
         else CU_PASS("write the WRecord");

      destroy (wr);

      destroy (ar);

    } /* end of while */
  destroy (w);
  destroy (a);

}



void test10 (void)
{

  void * w =  NIL; /* warc file object */
  void * r = NIL;  /* to recover records */


  w = bless (WFile, "test2.warc", WARC_MAX_SIZE, WARC_FILE_READER, WARC_FILE_UNCOMPRESSED, ".");

CU_ASSERT_PTR_NOT_EQUAL(w,NIL);
   assert(w);


  while (WFile_hasMoreRecords (w) )
    {
      unless ( (r = WFile_nextRecord (w) ) )
      {
        destroy (w);
     return ;
      }

      destroy (r);
    }

  CU_PASS ("success");

  destroy (w);
}

void test11 (void)
{
  void  * a = bless (AFile, "./app/wdata/arc2warc/file.arc.gz", ARC_FILE_COMPRESSED_GZIP, ".");
  void  * w = bless (WFile, "file.warc.gz",
                     WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_COMPRESSED_GZIP, ".");
 CU_ASSERT_PTR_NOT_EQUAL(a,NIL);
CU_ASSERT_PTR_NOT_EQUAL(w,NIL);
  assert(a);
 assert(w);
  while (AFile_hasMoreRecords (a) )
    {
      void * ar   = AFile_nextRecord (a);
      void * wr   = NIL;


      unless (ar)
      {
        CU_PASS ("unable to retrieve the ArcRecord object");
        destroy (w);
        destroy (a);
  return ;
      }
	

      wr = bless (WRecord);
      unless (wr)
      {
        CU_PASS("unable to create the WRecord object");
        destroy (w);
        destroy (a);
        destroy (ar);
        return ;
      }
	
      WRecord_setTargetUri (wr, makeS (ARecord_getUrl (ar) ) );

      WRecord_setRecordType (wr, WARC_RESOURCE_RECORD);

      WRecord_setDateFromArc (wr, makeS (ARecord_getCreationDate (ar) ) );

      WRecord_setContentType (wr, makeS (ARecord_getMimeType (ar) ) );

      WRecord_setRecordId (wr, makeS (ARecord_getUrl (ar) ) );

      WRecord_setIpAddress (wr, makeS (ARecord_getIpAddress (ar) ) );


      if (ARecord_transferContent (ar, wr, a) )
        {
          CU_FAIL (
                   "Unable to pass content to the Warc Record");
          destroy (w);
          destroy (a);
          destroy (wr);
          destroy (ar);
         return ;
        }
	else CU_PASS (
                   " pass content to the Warc Record");

      if (WFile_storeRecord (w, wr) )
        {
         CU_PASS ( "failed to write the WRecord");
          destroy (a);
          destroy (ar);
          destroy (w);
          destroy (wr);
      return ;
        }
else CU_PASS ( " write the WRecord");

      destroy (wr);

      destroy (ar);

    } /* end of while */



  destroy (w);

  destroy (a);

}

void test12 (void)
{
  void * w =  NIL; /* warc file object */
  void * r = NIL;  /* to recover records */
warc_u32_t  i      = 1;

  w = bless (WFile, "file.warc.gz", WARC_MAX_SIZE, WARC_FILE_READER, WARC_FILE_COMPRESSED_GZIP, ".");

CU_ASSERT_PTR_NOT_EQUAL(w,NIL);
 assert(w);
  while (WFile_hasMoreRecords (w) )
    {
      unless ( (r = WFile_nextRecord (w) ) )
      {
        destroy (w);
       return ;
      }

  /*    fprintf (stdout, "============================================================================\n");*/
      /*print WHDLine object for this WRecord */

      /*fprintf (stdout, "WarcId: %-20s\n",         WRecord_getWarcId      (r) );
      fprintf (stdout, "Content Length: %-20d\n", WRecord_getContentLength  (r) );
      fprintf (stdout, "RecordType: %-20d\n",     WRecord_getRecordType  (r) );
      fprintf (stdout, "TargetUri: %-20s\n",     WRecord_getTargetUri  (r) );
      fprintf (stdout, "CreationDate: %-20s\n",   WRecord_getDate (r) );
      fprintf (stdout, "ContentType: %-20s\n",    WRecord_getContentType (r) );
      fprintf (stdout, "Ip Address: %-20s\n",     WRecord_getIpAddress (r));
      fprintf (stdout, "RecordId: %-20s\n",       WRecord_getRecordId    (r) );*/
	if(i==1)
	{

	CU_ASSERT_STRING_EQUAL(WRecord_getWarcId      (r), WARC_VERSION); 
	CU_ASSERT( WRecord_getContentLength  (r) == 22566);
	CU_ASSERT(WRecord_getRecordType  (r)==8); 
	CU_ASSERT_STRING_EQUAL(WRecord_getTargetUri  (r) , "filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getDate (r) , "2007-05-24T11:33:01Z"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getContentType (r) , "text/plain"); 
	CU_ASSERT_STRING_EQUAL( WRecord_getIpAddress (r), "0.0.0.0"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getRecordId    (r), "filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc"); 
	}
	i++;

      destroy (r);
    }

  destroy (w);

}


void test13 (void)
{

  void  * a = bless (AFile, "./app/wdata/arc2warc/sfile.arc.gz", ARC_FILE_COMPRESSED_GZIP, ".");
  void  * w = bless (WFile, "sfile.warc.gz",
                     WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_COMPRESSED_GZIP, ".");
 CU_ASSERT_PTR_NOT_EQUAL(a,NIL);
CU_ASSERT_PTR_NOT_EQUAL(w,NIL);
  assert(a);
 assert(w);
  while (AFile_hasMoreRecords (a) )
    {
      void * ar   = AFile_nextRecord (a);
      void * wr   = NIL;


      unless (ar)
      {
        CU_PASS("unable to retrieve the ArcRecord object");
        destroy (w);
        destroy (a);
     return ;
      }
	/*else CU_FAIL(" retrieve the ArcRecord object");*/
      wr = bless (WRecord);
      unless (wr)
      {
        CU_PASS ("unable to create the WRecord object");
        destroy (w);
        destroy (a);
        destroy (ar);
       return ;
      }
/*else CU_FAIL ("create the WRecord object");*/

      WRecord_setTargetUri (wr, makeS (ARecord_getUrl (ar) ) );

      WRecord_setRecordType (wr, WARC_RESOURCE_RECORD);

      WRecord_setDateFromArc (wr, makeS (ARecord_getCreationDate (ar) ) );

      WRecord_setContentType (wr, makeS (ARecord_getMimeType (ar) ) );

      WRecord_setRecordId (wr, makeS (ARecord_getUrl (ar) ) );

      WRecord_setIpAddress (wr, makeS (ARecord_getIpAddress (ar) ) );



      if (ARecord_transferContent (ar, wr, a) )
        {
          CU_FAIL( "Unable to pass content to the Warc Record");
          destroy (w);
          destroy (a);
          destroy (wr);
          destroy (ar);
         return ;
        }

	else CU_PASS("pass content to the Warc Record");
      if (WFile_storeRecord (w, wr) )
        {CU_FAIL("failed to write the WRecord");}
             CU_PASS("write the WRecord");

      destroy (wr);

      destroy (ar);

    } /* end of while */



  destroy (w);

  destroy (a);

}
void test14 (void)
{
  void * w =  NIL; /* warc file object */
  void * r = NIL;  /* to recover records */
warc_u32_t  i      = 1;

  w = bless (WFile, "sfile.warc.gz", WARC_MAX_SIZE, WARC_FILE_READER, WARC_FILE_COMPRESSED_GZIP, ".");

CU_ASSERT_PTR_NOT_EQUAL(w,NIL);
 assert(w);
  while (WFile_hasMoreRecords (w) )
    {
      unless ( (r = WFile_nextRecord (w) ) )
      {
        destroy (w);
        return ;
      }

   /*   fprintf (stdout, "============================================================================\n");*/
      /*print WHDLine object for this WRecord */

    /*  fprintf (stdout, "WarcId: %-20s\n",         WRecord_getWarcId      (r) );
      fprintf (stdout, "Content Length: %-20d\n", WRecord_getContentLength  (r) );
      fprintf (stdout, "RecordType: %-20d\n",     WRecord_getRecordType  (r) );
      fprintf (stdout, "TargetUri: %-20s\n",     WRecord_getTargetUri  (r) );
      fprintf (stdout, "CreationDate: %-20s\n",   WRecord_getDate (r) );
      fprintf (stdout, "ContentType: %-20s\n",    WRecord_getContentType (r) );
      fprintf (stdout, "Ip Address: %-20s\n",     WRecord_getIpAddress (r));
      fprintf (stdout, "RecordId: %-20s\n",       WRecord_getRecordId    (r) );*/
if(i==1)
	{

	CU_ASSERT_STRING_EQUAL(WRecord_getWarcId      (r), WARC_VERSION); 
	CU_ASSERT( WRecord_getContentLength  (r) == 22566);
	CU_ASSERT(WRecord_getRecordType  (r)==8); 
	CU_ASSERT_STRING_EQUAL(WRecord_getTargetUri  (r) , "filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getDate (r) , "2007-05-24T11:33:01Z"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getContentType (r) , "text/plain"); 
	CU_ASSERT_STRING_EQUAL( WRecord_getIpAddress (r), "0.0.0.0"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getRecordId    (r), "filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc"); 
	}
	i++;

      destroy (r);
    }

  destroy (w);

}

void test15 (void)
{
 
  void  * a = bless (AFile, "./app/wdata/arc2warc/mfile.arc.gz", ARC_FILE_COMPRESSED_GZIP, ".");
  void  * w = bless (WFile, "mfile.warc.gz",
                     WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_COMPRESSED_GZIP, ".");
fprintf(stdout,"****************** Test 15:******************\n");
 CU_ASSERT_PTR_NOT_EQUAL(a,NIL);
CU_ASSERT_PTR_NOT_EQUAL(w,NIL);
  assert(a);
 assert(w);
  while (AFile_hasMoreRecords (a) )
    {
      void * ar   = AFile_nextRecord (a);
      void * wr   = NIL;


      unless (ar)
      {
        CU_PASS ("unable to retrieve the ArcRecord object");
        destroy (w);
        destroy (a);
        return ;
      }
	/*else
           CU_PASS(" retrieve the ArcRecord object");*/

      wr = bless (WRecord);
      unless (wr)
      {
        CU_PASS("unable to create the WRecord object");
        destroy (w);
        destroy (a);
        destroy (ar);
    return ;
      }
	/*else CU_FAIL("create the WRecord object");*/

      WRecord_setTargetUri (wr, makeS (ARecord_getUrl (ar) ) );

      WRecord_setRecordType (wr, WARC_RESOURCE_RECORD);

      WRecord_setDateFromArc (wr, makeS (ARecord_getCreationDate (ar) ) );

      WRecord_setContentType (wr, makeS (ARecord_getMimeType (ar) ) );

      WRecord_setRecordId (wr, makeS (ARecord_getUrl (ar) ) );

      WRecord_setIpAddress (wr, makeS (ARecord_getIpAddress (ar) ) );


      if (ARecord_transferContent (ar, wr, a) )
        {
          CU_FAIL("Unable to pass content to the Warc Record");
          destroy (w);
          destroy (a);
          destroy (wr);
          destroy (ar);
          return ;
        }
       else CU_PASS("pass content to the Warc Record");

      if (WFile_storeRecord (w, wr) )
       { CU_FAIL("failed to write the WRecord");}
	else CU_PASS(" write the WRecord");
          

      destroy (wr);

      destroy (ar);

    } /* end of while */



  destroy (w);

  destroy (a);

}
void test16 (void)
{
  void * w =  NIL; /* warc file object */
  void * r = NIL;  /* to recover records */
warc_u32_t  i      = 1;
  w = bless (WFile, "mfile.warc.gz", WARC_MAX_SIZE, WARC_FILE_READER, WARC_FILE_COMPRESSED_GZIP, ".");

CU_ASSERT_PTR_NOT_EQUAL(w,NIL);
 assert(w);
  while (WFile_hasMoreRecords (w) )
    {
      unless ( (r = WFile_nextRecord (w) ) )
      {
        destroy (w);
       return ;
      }

     /* fprintf (stdout, "============================================================================\n");*/
      /*print WHDLine object for this WRecord */

     /* fprintf (stdout, "WarcId: %-20s\n",         WRecord_getWarcId      (r) );
      fprintf (stdout, "Content Length: %-20d\n", WRecord_getContentLength  (r) );
      fprintf (stdout, "RecordType: %-20d\n",     WRecord_getRecordType  (r) );
      fprintf (stdout, "TargetUri: %-20s\n",     WRecord_getTargetUri  (r) );
      fprintf (stdout, "CreationDate: %-20s\n",   WRecord_getDate (r) );
      fprintf (stdout, "ContentType: %-20s\n",    WRecord_getContentType (r) );
      fprintf (stdout, "Ip Address: %-20s\n",     WRecord_getIpAddress (r));
      fprintf (stdout, "RecordId: %-20s\n",       WRecord_getRecordId    (r) );*/
	if(i==1)
	{
	CU_ASSERT_STRING_EQUAL(WRecord_getWarcId      (r), WARC_VERSION); 
	CU_ASSERT( WRecord_getContentLength  (r) == 22566);
	CU_ASSERT(WRecord_getRecordType  (r)==8); 
	CU_ASSERT_STRING_EQUAL(WRecord_getTargetUri  (r) , "filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getDate (r) , "2007-05-24T11:33:01Z"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getContentType (r) , "text/plain"); 
	CU_ASSERT_STRING_EQUAL( WRecord_getIpAddress (r), "0.0.0.0"); 
	CU_ASSERT_STRING_EQUAL(WRecord_getRecordId    (r), "filedesc://BnF-elec2007-20070524113301-00040-heritrix1.arc"); 
	}
	i++;

      destroy (r);
    }

  destroy (w);

}

void test17 (void)
{
  void  * a = bless (AFile, "./app/wdata/arc2warc/mmfile.arc.gz", ARC_FILE_COMPRESSED_GZIP, ".");
  void  * w = bless (WFile, "mmfile.warc.gz",
                     WARC_MAX_SIZE, WARC_FILE_WRITER, WARC_FILE_COMPRESSED_GZIP, ".");
fprintf(stdout,"****************** Test 17:******************\n");
 CU_ASSERT_PTR_NOT_EQUAL(a,NIL);
CU_ASSERT_PTR_NOT_EQUAL(w,NIL);
  assert(a);
 assert(w);
  while (AFile_hasMoreRecords (a) )
    {
      void * ar   = AFile_nextRecord (a);
      void * wr   = NIL;


      unless (ar)
      {
        CU_PASS("unable to retrieve the ArcRecord object");
        destroy (w);
        destroy (a);
    return ;
      }
/*else CU_FAIL("retrieve the ArcRecord object");*/

      wr = bless (WRecord);
      unless (wr)
      {
        CU_PASS("unable to create the WRecord object");
        destroy (w);
        destroy (a);
        destroy (ar);
       return ;
      }
/*else CU_FAIL(" create the WRecord object");*/

      WRecord_setTargetUri (wr, makeS (ARecord_getUrl (ar) ) );

      WRecord_setRecordType (wr, WARC_RESOURCE_RECORD);

      WRecord_setDateFromArc (wr, makeS (ARecord_getCreationDate (ar) ) );

      WRecord_setContentType (wr, makeS (ARecord_getMimeType (ar) ) );

      WRecord_setRecordId (wr, makeS (ARecord_getUrl (ar) ) );

      WRecord_setIpAddress (wr, makeS (ARecord_getIpAddress (ar) ) );


      if (ARecord_transferContent (ar, wr, a) )
        {
          CU_PASS (
                   "Unable to pass content to the Warc Record");
          destroy (w);
          destroy (a);
          destroy (wr);
          destroy (ar);
    return ;
        }
else CU_FAIL (" pass content to the Warc Record");

      if (WFile_storeRecord (w, wr) )
      {  CU_PASS ("failed to write the WRecord");}
else CU_FAIL(" write the WRecord");

      destroy (wr);

      destroy (ar);

    } /* end of while */



  destroy (w);

  destroy (a);

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
   if ((NULL == CU_add_test(pSuite, " 1: converting an uncompressed  Arc which contain a single record File to the Warc format", test1)) ||
       (NULL == CU_add_test(pSuite, " 2: reading an automatically created warc file from arc file", test2))||
       (NULL == CU_add_test(pSuite, " 3: converting a uncomprssed Arc File Which contains several records to the Warc format", test3))||
	(NULL == CU_add_test(pSuite, " 4: reading an automatically created warc file from arc file", test4)))
      
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

   if ((NULL == CU_add_test(pSuite, " 5: converting an Arc File Which contains several records to the Warc format", test5)) ||
       (NULL == CU_add_test(pSuite, " 6: reading a warc file created automatically from a arc file  ", test6))||
 (NULL == CU_add_test(pSuite, " 7: converting a corrupted  Arc File in the second record to the Warc format", test7))||
 (NULL == CU_add_test(pSuite, " 8: converting a corrupted Arc File in the first to the Warc format", test8)))   
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

   if ((NULL == CU_add_test(pSuite,  " 9: converting a  Arc File wich contains several records to the Warc format", test9)) ||
       (NULL == CU_add_test(pSuite, " 10: reading an automatically created warc file from arc file", test10))||
       (NULL == CU_add_test(pSuite, " 11: converting a compressed Arc File wich contains a single record to the Warc format", test11))||
       (NULL == CU_add_test(pSuite, " 12: reading an automatically created warc file from arc file", test12)))
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

   if ((NULL == CU_add_test(pSuite, " 13: converting a compressed Arc File wich contains several record to the Warc format ", test13)) ||
       (NULL == CU_add_test(pSuite, " 14: reading an automatically created warc file from arc file", test14))||
       (NULL == CU_add_test(pSuite, " 15: converting a corrupted Arc File at the second record to the Warc format", test15))||
       (NULL == CU_add_test(pSuite, " 16: reading an automatically created warc file from arc file", test16))||
       (NULL == CU_add_test(pSuite, " 17: converting a corrupted Arc File in the first record to the Warc format", test17)))
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
                CU_set_output_filename("./utest/outputs/a2w");
                CU_set_output_filename("./utest/outputs/a2w" );
                CU_automated_run_tests();
                CU_list_tests_to_file();
                break;
               }
     
   }
   CU_cleanup_registry();
   return CU_get_error();
}


