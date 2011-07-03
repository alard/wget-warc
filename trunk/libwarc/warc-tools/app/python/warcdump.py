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

import sys
from optparse import OptionParser

import wpath
import warc
from   wrecord  import WRecord
from   wfile    import WFile


def main () :
    
    usage =  "./app/python/warcdump.py -f <file.warc> [-v] [-t <working_dir>]\n "\
             "\t-f    : valid WARC file name\n"\
             "\t[-t]  : temporary working directory (default './')\n"\
             "\t[-v]  : dump ANVL (default false)"
 
    parser = OptionParser(usage)

    parser.add_option("-f", "--file", dest="filename",
                      help="read data from FILENAME")

    parser.add_option("-v", "--verbose",
                      action="store_true", dest="verbose")
    
    parser.add_option("-t", "--tempdir", dest="tmpdir",
                      help="Temporary working directory", default=".")

    (options, args) = parser.parse_args()

    if len (args) != 0 :
       parser.error("Incorrect arguments")

    if (not (options.filename)) :
        parser.error("You must provide a WARC file name")

    w = WFile (options.filename, 600 * 1024 * 1024, warc.WARC_FILE_READER, warc.WARC_FILE_DETECT_COMPRESSION, options.tmpdir)
    
    if w == None:
        print "Couldn't create a WARC File object"
        return

    print "%-20s %-20s %-10s %-20s %-45s %-44s %-86s " % ("Offset", "CSize", "WarcId", "Content-Length", "WARC-Type", "WARC-Date",  "WARC-Record-ID")

    while ( w.hasMoreRecords() ) :

          r = w.nextRecord()
          if r == None:
             w.destroy ()
             print "Couldn't get the WARC record object"
             return   

          sys.stdout.write ("%-20u " %  r . getOffset () )

          sys.stdout.write ("%-20u " %  r . getCompressedSize () )

          sys.stdout.write ("%-10s " %  r . getWarcId () )

          sys.stdout.write ("%-20u " %  r . getContentLength () ) 

          sys.stdout.write ("%-45u " %  r . getRecordType () ) 

          sys.stdout.write ("%-44s " %  r . getDate () )

          sys.stdout.write ("%-86s " %  r . getRecordId () )      
          #########
          m1 = warc.WARC_FALSE
          #########
          
          if (r . getContentType () ) :
              print "%-35s: %-20s" %  ("Content-Type" , r . getContentType () )
              m1 = warc.WARC_TRUE 

          if (r . getConcurrentTo ()) :
              print "%-35s: %-20s" % ("WARC-Concurrent-To" , r . getConcurrentTo ())
              m1 = warc.WARC_TRUE

          if (r . getBlockDigest ()) :
              print "%-35s: %-20s" % ( "WARC-Block-Digest", r . getBlockDigest ())
              m1 = warc.WARC_TRUE

          if (r . getPayloadDigest ()) :
              print "%-35s: %-20s"  % ("WARC-Payload-Digest", r . getPayloadDigest ())
              m1 = warc.WARC_TRUE

          if (r . getIpAddress ()) :
               print "%-35s: %-20s"  % ("WARC-IP-Address", r . getIpAddress ())
               m1 = warc.WARC_TRUE

          if ( r . getRefersTo ()) :
               print  "%-35s: %-20s" % ("WARC-Refers-To", r . getRefersTo ())
               m1 = warc.WARC_TRUE

          if (r . getTargetUri ()) :
               print   "%-35s: %-20s" % ( "WARC-Target-URI",r . getTargetUri ())
               m1 = warc.WARC_TRUE

          if (r . getTruncated ()) :
               print  "%-35s: %-20s" % ("WARC-Truncated", r . getTruncated ())
               m1 = warc.WARC_TRUE

          if ( r . getWarcInfoId ()) :
               print  "%-35s: %-20s" % ("WARC-Warcinfo-ID", r . getWarcInfoId ())
               m1 = warc.WARC_TRUE

          if (r . getFileName ()) :
               print "%-35s: %-20s" % ("WARC-Filename:", r . getFileName ())
               m1 = warc.WARC_TRUE

          if (r . getProfile ()) :
               print "%-35s: %-20s" % ("WARC-Profile", r . getProfile ())
               m1 = warc.WARC_TRUE

          if (r . getPayloadType ()) :
               print "%-35s: %-20s" % ("WARC-Identified-Payload-type", r . getPayloadType ())
               m1 = warc.WARC_TRUE

          if (r . getSegmentOriginId  ()) :
                print "%-35s: %-20s" % ("WARC-Segment-Origin-ID", r . getSegmentOriginId  ())
                m1 = warc.WARC_TRUE

          if (r . getSegmentNumber ()) :
                print "%-35s: %-20d" % ("WARC-Segment-Number", r . getSegmentNumber ())
                m1 = warc.WARC_TRUE

          if (r . getSegTotalLength ()) :
                print "%-35s: %-20d" % ("WARC-Segment-Total-Length", r . getSegTotalLength())
                m1 = warc.WARC_TRUE
         
          if (not (m1)) :
                print "--No One --"          

          if (options.verbose) :

                 nb = r . getAnvlFieldsNumber ()
                 if  nb != 0 :
                     i = 0
                     print "-- More Info--\n"
                     while ( i < nb ) :
                         print  "key :  ", r . getAnvlFieldKey (i)
                         print  "Value: ", r . getAnvlFieldValue (i)
                         i = i + 1
                        
          r.destroy()

    w.destroy ()
    return

if __name__ == "__main__":
    main()
