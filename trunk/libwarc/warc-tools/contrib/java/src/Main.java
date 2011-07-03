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

//import com.sun.jna.Pointer;


/** Simple example of native library declaration and usage. */
public class Main {



    public static void main(String[] args) {

    	    
    	    //open a wfile object in read mode
    	    wfile a = new wfile("./awanvl.warc", 600 * 1024 * 1024, 1, 0, ".");
    	    wrecord b = new wrecord();
    	    
    	    
    	    
    	    
    	      //open a wfile object in write mode
    	     //wfile a = new wfile("./2cwarc.warc.gz", 600 * 1024 * 1024, 2, 4, ".");    	    
    	     System.out.println(a.WFile_hasMoreRecords(a.file));

    	     
    	     b.record = a.WFile_nextRecord(a.file);
    	     
    	     System.out.println(a.WFile_isFull(a.file));
    	     //System.out.println(a.WFile_setMaxSize(p, 600 * 1024 * 1024));
    	    System.out.println(b.WRecord_getWarcId(b.record));
    	    
    	    a.destroy(a.file);
			b.destroy(b.record);

    }
}
