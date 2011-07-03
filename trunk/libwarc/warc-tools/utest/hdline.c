
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

#include <warc.h>

#include <wfsmhdl.h>     /* WFsmHDLine */
#include <whdline.h>     /* WHDLine */



#define makeS(s) ((warc_u8_t *) s), w_strlen((warc_u8_t *) (s))

int test1 (void)
{
  const char * t  = "TEST 1";
  void       * hl = bless (WHDLine,
                           makeS (WARC_VERSION),
                           12,
                           WARC_INFO_RECORD,
                           makeS ("http://www.w3c.org"),
                           makeS ("12172007"),
                           makeS ("warcproject/testheaderline"),
                           makeS ("warc-x584jz39") );

  assert (hl);
fprintf (stdout, "////////////////////////test 1//////////////////////////////\n");
  fprintf (stdout, "%s>\n", t);

  fprintf (stdout, "WarcId:       %s\n", WHDLine_getWarcId      (hl) );
  fprintf (stdout, "DataLength:   %d\n", WHDLine_getDataLength  (hl) );
  fprintf (stdout, "RecordType:   %d\n", WHDLine_getRecordType  (hl) );
  fprintf (stdout, "SubjectUri:   %s\n", WHDLine_getSubjectUri  (hl) );
  fprintf (stdout, "CreationDate: %s\n", WHDLine_getCreationDate (hl) );
  fprintf (stdout, "ContentType:  %s\n", WHDLine_getContentType (hl) );
  fprintf (stdout, "RecordId:     %s\n", WHDLine_getRecordId    (hl) );

  destroy (hl);

  return 0;
}


int test2 (void)
{
  const char * t  = "TEST 2";
  void       * hl = bless (WHDLine,
                           makeS (WARC_VERSION),
                           12,
                           WARC_INFO_RECORD,
                           makeS ("http://www.w3c.org"),
                           makeS ("12172007"),
                           makeS ("warcproject/testheaderline"),
                           makeS ("warc-x584jz39") );
fprintf (stdout, "////////////////////////test 2//////////////////////////////\n");
  fprintf (stdout, "%s>\n", t);

  assert (hl);

  WHDLine_setWarcId       (hl, makeS ("warc/1.0") );
  WHDLine_setDataLength   (hl, 15);
  WHDLine_setRecordType   (hl, WARC_CONTINUATION_RECORD);
  WHDLine_setSubjectUri   (hl, makeS ("http://www.iso.net") );
  WHDLine_setCreationDate (hl, makeS ("01012999") );
  WHDLine_setContentType  (hl, makeS ("application/xpdf") );
  WHDLine_setRecordId     (hl, makeS ("warc-007") );

  fprintf (stdout, "WarcId:       %s\n", WHDLine_getWarcId      (hl) );
  fprintf (stdout, "DataLength:   %d\n", WHDLine_getDataLength  (hl) );
  fprintf (stdout, "RecordType:   %d\n", WHDLine_getRecordType  (hl) );
  fprintf (stdout, "SubjectUri:   %s\n", WHDLine_getSubjectUri  (hl) );
  fprintf (stdout, "CreationDate: %s\n", WHDLine_getCreationDate (hl) );
  fprintf (stdout, "ContentType:  %s\n", WHDLine_getContentType (hl) );
  fprintf (stdout, "RecordId:     %s\n", WHDLine_getRecordId    (hl) );

  destroy (hl);

  return 0;
}

int test3 (void)
{
  const char * t        = "TEST 3";
  const char * filename = "app/wdata/testlnwarc/header.warc";
  void       * fin      = NIL;
  void       * hl       = NIL;
  void       * fsm      = NIL;
fprintf (stdout, "////////////////////////test 3//////////////////////////////\n");
  fprintf (stdout, "%s>\n", t);

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
  return (1);

  /* init HDL FSM */
  fsm = bless (WFsmHDL, fin);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (WFsmHDL_run (fsm) )
  {
    /* generate the WHDLine object from the FSM */
    hl = WFsmHDL_transform (fsm);

    fprintf (stdout, "WarcId:       %s\n", WHDLine_getWarcId      (hl) );
    fprintf (stdout, "DataLength:   %d\n", WHDLine_getDataLength  (hl) );
    fprintf (stdout, "RecordType:   %d\n", WHDLine_getRecordType  (hl) );
    fprintf (stdout, "SubjectUri:   %s\n", WHDLine_getSubjectUri  (hl) );
    fprintf (stdout, "CreationDate: %s\n", WHDLine_getCreationDate (hl) );
    fprintf (stdout, "ContentType:  %s\n", WHDLine_getContentType (hl) );
    fprintf (stdout, "RecordId:     %s\n", WHDLine_getRecordId    (hl) );

    destroy (hl);
  }

  else
    {
      /* error when parsing the WARC header line */
      fprintf (stderr,  "error in FSM state address %p, at offset %ld in \"%s\"\n",
               WFsmHDL_state (fsm), ftell (fin), filename);
    }


  destroy (fsm);

  fclose  (fin);

  return 0;
}

