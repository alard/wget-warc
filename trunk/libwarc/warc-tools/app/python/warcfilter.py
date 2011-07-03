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
from wrecord  import WRecord
from wfile import WFile 

import warc
from optparse import OptionParser
import re
import sys

def  searchFromAnyPosition (field, string) :

     res = None

     for i  in range (len (string)) :

         res = re.compile (field).match(string, i)

         if (res) : 
            break

     return res
        


def main () :
    
    usage =  "./app/python/warcfilter -f <file.warc> [-u <uri>] [-m <mime>] [-r <rtype>] [-v] [-t <working_dir>]\n "\
             "\t-f    : valid WARC file name\n"\
             "\t[-u]  : regular expression comparison with URI\n"\
             "\t[-m]  : regular expression comparison with MIME\n"\
             "\t[-r]  : regular expression comparison with record types (see 'public/wrectype.h' for possible values)\n"\
             "\t[-t]  : temporary working directory (default './')\n"\
             "\t[-v]  : dump ANVL (default false)"
 
    parser = OptionParser(usage)

    parser.add_option("-f", "--file", dest="filename",
                      help="read data from FILENAME")

    parser.add_option("-u", "--uri", dest="uri",
                      help="filter applied to uri field")

    parser.add_option("-m", "--mime", dest="mime",
                      help="filter applied to mime field")

    parser.add_option("-r", "--rtype", dest="rtype",
                      help="filter applied to record type field")

    parser.add_option("-v", "--verbose",
                      action="store_true", dest="verbose")
    
    parser.add_option("-t", "--tempdir", dest="tmpdir",
                      help="Temporary working directory", default="./")

    (options, args) = parser.parse_args()

    if len (args) != 0 :
       parser.error(" Incorrect arguments")

    if (not (options.filename)) :
        parser.error(" You must give WARC file name")
   
    cpt = 0

    rectype = {0 : 'WARC_UNKNOWN_RECORD', 1 : 'WARC_INFO_RECORD', 2 : 'WARC_RESPONSE_RECORD', 3 :  'WARC_REQUEST_RECORD',  4 :  'WARC_METADATA_RECORD', \
               5 : 'WARC_REVISIT_RECORD', 6 :  'WARC_CONVERSION_RECORD', 7 : 'WARC_CONTINUATION_RECORD', 8 : 'WARC_RESOURCE_RECORD'}

    if (options.uri) :
       cpt = cpt + 1

    if (options.mime) :
       cpt = cpt +1 

    if (options.rtype) :
       cpt = cpt +1


    if  cpt != 1  :
        parser.error(" You must apply filter to one of fields : uri or mime or record type")

    
    w = WFile ( options.filename ,600 * 1024 * 1024, warc.WARC_FILE_READER, warc.WARC_FILE_DETECT_COMPRESSION, options.tmpdir)
                

    if (not (w)) :
             print "WARC file  not found  "
             return

    while (w . hasMoreRecords ()) :

          r = w . nextRecord()
     
          if (not (r)) :
             print "bad WARC file "
             return              

          m1 = None
               
         
          if (options.uri) :
        
              string = r . getTargetUri ()
             

              if (string) :

                 m1 = searchFromAnyPosition (options.uri, string)
               
                
          if (options . mime) :
        
              string = r.getContentType ()

              if (string) :

                 m1 =  searchFromAnyPosition (options.mime, string)

          if (options.rtype) :

                 rt =  warc.WRecord_getRecordType (r)
                 m1 =  searchFromAnyPosition (options.rtype, rectype [rt])
          
          if m1 :

              

              sys.stdout.write ("%-20u " % r . getOffset ())

              sys.stdout.write ("%-20u " % r . getCompressedSize ())

	      sys.stdout.write ("%-10s " % r . getWarcId ())

              sys.stdout.write ("%-20u " % r . getContentLength ()) 

              sys.stdout.write ("%-45u " % r . getRecordType ()) 

              sys.stdout.write ("%-44s " % r . getDate ())

              sys.stdout.write ("%-86s " % r . getRecordId ())      

              m1 = warc.WARC_FALSE
              print "More Fields:\n"

              if (r . getContentType ()) :
                  print "%-35s: %-20s" %  ("Content-Type" , r . getContentType ())
                  m1 = warc.WARC_TRUE 

          

              if (r . getConcurrentTo()) :
                 print "%-35s: %-20s" % ("WARC-Concurrent-To" , r . getConcurrentTo())
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

              if (r . getSegTotalLength()) :
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
                        

          r . destroy ()

    w . destroy ()
    return

if __name__ == "__main__":
    main()
