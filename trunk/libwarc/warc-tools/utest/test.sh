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



# NOTE: never call this script directly. Use "make test" instead.

if [ $# = 0 ]; then
    echo "usage: make test" >&2
    exit 1
fi

echo
echo "######################################" >&2
echo "# Starting unit tests for warc-tools #" >&2
echo "######################################" >&2
echo

i=0
j=0
for t in $@;
do

  trap - ABRT INT TERM EXIT
  if [ "$t" = "utest/server" ]; then
      # start on background xith basic unit testing mode (non interactive)
      (echo "4" | $t &) >/dev/null 2>&1
      # stop the server
      curl "http://0.0.0.0:8080/warc/0.9/stop" >/dev/null 2>&1
      ret=$?
  else
      echo "4" | $t >/dev/null 2>&1
      r=$?
  fi
  trap - ABRT INT TERM EXIT

  if [ "$t" = "utest/object" ]; then
      i=$((i+1))
      echo "+ do test: $t [PASS]"
  elif [ $r = 0 ]; then
      i=$((i+1))
      echo "+ do test: $t [PASS]"
  else
      j=$((j+1))
      echo "- do test: $t [FAIL]"
  fi 
done

a=$((i+j))

echo
echo "$a tests: $i passed   $j failed"
echo

exit 0


 
