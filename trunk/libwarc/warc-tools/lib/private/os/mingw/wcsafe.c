
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
 * C default headers
 */

#include <stddef.h>
#include <ctype.h>


#include <wmisc.h>
#include <wcsafe.h>


WPUBLIC ptrdiff_t w_strncpy (warc_u8_t * dst0,
                             const warc_u8_t * src0, size_t size)
{
#define INIT_BITS	3
#define INIT_SIZE	(1UL << (INIT_BITS))	/* must be power of two	*/
#define INIT_MASK	((INIT_SIZE) - 1)

  if (size)
    {
      register size_t          n   = (size + INIT_MASK) / INIT_SIZE;
      register warc_u8_t * dst = dst0;
      register warc_u8_t * src = (warc_u8_t *) src0;

      * (dst + size) = '\0';

      switch (size & INIT_MASK)
        {

          case 0:

            do
              {
                if (! (*dst++ = *src++) ) break;

              case 7:
                if (! (*dst++ = *src++) ) break;

              case 6:
                if (! (*dst++ = *src++) ) break;

              case 5:
                if (! (*dst++ = *src++) ) break;

              case 4:
                if (! (*dst++ = *src++) ) break;

              case 3:
                if (! (*dst++ = *src++) ) break;

              case 2:
                if (! (*dst++ = *src++) ) break;

              case 1:
                if (! (*dst++ = *src++) ) break;
              }

            while (--n > 0);
        }

      return ( (ptrdiff_t) (dst - dst0 - 1) );
    }

  return (0);
}




/*
 * My personal strstr() implementation that beats most other algorithms.
 * Until someone tells me otherwise, I assume that this is the
 * fastest implementation of strstr() in C.
 * I deliberately chose not to comment it.  You should have at least
 * as much fun trying to understand it, as I had to write it :-).
 *
 * Stephen R. van den Berg, berg@pool.informatik.rwth-aachen.de	*/

/* Faster looping by precalculating bl, bu, cl, cu before looping.
 * 2004 Apr 08	Jose Da Silva, digital@joescat@com		*/

/* Port to WARC.
 * 2008 Feb 12	voidptrptr, Hanzo Archive Limited		*/

WPUBLIC warc_u8_t * w_strcasestr (const warc_u8_t * phaystack,
                                  const warc_u8_t * pneedle)
{
  register const warc_u8_t * haystack, * needle;
  register unsigned bl, bu, cl, cu;

  haystack = (const warc_u8_t *) phaystack;
  needle   = (const warc_u8_t *) pneedle;

  bl = tolower (* needle);

  if (bl != '\0')
    {
      bu = _toupper (bl);
      haystack--;				/* possible ANSI violation */

      do
        {
          cl = *++haystack;

          if (cl == '\0')
            goto ret0;
        }

      while ( (cl != bl) && (cl != bu) );

      cl = tolower (*++needle);

      if (cl == '\0')
        goto foundneedle;

      cu = _toupper (cl);

      ++needle;

      goto jin;

      for (;;)
        {
          register unsigned a;
          register const warc_u8_t *rhaystack, *rneedle;

          do
            {
              a = *++haystack;

              if (a == '\0')
                goto ret0;

              if ( (a == bl) || (a == bu) )
                break;

              a = *++haystack;

              if (a == '\0')
                goto ret0;

shloop:
              ;
            }

          while ( (a != bl) && (a != bu) );

jin:
          a = *++haystack;

          if (a == '\0')
            goto ret0;

          if ( (a != cl) && (a != cu) )
            goto shloop;

          rhaystack = haystack-- + 1;

          rneedle = needle;

          a = tolower (*rneedle);

          if (tolower (*rhaystack) == (int) a)
            do
              {
                if (a == '\0')
                  goto foundneedle;

                ++rhaystack;

                a = tolower (*++needle);

                if (tolower (*rhaystack) != (int) a)
                  break;

                if (a == '\0')
                  goto foundneedle;

                ++rhaystack;

                a = tolower (*++needle);
              }

            while (tolower (*rhaystack) == (int) a);

          needle = rneedle;		/* took the register-poor approach */

          if (a == '\0')
            break;
        }
    }

foundneedle:

  return (warc_u8_t *) haystack;

ret0:
  return 0;
}




