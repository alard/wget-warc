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
from arecord import ARecord
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



class AFile:

##Constructor ##

	def __init__(self, fname, compressed, dname):
		self.classtype=WTypes()
		self.me = arc.bless_AFile(fname, compressed, dname)
		self.type = self.classtype.AFile


## Warc Record Recovering ##

	def hasMoreRecords(self):
		if (arc.AFile_hasMoreRecords(self.me) == 0):
			return (False)
		else:
			return (True)



	def nextRecord(self):
		ar = ARecord()
		ar.external_set(self, arc.AFile_nextRecord(self.me))
		return ar



	def seek(self, offset):
		return arc.AFile_seek(self.me, offset)


## Python AFile class particular methods

	def type(self):
		return self.classtype
 

	def getInternal(self, r):
		if (r.type == self.classtype.ARecord):
			return self.me
		else:
			return 0
 
 

## Destructor ##

	def destroy(self):
		arc.destroy(self.me)



