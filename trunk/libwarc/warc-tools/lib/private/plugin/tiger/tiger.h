#ifndef	__TIGER_H__
#define	__TIGER_H__

/* to mix C and C++ */
#ifdef __cplusplus
extern "C"
  {
#endif


    /* sboxes.c: Tiger S boxes */

    /*
     * This file was originally called "sboxes.c".
     * Move the "table" array into "tiger.c"
     * change 2008-01-11 by voidptrptr - Hanzo Archives Limited.
     */

    /*
     * Portability header file
     */

#include <wport.h>

    typedef warc_u64_t    word64;
    typedef warc_u32_t    word32;
    typedef warc_u8_t byte;

    extern void tiger (word64 *, word64, word64 res[3]);


    /* to mix C and C++ */
#ifdef __cplusplus
    extern "C"
      {
#endif



#endif /* __TIGER_H__ */