/* locate character in string */
WPUBLIC const warc_u8_t * w_index (const warc_u8_t * s, int c)
{
  while (* s)
    {
      if (* s == c)
        return (s);

      ++ s;
    }

  return (0);
}

WPUBLIC warc_i32_t w_strcmp (const warc_u8_t * s1, const warc_u8_t * s2)
{
  while (*s1 && (*s1 == *s2) )
    {
      s1++;
      s2++;
    }

  return ( (warc_u8_t) *s1 - (warc_u8_t) *s2);
}


WPUBLIC warc_u32_t w_strlen (const warc_u8_t * s)
{
  register const warc_u8_t * pos = s;

  while (* pos)
    {
      ++ pos;
    }

  return (pos - s);
}


/*
 * Verify whether the given argument is a power of two.
 */

#define ROUND_POWER_OF_2 \
  while ( i < arg ) \
    { \
      j = i; \
      i = i << 1; \
    } \
  return (arg - j) < (i - arg) ? j : i


#define IS_POWER_OF_2 \
  if (arg == 0) \
    return 0; \
  while ((arg & 1) == 0) \
    arg >>= 1; \
  return (arg == 1)

WPUBLIC warc_u32_t roundToPowerOfTwoUInt (warc_u32_t arg)
{
  warc_u32_t i = 1U;
  warc_u32_t j = i;

  ROUND_POWER_OF_2;
}

WPUBLIC warc_bool_t isPowerOfTwoUInt (warc_u32_t arg)
{
  IS_POWER_OF_2;
}


WPUBLIC warc_u32_t computeHash (const char * _str, warc_u32_t len)
{
  char       * str  = (char *) _str;
  warc_u32_t   hash = 5381;
  warc_u32_t   i    = 0;

  for (i = 0; i < len; ++ str, ++ i)
    {
      hash = ( (hash << 5) + hash) + (*str);
    }

  return (hash & 0x7FFFFFFF);
}

/**
 * @param dstring: the string to chek
 *
 * @return True if succeedds, False otherwise
 *
 * Cheks if a string represents a Numerical value or not
 */

WPUBLIC warc_bool_t w_check_digital_string (const warc_u8_t * dstring,
    warc_u32_t len)
{
  warc_u32_t i = 0;

  while ( (i < len) && ( (dstring[i] >= '0') || (dstring[i] <= '9') ) )
    ++ i;

  if (i < len)
    return (WARC_FALSE);

  return (WARC_TRUE);
}


/*
  convert string to unsigned integern,
  returns false if succeed and true if en error occure
*/

WPUBLIC warc_bool_t w_atou (const warc_u8_t * s, warc_u32_t len,
                            warc_u32_t * val)
{
  unless (w_check_digital_string (s, len) )
  return (WARC_TRUE);
  else
    * val = atoi ( (const char *) s);

  return (WARC_FALSE);
}


/**
 * Convert a digit its string representation
 */

WPUBLIC warc_u8_t * w_numToString (warc_u64_t numericvalue,
                                   warc_u8_t * strvalue)
{
  warc_u32_t i;
  warc_u64_t quaut = 1;

  while ( (numericvalue / quaut) != 0)
    quaut *= 10;

  unless (numericvalue)
  quaut = 1;
  else
    quaut /= 10;

  i = 0;

  while (quaut != 0)
    {
      strvalue [i] = '0' + (numericvalue / quaut);
      numericvalue %= quaut;
      quaut /= 10;
      ++ i;
    }

  strvalue [i] = '\0';

  return (strvalue);
}
