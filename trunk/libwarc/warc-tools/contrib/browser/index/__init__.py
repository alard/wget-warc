import warcindex 
import look
import timetools

def column( s , idx ):
    return s.strip().split(' ')[idx]

class Index( object ):
    def __init__(self , location , recurse = False ):
        self.location = location 
        self.recurse = recurse 
        self.indexn = warcindex.makeindex( location , recurse )
    def search( self  , url ):
        bs = look.BinSearch ( self.indexn )
        bs.search (url + ' ' )
        return  [ x for x in bs.next()]
    def get( self , timestamp , url ):
        candidates = self.search( url )
        if len(candidates) > 0:
            idx = timetools.closest( timestamp , [  column( x , 1 )  for x in candidates ] )    
            return candidates[idx]
        else:   
            return None