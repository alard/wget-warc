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

import os, stat, re, sys
from optparse import OptionParser

def chomp (s):
    return s.rstrip("\n\r")
    #return re.sub(r"[\r\n]+$", "", s)
        
class BinSearch:
    def __init__(self, filename):
        self.fh    = open (filename, 'r')
        self.fsize = int (os.stat (filename) . st_size)

    def __del__(self):
        self.fh.close ()

    def next (self):
        for line in self.fh:
            #if self.p.match (line):
            if line.startswith(self.str):
                yield chomp(line)
            else:
                break

    def search (self, str):
        def upordown_gen (s):
            return lambda x: cmp(x, s.rstrip(" "))
        
        self.fh.seek (0, os.SEEK_SET)
        #self.p     = re.compile ('^' + re.escape(str))
        self.str   = str
        upordown = upordown_gen (str)
    
        lo  = 0
        hi  = self.fsize

        while (1):
            mid = int ((lo + hi) / 2) 
        
            if (mid):
                self.fh.seek (mid - 1, os.SEEK_SET)
                self.fh.readline ()
            else: 
                self.fh.seek (mid, os.SEEK_SET)
                

            start = self.fh.tell ()
            rec   = self.fh.readline ()
            
            if rec == None:
                return

            rec = chomp(rec)

            if (hi == lo):
                self.fh.seek (start, os.SEEK_SET)
                return rec # found record

            if (upordown(rec.split(' ')[0]) < 0):
                lo = mid + 1
            else:
                hi = mid

        
def main () :
    usage = sys.argv[0] + " -s <search_string> -f <cdx_idx>";
     
    parser = OptionParser(usage)
    parser.add_option("-s", "--search", dest="string",
                      help="search string")
    parser.add_option("-f", "--file", dest="filename",
                      help="sorted CDX/IDX file")

    (options, args) = parser.parse_args()
    if len (args) != 0 :
       parser.error("use -h for help")
    if (not (options.string)) :
        parser.error("must provide a search string")
    if (not (options.filename)) :
        parser.error("must provide a sorted CDX/IDX file")

    # create a binary search object on sorted file "options.filename"
    bs = BinSearch (options.filename)

    # search for a string "options.string"
    bs.search (options.string)
    
    # use function generator (lazy) "next" to get the next line that match
    for line in bs.next():
        print line
    
if __name__ == "__main__":
    main()
