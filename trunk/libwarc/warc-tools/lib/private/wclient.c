
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

#include <wstring.h> /* for class's prototypes */
#include <wclass.h>   /* bless, destroy, cassert, struct Class */
#include <wsign.h>    /* CASSET macro */
#include <wclient.h>    /* for class's prototypes */
#include <wmem.h>     /* wmalloc, wfree */
#include <wmisc.h>    /* warc_bool_t ... */
#include <wendian.h>  /* warc_i32ToEndian ... */
#include <wcsafe.h>
#include <wfile.h>   /* for class's prototypes */
#include <wrecord.h> /* for class's prototypes */
#include <wversion.h> /* for the warc version */

#include <event.h> /* event structures and functions */
#include <evhttp.h> /* evhttp structures and functions */


#define makeS(s) (s), w_strlen ((s))
#define makeU(s) ((warc_u8_t *) (s))

/**
 * The requests kinds
 */

typedef enum 
  {
    WARC_ONE_RECORD_REQUEST = 0,
    WARC_FULL_FILE_REQUEST,
    WARC_DISTANT_DUMP_REQUEST,
    WARC_DISTANT_FILTER_REQUEST
  } warc_request_t;

/* This structure that will be ginven to callback function as user argument*/

struct Callbacks_arg
  {
    FILE       * file;
    warc_u32_t   size;
    warc_request_t request;
  };

WPRIVATE void WGetChunks_callback (struct evhttp_request * req, void * _arg)
{

  struct Callbacks_arg * arg = _arg;

  warc_u32_t  * size       = & (arg -> size);
  warc_u32_t    wrote_size = 0;
  int           file       = w_fileno (arg -> file);

  if (file > 0)
    {
      wrote_size = evbuffer_write (req -> input_buffer, file);

      if (wrote_size)
        * size = * size + wrote_size;
    }
}

WPRIVATE void WResponse_callback (struct evhttp_request * req, void * _arg)
{

  struct Callbacks_arg * arg      = NIL;
  warc_u64_t             size     = 0;
  warc_u64_t             sentsize = 0;
  warc_u8_t            * strsize  = NIL;

  arg = _arg;
  size  = arg -> size;

  if (req -> response_code != HTTP_OK)
    {
      w_fprintf (fprintf (stderr, "request result: %s\n", req -> response_code_line) );
      arg -> size = 0;
      event_loopexit (NIL);
      return ;
    }

  if ( arg -> request == WARC_DISTANT_DUMP_REQUEST)
    {
     #define WRITESIZE 1024
     FILE * file = arg -> file;

/*     sentsize = (warc_u32_t) req -> input_buffer -> totallen;*/
     sentsize = w_strlen (makeU (req -> input_buffer -> orig_buffer));
     arg -> size = sentsize;
     size = WRITESIZE;

     while (sentsize)
         {
          if (sentsize <= WRITESIZE)
             sentsize -= WRITESIZE;
          else
            {
             size = sentsize;
             sentsize = 0;
            }
          if ((w_fwrite (req -> input_buffer -> orig_buffer, 1, size, file) != size))
             {
              w_fprintf (fprintf (stderr, "request result: %s\n", req -> response_code_line) );
              arg -> size = 0;
              event_loopexit (NIL);
              return ;
             }
         }
    }
  else
    {
     /* testing if the size of the recovered data is the same
        that the size sent by the server */
     strsize = (warc_u8_t *) evhttp_find_header (req -> input_headers, "Content-Length");
   
     if (strsize)
         w_atou (strsize, w_strlen (strsize), & sentsize);
     else
        sentsize = size;
 
     if (sentsize != size )
       {
         WarcDebugMsg ("Error: transmitted data size isn't equals to the expected size\n");
         event_loopexit (NIL);
         return ;
       }
    }

  /* Success we're done. Exit the dispatch loop */
  /*   w_fprintf (fprintf (stderr, "request result: %s\n", req -> response_code_line)); */
  /*   w_fprintf (fprintf (stderr, "%d Bytes of data wrote\n", size)); */

  event_loopexit (NIL);
}

/**
 * WARC WClient class internal
 */

#define SIGN 19

