/*
  Here is two foctions to recover an anvl field key and value from a wWarc Record
  the Anvl field is identified by its rank

*/

#include <warc.h> 


WPUBLIC const warc_u8_t * getFieldKey (void * r, warc_u32_t rank)
{
  const warc_u8_t * key = NIL;
  const warc_u8_t * value = NIL;

  unless (r)
    return (NIL);

  if (WRecord_getAnvlField (r, rank, &key, &value))
    return (NIL);

  return (key);
}

WPUBLIC const warc_u8_t * getFieldValue (void * r, warc_u32_t rank)
{
  const warc_u8_t * key = NIL;
  const warc_u8_t * value = NIL;

  unless (r)
    return (NIL);

  if (WRecord_getAnvlField (r, rank, &key, &value))
    return (NIL);

  return (value);
}
