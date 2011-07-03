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

/* file.i */
 %{
   #include <wport.h>
   #include <wclass.h>
   #include "pyback.h"
    /*
     * C default headers
     */


#include <wfile.h>  /* WARC base class structure */

/* statically defined C to Python callback adapter */
static warc_bool_t PythonCallback(void* user_data, const char * buff, 
                                  unsigned int size) 
{
  PyObject *func, *arglist, *result, *arg;
  callbackStruct* cbStruct = (callbackStruct*)user_data;

  /* PyEval_AcquireThread(thread_state); */
  func = cbStruct->callback;
  arg = cbStruct->arg;
  
  /* arglist = Py_BuildValue("(O)", arg);*/
  arglist = Py_BuildValue("(Osi)", arg, buff, size);
  result = PyEval_CallObject(func, arglist);
  Py_DECREF(arglist);
  Py_XDECREF(result);

  /* PyEval_ReleaseThread(thread_state); */
  return;
}
 
 
/* Function to register a request for a callback */

PyObject* pyWFile_register(void  * a, void * b, 
                            PyObject * pyfunc, PyObject * arg) 
{
  int ret;
  PyObject *tuple, *ptr;
  char* tmpstr;
  callbackStruct* cbStruct;
  
  if (! (tuple = PyTuple_New (2)) )
    {
      PyErr_SetString (PyExc_RuntimeError, "pyWFile_register: PyTuple_New()");
      return NULL;
    }

  cbStruct = (callbackStruct*) malloc(sizeof(callbackStruct));

  Py_XINCREF (pyfunc);
  Py_XINCREF (arg);

  cbStruct -> callback = pyfunc;
  cbStruct -> arg = arg;
  
  ret = WFile_register (a, b, PythonCallback, (void*)cbStruct);
  
  ptr = SWIG_NewPointerObj ((void*)cbStruct, 
                            SWIGTYPE_p_callbackStruct, 0);

  PyTuple_SET_ITEM(tuple, 0, Py_BuildValue("i", 0));
  PyTuple_SET_ITEM(tuple, 1, ptr);

  return tuple;
}
     
 %}

%apply unsigned int { warc_u32_t }
%apply unsigned long long { warc_u64_t }

/* %typemap(in) warc_u64_t max_size { */
/*     $1 = (warc_u64_t) PyLong_AsUnsignedLongLong($input); */
/* } */

typedef enum {WARC_FILE_UNCOMPRESSED = 0,
              WARC_FILE_COMPRESSED_GZIP,
              WARC_FILE_COMPRESSED_GZIP_DEFAULT_COMPRESSION,
              WARC_FILE_COMPRESSED_GZIP_NO_COMPRESSION,
              WARC_FILE_COMPRESSED_GZIP_BEST_SPEED,
              WARC_FILE_COMPRESSED_GZIP_BEST_COMPRESSION,
              WARC_FILE_DETECT_COMPRESSION
} wfile_comp_t;

  
typedef enum {WARC_FILE_INVALID = 0,
              WARC_FILE_READER,
              WARC_FILE_WRITER
} wfile_mode_t;

typedef enum {WARC_FALSE = 0,
              WARC_TRUE  = 1
} warc_bool_t;

extern const void * WFile;

extern warc_bool_t     WFile_hasMoreRecords (const void * const);
extern void *          WFile_nextRecord     (void *);
extern warc_bool_t     WFile_storeRecord    (void *, const void *);
extern wfile_comp_t    WFile_getCompressionMode (const void * const );

extern warc_bool_t     WFile_seek (void *, warc_u64_t max_size);




%init %{
  PyEval_InitThreads();
  %}
%include "pyback.h"