struct WClient
  {

    const void * class;

    /*@{*/

    void *      server_uri; /**< The uri wher the server is installed as a WString object */
    warc_u8_t * server; /**< the name of the asked server */
    warc_u32_t  port; /**< The port where to send the requests*/

    struct event_base * base_event; /**< The base event associated with the client */

    struct evhttp_connection * connection; /**< The connection object which with we connect to the server */
    void        (*read_callback) (struct evhttp_request *, void *); /**< callback function that will be executed when the response to the record access request arrives*/
    void        (*chunk_callback) (struct evhttp_request *, void *); /**< The callback function that will be called when a chunk from the requested record arrives*/
    /*@}*/
  };


#define URI       (self -> server_uri)
#define PORT      (self -> port)
#define SERVER    (self -> server)
#define BASE      (self -> base_event)
#define CONNECT   (self -> connection)

#define READCB    (self -> read_callback)
#define CHUNKCB   (self -> chunk_callback)



/**
 * @param _self: a WClient object instance
 *
 * @return False if succeeds, True otherwise
 *
 * Warc Client object initialisation function
 */

WPRIVATE warc_bool_t WClient_init (void * _self)
{

  struct WClient * self = _self;

  /* Preconditions */
  CASSERT (self);

  CONNECT = evhttp_connection_new ( (const char *) WString_getText (URI), PORT);

  unless (CONNECT)
  return (WARC_TRUE);

  BASE = event_init ();

  unless (BASE)
  {
    evhttp_connection_free (CONNECT);
    return (WARC_TRUE);
  }

  evhttp_connection_set_base (CONNECT, BASE);

  READCB  = WResponse_callback;
  CHUNKCB = WGetChunks_callback;

  return (WARC_FALSE);
}


/**
 * @param _self: a WClient object instance
 *
 * @return False if succedds, True otherwise
 *
 * Warc Client connection ending
 */

WPRIVATE warc_bool_t WClient_end (void * _self)
{

  struct WClient * self = _self;
  warc_bool_t      out = WARC_FALSE;

  /* Preconditions */
  CASSERT (self);

  unless (CONNECT)
  out = WARC_TRUE;
  else
    evhttp_connection_free (CONNECT), CONNECT = NIL;

  unless (BASE)
  out = WARC_TRUE;
  else
    event_base_free (BASE), BASE = NIL;

  return (out);
}



/**
 * @param _self: a WClient object instance
 * @param offset: The offset of the reclaimed record
 * @param path: The patrh where the warc file is stored
 * @param pathlen: the length of the path string
 * @param outf: the ouput file where to put the result
 *
 * @return False if succeeds, True otherwise
 *
 * Warc Client Record reclaiming function
 */

WPUBLIC warc_bool_t WClient_getWRecord (void * const _self,
                                        warc_u64_t offset,
                                        const warc_u8_t * path,
                                        warc_u32_t pathlen,
                                        const warc_u8_t * outf)
{

  struct WClient        * const self         = _self;
  warc_u8_t                     char_offset [26] ;

  struct evhttp_request *       request      = NIL;
  void                  *       rest_request = NIL;
  FILE *                        outfh;

  struct Callbacks_arg          arg;

  /* Precondition */
  CASSERT (self);

  if (WClient_init (self) )
    return (WARC_TRUE);

  rest_request = bless (WString, "/", 1);

  unless (rest_request)
  return (WARC_TRUE);

  unless (outf)
  return (WARC_TRUE);

  outfh = w_fopen_wb ( (const char *) outf);

  unless (outfh)
  return (WARC_TRUE);

  arg . size = 0;

  arg . file = outfh;

  arg . request = WARC_ONE_RECORD_REQUEST;

  WString_concat (rest_request, SERVER);

  WString_append (rest_request, makeS ((const warc_u8_t *) WARC_VERSION) );

  WString_append (rest_request, makeS ((const warc_u8_t *) "/record"));

  WString_append (rest_request, (const warc_u8_t *) "/", 1);

  unless (w_numToString (offset, char_offset))
    {
    w_fclose (outfh);
    destroy (rest_request);
    return (WARC_TRUE);
    }

  WString_append (rest_request, makeS (char_offset) );

  WString_append (rest_request, (const warc_u8_t *) "/", 1);

  WString_append (rest_request, path, pathlen);

  request = evhttp_request_new (READCB, & arg);

  unless (request)
    {
    w_fclose (outfh);
    destroy (rest_request);
    return (WARC_TRUE);
    }

  evhttp_add_header (request -> output_headers, "Host", "Warc-client");

  evhttp_request_set_chunked_cb (request, CHUNKCB);

  if (evhttp_make_request (CONNECT, request, EVHTTP_REQ_GET, (const char *) WString_getText (rest_request) ) )
    {
      w_fclose (outfh);
      evhttp_request_free (request);
      destroy (rest_request);
      return (WARC_TRUE);
    }

  destroy (rest_request);

  event_base_dispatch (BASE);

  w_fclose (outfh);

  unless (arg . size)
     return (WARC_TRUE);

  if (WClient_end (self) )
    return (WARC_TRUE);

  return (WARC_FALSE);
}


