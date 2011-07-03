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
require 'wfile'
require 'wrecord'


class WBloc

  ## Constructor ##
  def initialize(wfile, wrec, http, alloc)

    if (wfile.type != :WFile or wrec.type != :WRecord) then
      raise RuntimeError, "Error: The two first arguments must be respectively be a WFile object and a WRecord object"
    else
      
      if (http == false)
        @httpheaders = 0
      else
        @httpheaders = 1
      end
      
      @type = :WBloc
      @me   = Warctools.bless_WBloc(wfile.getInternal(self), 
                                    wrec.getInternal(self), 
                                    @httpheaders, alloc)

      if (@me == nil) then
        raise RuntimeError, "WBloc object not created. Check your arguments"
      end
    end
  end
  
  
  ## Bloc chunks recovering ##
  
  def getNext()
    return Warctools.WBloc_next(@me)
  end
  
  
  def getHttpCode()
    return Warctools.WBloc_getHttpCode(@me)
  end
  
  def getLastChunkSize()
    return Warctools.WBloc_getLastChunkSize(@me)
  end
  
  def copyPayloadToTemporary(tmpfile)
    return Warctools.WBloc_copyPayloadToTemporary(@me, tmpfile.fileno)
  end
  
  ## Ruby WBloc class particular methods
  
  def type()
    return @type
  end
  
  
  ## Destructor ##
  
  def destroy()
    Warctools.destroy(@me)
  end
  
  
##  end  ##
end
