#!/usr/bin/env python

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
import time, sha

from afile import AFile
from arecord import ARecord

from wrecord  import WRecord
from wfile import WFile

import warc
import arc

from optparse import OptionParser

# attempt to make a warc name 
# from an arcfile name 
# basic strategy is to replace arc with warc
# if that fails just paste '.warc' on the end 
# if cmode is True  then add a gz if necessary
# otherwise remove it if necessary 

def guessname( fname , cmode ):
    if cmode:
        if not '.gz' in fname:
            fname += '.gz'
    else:
        if '.gz' in fname:
            fname = fname.replace( '.gz' , '' )
            
    if '.arc.gz' in fname:
        fname = fname.replace( '.arc.gz' , '.warc.gz' )
    else:
        fname += '.warc'
        
    return fname            
            


def convert( fname , outfname , tmpdir , cmode  ):
    a = AFile (fname , arc.ARC_FILE_DETECT_COMPRESSION, tmpdir)

    if (not (a)) :
       print "ARC file not found "
       return

    if (cmode) :
        cmode = warc.WARC_FILE_COMPRESSED_GZIP
    else :
        cmode = warc.WARC_FILE_UNCOMPRESSED

    w = WFile (outfname, 16 * 1024 * 1024 * 1024,
               warc.WARC_FILE_WRITER, cmode, tmpdir);
  

    if w == None:
       print "given temporary directory does not exist "
       a . destroy ()
       return
  
    while (a . hasMoreRecords () ) :

          ar = a. nextRecord ()

          if ar == None:
             print "bad ARC file"
             a . destroy ()
             w . destroy ()
             return

          wr = WRecord ()

          if wr == None:
             print "can not create WARC record object"
             a . destroy ()
             w . destroy ()
             ar . destroy ()
             return

          wr . setRecordType (warc.WARC_RESPONSE_RECORD)

          uri = ar . getUrl ()
          wr . setTargetUri (uri, len (uri))

          date = ar . getCreationDate ()
          wr . setDateFromArc  (date, len(date))

          mime = ar . getMimeType ()
          wr . setContentType (mime, len (mime))

          ip = ar . getIpAddress()
          wr . setIpAddress (ip, len(ip))
          
          s = time.strftime ("%Y-%m-%dT%H:%M:%SZ", time.localtime())
          sh = sha.new(uri + s)
          rid = sh.hexdigest()
          rid = "uuid:" + rid
          wr. setRecordId  (rid, len (rid))

          if (ar . transferContent (wr, a)) :
              print "Unable to pass content to the WRecord"
              a . destroy ()
              w . destroy ()
              ar . destroy ()
              return

          if (w . storeRecord (wr) ) :
              print "failed to write WRecord" 
              a . destroy ()
              w . destroy ()
              ar . destroy ()
              return

          ar . destroy ()
          wr . destroy ()
    
    a . destroy ()
    w . destroy ()

def main () :

    usage =  "./app/python/arc2warc.py  [OPTIONS] ARC-FILES "
             
 
    parser = OptionParser(usage)

    parser.add_option("-c", "--compressed", dest="cmode",
                      action="store_true", help="compressed outpout  WARC FILE")
    parser.add_option("-t", "--tempdir", dest="tmpdir",
                      help="Temporary working directory", default="./")
    parser.add_option("-v","--verbose",
                                action="store_true",dest="verbose" , default=False,
                                help="print more information")                        

    (options, args) = parser.parse_args()


    if not ( len (args) > 0 ):
       parser.error(" Please give one or more arcs to convert")


    for fname in args:
        ofname = guessname( fname , options.cmode )
        if options.verbose:
            print 'Converting %s to %s' % ( fname , ofname )            
        convert( fname , ofname , options.tmpdir , options.cmode )
        if options.verbose:
            print 'Done'    
        



  
    return


if __name__ == "__main__":
    main()
