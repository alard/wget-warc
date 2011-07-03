from datetime import datetime

def getDatetimeFromTimestamp( timestamp ):
   """convert an Internet Archive timestamp into a datetime object"""
   return datetime.strptime( timestamp , "%Y%m%d%H%M%S" )
   
   
def getTimestampFromDatetime( dt ):
    return  dt.strftime( "%Y%m%d%H%M%S" )
    
def getDisplayTime( timestamp ):
    dt = getDatetimeFromTimestamp( timestamp )
    return  dt.strftime( "%b %d %Y at %H:%M:%S" )

def getTimestampFromWarcDate( wd ):
    return getTimestampFromDatetime (datetime.strptime( wd , '%Y-%m-%dT%H:%M:%SZ') )

def closest( timestamp , timestamplist ):
    d = getDatetimeFromTimestamp( timestamp )    
    tlist = [ getDatetimeFromTimestamp(x) for x in timestamplist ]
    distances = [ abs( d - s ) for s in  tlist]
    return distances.index( min(distances) )