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

usage () {
    echo >&2
    echo "Convert all ARC files in a directory to WARC files" >&2
    echo "Usage: $0 <-d dirname> [-b] [-c] [-t <working_dir>] [-v] [-h]"  >&2
    echo "       -d     : directory name containing ARC files"  >&2
    echo "       -c     : WARC files will be GZIP compressed (default no)"  >&2
    echo "       -t     : temporary working directory (default \".\")"  >&2
    echo "       -h     : print this help message"  >&2
    echo "       -v     : output version information and exit"  >&2
    exit 1
}

# default settings
version="0.18"

while getopts hvcd:t: o
do
  case "$o" in
    d)   dn=$OPTARG ;;
    t)   wdir=$OPTARG ;;
    c)   ccomp="-c" ;;
    h)   usage ;;
    v)   echo "$0 v$version"; exit 0 ;;
    *)   usage ;;
  esac
done

if [ -z "$dn" ]; then
    echo ">> you must supply \"-d\" option" >&2
    usage
fi

if [ ! -d "$dn" ]; then
    echo ">> directory \"$dn\" doesn't exist" >&2
    usage
fi

if [ `touch $dn/$$ 2>/dev/null; echo "$?"` -eq 0 ]; then
    rm -f $dn/$$
else
    echo ">> directory \"$dn\" isn't writable" >&2
    usage
fi 

if [ -z "$wdir" ]; then
    wdir="."
fi

orig_dir=$(pwd)
cd ${0%/*}/..
a2w="`pwd`/app/arc2warc"

if ! type $a2w &>/dev/null; then
    echo ">> \"$a2w\" doesn't exist" >&2
    usage
fi

dn=`echo $dn | sed -e "s|/*$||"`
for i in `find $dn/ -name "*.arc*" -type "f"`;
do
  dn=`dirname $i`
  bn=`basename $i`
  
if [ "$ccomp" = "-c" ]; then
  wf=`echo "$bn" | sed -e "s|\(.*\)\.arc.*$|\1.warc\.gz|"`

else
  wf=`echo "$bn" | sed -e "s|\(.*\)\.arc.*$|\1.warc|"`
fi

  wf="$dn/$wf"

  echo "converting $i -> $wf"

  if [ -f "$wf" ]; then
      echo ">> \"$wf\" already exist, skip it" >&2
      continue
  fi

  $a2w -a $i -f $wf $ccomp -t $wdir &>/dev/null
  if [ $? -ne 0 ]; then
      echo ">> error when converting \"$i\"" >&2
      #exit 2
  fi
done 

exit 0
