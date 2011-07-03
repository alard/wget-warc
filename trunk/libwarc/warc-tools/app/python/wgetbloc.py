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
from   wbloc    import WBloc


def main () :
    
    usage =  "./app/python/wgetbloc.py <-f file.warc> <-o offset> [-e] [-v] [-t <working_dir>]\n "\
             "\t-f    : valid WARC file name\n"\
             "\t-o    : record offset\n"\
             "\t[-e]  : print HTTP response headers (default 'no')\n"\
             "\t[-t]  : temporary working directory (default './')\n"\
             "./app/python/wgetbloc.py -f foo.warc.gz -n 7"
 
    parser = OptionParser(usage)

    parser.add_option("-f", "--file", dest="filename",
                      help="read data from FILENAME")

    parser.add_option("-o", "--offset", dest="offset",
                      help="record offset", type="int")

    parser.add_option("-e", "--headers",
                    action="store_false", default=True, dest="headers")
    
    parser.add_option("-t", "--tempdir", dest="tmpdir",
                      help="Temporary working directory", default=".")

    (options, args) = parser.parse_args()

    if len (args) != 0 :
       parser.error(" Incorrect arguments")

    if (not (options.filename)) :
        parser.error(" You must give WARC file name")

    if options.offset == None:
        parser.error(" You must provide a valid record offset")

    w = WFile (options.filename, 600 * 1024 * 1024, warc.WARC_FILE_READER, warc.WARC_FILE_DETECT_COMPRESSION, options.tmpdir)

    if (not (w)) :
        print "WARC file  not found "

    # go to the specified offset
    w.seek(options.offset);
    if w . hasMoreRecords ():
        r  = w . nextRecord ()
    else:
        print "End of file reached, or no record at this offset", options.offset
        sys.exit(0);

    # choose your buffer size (ex. 64K = 64 * 1024) to read the payload
    # (with the HTTP headers or not, use the boolean flag) chunk by chunk 
    b = WBloc (w, r, options.headers, 64 * 1024)
    while True:
        buff = b.getNext()
        if buff:
            # the chunk size is returned by calling "b.getLastChunkSize()"
            #sys.stderr.write("chunk size:" + b.getLastChunkSize())
            sys.stdout.write(buff)
        else: # no more data to read. reach the end of record
            break

    b.destroy ()
    r.destroy ()
    w.destroy ()

if __name__ == "__main__":
    main()
