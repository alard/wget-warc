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

import sys, time, sha

from   optparse import OptionParser

import wpath
import warc
from   wfile   import WFile
from   wrecord import WRecord 



def addToWarc (fname, wfile, uri, mime, date, ip, cmode, maxsize, tmpdir):
    ##  creating a new record  ##
    ##  don't forget to check return values of each functions  ##

        w = WFile(wfile, maxsize, warc.WARC_FILE_WRITER, cmode, tmpdir)
        if w == None:
                print "Couldn't create a WARC File object"
                return
    
	r = WRecord()
        if r == None:
                w.destroy ()
                print "Couldn't create an empty WARC record object"
                return   

	r . setRecordType(warc.WARC_RESOURCE_RECORD)
       	r . setTargetUri(uri, len(uri))	
       	r . setDate(date, len(date))
       	r . setContentType(mime, len(mime))

        # use your "unique identifier" function here
        #s = time.strftime ("%Y-%m-%dT%H:%M:%SZ", time.localtime())
        s = "%Y-%m-%dT%H:%M:%SZ"
	sh = sha.new(uri + s)
	rid = sh.hexdigest()
	rid = "uuid:" + rid
	r . setRecordId(rid, len(rid))
	r . setIpAddress(ip, len(ip))

	r . setContentFromFileName(fname)

	w . storeRecord(r)
	r . destroy()

	w . destroy()



def main () :

    usage = "Converts a file on disk to a WARC record \n\n"\
            + sys.argv[0] + " -f <file> -w <file.warc> \n"\
            "                     -u <url> -m <mime> -d <date> -i <ip> \n"\
            "                     [-c] [-x <maxsize>] [-t <working_dir>]\n "
    
    parser = OptionParser(usage)

    parser.add_option("-f", "--file", dest="filename",
                      help="valid file name on disk")
    parser.add_option("-w", "--wfile", dest="wfilename",
                      help="valid WARC file name")
    parser.add_option("-u", "--url", dest="url",
                      help="Url that match the file")
    parser.add_option("-m", "--mime", dest="mime",
                      help="file MIME type")
    parser.add_option("-d", "--date", dest="date",
                      help="file date")
    parser.add_option("-i", "--ipaddress", dest="ip",
                      help="Server IP address fom where the file was retrieved")
    parser.add_option("-c", "--compressed", dest="cmode",
                      action="store_true", help="WARC file will be GZIP compressed (default no)")
    parser.add_option("-x", "--maxsize", type="int", dest="maxsize",
                      help="WARC file maximum size in bytes (default 600Mb)", default=16 * 1024 * 1024 * 1024)
    parser.add_option("-t", "--tempdir", dest="tmpdir",
                      help="Temporary working directory (default \".\")", default=".")
    (options, args) = parser.parse_args()

    compmode = warc.WARC_FILE_UNCOMPRESSED
    if (options.cmode):
        compmode = warc.WARC_FILE_COMPRESSED_GZIP

    addToWarc(options.filename, options.wfilename, options.url, options.mime, options.date, options.ip, compmode, options.maxsize, options.tmpdir)
    
if __name__ == "__main__":
    if len(sys.argv) < 2:
        print "For help, try: " + sys.argv[0] + " -h"
        sys.exit(1)
    
    main()