/**
 * @param _self: a WClient object instance
 * @param offset: the offset from where the transfert must be done
 * @param path: The patrh where the warc file is stored
 * @param pathlen: the length of the path string
 * @param outf the file where to put the result
 *
 * @return False if succeeds, True otherwise
 *
 * Warc Client warc file reclaiming function
 */

WPUBLIC warc_bool_t WClient_getWFile (void * const _self, 
                                      warc_u64_t offset,
                                      const warc_u8_t * path,
                                      warc_u32_t pathlen,
                                      const warc_u8_t * outf)
{

  struct WClient        * const self        = _self;
  warc_u8_t                     char_offset [26];

  struct evhttp_request *       request      = NIL;
  void                  *       rest_request = NIL;
  FILE *                        outfh;

  struct Callbacks_arg          arg;

  /* Precondition */
  CASSERT (self);

  if (WClient_init (self) )
    return (WARC_TRUE);

  rest_request = bless (WString, "/", 1);

  unless (rest_request)
  return (WARC_TRUE);

  outfh = w_fopen_wb ( (const char *) outf);

  unless (outfh)
  return (WARC_TRUE);

  arg . size = 0;

  arg . file = outfh;

  arg . request = WARC_FULL_FILE_REQUEST;

  WString_concat (rest_request, SERVER);

  WString_append (rest_request, makeS ( (const warc_u8_t *) WARC_VERSION) );

  WString_append (rest_request, (const warc_u8_t *) "/file", 5);

  WString_append (rest_request, (const warc_u8_t *) "/", 1);

  unless (w_numToString (offset, char_offset))
    {
    w_fclose (outfh);
    destroy (rest_request);
    return (WARC_TRUE);
    }

  WString_append (rest_request, makeS (char_offset) );

  WString_append (rest_request, (const warc_u8_t *) "/", 1);

  WString_append (rest_request, path, pathlen);


  request = evhttp_request_new (READCB, &arg);

  unless (request)
    {
    w_fclose (outfh);
    destroy (rest_request);
    return (WARC_TRUE);
    }

  evhttp_add_header (request -> output_headers, "Host", "Warc-client");

  evhttp_request_set_chunked_cb (request, CHUNKCB);

  if (evhttp_make_request (CONNECT, request, EVHTTP_REQ_GET, (const char *) WString_getText (rest_request) ) )
    {
      w_fclose (outfh);
      evhttp_request_free (request);
      destroy (rest_request);
      return (WARC_TRUE);
    }

  destroy (rest_request);

  event_base_dispatch (BASE);

  w_fclose (outfh);

  unless (arg . size)
  return (WARC_TRUE);

  if (WClient_end (self) )
    return (WARC_TRUE);

  return (WARC_FALSE);
}


/**
 * @param _self: a WClient object instance
 * @param offset: The offset of the reclaimed record
 * @param what: on each field the filter will be done
 * @param whatlen: the length of the what string
 * @param value: the value of the filter
 * @param valuelen: the length of the value string
 * @param path: The patrh where the warc file is stored
 * @param pathlen: the length of the path string
 * @param outf: the file where to put the result
 *
 * @return False if succeeds, True otherwise
 *
 * Warc Client filtred Warc File reclaiming function
 */

