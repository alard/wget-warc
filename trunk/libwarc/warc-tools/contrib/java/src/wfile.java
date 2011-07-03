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

package warctools;



import com.sun.jna.Pointer;


/** Simple example of native library declaration and usage. */
public class wfile implements wfilei {
	
	public Pointer file = null;
	
	
public wfile (String path, long dim, int mode, int compressed, String dname)
{
	file = bless_Wfile( path,  dim,  mode,  compressed,  dname);
}

public Pointer bless_Wfile (String path, long dim, int mode, int compressed, String dname )
{
	return wfilei.INSTANCE.bless_Wfile( path,  dim,  mode,  compressed,  dname);
}

public int     WFile_hasMoreRecords (Pointer x )
{
	return wfilei.INSTANCE.WFile_hasMoreRecords(x);
}

public Pointer WFile_nextRecord     (Pointer x)
{
	return wfilei.INSTANCE.WFile_nextRecord(x);
}

public int      WFile_isFull          (Pointer x )
{
	return wfilei.INSTANCE.WFile_isFull(x);
}

public int     WFile_setMaxSize     (Pointer x,  long size)
{
	return wfilei.INSTANCE.WFile_setMaxSize(x, size);
}

public int     WFile_storeRecord    (Pointer x, Pointer wrec)
{
	return wfilei.INSTANCE.WFile_storeRecord(x, wrec);
}

public int     WFile_seek           (Pointer x,  long offset)
{
	return wfilei.INSTANCE.WFile_seek(x,offset);
}

public void     destroy           (Pointer x)
{
	wfilei.INSTANCE.destroy(x);
}

public  int isValid (Pointer x)
{
	return wfilei.INSTANCE.isValid(x);
}

}
