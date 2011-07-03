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

import wpath

import warc 
from   wfile    import WFile
from  wbloc  import WBloc
import re

CONSTANT  =  600 * 1024 * 1024
BLOCKSIZE = 32 * 1024
liner = re.compile( r'\n' , re.I | re.S | re.UNICODE)

rectypes ={} 
recnames ={} 

tmp = []
d = dir( warc )
for item in d:
    if 'RECORD' in item:
        tmp.append( item )
        
for rt in tmp:
    id =  warc.__dict__[rt]
    name =  rt 
    rectypes[id] = name 
    recnames[name] = id 
    
def getRecTypeName( id ):
    if rectypes.has_key( id ):
        return rectypes[id]
    return 'UNKNOWN' 
    
def getRecType( name ):
    if recnames.has_key( name ):
        return recnames[ name ]
    return None
    
def getHeaders( warcf , warcr ):
    dat = ''
    headers = {}
    b = WBloc(warcf, warcr, False , BLOCKSIZE )        
    while True:
        nd = b.getNext()
        if nd:
            dat += nd
        else:            
            break            
    if len(dat) > 0 :
        bits = dat.split( '\r\n\r\n' )
        content = "\r\n".join( bits[1:] )       
        header = bits[0].split('\r\n' )
        for line in header:
            line = line.strip()
            if len( line.strip() ) == 0:
                break
            else:
                if ':' in line:
                    bits = line.split(':')
                    headers[bits[0].lower() ] = ''.join( bits[1:] )
                elif line.startswith('HTTP' ):
                    parts = line.split(' ')
                    try:
                        headers[ 'protocol' ] = parts[0]
                        headers[ 'status' ] = parts[1]
                        headers[ 'code' ] = parts[2]
                    except:
                        pass
    b.destroy() 
    return headers
    
def getRecord( warcname , offset , tempdir ='.' ):
    w = WFile ( warcname  ,  CONSTANT , warc.WARC_FILE_READER, warc.WARC_FILE_DETECT_COMPRESSION, tempdir )
    w.seek( int(offset ))
    rec = w.nextRecord()
    b = WBloc( w , rec , False , BLOCKSIZE )
    dat = ''
    while True:
        nd = b.getNext()
        if nd:
            dat += nd
        else:            
            break       
    headers = {}                            
    if len(dat) > 0 :
        bits = dat.split( '\r\n\r\n' )
        content = "\r\n\r\n".join( bits[1:] )       
        header = bits[0].split('\r\n' )
        for line in header:
            line = line.strip()
            if len( line.strip() ) == 0:
                break
            else:
                if ':' in line:
                    bits = line.split(':')
                    headers[bits[0] ] = ''.join( bits[1:] )
                elif line.startswith('HTTP' ):
                    parts = line.split(' ')
                    try:
                        headers[ 'protocol' ] = parts[0]
                        headers[ 'status' ] = parts[1]
                        headers[ 'code' ] = parts[2]
                    except:
                        pass        
    b.destroy() 
    rec.destroy()
    w.destroy()
    return ( headers , content )
    
    
    return ( w , rec )
    
    
if __name__ == "__main__":
    for i in range( 9 ):
        print i,getRecTypeName(i)
            
        
        
