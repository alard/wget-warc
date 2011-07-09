
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


#include <wclass.h>  /* bless, destroy, cassert, struct Class */
#include <wsign.h>   /* CASSET macro */
#include <wmem.h>    /* wmalloc, wfree */
#include <wmisc.h>   /* unless, ... */
#include <wcsafe.h>  /* w_strncpy, ... */
#include <wgetopt.h> /* for class's prototypes */
#include <wstring.h> /* WString */

#define SIGN 15

/**
 * WARC WGetOpt internal structure
 */

struct WGetOpt
  {

    const void * class;

    /*@{*/
    void       * flags;
    char       * optarg;
    warc_i32_t   optind;
    warc_i32_t   sp;
    /*@}*/
  };


#define FLAGS    (self -> flags)
#define OPTIND   (self -> optind)
#define OPTARG   (self -> optarg)
#define SP       (self -> sp)


/* #define ERR(s, c)	if(opterr){\ */
/* 	char errbuf[2];\ */
/* 	errbuf[0] = c; errbuf[1] = '\n';\ */
/* 	(void) write(2, argv[0], (unsigned)strlen(argv[0]));\ */
/* 	(void) write(2, s, (unsigned)strlen(s));\ */
/* 	(void) write(2, errbuf, 2);} */

#define ERR(s, c)	if(opterr){\
      fprintf (stderr, "%s", s);\
      fprintf (stderr, "%c\n", c);\
      fprintf (stderr, "-- ");}


/**
 * @param _self WGetOpt object
 * @return a character pointer
 * Returns s string representing the argument
 */

WIPUBLIC char * WGetOpt_argument (const void * const _self)
{

  const struct WGetOpt * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (OPTARG);
}


/**
 * @param _self WGetOpt object
 * @return an integer indice value
 * Returns the indice of the argument
 */

WIPUBLIC warc_i32_t WGetOpt_indice (const void * const _self)
{

  const struct WGetOpt * const self = _self;


  /* preconditions */
  CASSERT (self);

  return (OPTIND);
}


/**
 * @param _self WGetOpt object
 * @param argc number of arguments
 * @param argv array of strings arguments
 * @param env environment data
 * @return the character found in the pattern or -1 if error
 * Parse the arguments against the pattern
 */

WPUBLIC warc_i32_t WGetOpt_parse (void * const _self, warc_i32_t argc,
                                  const char ** argv)
{

  struct WGetOpt * const self = _self;

  const warc_u8_t * cp     = NIL;
  warc_i32_t            c      = 0;
  warc_i32_t	        opterr = 1;

  /* preconditions */
  CASSERT (self);
  assert (argc > 1);
  assert (argv);

  if (SP == 1)
    {
      if (OPTIND >= argc ||
              argv [OPTIND][0] != '-' || argv [OPTIND][1] == '\0')
        {
          return (-1);
        }

      else if (w_strcmp ( (warc_u8_t *) argv [OPTIND], (warc_u8_t *) "--") == 0)
        {
          ++ OPTIND;
          return (-1);
        }
    }

  c = argv [OPTIND][SP];

  if (c == ':' || (cp = w_index (WString_getText (FLAGS), c) ) == 0)
    {
      ERR ("illegal option -- ", c);

      if (argv [OPTIND][++ SP] == '\0')
        {
          ++ OPTIND;
          SP = 1;
        }

      return ('?');
    }

  if (* ++ cp == ':')
    {
      if (argv [OPTIND][SP + 1] != '\0')
        {
          OPTARG = (char *) (& argv [OPTIND ++][SP+1]);
        }

      else if (++ OPTIND >= argc)
        {
          ERR ("option requires an argument -- ", c);
          SP = 1;

          return ('?');
        }

      else
        OPTARG = (char *) (argv [OPTIND ++]);

      SP = 1;
    }

  else
    {
      if (argv [OPTIND][++ SP] == '\0')
        {
          SP = 1;
          ++ OPTIND;
        }

      OPTARG = 0;
    }

  return (c);
}


/**
 * WGetOpt_constructor - create a new WGetOpt object instance
 *
 * @param _self WGetOpt class object
 * @param app: constructor list parameters
 *
 * @return a valid WGetOpt object or NIL
 *
 * @brief WARC String constructor
 */

WPRIVATE void * WGetOpt_constructor (void * _self, va_list * app)
{

  struct WGetOpt     * const self = _self;

  const char         * text = va_arg (* app, const char *);
  const warc_u32_t     len  = va_arg (* app, const warc_u32_t);

  /* preconditions */
  assert (text);

  FLAGS = bless (WString, text, len);
  unless (FLAGS)
  {
    destroy (self);
    return (NIL);
  }

  OPTIND = 1;
  OPTARG = 0;
  SP     = 1;

  return (self);
}


/**
 * WGetOpt_destructor - delete an existing WGetOpt object
 *
 * @param _self WGetOpt object instance
 *
 * WARC String destructor
 */

WPRIVATE void * WGetOpt_destructor (void * _self)
{

  struct WGetOpt * self = _self;

  /* preconditions */
  CASSERT (self);

  if (FLAGS)
    destroy (FLAGS), FLAGS = NIL;

  OPTIND = 1;

  OPTARG = 0;

  SP     = 1;

  return (self);
}


/**
 * WARC WGetOpt class
 */

static const struct Class _WGetOpt =
  {
    sizeof (struct WGetOpt),
    SIGN,
    WGetOpt_constructor, WGetOpt_destructor
  };

const void * WGetOpt = & _WGetOpt;
