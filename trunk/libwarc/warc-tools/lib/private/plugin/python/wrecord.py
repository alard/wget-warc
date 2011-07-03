# -------------------------------------------------------------------  #
# Copyright (c) 2007-2008 Hanzo Archives Limited.                      #
#                                                                      #
# Licensed under the Apache License, Version 2.0 (the "License");      #
# you may not use this file except in compliance with the License.     #
# You may obtain a copy of the License at                              #
#                                                                      #
#     http://www.apache.org/licenses/LICENSE-2.0                       #
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
#     http://www.hanzoarchives.com/                                    #
#                                                                      #
# You may find more information about the WARC Tools project at        #
#                                                                      #
#     http://code.google.com/p/warc-tools/                             #
# -------------------------------------------------------------------  #

import warc
import types
import sys


sys.path.insert (0, ".")
from wtypes import WTypes

#class WTypes:
	#"A class to define Python WARC classes types"
	#def __init__(self):
		#self.WNone   = 0
		#self.WFile   = 1
		#self.WRecord = 2
		#self.WBloc   = 3
		#self.AFile   = 4
		#self.ARecord = 5
		

class WRecord:

##Constructor ##
	def __init__(self):
		self.classtype = WTypes()
		self.me = warc.bless_WRecord()
		
		self.type = self.classtype.WRecord


##The getters ##

	def getWarcId(self):
		return warc.WRecord_getWarcId(self.me)


	def getRecordType(self):
		return warc.WRecord_getRecordType(self.me)

	########
	def getTargetUri(self):
		return warc.WRecord_getTargetUri(self.me)


	def getDate(self):
		return warc.WRecord_getDate(self.me)

	########
	def getContentType(self):
		return warc.WRecord_getContentType(self.me)


	def getRecordId(self):
		return warc.WRecord_getRecordId(self.me)


	def getContent(self):
		return warc.WRecord_getContent(self.me)


	def getAnvlValue (self, key):
		return warc.WRecord_getAnvlValue(self.me,key)


	def getContentLength(self):
		return warc.WRecord_getContentLength(self.me)

	########
	def getConcurrentTo(self):
		return warc.WRecord_getConcurrentTo(self.me)

	########
	def getBlockDigest(self):
		return warc.WRecord_getBlockDigest(self.me)

	########
	def getPayloadDigest(self):
		return warc.WRecord_getPayloadDigest(self.me)

        ########
	def getIpAddress(self):
		return warc.WRecord_getIpAddress(self.me)

        ########
	def getRefersTo(self):
		return warc.WRecord_getRefersTo(self.me)

	########
	def getTruncated(self):
		return warc.WRecord_getTruncated(self.me)

 	########
	def getWarcInfoId(self):
		return warc.WRecord_getWarcInfoId(self.me)

	########
	def getProfile(self):
		return warc.WRecord_getProfile(self.me)

	########
	def getPayloadType(self):
		return warc.WRecord_getPayloadType(self.me)

	########
	def getSegmentOriginId(self):
		return warc.WRecord_getSegmentOriginId(self.me)

	########
	def getSegmentNumber(self):
		return warc.WRecord_getSegmentNumber(self.me)

	########
 	def getSegTotalLength(self):
		return warc.WRecord_getSegTotalLength(self.me)

	########
	def getFileName(self):
		return warc.WRecord_getFilename(self.me)


	def getOffset(self):
		return warc.WRecord_getOffset(self.me)


	def getUncompressedSize(self):
		return warc.WRecord_getUncompressedSize(self.me)


	def getCompressedSize(self):
		return warc.WRecord_getCompressedSize(self.me)


	def getBloc(self, with_http, wfile, code):
		if (wfile.type() == classtype.WFile):
			withh = warc.cvar.WARC_TRUE
			if (withh_http == false):
				withh = warc.cavar.WARC_FALSE 
			
			return warc.WRecord_getBloc(self.me, wfile.getInternal(self), withh, code)
		else:
			return ""


	def getAnvlFieldsNumber(self):
		return (warc.WRecord_getAnvlFieldsNumber(self.me))


	def getAnvlFieldKey(self, rank):
		key = warc.getFieldKey (self.me, rank)
		if (not (key)):
			return ""
		else:
			return (key)


	def getAnvlFieldValue(self, rank):
		val = warc.getFieldValue (self.me, rank)
		if (not (val)):
			return ""
		else:
			return (val)



##The setters ##

	def setRecordType(self,type):
		return warc.WRecord_setRecordType(self.me,type)


	def setTargetUri(self, uri, uri_len):
		return warc.WRecord_setTargetUri(self.me, uri, uri_len)


	def setDate(self, date, date_len):
		return warc.WRecord_setDate(self.me, date, date_len)


	def setContentType(self, ctype, ctype_len):
		return warc.WRecord_setContentType(self.me, ctype, ctype_len)


	def setRecordId(self, rid, id_len):
		return warc.WRecord_setRecordId(self.me, rid, id_len)


	def setConcurrentTo(self, rid, id_len):
		return warc.WRecord_setConcurrentTo(self.me, rid, id_len)


	def setBlockDigest(self, digest, digest_len):
		return warc.WRecord_setBlockDigest(self.me, digest, digest_len)


	def setPayloadDigest(self, digest, digest_len):
		return warc.WRecord_setPayloadDigest(self.me, digest, digest_len)


	def setIpAddress(self, ip, ip_len):
		return warc.WRecord_setIpAddress(self.me, ip, ip_len)


	def setRefersTo(self, rid, id_len):
		return warc.WRecord_setRefersTo(self.me, rid, id_len)


	def setTruncated(self, cause, cause_len):
		return warc.WRecord_setTruncated(self.me, cause, cause_len)


	def setWarcInfoId(self, rid, id_len):
		return warc.WRecord_setWarcInfoId(self.me, rid, id_len)


	def setProfile(self, profile, profile_len):
		return warc.WRecord_setProfile(self.me, profile, profile_len)


	def setPayloadType(self, ptype, type_len):
		return warc.WRecord_setPayloadType(self.me, ptype, type_len)


	def setSegmentOriginId(self, rid, id_len):
		return warc.WRecord_setSegmentOriginId(self.me, rid, id_len)


	def setSegmentNumber(self, num):
		return warc.WRecord_setSegmentNumber(self.me, num)


	def setSegTotalLength(self, slen):
		return warc.WRecord_setSegTotalLength(self.me, slen)


	def setFilename(self, name, name_len):
		return warc.WRecord_setFilename(self.me, name, name_len)


	def addAnvl(self, key, key_len, value, value_len):
		return warc.WRecord_addAnvl(self.me, key, key_len, value, value_len)


	def setContentFromFileName(self, name):
		return warc.WRecord_setContentFromFileName(self.me, name)


	def setDateFromArc(self, date, date_len):
		return warc.WRecord_setDateFromArc(self.me, date, date_len)
	
	def setContentFromString(self, str_v, str_len):
		return warc.WRecord_setContentFromString (self.me, str_v, str_len)
	
	
	def setContentFromStringConcat(self, str_v, str_len):
		return warc.WRecord_setContentFromStringConcat (self.me, str_v, str_len)


##Python WRecord class methods ##

	def external_set(self, w,r):
		if (w.type == self.classtype.WFile):
			self.me = r 

	def getInternal (self, w):
		if (w.type == self.classtype.WFile or w.type == self.classtype.WBloc or w.type == self.classtype.ARecord):
			return self.me
		else:
			return 0

 

	def type(self):
		return self.type
 

## destructor ##
	def destroy(self):
		warc.destroy(self.me)


##  end of wrapper ##
