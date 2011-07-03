# -------------------------------------------------------------------  #
# Copyright (c) 2007-2008 Hanzo Archives Limited.                      #
#                                                                      #
# Licensed under the Apache License, Version 2.0 (the "License");      #
# you may not use this file except in compliance with the License.     #
# You may obtain a copy of the License at                              #
#                                                                      #
#     http//www.apache.org/licenses/LICENSE-2.0                        #
#                                                                      #
# Unless required by applicable law or agreed to in writing, software  #
# distributed under the License is distributed on an "AS IS" BASIS,    #
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or      #
# implied.                                                             #
# See the License for the specific language governing permissions and  #
# limitations under the License.                                       #
#                                                                      #
# You may find more information about Hanzo Archives at                #
#                                                                      #
#     http//www.hanzoarchives.com/                                     #
#                                                                      #
# You may find more information about the WARC Tools project at        #
#                                                                      #
#     http//code.google.com/p/warc-tools/                              #
# -------------------------------------------------------------------  #

require 'warctools'


class WRecord

  @me = 0
  @type = :None

##Constructor ##
  def initialize
      @me = Warctools.bless_WRecord()
     if (@me == nil) then
        raise RuntimeError, "Can not create a WRecord object."
     end
      @type = :WRecord
  end


##The getters ##

  def getWarcId()
     return Warctools.WRecord_getWarcId(@me)
  end


  def getRecordType()
     return Warctools.WRecord_getRecordType(@me)
  end


  def getTargetUri()
     return Warctools.WRecord_getTargetUri(@me)
  end


  def getDate()
     return Warctools.WRecord_getDate(@me)
  end


  def getContentType()
     return Warctools.WRecord_getContentType(@me)
  end


  def getRecordId()
     return Warctools.WRecord_getRecordId(@me)
  end


  def getContent()
     return Warctools.WRecord_getContent(@me)
  end


  def getAnvlValue (key)
     return Warctools.WRecord_getAnvlValue(@me,key)
  end


  def getContentLength()
     return Warctools.WRecord_getContentLength(@me)
  end


  def getConcurrentTo()
     return Warctools.WRecord_getConcurrentTo(@me)
  end


  def getBlockDigest()
     return Warctools.WRecord_getBlockDigest(@me)
  end


  def getPayloadDigest()
     return Warctools.WRecord_getPayloadDigest(@me)
  end


  def getIpAddress()
     return Warctools.WRecord_getIpAddress(@me)
  end


  def getRefersTo()
     return Warctools.WRecord_getRefersTo(@me)
  end


   def getTruncated()
     return Warctools.WRecord_getTruncated(@me)
  end


  def getWarcInfoId()
     return Warctools.WRecord_getWarcInfoId(@me)
  end


  def getProfile()
     return Warctools.WRecord_getProfile(@me)
  end


  def getPayloadType()
     return Warctools.WRecord_getPayloadType(@me)
  end


  def getSegmentOriginId()
     return Warctools.WRecord_getSegmentOriginId(@me)
  end


  def getSegmentNumber()
     return Warctools.WRecord_getSegmentNumber(@me)
  end


   def getSegTotalLength()
     return Warctools.WRecord_getSegTotalLength(@me)
  end


  def getFileName()
     return Warctools.WRecord_getFilename(@me)
  end


  def getOffset()
     return Warctools.WRecord_getOffset(@me)
  end


  def getUncompressedSize()
     return Warctools.WRecord_getUncompressedSize(@me)
  end


  def getCompressedSize()
     return Warctools.WRecord_getCompressedSize(@me)
  end



  def getBloc(wfile, with_http, code)
      if (wfile.type == :WFile) then
         withh = Warctools::WARC_TRUE

         if (withh_http == false) then
            withh = Warctools::WARC_FALSE
         end

         return Warctools.WRecord_getBloc(@me, wfile.getInternal(), withh, code)
      else
         return ""
     end
  end


  def getAnvlFieldsNumber()
     return (Warctools.WRecord_getAnvlFieldsNumber(@me))
  end


  def getAnvlFieldKey(rank)
      key = Warctools.getFieldKey(@me, rank)

     if (key == nil) then
         return ""
     else
        return (key)
     end
  end


  def getAnvlFieldValue(rank)
     val = Warctools.getFieldValue(@me, rank)

     if (val == nil)
        return ""
     else
        return (val)
     end
  end



##The setters ##

  def setRecordType(type)
     return Warctools.WRecord_setRecordType(@me,type)
  end


  def setTargetUri(uri, uri_len)
     return Warctools.WRecord_setTargetUri(@me, uri, uri_len)
  end

  def setDate(date, date_len)
     return Warctools.WRecord_setDate(@me, date, date_len)
  end


  def setContentType(ctype, ctype_len)
     return Warctools.WRecord_setContentType(@me, ctype, ctype_len)
  end


  def setRecordId(rid, id_len)
     return Warctools.WRecord_setRecordId(@me, rid, id_len)
  end


  def setConcurrentTo(rid, id_len)
     return Warctools.WRecord_setConcurrentTo(@me, rid, id_len)
  end


  def setBlockDigest(digest, digest_len)
     return Warctools.WRecord_setBlockDigest(@me, digest, digest_len)
  end


  def setPayloadDigest(digest, digest_len)
     return Warctools.WRecord_setPayloadDigest(@me, digest, digest_len)
  end


  def setIpAddress(ip, ip_len)
     return Warctools.WRecord_setIpAddress(@me, ip, ip_len)
  end


  def setRefersTo(rid, id_len)
     return Warctools.WRecord_setRefersTo(@me, rid, id_len)
  end


  def setTruncated(cause, cause_len)
     return warc.WRecord_setTruncated(@me, cause, cause_len)
  end


  def setWarcInfoId(rid, id_len)
     return Warctools.WRecord_setWarcInfoId(@me, rid, id_len)
  end


  def setProfile(profile, profile_len)
     return Warctools.WRecord_setProfile(@me, profile, profile_len)
  end


  def setPayloadType(ptype, type_len)
     return Warctools.WRecord_setPauloadType(@me, ptype, type_len)
  end


  def setSegmentOriginId(rid, id_len)
     return Warctools.WRecord_setSegmentOriginId(@me, rid, id_len)
  end


  def setSegmentNumber(num)
     return warc.WRecord_setSegmentNumber(@me, num)
  end


  def setSegTotalLength(slen)
     return Warctools.WRecord_setSegTotalLength(@me, slen)
  end


  def setFilename(name, name_len)
     return Warctools.WRecord_setFilename(@me, name, name_len)
  end


  def addAnvl(key, key_len, value, value_len)
     return Warctools.WRecord_addAnvl(@me, key, key_len, value, value_len)
  end


  def setContentFromFileName(name)
     return Warctools.WRecord_setContentFromFileName(@me, name)
  end


  def setDateFromArc(date, date_len)
     return Warctools.WRecord_setDateFromArc(@me, date, date_len)
  end

  def setContentFromString(str_v, str_len)
     return Warctools.WRecord_setContentFromString(@me, str_v, str_len)
  end


  def setContentFromStringConcat(str_v, str_len)
     return Warctools.WRecord_setContentFromStringConcat(@me, str_v, str_len)
  end


##Ruby WRecord class methods ##

  def external_set(w,r)
     if (w.type == :WFile) then
         @me = r 
     end
  end


  def getInternal (w)
     if (w.type == :WFile || w.type == :WBloc || w.type == :ARecord) then
        return @me
     else
       return (nil)
     end
  end
 

  def type
     return @type
  end

## destructor ##

  def destroy()
     Warctools.destroy(@me)
  end


##  end of wrapper ##

end