#! /usr/bin/env python

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

import sys, time, sha, os, re, socket
from urlparse import urlparse
import pycurl

sys.path.insert (0, "/usr/local/warc-tools/python/")
sys.path.insert (0, "../../lib/private/plugin/python/")

import warc
from   wfile   import WFile
from   wrecord import WRecord 

from optparse import OptionParser

# default values
tmpfile     = ".tmpfile"
tmpdir      = "."
debug       = True
p           = re.compile( ';.*$')



# ignore SIGPIPE when using pycurl.NOSIGNAL
try:
    import signal
    from signal import SIGPIPE, SIG_IGN
    signal.signal(signal.SIGPIPE, signal.SIG_IGN)
except ImportError:
    pass

## Callback function invoked when header data is ready
def header(buf):
    if buf == None:
        fp.write ( '\n\r\r\n' )
        return
        
    fp.write (buf)

def getIpAddress(sd, url):
    ip = "0.0.0.0"
    if sd != -1:
        sock      = socket.fromfd(sd, socket.AF_INET, socket.SOCK_STREAM)
        (ip,port) = sock.getpeername()
        sock.close()
    else:
        try:
            ip = socket.gethostbyname_ex( urlparse(url)[1] )[2][0]
        except:
            ip = "0.0.0.0"
            
    return ip
        
def fetcher(url):
    global fp
    fp   = open(tmpfile, "wb")
    curl = pycurl.Curl()
    curl.setopt(pycurl.URL, url)
    curl.setopt(pycurl.FOLLOWLOCATION, 1)
    curl.setopt(pycurl.MAXREDIRS, 5)
    curl.setopt(pycurl.CONNECTTIMEOUT, 30)
    curl.setopt(pycurl.TIMEOUT, 300)
    curl.setopt(pycurl.NOSIGNAL, 1)
    curl.setopt(pycurl.WRITEDATA, fp)
    curl.setopt(pycurl.HEADERFUNCTION, header)
    
    try:
        curl.perform()
    except:
        import traceback
        traceback.print_exc(file=sys.stderr)
        sys.stderr.flush()

    date = time.strftime ("%Y-%m-%dT%H:%M:%SZ", time.localtime())
            
    mime = p.sub( '', curl.getinfo(pycurl.CONTENT_TYPE))
    if mime == None :
        mime = 'octet/stream'

    url2      = curl.getinfo(pycurl.EFFECTIVE_URL)
    status    = curl.getinfo(pycurl.HTTP_CODE)
    redirect  = curl.getinfo(pycurl.REDIRECT_COUNT)

    # for pyCurl (compatible with libcurl 7.18.2)
    #sd        = curl.getinfo(pycurl.LASTSOCKET);
    #ipaddr    = getIpAddress(sd, url)

    # for puCurl from CVS (compatible with libcurl 7.19.x or above)
    ipaddr      = curl.getinfo(pycurl.PRIMARY_IP);
    
    curl.close()
    fp.close()
    
    return url2, mime, date, ipaddr, status, redirect




def logger (log) :
    if debug == True:
        sys.stderr.write(log)
        sys.stderr.flush()


def addToWarc (w, uri, mime, date, ip) :
##  don't forget to check return values of each functions  ##
	r = WRecord()

    # change the record type if you want
	r . setRecordType(warc.WARC_RESOURCE_RECORD)
	r . setTargetUri(uri, len(uri))	
	r . setDate(date, len(date))
	r . setContentType(mime, len(mime))
    # use your "unique identifier" function here
	s = time.strftime ("%a, %Y-%m-%dT%H:%M:%SZ", time.localtime())
	sh  = sha.new(uri + s)
	rid = sh.hexdigest()
	rid = "uuid:" + rid
	r . setRecordId(rid, len(rid))
	r . setIpAddress(ip, len(ip))
	r . setContentFromFileName(tmpfile)

	w . storeRecord(r)
	r . destroy()


def main () :
    global debug
    global tmpdir
    global tmpfile
    
    usage = " Download list of URLs and store them in WARC container \n\n"\
            + sys.argv[0] + " -f <urls_list> -o <warc_file> [-s <tmpfile>] [-t <tmpdir>] [-q]"
    
    parser = OptionParser(usage)
    parser.add_option("-f", "--file", dest="input", type="string",
                      help="file name containing a list of URLs")
    parser.add_option("-o", "--output", dest="out", type="string",
                      help="WARC output file name to store documents")
    parser.add_option("-s", "--tmpfile", dest="tmpfile", type="string",
                      help="temporary file (default \".tmpfile\")")
    parser.add_option("-t", "--tmpdir", dest="tmpdir", type="string",
                      help="temporary directory (default \".\")")
    parser.add_option("-q", "--quiet", dest="quiet", action="store_true",
                      help="quiet, no debug")
    
    (options, args) = parser.parse_args()

    if (options.tmpfile):
        tmpfile = options.tmpfile

    if (options.quiet):
            debug = False    

    if (options.input):
        urls = open(options.input).readlines()
    else:
        urls = sys.stdin.readlines()

    if (options.tmpdir):
        tmpdir = options.tmpdir

    if (options.out == None):
            logger("-o <warc_file> option is mandatory\n")
            sys.exit(1)

    # open the WARC file for writing
    cmode   = warc.WARC_FILE_COMPRESSED_GZIP
    maxsize = 600 * 1024 *1024
    w       = WFile(options.out, maxsize, warc.WARC_FILE_WRITER, cmode, tmpdir)
    
    # Make a queue with (url, filename) tuples
    for url in urls:
        url = url.strip()
        if not url or url[0] == "#":
            continue

        # fetch the RL
        effective_url, mime, date, ip, status, redirect = fetcher(url)

        addToWarc (w, effective_url, mime, date, ip)
        logger("%-30s %-3d  %-3d %-30s\n" % (url, status,
                                             redirect, effective_url))

        os.remove(tmpfile)
        
    w . destroy()
            

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print "For help, try: " + sys.argv[0] + " -h"
        sys.exit(1)
    
    main()
