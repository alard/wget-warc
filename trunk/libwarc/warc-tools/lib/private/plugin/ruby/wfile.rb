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


require 'warctools'
require 'wrecord'



class WFile

##Constructor ##

  def initialize (fname, max_size, mode, compressed, dname)

     @me = Warctools.bless_WFile(fname, max_size, mode, compressed, dname)
     if (@me == nil) then
        raise RuntimeError, "WFile object not created. Check your arguments"
     end
     @type = :WFile
  end


## Warc Record Recovering ##

  def hasMoreRecords()

     ret = Warctools.WFile_hasMoreRecords(@me)

     if (ret == 1) then
        return true
     else
        return false
     end
  end



  def nextRecord()
     r = WRecord.new()
     r.external_set(self, Warctools.WFile_nextRecord(@me))
     return r
  end



## Warc File info settings ##

  def setMaxSize (size)
     return Warctools.WFile_setMaxSize(@me, size)
  end


  def storeRecord (record)
     if (record.type == :WRecord) then
        r = record.getInternal()
        return Warctools.WFile_storeRecord(@me, r)
    else
        return (true)
    end
  end



  def seek(offset)
     return Warctools.WFile_seek(@me, offset)
  end


## : WFile class particular methods

  def type
     return @type
  end
 

  def getInternal(r)
     if (r.type == :WRecord || r.type == :WBloc) then
       return @me
     else
       return 0
     end
  end
 
 

## Destructor ##

  def destroy()
     Warctools.destroy(@me)
  end

end



