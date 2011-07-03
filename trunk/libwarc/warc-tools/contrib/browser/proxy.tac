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

from twisted.application import service , internet 
from twisted.python import log
from twisted.internet import reactor, protocol
from twisted.web import resource, server, http
import sys
import re
#log.startLogging(sys.stdout)
# system imports
import urlparse
import datetime

basehost = "127.0.0.1:8080"
base = "http://127.0.0.1:8080/archive"
indexbase = "http://127.0.0.1:8080/archive"
idx = 1

# rules
# 1) if of the form base + timestamp + url then just do it 
# 2) if of the form url - then look to see if it is via type (1) if so nick the timestamp and 
#       do base + timestamp + url 
# 3) if of the form url and no via (or wrong via) then do base + * + url
# if not 200 - 399 then log as missing 
# 4) always let google through 


class SessionManager( object ):
    def __init__(self ):
        self.sessions = {}
    def session( self , ip , useragent ):
        key = ip + useragent 
        if not self.sessions.has_key( key ):
            self.sessions[ key ] = {}
        return self.sessions[ key ]            

sm = SessionManager()

rawr = re.compile( r'^/archive/(?P<timestamp>[0-9]{14})/(?P<url>.*)$' )
normr = re.compile( r'/archive/(?P<timestamp>[0-9]{14})/(?P<url>.*)$' )

def sendto( pr , timestamp , url ):
    pr.channel.transport.write(
                                        """HTTP/1.1 302 Found
Location: %s/%s/%s

Folding Back""" % ( base , timestamp , url  ))
    pr.channel.transport.loseConnection()
    print '302'
    return      
    
def indexof( pr , timestamp , url ):
    pr.channel.transport.write(
                                        """HTTP/1.1 302 Found
Location: %s/%s/%s

Folding Back""" % ( indexbase , timestamp , url  ))
    pr.channel.transport.loseConnection()
    print '302'
    return      
    
class ProxyClient(http.HTTPClient):
    """Used by ProxyClientFactory to implement a simple web proxy."""

    def __init__(self, command, rest, version, headers, data, father , injectheaders ):
        self.father = father
        self.command = command
        self.rest = rest
        if headers.has_key("proxy-connection"):
            del headers["proxy-connection"]
        headers["connection"] = "close"
        self.headers = headers
        self.injectheaders = injectheaders
        self.data = data

    def connectionMade(self):
        self.sendCommand(self.command, self.rest)
        for header, value in self.headers.items():
            self.sendHeader(header, value)
            #print 'Sending',header,value
        self.endHeaders()
        self.transport.write(self.data)

    def handleStatus(self, version, code, message):
        self.father.transport.write("%s %s %s\r\n" % (version, code, message))

    def handleHeader(self, key, value):
        #print 'Return header',key,value
        self.father.transport.write("%s: %s\r\n" % (key, value))

    def handleEndHeaders(self):
        for header in self.injectheaders:
            self.handleHeader( header , self.injectheaders[ header ] )
        self.father.transport.write("\r\n")
    
    def handleResponsePart(self, buffer):
        self.father.transport.write(buffer)

    def handleResponseEnd(self):
        self.transport.loseConnection()
        self.father.channel.transport.loseConnection()


class ProxyClientFactory(protocol.ClientFactory):
    """Used by ProxyRequest to implement a simple web proxy."""

    protocol = ProxyClient

    def __init__(self, command, rest, version, headers, data, father , injectheaders ):
        self.father = father
        self.command = command
        self.rest = rest
        self.headers = headers
        self.data = data
        self.version = version
        self.injectheaders = injectheaders


    def buildProtocol(self, addr):
        return self.protocol(self.command, self.rest, self.version,
                             self.headers, self.data, self.father , self.injectheaders )


    def clientConnectionFailed(self, connector, reason):
        self.father.transport.write("HTTP/1.0 501 Gateway error\r\n")
        self.father.transport.write("Content-Type: text/html\r\n")
        self.father.transport.write("\r\n")
        self.father.transport.write('''<H1>Could not connect</H1>''')
    

class ProxyRequest(server.Request):
    """Used by Proxy to implement a simple web proxy."""

    protocols = {'http': ProxyClientFactory}
    ports = {'http': 80}
    #sitepath = ["EAPROXY"]
    #site = server.Site(None)
    def process(self):    
        parsed = urlparse.urlparse(self.uri)
        protocol = parsed[0]
        host = parsed[1]
        port = self.ports[protocol]
        if ':' in host:
            host, port = host.split(':')
            port = int(port)
        rest = urlparse.urlunparse(('','')+parsed[2:])
        print host , rest
        headers = self.getAllHeaders().copy()     
        if headers.has_key( 'user-agent' ):
            ua = headers['user-agent']
        else:
            ua = 'unknown'   
        sesh = sm.session( self.getClientIP() , ua )
        if sesh.has_key( 'timestamp' ):        
            timestamp = sesh['timestamp']
        else:
            timestamp = '*'
        if  'googlez' not in host and 'hedev.hanzoarchives' not in host:
            if host != basehost :
                if headers.has_key( 'referer' ):
                    frmall = headers['referer']
                    parsed = urlparse.urlparse( frmall )
                    rest = urlparse.urlunparse(('','')+parsed[2:])
                    mob = rawr.match( rest )
                    print 'From',rest                    
                    if mob :
                        return sendto( self , mob.group('timestamp') , self.uri )                        
                return indexof( self , timestamp , self.uri )       
            elif not rawr.match( rest ) :
                print rest
                mob = normr.match( rest )
                if mob:
                    if mob.group('timestamp').strip() != '*':
                        return sendto( self , mob.group('timestamp') , mob.group( 'url' ) )                         
                    else:
                        rest = '/tna/*' + mob.group( 'url' )                        
        if not rest:
            rest = rest+'/'
        class_ = self.protocols[protocol]
        #print class_
        mob = normr.match( rest )
        injectheaders = {}
        if mob:
            print '<----' , mob.group('timestamp')
            sesh['timestamp'] = mob.group('timestamp')
        else:
            print '*' * 20
        if not headers.has_key('host'):
            headers['host'] = host
        #print "Refered by",headers['referer']            
        self.content.seek(0, 0)
        s = self.content.read()
        clientFactory = class_(self.method, rest, self.clientproto, headers,
                               s, self , injectheaders)
        print "Connect to",host,port                               
        reactor.connectTCP(host, port, clientFactory)


class Proxy(http.HTTPChannel):
    requestFactory = ProxyRequest
    

 
class ProxyFactory(http.HTTPFactory):
        protocol = Proxy
 
application = service.Application( 'ArchiveProxy' )
service.IProcess(application).processName = "proxy"
proxyservice = internet.TCPServer( 8087 ,  ProxyFactory() )
proxyservice.setServiceParent( application )
 
