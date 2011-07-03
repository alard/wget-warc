
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

import re

jscripttempB="""<SCRIPT language="Javascript">
<!--

// FILE ARCHIVED ON %s AND RETRIEVED FROM 
// Warc-Browser  
// JAVASCRIPT APPENDED BY Warc Browser
// ALL OTHER CONTENT MAY ALSO BE PROTECTED BY COPYRIGHT
   var archiveurl = "%s";

   function xResolveUrl(url) {
      var image = new Image();
      image.src = url;
      return image.src;
   }
   function xLateUrl(aCollection, sProp) {
      var i = 0;
      for(i = 0; i < aCollection.length; i++) {
         if (typeof(aCollection[i][sProp]) == "string") { 
          if (aCollection[i][sProp].indexOf("mailto:") == -1 &&
             aCollection[i][sProp].indexOf("javascript:") == -1) {
            if(aCollection[i][sProp].indexOf("http") == 0) {
                aCollection[i][sProp] = archiveurl + aCollection[i][sProp];
            } else {
                aCollection[i][sProp] = archiveurl + xResolveUrl(aCollection[i][sProp]);
            }
         }
         }
      }
   }

    // testing 

   xLateUrl(document.getElementsByTagName("IMG"),"src");
   xLateUrl(document.getElementsByTagName("A"),"href");
   xLateUrl(document.getElementsByTagName("AREA"),"href");
   xLateUrl(document.getElementsByTagName("OBJECT"),"codebase");
   xLateUrl(document.getElementsByTagName("OBJECT"),"data");
   xLateUrl(document.getElementsByTagName("APPLET"),"codebase");
   xLateUrl(document.getElementsByTagName("APPLET"),"archive");
   xLateUrl(document.getElementsByTagName("EMBED"),"src");
   xLateUrl(document.getElementsByTagName("BODY"),"background");
   // xLateUrl(document.getElementsByTagName("FRAME"),"src");
   var forms = document.getElementsByTagName("FORM");
   if (forms) {
       var j = 0;
       for (j = 0; j < forms.length; j++) {
              f = forms[j];
              if (typeof(f.action)  == "string") {
                 if(typeof(f.method)  == "string") {
                     if(typeof(f.method) != "post") {
                        f.action = archiveurl + f.action;
                     }
                  }
              }
        }
    }
    
//-->
</SCRIPT>
"""

headr = re.compile( r'<head(.*?)>' , re.I | re.S | re.UNICODE)
htmlr = re.compile( r'<html(.*?)>' , re.I | re.S | re.UNICODE)

def makeAccessible( mimetype ,  url , timestamp , content ):
    if mimetype.startswith( 'text/html' ):
        ( content , n ) = re.subn( headr , 
                r'<head\1>\n<BASE HREF="%s">\n\n' % ( url )
                , content )                             
        if n == 0:                  
            content = re.sub( htmlr , 
                r'<html\1><head>\n<BASE HREF="%s">\n</head>' \
                % ( url ) , content )
        content += jscripttempB % ( timestamp , 'http://127.0.0.1:8080/archive/%s/' % ( timestamp ) )
    return content