#---------------------------------------------------------------
#Copyright (c) 2005-2008 Hanzo Archives Limited. All rights reserved.
#---------------------------------------------------------------
import re
import urllib

base_pattern = re.compile("^(.+)(?:(?:(?:jsessionid)|(?:phpsessid))=[0-9a-zA-Z]{32})(?:(.+))?$" , re.I)
sid_pattern = re.compile( "^(.+)(?:sid=[0-9a-zA-Z]{32})(&.*)?$"  , re.I)
aspsession_pattern = re.compile( "^(.+)(?:ASPSESSIONID[a-zA-Z]{8}=[a-zA-Z]{24})(&.*)?$"  , re.I)
urlr = re.compile("^(([^:/?#]+):)?((//([^/?#]*))?([^?#]*)(\\?([^#]*))?)?(#(.*))?")
httpschemer = re.compile( "^(https?://)/+(.*)")
userinfo_pattern = re.compile( "^((?:(?:https?)|(?:ftps?))://)(?:[^/]+@)(.*)$" , re.I )
www_pattern= re.compile( "(?i)^(https?://)(?:www\\.)([^/]*/.+)$" )



keys = { "scheme":1 , "authority_path": 2, "_authority":3 , 
             "authority":4 , "path":5 , "_query":6 , "query":7 , "_fragment":8 , "fragment":9 }

def FixUpQueryString( url ):
    idx = url.rfind( "?" )
    if idx > 0:
        if url.endswith( "?"):
            url = url[:-1]
        elif url[idx+1] == '&':
            if url.endswith( "?&" ):
                url = url[:-2]
            else:
                url = url[:idx + 1] + url[idx + 2:]
        if url.endswith('&'):
            url = url[:-1]
    return url            

def  MakeLowercase( url ):
    return url.lower()
    
def ReRule( url , regex ):
    m = regex.match( url )
    if m is not None:
        url = "".join( [ x for x in m.groups() if x is not None] )
    return url        
    
sessionrules = [ base_pattern , sid_pattern , aspsession_pattern]    
    
def DropSession( url ):
    for rule in sessionrules:
        url = ReRule( url , rule  )
    return url       
   
def StripUserInfo( url ):
    return ReRule( url , userinfo_pattern )
    
def StripWWW( url ):
    return ReRule( url , www_pattern )

canonicalize_rules = [ MakeLowercase , DropSession  , StripUserInfo , StripWWW , FixUpQueryString   ] 

def canonicalize( url ):
    for rule in canonicalize_rules:
        url = rule( url )
    return url        
       
                 
def geturlparts( url ):
    m = urlr.match( url )
    if m:
        ret = {}
        for k in keys:
            ret[ k ] = m.groups()[ keys[ k ]] 
        return ret
    else:
        return {}             
    
NBSP = "\u00A0"    
    
def patchup( url ):
    if not ( url.startswith('http://') or url.startswith('https://')):
        url = 'http://' + url
    if url.find( NBSP ) != -1:
        url = url.replace( NBSP , " " )
    url = url.strip()        
    if url.find( '\\' ) != -1:
        url = url.replace( '\\' , '/' )    
    if url.find( '\n' ) != -1:
        url = url.replace( '\n' , '/' )         
    if url.find( '\r' ) != -1:
        url = url.replace( '\r' , '/' )        
    m = httpschemer.match( url )
    if m is not None and len( m.groups() ) >= 2:
        url = m.groups()[0] + m.groups()[1]
    return url        
                         
class URLParts( object ):
    def __init__(self , url ):
        url = patchup( url )
        self.parts = geturlparts( url )
        self.parts['url'] = url
        self._postpatchup()        
    def _postpatchup(self):
        if self.path == "":
            self.parts["path"] = '/'
            self.parts['authority_path'] += '/'
        if  self.scheme == 'https':
            self.parts['keypath'] = 'https:' + self.authority_path.replace(':80/' , '/' )
        elif self.authority_path.startswith('//www') and self.authority_path.find('.') != -1:
            self.parts['keypath'] = self.authority_path[ self.authority_path.index('.') + 1: ].replace(':80/' , '/' )
        else:
            self.parts['keypath'] = self.authority_path[ 2: ].replace(':80/' , '/' )
        # normalisation fixes                        
        self.parts['keypath'] = self.parts['keypath'].replace( ' ' , '%20')    
        self.parts['keypath'] = self.parts['keypath'].replace( '&amp;' , '&')  
        self.parts['keypath'] = self.parts['keypath'].replace( '[' , '%5B')  
        self.parts['keypath'] = self.parts['keypath'].replace( ']' , '%5D')  
        self.parts['keypath_not_canonical'] = self.parts['keypath']
        self.parts['keypath'] = canonicalize( self.parts['keypath'] )
    def __getattr__(self , name ):
        if name == 'parts':
            return self.parts
        if len( self.parts ) and self.parts.has_key( name ):
            return self.parts[ name ]
        else:
            return None
    def __repr__(self):
        ret = ""
        for k in self.parts:
            ret += "%s: %s\n" % ( k , self.parts[k])
        return ret            


def keypath( url ):
    bits = URLParts( url )
    return bits.keypath