# from activestate python recipes 
#
#  http://code.activestate.com/recipes/466302/
# 
#   Thanks to Nicolas Lehuen
 
from heapq import heapify, heappop, heappush
from itertools import islice, cycle
from tempfile import gettempdir
import os

def merge(chunks,key=None):
    if key is None:
        key = lambda x : x

    values = []

    for index, chunk in enumerate(chunks):
        try:
            iterator = iter(chunk)
            value = iterator.next()
        except StopIteration:
            try:
                chunk.close()
                os.remove(chunk.name)
                chunks.remove(chunk)
            except:
                pass
        else:
            heappush(values,((key(value),index,value,iterator,chunk)))

    while values:
        k, index, value, iterator, chunk = heappop(values)
        yield value
        try:
            value = iterator.next()
        except StopIteration:
            try:
                chunk.close()
                os.remove(chunk.name)
                chunks.remove(chunk)
            except:
                pass
        else:
            heappush(values,(key(value),index,value,iterator,chunk))

def batch_sort(input,output,key=None,buffer_size=32000,tempdirs=[]):
    if not tempdirs:
        tempdirs.append(gettempdir())
    
    input_file = file(input,'rb',64*1024)
    try:
        input_iterator = iter(input_file)
        
        chunks = []
        try:
            for tempdir in cycle(tempdirs):
                current_chunk = list(islice(input_iterator,buffer_size))
                if current_chunk:
                    current_chunk.sort(key=key)
                    output_chunk = file(os.path.join(tempdir,'%06i'%len(chunks)),'w+b',64*1024)
                    output_chunk.writelines(current_chunk)
                    output_chunk.flush()
                    output_chunk.seek(0)
                    chunks.append(output_chunk)
                else:
                    break
        except:
            for chunk in chunks:
                try:
                    chunk.close()
                    os.remove(chunk.name)
                except:
                    pass
            if output_chunk not in chunks:
                try:
                    output_chunk.close()
                    os.remove(output_chunk.name)
                except:
                    pass
            return
    finally:
        input_file.close()
    
    output_file = file(output,'wb',64*1024)
    try:
        output_file.writelines(merge(chunks,key))
    finally:
        for chunk in chunks:
            try:
                chunk.close()
                os.remove(chunk.name)
            except:
                pass
        output_file.close()

