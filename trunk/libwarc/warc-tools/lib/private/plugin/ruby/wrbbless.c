/* 
  Here, there is a specialized bless functin for warc-tools library classes
  It is for Ruby wrapper use purpuse, only the class used in this warpper may have
  a specialized blmess function here
 */


#include <warc.h>
#include <wbloc.h>

#include <wrbbless.h>


WPUBLIC void * bless_WFile (const char  * fname, warc_u64_t  max_size,
                            wfile_mode_t mode, wfile_comp_t compressed,
                            const char * dname )
{
  return (bless (WFile, fname, (warc_u64_t) max_size, mode, compressed, dname));
}

WPUBLIC void * bless_WRecord (void)
{
  return (bless (WRecord));
}

WPUBLIC void * bless_WBloc (void  * wfile, void  * wrecord, 
                            warc_bool_t httphead, const warc_u32_t alloc)
{
  return (bless (WBloc, wfile, wrecord, httphead, alloc));
}
