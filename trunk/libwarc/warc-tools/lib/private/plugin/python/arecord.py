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

import arc
import warc
from wrecord import WRecord
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

class ARecord:

##Constructor ##
	def __init__(self):
		self.classtype = WTypes()
		self.me = 0
		
		self.type = self.classtype.ARecord

##The getters ##

	def getMimeType(self):
		return arc.ARecord_getMimeType(self.me)


	def getUrl(self):
		return arc.ARecord_getUrl(self.me)


	def getCreationDate(self):
		return arc.ARecord_getCreationDate(self.me)


	

	def getIpAddress(self):
		return arc.ARecord_getIpAddress(self.me)


	def transferContent (self, wrec, afile):
		if (afile == 0 or wrec == 0):
			return (True)
		else:
			if (arc.ARecord_transferContent (self.me, wrec.getInternal(self), afile.getInternal(self)) == 0):
				return (False)
			else:
				return (True)


##Python ARecord class methods ##

	def external_set(self, a,r):
		if (a.type == self.classtype.AFile):
			self.me = r 

	def getInternal (self, a):
		if (a.type == self.classtype.AFile):
			return self.me
		else:
			return 0

 

	def type(self):
		return self.type
 

## destructor ##
	def destroy(self):
		arc.destroy(self.me)


##  end of wrapper ##

