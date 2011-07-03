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

#
# Script to be hacked ;-)
#

set -e

# set path to include "file2warc.py" and "youtube-dl"
export PATH=.:../../app/python:$PATH


###############################
# default settings for YouTube 
###############################

TMP=`mktemp -t youtube2warc.sh.XXXXXX`
trap "rm $TMP* 2>/dev/null" EXIT

#mime="video/x-flv"
mime="flv-application/octet-stream"

# YouTube IP address may be different than this one
ipaddr="208.65.153.238"

# video bit rate (default is low)
bitrate=""

checkFile () {
    if [ -z $1 ]; then
        echo "> -$2 filename option is mandatory" >&2
        usage
    fi
}

isFile () {
    if [ ! -f $1 ]; then
        echo "> $1 file doesn't exist" >&2
        usage
    fi
}

usage () {
    echo "" >&2
    echo "Download videos from YouTube to WARC containers" >&2
    echo "Usage: $0 <-f filename> -o <warc_file> [-b] [-h]"  >&2
    echo "       -f : Valid filename containing YouTube URLs"  >&2
    echo "       -o : Store all videos on this WARC file"  >&2
    echo "       -b : Grab videos with best quality"  >&2
    echo "       -h : Dump help message"  >&2
    echo "Example:"  >&2
    echo "       $0 -f urls.txt -o foo.warc.gz"  >&2
    echo "       $0 -f urls.txt -o foo.warc.gz -b" >&2
    exit 1
}


# lecture des arguments de la ligne de commande
while getopts f:o:bh o
do
  case "$o" in
    f)   filename=$OPTARG ;;
    o)   warc=$OPTARG ;;
    b)   biterate="-b" ;;
    *)   usage ;;
  esac
done


checkFile "$filename" "f" && isFile "$filename"
checkFile "$warc" "o"

while read url
do
    if [ -z $url ]; then
        continue
    fi

    # flush the temporary file
    cat /dev/null > $TMP

    # fetch the video from YouTube
    echo "+ Grab YouTube video :  $url"  >&2
    youtube-dl $biterate -o $TMP $url

    # store the file in the specified WARC
    echo "+ Store YouTube video : $url"  >&2
    file2warc.py -f $TMP -w $warc -u $url -m $mime -i $ipaddr -d `date +"%Y-%m-%dT%H:%M:%SZ"` -c


done < $filename

exit 0



