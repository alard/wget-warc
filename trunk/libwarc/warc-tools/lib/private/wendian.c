
/* ------------------------------------------------------------------- */
/* Copyright (c) 2007-2008 Hanzo Archives Limited.                     */
/*                                                                     */
/* Licensed under the Apache License, Version 2.0 (the "License");     */
/* you may not use this file except in compliance with the License.    */
/* You may obtain a copy of the License at                             */
/*                                                                     */
/*     http://www.apache.org/licenses/LICENSE-2.0                      */
/*                                                                     */
/* Unless required by applicable law or agreed to in writing, software */
/* distributed under the License is distributed on an "AS IS" BASIS,   */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or     */
/* implied.                                                            */
/* See the License for the specific language governing permissions and */
/* limitations under the License.                                      */
/*                                                                     */
/* You may find more information about Hanzo Archives at               */
/*                                                                     */
/*     http://www.hanzoarchives.com/                                   */
/*                                                                     */
/* You may find more information about the WARC Tools project at       */
/*                                                                     */
/*     http://code.google.com/p/warc-tools/                            */
/* ------------------------------------------------------------------- */

/*
 * Portability header file
 */

#include <wport.h>


/*
 * WARC default headers
 */


#include <wmisc.h>   /* warc_bool_t ... */

/**
 * @return: return WLITTLE_ENDIAN or WBIG_ENDIAN boolean
 *
 * Identify endieness.
 */

#define WLITTLE_ENDIAN WARC_FALSE
#define WBIG_ENDIAN    WARC_TRUE

warc_bool_t warc_isBigEndian (void)
{
  warc_i32_t  i = 1;
  char      * p = (char *) & i;

  if (p[0] == 1)
    return WLITTLE_ENDIAN;

  return WBIG_ENDIAN;
}


/**
 * @return return warc_u32_t
 *
 * Write warc_i32_t based endianness
 */

warc_u32_t warc_i32ToEndian (warc_u32_t i)
{
  warc_u8_t c1, c2, c3, c4;

  unless (warc_isBigEndian () )
  {
    c1 = i & 255;
    c2 = (i >> 8) & 255;
    c3 = (i >> 16) & 255;
    c4 = (i >> 24) & 255;

    i = ( (warc_i32_t) c1 << 24) + ( (warc_i32_t) c2 << 16) +
        ( (warc_i32_t) c3 << 8)  + (warc_i32_t) c4;
  }

  return (i);
}