WPUBLIC warc_bool_t WClient_getFiltredWFile (void * const _self,
                                            warc_u64_t offset,
                                            const warc_u8_t * what,
                                            warc_u32_t whatlen,
                                            const warc_u8_t * value,
                                            warc_u32_t valuelen,
                                            const warc_u8_t * path,
                                            warc_u32_t pathlen,
                                            const warc_u8_t * outf)
{

  struct WClient        * const self         = _self;
  warc_u8_t                     char_offset [26] ;

  struct evhttp_request *       request      = NIL;
  void                  *       rest_request = NIL;
  FILE *                        outfh;

  struct Callbacks_arg          arg;

  /* Precondition */
  CASSERT (self);

  if (WClient_init (self) )
    return (WARC_TRUE);

  rest_request = bless (WString, "/", 1);

  unless (rest_request)
  return (WARC_TRUE);

  unless (outf)
  return (WARC_TRUE);

  outfh = w_fopen_wb ( (const char *) outf);

  unless (outfh)
  return (WARC_TRUE);

  arg . size = 0;

  arg . file = outfh;

  arg . request = WARC_DISTANT_FILTER_REQUEST;

  WString_concat (rest_request, SERVER);

  WString_append (rest_request, makeS ( (const warc_u8_t *) WARC_VERSION) );

  WString_append (rest_request, makeS ((const warc_u8_t *) "/filter"));

  WString_append (rest_request, (const warc_u8_t *) "/", 1);

  unless (w_numToString (offset, char_offset))
    {
    w_fclose (outfh);
    destroy (rest_request);
    return (WARC_TRUE);
    }

  WString_append (rest_request, makeS (char_offset) );

  WString_append (rest_request, (const warc_u8_t *) "/", 1);

  WString_append (rest_request, what, whatlen);

  WString_append (rest_request, (const warc_u8_t *) "/", 1);

  WString_append (rest_request, value, valuelen);

  WString_append (rest_request, (const warc_u8_t *) "/", 1);

  WString_append (rest_request, path, pathlen);

  request = evhttp_request_new (READCB, & arg);

  unless (request)
    {
    w_fclose (outfh);
    destroy (rest_request);
    return (WARC_TRUE);
    }

  evhttp_add_header (request -> output_headers, "Host", "Warc-client");

  evhttp_request_set_chunked_cb (request, CHUNKCB);

  if (evhttp_make_request (CONNECT, request, EVHTTP_REQ_GET, (const char *) WString_getText (rest_request) ) )
    {
      w_fclose (outfh);
      evhttp_request_free (request);
      destroy (rest_request);
      return (WARC_TRUE);
    }

  destroy (rest_request);

  event_base_dispatch (BASE);

  w_fclose (outfh);

  unless (arg . size)
  return (WARC_TRUE);

  if (WClient_end (self) )
    return (WARC_TRUE);

  return (WARC_FALSE);
}


/**
 * @param _self: a WClient object instance
 * @param offset: the offset from where the transfert must be done
 * @param format: the output format of the list
 * @param format: the length of the  format stirng
 * @param path: The patrh where the warc file is stored
 * @param pathlen: the length of the path string
 * @param outf the file where to put the result
 *
 * @return False if succeeds, True otherwise
 *
 * Warc Client WARC file Records list reclaiming function
 */

WPUBLIC warc_bool_t WClient_getList (void * const _self, 
                                      warc_u64_t offset,
                                      const warc_u8_t * format,
                                      warc_u32_t        formatlen,
                                      const warc_u8_t * path,
                                      warc_u32_t pathlen,
                                      const warc_u8_t * outf)
{

  struct WClient        * const self        = _self;
  warc_u8_t                     char_offset [26];

  struct evhttp_request *       request      = NIL;
  void                  *       rest_request = NIL;
  FILE *                        outfh;
  void        (*oldcb) (struct evhttp_request *, void *) = NIL;

  struct Callbacks_arg          arg;

  /* Precondition */
  CASSERT (self);

  if (WClient_init (self) )
    return (WARC_TRUE);

  rest_request = bless (WString, "/", 1);

  unless (rest_request)
  return (WARC_TRUE);

  outfh = w_fopen_wb ( (const char *) outf);

  unless (outfh)
  return (WARC_TRUE);

  arg . size = 0;

  arg . file = outfh;

  arg . request = WARC_DISTANT_DUMP_REQUEST;

  oldcb = CHUNKCB;
  CHUNKCB = NIL;

  WString_concat (rest_request, SERVER);

  WString_append (rest_request, makeS ( (const warc_u8_t *) WARC_VERSION) );

  WString_append (rest_request, (const warc_u8_t *) "/list", 5);

  WString_append (rest_request, (const warc_u8_t *) "/", 1);

  unless (w_numToString (offset, char_offset))
    {
    w_fclose (outfh);
    destroy (rest_request);
    return (WARC_TRUE);
    }

  WString_append (rest_request, makeS (char_offset) );

  WString_append (rest_request, (const warc_u8_t *) "/", 1);

  WString_append (rest_request, format, formatlen);

  WString_append (rest_request, (const warc_u8_t *) "/", 1);

  WString_append (rest_request, path, pathlen);


  request = evhttp_request_new (READCB, &arg);

  unless (request)
    {
    w_fclose (outfh);
    destroy (rest_request);
    return (WARC_TRUE);
    }

  evhttp_add_header (request -> output_headers, "Host", "Warc-client");

  evhttp_request_set_chunked_cb (request, CHUNKCB);

  if (evhttp_make_request (CONNECT, request, EVHTTP_REQ_GET, (const char *) WString_getText (rest_request) ) )
    {
      w_fclose (outfh);
      evhttp_request_free (request);
      destroy (rest_request);
      return (WARC_TRUE);
    }

  destroy (rest_request);

  event_base_dispatch (BASE);

  w_fclose (outfh);

  CHUNKCB = oldcb;

  unless (arg . size)
  return (WARC_TRUE);

  if (WClient_end (self) )
    return (WARC_TRUE);

  return (WARC_FALSE);
}




