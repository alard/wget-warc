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

import os
import warc
from   wrecord  import WRecord
from   wfile    import WFile
from warcutils import getRecTypeName , getHeaders
from keypath import keypath 
import glob
from filesort import batch_sort
from timetools import getTimestampFromWarcDate

CONSTANT =  600 * 1024 * 1024

def getTempName( location ):
    fname = os.path.join(  makeindexdir( location ) , 'index-input.in' )
    return fname 
    
def getIndexName( location ):
    fname = os.path.join( makeindexdir( location ) , 'index.ix' )
    return fname 
    
def getWarcList( location ):
    fname = os.path.join( makeindexdir( location ) , 'warc.lst' )
    warclist = []
    if os.path.exists( fname ):
        f = open( fname )
        for line in f:
            warclist.append( line.strip() )
    return warclist 
    
def writeWarcList( location , warcl ):
    fname = os.path.join( makeindexdir( location ) , 'warc.lst' )
    f = open( fname , 'w' )
    f.write( '\n'.join( warcl ))
    f.close()

def makeindexdir( location ):
    idxn = os.path.join( location , '.index' )
    if not os.path.exists( idxn ):
        os.mkdir( idxn )
    return idxn        
    
def cleanup( location ):
    fragments = glob.glob( os.path.join( location , 'index-input.*' ))
    for f in fragments:
        os.unlink( f )
        #pass

def findWarcs( location , recurse ):
    ret = []
    for root , dirs , files  in os.walk( location ):
        for item in files:
            if '.warc' in item:
                ret.append( os.path.join( root , item ))
        if not recurse:
            break
    return ret            

def indexWarc( indexin ,  fname , tempdir ):   
    f = open( indexin , 'a' )
    w = WFile ( fname  ,  CONSTANT , warc.WARC_FILE_READER, warc.WARC_FILE_DETECT_COMPRESSION, tempdir )
    if not w :
        return None 
    while ( w.hasMoreRecords() ) :
        r = w.nextRecord()
        rect =  getRecTypeName( r.getRecordType() )

        if rect == 'WARC_RESPONSE_RECORD' or rect == 'WARC_RESOURCE_RECORD':
            headers = getHeaders( w , r )           
            try:
                if 'status' in headers:
                    status = int( headers['status'] )
                else:
                    status = 200
            except:
                status = 200
            if 'content-type' in headers:
                contenttype= headers['content-type'].split(';')[0]
            else:
                contenttype='text/plain'
            # fix this     
            if status >300 and status < 400 and False: 
                redirect = ''
            else:
                redirect = '-'
            kp =  keypath( r.getTargetUri() )
            if len( kp ) < 400:
                f.write( '%s %s %s %u %u %s %s warc\n' % ( kp , 
                                                                getTimestampFromWarcDate( r.getDate() ) , 
                                                                contenttype,
                                                                200,
                                                                r.getOffset() , 
                                                                redirect ,
                                                                fname ))                                                                
        r.destroy()
    w.destroy( )          
    f.close()
    
def sortIndex( location ,  indexin , iname ):
    batch_sort( indexin , iname )

def makeindex( location , recurse=False ):
    print 'Locating warc files...'
    idxn = makeindexdir( location )
    olist = findWarcs( location  , recurse )
    wlist = olist[:]
    warcl = getWarcList( location )
    deletions = False
    for item in warcl: 
        if item in wlist:
            wlist.remove( item )
        else:
            deletions = True
            break
    if deletions:
        print 'Deleted warcs detected,  reindexing....' 
        wlist = olist 
        warcl = olist
    else:
        warcl.extend( wlist )
    writeWarcList( location , warcl )      
    iname = getIndexName( location )
    if len( wlist ) > 0:
        print 'Indexing....'
        indexin = getTempName( location )
        for item in wlist:
            print '\t', item
            indexWarc( indexin , item  , idxn )
        print 'Sorting....'
        sortIndex( location , indexin , iname )        
        cleanup( idxn )
    return iname
            
#makeindex( '/Users/markwilliamson/Desktop/warcs' )    
    
#print findWarcs( '/Users/markwilliamson/Desktop/'  , False )    
#print 'xxxxxxxxxxxxxxx'
#print findWarcs( '/Users/markwilliamson/Desktop/'  , True )    