int test4 (void)
{
  const char * t        = "TEST 4";
  const char * filename = "app/wdata/testlnwarc/header-err1.warc";
  void       * fin      = NIL;
  void       * hl       = NIL;
  void       * fsm      = NIL;
fprintf (stdout, "////////////////////////test 4//////////////////////////////\n");
  fprintf (stdout, "%s>\n", t);

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
  return (1);

  /* init HDL FSM */
  fsm = bless (WFsmHDL, fin);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (WFsmHDL_run (fsm) )
  {
    /* generate the WHDLine object from the FSM */
    hl = WFsmHDL_transform (fsm);

    fprintf (stdout, "WarcId:       %s\n", WHDLine_getWarcId      (hl) );
    fprintf (stdout, "DataLength:   %d\n", WHDLine_getDataLength  (hl) );
    fprintf (stdout, "RecordType:   %d\n", WHDLine_getRecordType  (hl) );
    fprintf (stdout, "SubjectUri:   %s\n", WHDLine_getSubjectUri  (hl) );
    fprintf (stdout, "CreationDate: %s\n", WHDLine_getCreationDate (hl) );
    fprintf (stdout, "ContentType:  %s\n", WHDLine_getContentType (hl) );
    fprintf (stdout, "RecordId:     %s\n", WHDLine_getRecordId    (hl) );

    destroy (hl);
  }

  else
    {
      /* error when parsing the WARC header line */
      fprintf (stderr, "error in FSM state address %p, at offset %ld in \"%s\"\n",
               WFsmHDL_state (fsm), ftell (fin), filename);
    }

  destroy (fsm);

  fclose  (fin);

  return 0;
}


int test5 (void)
{
  const char * t        = "TEST 5";
  const char * filename = "app/wdata/testlnwarc/header-err2.warc";
  void       * fin      = NIL;
  void       * hl       = NIL;
  void       * fsm      = NIL;
fprintf (stdout, "////////////////////////test 5//////////////////////////////\n");
  fprintf (stdout, "%s>\n", t);

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
  return (1);

  /* init HDL FSM */
  fsm = bless (WFsmHDL, fin);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (WFsmHDL_run (fsm) )
  {
    /* generate the WHDLine object from the FSM */
    hl = WFsmHDL_transform (fsm);

    fprintf (stdout, "WarcId:       %s\n", WHDLine_getWarcId      (hl) );
    fprintf (stdout, "DataLength:   %d\n", WHDLine_getDataLength  (hl) );
    fprintf (stdout, "RecordType:   %d\n", WHDLine_getRecordType  (hl) );
    fprintf (stdout, "SubjectUri:   %s\n", WHDLine_getSubjectUri  (hl) );
    fprintf (stdout, "CreationDate: %s\n", WHDLine_getCreationDate (hl) );
    fprintf (stdout, "ContentType:  %s\n", WHDLine_getContentType (hl) );
    fprintf (stdout, "RecordId:     %s\n", WHDLine_getRecordId    (hl) );

    destroy (hl);
  }

  else
    {
      /* error when parsing the WARC header line */
      fprintf (stderr, "error in FSM state address %p, at offset %ld in \"%s\"\n",
               WFsmHDL_state (fsm), ftell (fin), filename);
    }

  destroy (fsm);

  fclose  (fin);

  return 0;
}


int test6 (void)
{
  const char * t        = "TEST 6";
  const char * filename = "app/wdata/testlnwarc/header-err3.warc";
  void       * fin      = NIL;
  void       * hl       = NIL;
  void       * fsm      = NIL;
fprintf (stdout, "////////////////////////test 6//////////////////////////////\n");
  fprintf (stdout, "%s>\n", t);

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
  return (1);

  /* init HDL FSM */
  fsm = bless (WFsmHDL, fin);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (WFsmHDL_run (fsm) )
  {
    /* generate the WHDLine object from the FSM */
    hl = WFsmHDL_transform (fsm);

    fprintf (stdout, "WarcId:       %s\n", WHDLine_getWarcId      (hl) );
    fprintf (stdout, "DataLength:   %d\n", WHDLine_getDataLength  (hl) );
    fprintf (stdout, "RecordType:   %d\n", WHDLine_getRecordType  (hl) );
    fprintf (stdout, "SubjectUri:   %s\n", WHDLine_getSubjectUri  (hl) );
    fprintf (stdout, "CreationDate: %s\n", WHDLine_getCreationDate (hl) );
    fprintf (stdout, "ContentType:  %s\n", WHDLine_getContentType (hl) );
    fprintf (stdout, "RecordId:     %s\n", WHDLine_getRecordId    (hl) );

    destroy (hl);
  }

  else
    {
      /* error when parsing the WARC header line */
      fprintf (stderr, "error in FSM state address %p, at offset %ld in \"%s\"\n",
               WFsmHDL_state (fsm), ftell (fin), filename);
    }

  destroy (fsm);

  fclose  (fin);

  return 0;
}




