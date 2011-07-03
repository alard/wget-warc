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
import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;
 
 
public interface wfilei extends Library {
        wfilei INSTANCE = (wfilei)
            Native.loadLibrary("warc",
                               wfilei.class);
    


        Pointer bless_Wfile (String path, long dim, int mode, int compressed, String dname );
        
     int     WFile_hasMoreRecords (Pointer x );
     Pointer  WFile_nextRecord     (Pointer x);
     int      WFile_isFull          (Pointer x );
     int     WFile_setMaxSize     (Pointer x,  long size);
     int     WFile_storeRecord    (Pointer x, Pointer wrec);
     int     WFile_seek           (Pointer x,  long offset);
     
     int isValid (Pointer x);
	 void destroy (Pointer x);

    }
