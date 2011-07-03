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


set -u

usage () {
    echo >&2
    echo "Convert all ARC files in a directory to WARC files" >&2
    echo "Usage: $0 <-d dirname> [-c] [-h]"  >&2
    echo "       -d     : directory name containing ARC files"  >&2
    echo "       -c     : WARC files will be GZIP compressed (default no)"  >&2
    echo "       -h     : print this help message"  >&2
    exit 1
}

dn=""
ccomp=""
while getopts hcd: o
do
  case "$o" in
    d)   dn=$OPTARG ;;
    c)   ccomp="-c" ;;
    *)   usage ;;
  esac
done

if [ ! -d "$dn" ]; then
    echo "must specify a valid ARC directory: $dn" >&2
    usage
fi

TMP=`mktemp -t arc2warc.sh.XXXXXX`
trap "rm -f $TMP 2>/dev/null" INT TERM EXIT
of=$TMP

find $dn -name "*.arc.gz" -type "f" > $of
lines=`wc -l $of | awk '{print $1}'`
cnt=1

while read i;
do
   if [ ! -f $i ];
   then
      continue
   fi
 
   echo -n "[$cnt/$lines] $i ... "

   (./arc2warc.py $ccomp $i &>$i.err)
   err=`cat $i.err`
   if [ -z "$err" ]; 
   then
      rm -f $i.err
      echo "OK"
   else
      echo "ERROR"
   fi

   let "cnt=cnt+1"
done < $of

exit 0