/**
 * WClient_constructor - create a new WClient object instance
 *
 * @param _self WList class object
 * @param app constructor list parameters
 *
 * @return a valid WClient object or NIL
 *
 * @brief WARC Client constructor
 */

WPRIVATE void * WClient_constructor (void * _self, va_list * app)
{

  struct WClient * self = _self;

  const warc_u8_t * uri     =   va_arg (*app, const warc_u8_t *);
  warc_u32_t        uri_len =   va_arg (*app, warc_u32_t);
  warc_u32_t        port    =   va_arg (*app, warc_u32_t);
  const warc_u8_t * server  =   va_arg (*app, warc_u8_t *);
  warc_u32_t        serverlen = va_arg (*app, warc_u32_t);

  unless (uri || uri_len)
  {
    destroy (self);
    return NIL;
  }

  URI = bless (WString, uri, uri_len);
  unless (URI)
  {
    destroy (self);
    return (NIL);
  }

  if ( (port <= 1024) || (port >= 65536) )
    {
      destroy (self);
      return (NIL);
    }

  unless (server || serverlen)
    {
    destroy (self);
    return (NIL);
    }

  unless ((w_strcmp (server, makeU ("warcserver"))) && (w_strcmp (server, makeU ("apache2"))))
     SERVER = bless (WString, "", 0);
  else
     unless (w_strcmp (server, makeU ("lighttpd-cgi")))
        SERVER = bless (WString, makeS (makeU ("warc.cgi/")));
     else
        unless (w_strcmp (server, makeU ("lighttpd-fcgi")))
          SERVER = bless (WString, makeS (makeU ("warc.fcgi/")));
        else
          {
          destroy (self);
          WarcDebugMsg ("unknown server name");
          return (NIL);
          }

  unless (SERVER)
    {
    destroy (self);
    return (NIL);
    }

  PORT = port;

  CONNECT = NIL;
  BASE    = NIL;
  READCB  =  NIL;
  CHUNKCB =  NIL;

  return (self);
}


/**
 * @param _self WClient object instance
 *
 * @brief WARC Client destructor
 */

WPRIVATE void * WClient_destructor (void * _self)
{

  struct WClient  * const self = _self;

  /* preconditions */
  CASSERT (self);

  /* no need for that */
  /*  WClient_end (self); */

  if (URI)
    destroy (URI), URI = NIL;

  if (PORT)
    PORT = 0;

  if (SERVER)
    destroy (SERVER), SERVER = NIL;

  if (CONNECT)
    evhttp_connection_free (CONNECT), CONNECT = NIL;

  if (BASE)
    event_base_free (BASE), BASE = NIL;

  if (READCB)
    READCB = NIL;

  if (CHUNKCB)
    CHUNKCB = NIL;

  return (self);
}


/**
 * WARC WClient class
 */

static const struct Class _WClient =
  {
    sizeof (struct WClient),
    SIGN,
    WClient_constructor, WClient_destructor
  };

const void * WClient = & _WClient;
