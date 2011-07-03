#include <stdio.h>
#include <string.h>
#include <warc.h>

#define makeS(s) ((warc_u8_t *) s), w_strlen((warc_u8_t *) (s))
#define makeU(s) (const warc_u8_t *) (s), (warc_u64_t) w_strlen((warc_u8_t *) (s))

warc_bool_t callback (void * env, const char* buff, const warc_u32_t size)
{
  UNUSED (env);
  UNUSED (size);
  UNUSED (buff);
  return (WARC_TRUE);
}

void * bless_Wfile (char * path, warc_u64_t dim, 
                    int mode, int compressed, char * dname)
{	
	return bless (WFile, path, dim, mode, compressed, dname);
}

int isValid ( void * const w)
{
  void           * r       = NIL;  /* to recover records */
  warc_u32_t       ret     = 0;

  while (WFile_hasMoreRecords (w) )
    {
      r = WFile_nextRecord (w);
      unless (r)
      {
        ret = 1;
        break;
      }

      WFile_register (w, r, callback, (void *) 0);
      WRecord_getContent (r);
      
      destroy (r);
    }

  destroy (w);

  return (ret);
}

void * bless_Wrecord ()
{	
  return bless (WRecord);
}
