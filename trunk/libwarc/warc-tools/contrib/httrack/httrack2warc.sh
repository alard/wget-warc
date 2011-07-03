#!/usr/bin/env bash

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

set -e

prefix=""
maxsize=629145600
warcnum=1
urlprefix="http://"
compressed=""
tempdir="."
ip="0.0.0.0"

file2warc="../app/python/file2warc.py"


isDigit () {
   nodigits="$(echo $1 | sed 's/[[:digit:]]//g')"
}

getFileSize () {
    warcsize=$(stat $1 | awk '{print $8}')
}

getDate () {
    indate=$(perl -nle 'print $1 if /HTTrack Website Copier.*(.{3},.*) --\>$/' $1 | head -1)

    if [ -z "$indate" ]; then
        indate=$(perl -MPOSIX -e 'print strftime "%Y-%m-%dT%H:%M:%SZ", localtime((stat($ARGV[0])) [10]);' $1)
    else
        indate=$(perl -e '%abbr = ("Jan" => "01", "Feb" => "02", "Mar" => "03", "Apr" => "04", "May" => "04", "Jun" => "06", "Jul" => "07", "Aug" => "08", "Sep" => "09", "Oct" => "10", "Nov" => "11", "Dec" => "12"); $_=$ARGV[0]; s/^.{3}, (\d{1,2}) (.{3}) (\d{4}) (\d{2}):(\d{2}):(\d{2}).*/$3-$abbr{$2}-$1T$4:$5:$6Z/; print;' "$indate")
    fi
}

getMimeType () {
    inmime=$(perl -nle 'print $1 if /Added by HTTrack.*content=\"([^;]*)/' $1 | head -1)
    if [ -z "$inmime" ]; then
        inmime=$(file -b --mime $1 | sed -e "s|;.*||")
    fi
}

getURL () {
    inurl=$(perl -nle 'print $1 if /Mirrored from (.*) by HTTrack/' $1 | head -1)
    if [ -z "$inurl" ]; then
        inurl="$urlprefix$1"
    fi
}


usage () {
    echo "" >&2
    echo "Convert an HTTrack collection to WARC" >&2
    echo "Usage: $0 <-s stage_id> [-f files_list] [-d httrack_dir]"  >&2
    echo "          [-p prefix] [-r filter] [-i ip_address] [-c config] [-m max_size] [-t temp_dir] [-h] [-g]"  >&2
    echo "Where:" >&2
    echo "       stage_id     : Stage number"  >&2
    echo "       files_list   : List of documents to convert"  >&2    
    echo "       httrack_dir  : Valid HTTrack directory name"  >&2
    echo "       prefix       : Prefix string to add to each WARC file"  >&2
    echo "       ip_address   : Set IP address for record (defaut 0.0.0.0)"  >&2
    echo "       filter       : Regular expression to strip out unwanted document"  >&2
    echo "       config       : Configuration file strategy for WARC"  >&2
    echo "       max_size     : Maximum size for a WARC file in bytes (default 600Mb)"  >&2
    echo "       temp_dir     : Working directory for the script (default \".\")"  >&2
    echo "       -g           : crate gzip compressed warc files (default \"No\")"  >&2
    echo "       help         : Dump this help message"  >&2
    echo "Example:"  >&2
    echo "       $0 -s 1 -d /tmp/httrack_dir > foo.txt"  >&2
    echo "       $0 -s 2 -f foo.txt -r \".*~\" > bar.txt"  >&2
    echo "       $0 -s 3 -f bar.txt -p \"hanzo-\" -m 1073741824"  >&2
    echo "       $0 -s 3 -f bar.txt -p \"bnf-\" -c \"config.ini\""  >&2
    echo "       $0 -s 3 -f bar.txt -p \"bl-\" -i \"127.0.0.1\""  >&2
    exit 1
}

