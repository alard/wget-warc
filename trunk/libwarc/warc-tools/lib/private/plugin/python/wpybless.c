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

/* 
  Here, there is a specialized bless functin for warc-tools library classes
 */


#include <warc.h>
#include <wbloc.h>

#include <wpybless.h>


WPUBLIC void * bless_WFile (const char  * fname, 
                            warc_u64_t max_size, 
                            wfile_mode_t mode, warc_bool_t compressed,
                            const char * dname)
{
  return (bless (WFile, fname, (warc_u64_t) max_size,  mode, compressed,dname));
}

WPUBLIC void * bless_WRecord (void)
{
  return (bless (WRecord));
}

WPUBLIC void * bless_WBloc (void  * wfile, void  * wrecord, 
                            warc_bool_t httpheaders, warc_u32_t alloc)
{
  return (bless (WBloc, wfile, wrecord, httpheaders, alloc));
}
