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



public interface wrecordi extends Library
{
        wrecordi INSTANCE = (wrecordi)
            Native.loadLibrary("warc",
                               wrecordi.class);
        	
   //Getters 
     String  WRecord_getWarcId      (Pointer x);
     int         WRecord_getRecordType  (Pointer x);
     String  WRecord_getTargetUri   (Pointer x);
     String  WRecord_getDate        (Pointer x);
     String  WRecord_getContentType (Pointer x);
     String  WRecord_getRecordId    (Pointer x);
     int        WRecord_getContent     (Pointer x);
     String  WRecord_getAnvlValue   (Pointer x, String val);
      long        WRecord_getContentLength   (Pointer x);
      String WRecord_getConcurrentTo    (Pointer x);
      String WRecord_getBlockDigest     (Pointer x);
      String WRecord_getPayloadDigest   (Pointer x);
      String WRecord_getIpAddress       (Pointer x);
      String WRecord_getRefersTo        (Pointer x);
      String WRecord_getTruncated       (Pointer x);
      String WRecord_getWarcInfoId      (Pointer x);
      String WRecord_getProfile         (Pointer x);
      String WRecord_getPayloadType     (Pointer x);
      String WRecord_getSegmentOriginId (Pointer x);
      int  WRecord_getSegmentNumber         (Pointer x);
      int WRecord_getSegTotalLength         (Pointer x);
     String WRecord_getFilename         (Pointer x);
     
     Pointer WRecord_getAnvl             (Pointer x);
     long   WRecord_getOffset           (Pointer x);
     long   WRecord_getUncompressedSize (Pointer x);
     long   WRecord_getCompressedSize   (Pointer x);
   
     //int  WRecord_getAnvlField        (Pointer x, int , String , String );
    
     int WRecord_getAnvlFieldsNumber (Pointer x );
    
     Pointer WRecord_getBloc (Pointer x, Pointer y,  int with, String code);
	
	//Setters
	 int WRecord_setRecordType (Pointer x, int rtype);
 
	 int WRecord_setTargetUri (Pointer x, String uri,int uri_len);
	 
	 int WRecord_setDate (Pointer x, String date,int date_len);
	 
	 int WRecord_setContentType (Pointer x, String ctype, int ctype_len );
	 
	 int WRecord_setRecordId (Pointer x, String rid,int rid_len);
	 
	 int WRecord_setConcurrentTo (Pointer x, String cto, int cto_len);

	 int WRecord_setBlockDigest (Pointer x, String bdig, int bdig_len);

	 int WRecord_setPayloadDigest (Pointer x, String pdig, int pdig_len);

	 int WRecord_setIpAddress (Pointer x, String ip, int ip_len);

	 int WRecord_setRefersTo (Pointer x, String rto, int rto_len);

	 int WRecord_setTruncated (Pointer x, String cause, int cause_len);
	 
	 int WRecord_setWarcInfoId (Pointer x, String ridi, int ridi_len);

	 int WRecord_setProfile (Pointer x, String prof, int prof_len);

	 int WRecord_setPayloadType (Pointer x, String payt, int payt_len);

	 int WRecord_setSegmentOriginId (Pointer x, String soi, int soi_len);

	 int WRecord_setSegmentNumber (Pointer x, int sn);

	 int WRecord_setSegTotalLength (Pointer x, int slen);

	 int WRecord_setFilename (Pointer x, String fname, int fname_len);

	 int WRecord_addAnvl (Pointer x, String key,int key_len, String val,int val_len);
	 
	 int WRecord_setContentFromFileName (Pointer x, String ct);
	 
	 int WRecord_setDateFromArc (Pointer x , String date, int date_len);
	 
	 int WRecord_setContentFromString (Pointer x, String ct, int ct_len);
	 
	 int WRecord_setContentFromStringConcat (Pointer x, String ctc, int ctc_len);
	
	 void     destroy           (Pointer x);
	 
	 Pointer bless_Wrecord ();

}
