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

public class wrecord implements wrecordi
{
	public Pointer record = null;
	
	public wrecord ()
	{
		record = bless_Wrecord();
	}	
	
   //Getters 
     public String  WRecord_getWarcId      (Pointer x){
     return wrecordi.INSTANCE.WRecord_getWarcId      (x);
     }
     public int  WRecord_getRecordType  (Pointer x){
     return wrecordi.INSTANCE.WRecord_getRecordType (x);
     }
     public String  WRecord_getTargetUri   (Pointer x){
     return wrecordi.INSTANCE.WRecord_getTargetUri   (x);
     }
     public String  WRecord_getDate        (Pointer x){
     return wrecordi.INSTANCE.WRecord_getDate        (x);
     }
     public String  WRecord_getContentType (Pointer x){
     return wrecordi.INSTANCE.WRecord_getContentType (x);
     }
     public String  WRecord_getRecordId    (Pointer x){
     return wrecordi.INSTANCE.WRecord_getRecordId    (x);
     }
     public int  WRecord_getContent     (Pointer x){
     return wrecordi.INSTANCE.WRecord_getContent     (x);
     }
     public String  WRecord_getAnvlValue   (Pointer x, String val){
     return wrecordi.INSTANCE.WRecord_getAnvlValue   ( x,  val);
     }
      public long   WRecord_getContentLength   (Pointer x){
      return wrecordi.INSTANCE.WRecord_getContentLength   (x);
     }
      public String WRecord_getConcurrentTo    (Pointer x){
      return wrecordi.INSTANCE.WRecord_getConcurrentTo    (x);
     }
      public String WRecord_getBlockDigest     (Pointer x){
      return wrecordi.INSTANCE.WRecord_getBlockDigest     (x);
     }
      public String WRecord_getPayloadDigest   (Pointer x){
      return wrecordi.INSTANCE.WRecord_getPayloadDigest   (x);
     }
      public String WRecord_getIpAddress       (Pointer x){
      return wrecordi.INSTANCE.WRecord_getIpAddress       (x);
     }
      public String WRecord_getRefersTo        (Pointer x){
      return wrecordi.INSTANCE.WRecord_getRefersTo        (x);
     }
      public String WRecord_getTruncated       (Pointer x){
      return wrecordi.INSTANCE.WRecord_getTruncated       (x);
     }
      public String WRecord_getWarcInfoId      (Pointer x){
      return wrecordi.INSTANCE.WRecord_getWarcInfoId      (x);
     }
      public String WRecord_getProfile         (Pointer x){
      return wrecordi.INSTANCE.WRecord_getProfile         (x);
     }
      public String WRecord_getPayloadType     (Pointer x){
      return wrecordi.INSTANCE.WRecord_getPayloadType     (x);
     }
      public String WRecord_getSegmentOriginId (Pointer x){
      return wrecordi.INSTANCE.WRecord_getSegmentOriginId (x);
     }
      public int  WRecord_getSegmentNumber         (Pointer x){
      return wrecordi.INSTANCE.WRecord_getSegmentNumber         (x);
     }
      public int WRecord_getSegTotalLength         (Pointer x){
      return wrecordi.INSTANCE.WRecord_getSegTotalLength         (x);
     }
     public String WRecord_getFilename         (Pointer x){
     return wrecordi.INSTANCE.WRecord_getFilename         (x);
     }
     public Pointer WRecord_getAnvl             (Pointer x){
     return wrecordi.INSTANCE.WRecord_getAnvl             (x);
     }
     public long WRecord_getOffset           (Pointer x){
     return wrecordi.INSTANCE.WRecord_getOffset           (x);
     }
     public long WRecord_getUncompressedSize (Pointer x){
     return wrecordi.INSTANCE.WRecord_getUncompressedSize (x);
     }
     public long WRecord_getCompressedSize   (Pointer x){
     return wrecordi.INSTANCE.WRecord_getCompressedSize   (x);
 	 }
   
    
     public int WRecord_getAnvlFieldsNumber (Pointer x ){
     return wrecordi.INSTANCE.WRecord_getAnvlFieldsNumber ( x );
 	}
    
     public Pointer WRecord_getBloc (Pointer x, Pointer y,  int with, String code){
     return wrecordi.INSTANCE.WRecord_getBloc ( x,  y,   with,  code);
 	}
 	
 	//Setters

	 public int WRecord_setRecordType (Pointer x, int rtype) {return wrecordi.INSTANCE.WRecord_setRecordType (x,  rtype);}
	 