int test7 (void)
{
  const char * t        = "TEST 7";
  const char * filename = "app/wdata/testlnwarc/header-err4.warc";
  void       * fin      = NIL;
  void       * hl       = NIL;
  void       * fsm      = NIL;
fprintf (stdout, "////////////////////////test 7//////////////////////////////\n");
  fprintf (stdout, "%s>\n", t);

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
  return (1);

  /* init HDL FSM */
  fsm = bless (WFsmHDL, fin);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (WFsmHDL_run (fsm) )
  {
    /* generate the WHDLine object from the FSM */
    hl = WFsmHDL_transform (fsm);

    fprintf (stdout, "WarcId:       %s\n", WHDLine_getWarcId      (hl) );
    fprintf (stdout, "DataLength:   %d\n", WHDLine_getDataLength  (hl) );
    fprintf (stdout, "RecordType:   %d\n", WHDLine_getRecordType  (hl) );
    fprintf (stdout, "SubjectUri:   %s\n", WHDLine_getSubjectUri  (hl) );
    fprintf (stdout, "CreationDate: %s\n", WHDLine_getCreationDate (hl) );
    fprintf (stdout, "ContentType:  %s\n", WHDLine_getContentType (hl) );
    fprintf (stdout, "RecordId:     %s\n", WHDLine_getRecordId    (hl) );

    destroy (hl);
  }

  else
    {
      /* error when parsing the WARC header line */
      fprintf (stderr,  "error in FSM state address %p, at offset %ld in \"%s\"\n",
               WFsmHDL_state (fsm), ftell (fin), filename);
    }

  destroy (fsm);

  fclose  (fin);

  return 0;
}

int test8 (void)
{
  const char * t        = "TEST 8";
  const char * filename = "app/wdata/testlnwarc/header-err5.warc";
  void       * fin      = NIL;
  void       * hl       = NIL;
  void       * fsm      = NIL;
fprintf (stdout, "////////////////////////test 8//////////////////////////////\n");
  fprintf (stdout, "%s>\n", t);

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
  return (1);

  /* init HDL FSM */
  fsm = bless (WFsmHDL, fin);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (WFsmHDL_run (fsm) )
  {
    /* generate the WHDLine object from the FSM */
    hl = WFsmHDL_transform (fsm);

    fprintf (stdout, "WarcId:       %s\n", WHDLine_getWarcId      (hl) );
    fprintf (stdout, "DataLength:   %d\n", WHDLine_getDataLength  (hl) );
    fprintf (stdout, "RecordType:   %d\n", WHDLine_getRecordType  (hl) );
    fprintf (stdout, "SubjectUri:   %s\n", WHDLine_getSubjectUri  (hl) );
    fprintf (stdout, "CreationDate: %s\n", WHDLine_getCreationDate (hl) );
    fprintf (stdout, "ContentType:  %s\n", WHDLine_getContentType (hl) );
    fprintf (stdout, "RecordId:     %s\n", WHDLine_getRecordId    (hl) );

    destroy (hl);
  }

  else
    {
      /* error when parsing the WARC header line */
      fprintf (stderr,  "error in FSM state address %p, at offset %ld in \"%s\"\n",
               WFsmHDL_state (fsm), ftell (fin), filename);
    }

  destroy (fsm);

  fclose  (fin);

  return 0;
}

int test9 (void)
{
  const char * t        = "TEST 9";
  const char * filename = "app/wdata/testlnwarc/header-err6.warc";
  void       * fin      = NIL;
  void       * hl       = NIL;
  void       * fsm      = NIL;
fprintf (stdout, "////////////////////////test 9//////////////////////////////\n");
  fprintf (stdout, "%s>\n", t);

  /* open a valid WARC header file */
  fin = fopen (filename, "r");
  unless (fin)
  return (1);

  /* init HDL FSM */
  fsm = bless (WFsmHDL, fin);
  assert (fsm);

  /* run the FSM for WHDLine object detection */
  unless (WFsmHDL_run (fsm) )
  {
    /* generate the WHDLine object from the FSM */
    hl = WFsmHDL_transform (fsm);

    fprintf (stdout, "WarcId:       %s\n", WHDLine_getWarcId      (hl) );
    fprintf (stdout, "DataLength:   %d\n", WHDLine_getDataLength  (hl) );
    fprintf (stdout, "RecordType:   %d\n", WHDLine_getRecordType  (hl) );
    fprintf (stdout, "SubjectUri:   %s\n", WHDLine_getSubjectUri  (hl) );
    fprintf (stdout, "CreationDate: %s\n", WHDLine_getCreationDate (hl) );
    fprintf (stdout, "ContentType:  %s\n", WHDLine_getContentType (hl) );
    fprintf (stdout, "RecordId:     %s\n", WHDLine_getRecordId    (hl) );

    destroy (hl);
  }

  else
    {
      /* error when parsing the WARC header line */
      fprintf (stderr,  "error in FSM state address %p, at offset %ld in \"%s\"\n",
               WFsmHDL_state (fsm), ftell (fin), filename);
    }

  destroy (fsm);

  fclose  (fin);

  return 0;
}

int main (void)
{
  int (* tests []) () = { test1, test2, test3, test4,
                          test5, test6, test7, test8, test9
                        };

  warc_u32_t  i      = 0;

  for (i = 0; i < ARRAY_LEN (tests); ++ i)
    {
      tests[i] ();
    }

  return 0;
}