if [ $# -eq 0 ]; then
    usage
fi

while getopts s:d:f:r:p:i:c:m:t:hg o
do
  case "$o" in
    s)   stage=$OPTARG ;;
    d)   httrack=$OPTARG ;;
    f)   list=$OPTARG ;;
    r)   filter=$OPTARG ;;
    i)   ip=$OPTARG ;;
    p)   prefix=$OPTARG ;;
    c)   config=$OPTARG ;;
    m)   maxsize=$OPTARG ;;
    t)   tempdir=$OPTARG;;
    h)   usage ;;
    g)   compressed="-c";;
    *)   usage ;;
  esac
done

if [ -z "$stage" ]; then
    echo "> -s $stage option is mandatory" >&2
    usage
fi

isDigit $stage
if [ ! -z "$nodigits" ] ; then
    echo "> -s $stage must be a valid number. Only digits, no commas, spaces, etc." >&2
    usage
fi

if [ $stage -lt 1 -o $stage -gt 3 ]; then
    echo "> -s $stage needs to be in [1, 3]" >&2
    usage
fi

# stage = 1
if [ $stage -eq 1 ]; then
    if [ -z "$httrack" ]; then
        echo "> -d $httrack option is mandatory" >&2
        usage
    fi
    
    if [ ! -d "$httrack" ]; then
        echo "> -d $httrack must be a valid directory" >&2
        usage
    fi
    
    # find all files in $httrack directory
    find "$httrack" -xdev -type "f" -print
fi

# stage = 2
if [ $stage -eq 2 ]; then
    if [ -z "$list" ]; then
        echo "> -f $list option is mandatory" >&2
        usage
    fi
    
    if [ ! -f "$list" ]; then
        echo "> -f $list must be a valid filename" >&2
        usage
    fi
    
    if [ ! -z "$filter" ]; then
         # strip out unwanted file names from conversion
        perl -nle "print if !/$filter/" $list
    fi
fi

# stage = 3
if [ $stage -eq 3 ]; then
    if [ -z "$list" ]; then
        echo "> -f $list option is mandatory" >&2
        usage
    fi
    
    if [ ! -f "$list" ]; then
        echo "> -f $list must be a valid filename" >&2
         usage
    fi
    
    if [ ! -z "$config" ]; then
        if [ ! -f "$config" ]; then
            echo "> -c $config must be a valid filename" >&2
            usage
        fi
    fi
    
    isDigit $maxsize
    if [ ! -z "$nodigits" ] ; then
        echo "> -m $maxsize must be a valid number. Only digits, no commas, spaces, etc." >&2
        usage
    fi

    if [ ! -d "$tempdir" ]; then
        echo "> -t $tmpdir must be a valid directory" >&2
        usage
    fi
    
    # construct the WARC file name
    if [ -z $compressed ] ; then
        warcname="$prefix$warcnum.warc"
    else 
        warcname="$prefix$warcnum.warc.gz"
    fi

    echo "* creating a new WARC file: $warcname" >&2

    # loop overall file names in $list
    while read r
    do
       if [ -f $warcname ]; then
           # get file size. Result will be in $warcsize
           getFileSize $warcname

           if [ $warcsize -ge $maxsize ]; then
               # open a new WARC file and increment its number
               let warcnum=warcnum+1
               warcname="$prefix$warcnum.warc"
               echo "* creating a new WARC file: $warcname" >&2
           fi
       fi

       # if the file really exist on disk, convert it
       if [ ! -f "$r" ]; then
           echo "- skipping file $r" >&2
           continue
       fi

       echo "+ converting: $r" >&2

       # get MIME type. Result will be stored in variable $inmime
       getMimeType $r

       # get URL name. Result will be stored in variable $inurl
       getURL $r

       # get URL name. Result will be stored in variable $indate
       getDate $r

       #./file2warc.py $inurl $inmime $indate $ip $maxsize ...
       python $file2warc -f "$r" -w "$warcname" -u "$inurl" -m "$inmime" -d "$indate" -i "$ip" -x "$maxsize" -t "$tempdir" $compressed >&2

    done < "$list"

    echo "" >&2
    echo "conversion done." >&2

fi

exit 0

