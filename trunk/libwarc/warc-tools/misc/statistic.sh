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

if [ $# != 1 ]; then
    echo "Line of code statistics"
    echo "Usage:   $0 dirname" >&2
    echo "Example: $0 ." >&2
    exit 1
fi

if [ ! -d $1 ]; then
    echo "dirname '$1' must be a valid directory name"
    exit 1
fi

ccount="c_count"
if ! type "$ccount" &>/dev/null; then
    echo ">> \"$ccount\" command is missing" >&2
    echo "Install it from: http://www.chris-lott.org/resources/cmetrics/"
    exit 1
fi

cf=`find $1 -name "*.c" -o -name "*.h" -o -name "*.x" | grep -v "plugin/" | grep -v "app/wdata" | grep -v "doc" | grep -v "license" | wc -l`

echo " ------------------------------------------------"
echo "|                  SMARTLIB                      |"
echo "| C SOURCE STATEMENTS, AND SUMMARIZES STATISTICS |"
echo " ------------------------------------------------"
echo
echo " $cf SOURCE FILES"
echo
echo
echo " LINES    STATEMENTS          FILENAME"
echo

$ccount `find $1 -name "*.c" -o -name "*.h" -o -name "*.x" | grep -v "plugin/" | sed -e "s|./|  |"`