	 public int WRecord_setTargetUri (Pointer x, String uri,int uri_len) {return wrecordi.INSTANCE.WRecord_setTargetUri ( x,  uri, uri_len);}
	 
	 public int WRecord_setDate (Pointer x, String date,int date_len) {return wrecordi.INSTANCE.WRecord_setDate ( x,  date, date_len);}
	 
	 public int WRecord_setContentType (Pointer x, String ctype, int ctype_len ) {return wrecordi.INSTANCE.WRecord_setContentType ( x,  ctype,  ctype_len );}
	 
	 public int WRecord_setRecordId (Pointer x, String rid,int rid_len) {return wrecordi.INSTANCE.WRecord_setRecordId ( x,  rid, rid_len);}
	 
	 public int WRecord_setConcurrentTo (Pointer x, String cto, int cto_len) {return wrecordi.INSTANCE.WRecord_setConcurrentTo ( x,  cto,  cto_len);}

	 public int WRecord_setBlockDigest (Pointer x, String bdig, int bdig_len) {return wrecordi.INSTANCE.WRecord_setBlockDigest ( x,  bdig,  bdig_len);}

	 public int WRecord_setPayloadDigest (Pointer x, String pdig, int pdig_len) {return wrecordi.INSTANCE.WRecord_setPayloadDigest ( x,  pdig,  pdig_len);}

	 public int WRecord_setIpAddress (Pointer x, String ip, int ip_len) {return wrecordi.INSTANCE.WRecord_setIpAddress ( x,  ip,  ip_len);}

	 public int WRecord_setRefersTo (Pointer x, String rto, int rto_len) {return wrecordi.INSTANCE.WRecord_setRefersTo ( x,  rto,  rto_len);}

	 public int WRecord_setTruncated (Pointer x, String cause, int cause_len) {return wrecordi.INSTANCE.WRecord_setTruncated ( x,  cause,  cause_len);}
	 
	 public int WRecord_setWarcInfoId (Pointer x, String ridi, int ridi_len) {return wrecordi.INSTANCE.WRecord_setWarcInfoId ( x,  ridi,  ridi_len);}

	 public int WRecord_setProfile (Pointer x, String prof, int prof_len) {return wrecordi.INSTANCE.WRecord_setProfile ( x,  prof,  prof_len);}

	 public int WRecord_setPayloadType (Pointer x, String payt, int payt_len) {return wrecordi.INSTANCE.WRecord_setPayloadType ( x,  payt,  payt_len);}

	 public int WRecord_setSegmentOriginId (Pointer x, String soi, int soi_len) {return wrecordi.INSTANCE.WRecord_setSegmentOriginId ( x,  soi,  soi_len);}

	 public int WRecord_setSegmentNumber (Pointer x, int sn) {return wrecordi.INSTANCE.WRecord_setSegmentNumber ( x,  sn);}

	 public int WRecord_setSegTotalLength (Pointer x, int slen) {return wrecordi.INSTANCE.WRecord_setSegTotalLength ( x,  slen);}

	 public int WRecord_setFilename (Pointer x, String fname, int fname_len) {return wrecordi.INSTANCE.WRecord_setFilename ( x,  fname,  fname_len);}

	 public int WRecord_addAnvl (Pointer x, String key,int key_len, String val,int val_len) {return wrecordi.INSTANCE.WRecord_addAnvl ( x,  key, key_len,  val, val_len);}
	 
	 public int WRecord_setContentFromFileName (Pointer x, String ct) {return wrecordi.INSTANCE.WRecord_setContentFromFileName ( x,  ct);}
	 
	 public int WRecord_setDateFromArc (Pointer x , String date, int date_len) {return wrecordi.INSTANCE.WRecord_setDateFromArc ( x ,  date,  date_len);}
	 
	 public int WRecord_setContentFromString (Pointer x, String ct, int ct_len) {return wrecordi.INSTANCE.WRecord_setContentFromString ( x,  ct,  ct_len);}
	 
	 public int WRecord_setContentFromStringConcat (Pointer x, String ctc, int ctc_len) {return wrecordi.INSTANCE.WRecord_setContentFromStringConcat ( x,  ctc,  ctc_len);}
	
	
	public void     destroy           (Pointer x)
	{
	   wrecordi.INSTANCE.destroy(x);
	}
	
	public Pointer bless_Wrecord ()
	{
		return wrecordi.INSTANCE.bless_Wrecord();
	}
	
}
