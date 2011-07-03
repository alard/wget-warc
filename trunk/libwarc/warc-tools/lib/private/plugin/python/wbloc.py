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
from wfile import WFile
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

class WBloc:

##Constructor ##
	def __init__(self, wfile, wrec, httpheaders, alloc):
		self.classtype = WTypes()
                self.httpheaders = httpheaders
                if (wfile.type != self.classtype.WFile or wrec.type != self.classtype.WRecord):
                        return 0
                self.type = self.classtype.WBloc
		self.me = warc.bless_WBloc(wfile.getInternal(self), wrec.getInternal(self), self.httpheaders, alloc)


## Bloc chunks recovering ##

	def getNext0(self):
		return warc.WBloc_next(self.me)

 	def getNext(self):
 		return warc.WRAPPER_WBloc_next(self.me)
    
 	def getHttpCode(self):
		return warc.WBloc_getHttpCode(self.me)

	def getLastChunkSize (self):
		return warc.WBloc_getLastChunkSize(self.me)


## Python WBloc class particular methods

	def type(self):
		return self.type
 

## Destructor ##

	def destroy(self):
		warc.destroy(self.me)


##  end  ##
