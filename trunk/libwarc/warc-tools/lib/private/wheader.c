
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

#include <wclass.h>   /* bless, destroy, cassert, struct Class */
#include <wsign.h>    /* CASSET macro */
#include <wstring.h>  /* for class's prototypes */
#include <wmem.h>     /* wmalloc, wfree */
#include <wmisc.h>    /* warc_bool_t ... */
#include <wrectype.h> /* revist, resource ... */
#include <wlist.h>    /* for Class's Prototype*/
#include <wversion.h> /* WARC_VERSION */
#include <wfsmanvl.h>
#include <wanvl.h>
#include <wcsafe.h>


#define makeS(s) (s), w_strlen ((s))
#define makeU(s) ((const warc_u8_t *) (s))

#define SIGN 20


/**
 * WARC Header internal structure
 */


struct WHeader
  {

    const void * class;

    /*@{*/
    /* Old warc format header line fileds */
    void       * warc_id; /**< The WARC version (Mandatory) */
    warc_u64_t   data_length; /**< The WARC record data length */
    warc_rec_t   record_type; /**< The WARC record type (Mandatory) */
    void       * subject_uri; /**< The WARC record subject uri */
    void       * creation_date; /**< The WARC record date of creation (Mandatory) */
    void       * content_type; /**< The WARC record content type (Mandatory) */
    void       * record_id; /**< The WARC record id  (Mandatory) */

    /* New warc format header fileds */
    warc_u64_t   content_length; /**< The length of the data bloc (Mandatory) */
    void       * concurrent_to; /**< The id of the record related to this one */
    void       * block_digest; /**< The algorithme and value of the disgest applied on the block this record */
    void       * payload_digst; /**< The algorithme and value of the digest applied on the payload of this record */
    void       * ip_address; /**< The material IP address of the source of the record */
    void       * refers_to; /**< The id of the record which has content holded in this record */
    void       * truncated; /**< The reason of the truncation if it exists */
    void       * info_id;  /**< The id of the warc info record of the warc file which this record is associated to */
    void       * file_name; /**< The name of the warc file containing the warc info record related to this one */
    void       * profile; /**< In "revisit record" case, it indicates the kind of analysis and handling applied to */
    void       * payload_type; /**< The content type of the payload of the record */
    void       * segm_origin_id; /**< In "segment record" case, it gives the id of the first record */
    warc_u64_t   segm_number; /**< In "segmented record" case, it gives the row of the segment */
    warc_u64_t   segm_total_length; /**< In "segmented record" case, it gives the length of the whole segmented data */
    
    void       * other_fields; /**< The list of other non predefined named fields */
    /*@}*/
  };


#define    WARC_ID         (self -> warc_id)
#define    DATA_LENGTH     (self -> data_length)
#define    RECORD_TYPE     (self -> record_type)
#define    SUBJECT_URI     (self -> subject_uri)
#define    CREATION_DATE   (self -> creation_date)
#define    CONTENT_TYPE    (self -> content_type)
#define    RECORD_ID       (self -> record_id)

#define    CONT_LENGTH     (self -> content_length)
#define    CONC_TO         (self -> concurrent_to)
#define    BLOCK_DIG       (self -> block_digest)
#define    PAYLOAD_DIG     (self -> payload_digst)
#define    IP_ADDRESS       (self -> ip_address)
#define    REFERS_TO       (self -> refers_to)
#define    TRUNCATED       (self -> truncated)
#define    INFO_ID         (self -> info_id)
#define    FILE_NAME       (self -> file_name)
#define    PROFILE         (self -> profile)
#define    PAYLOAD_TYPE    (self -> payload_type)
#define    S_ORG_ID        (self -> segm_origin_id)
#define    S_NUMBER        (self -> segm_number)
#define    S_TOTAL_LENGTH  (self -> segm_total_length)

#define    OTHER_FIELDS    (self -> other_fields)


/**
 * @param _self WHeader object
 *
 * @return constant WARC id string
 *
 * Returns the WARC id of the WARC-record
 */

WIPUBLIC const warc_u8_t * WHeader_getWarcId (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WString_getText (WARC_ID) );
}


/**
 * @param _self WHeader object instance
 *
 * @return the header warc id field length as a 4 bytes integer
 *
 * WHeader warc id field length provider
 */

WIPUBLIC warc_u32_t WHeader_getWarcIdLen (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WString_getLength (WARC_ID) );
}


/**
 * @param _self WHeader object
 *
 * @return Warc Record length as a 64 bits unsigned integer
 *
 * Returns the length of the WARC-record
 */

WIPUBLIC warc_u64_t WHeader_getDataLength (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (DATA_LENGTH);
}


/**
 * @param _self WHeader object
 * @param len data length in bytes
 *
 * @return False if it succeeds, True otherwise
 *
 * Sets the size of the WARC-record
 */

WIPUBLIC warc_bool_t WHeader_setDataLength (void * const _self, const warc_u64_t len)
{

  struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (len)
  return (WARC_TRUE);

  DATA_LENGTH = len;
  return (WARC_FALSE);

}



/**
 * @param _self WHeader object
 *
 * @return content length as a 32 bits unsigned integer
 *
 * Returns the content length of the WARC-record
 */

WIPUBLIC warc_u64_t WHeader_getContentLength (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (CONT_LENGTH);
}

/**
 * @param _self WHeader object
 * @param len content length in bytes
 *
 * @return False if it succeeds, True otherwise
 *
 * Sets the content length of the WARC-record
 */

WIPUBLIC warc_bool_t WHeader_setContentLength (void * const _self, 
                                               const warc_u64_t len)
{

  struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  if (len == WARC_U64_MAX )
     return (WARC_TRUE);

  CONT_LENGTH = len;
  return (WARC_FALSE);

}


/**
 * @param _self WHeader object
 *
 * @return WARC-record type as an integer (see "wrectype.h")
 *
 * Returns the WARC-record type
 */

WIPUBLIC warc_rec_t WHeader_getRecordType (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (RECORD_TYPE);
}


/**
 * @param _self WHeader object
 * @param t WARC-record type integer (see "wrectype.h")
 *
 * @param[out]: False if succeeds, True otherwise
 *
 * Sets the WARC-record type
 */

WIPUBLIC warc_bool_t WHeader_setRecordType (void * const _self,
                                            const warc_rec_t t)
{

  struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  if (t > WARC_RESOURCE_RECORD)
    return (WARC_TRUE);

  RECORD_TYPE = t;

  return (WARC_FALSE);
}


/**
 * @param _self WHeader object
 *
 * @return WARC-record subject URI string
 *
 * Returns a constant string which represent the
 * subject URI of the WARC-record
 */

WIPUBLIC const warc_u8_t * WHeader_getSubjectUri (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (SUBJECT_URI)
     return (NIL);

  return (WString_getText (SUBJECT_URI) );
}

/**
 * @param _self WHeader object instance
 *
 * @return The subject uri field length
 * as a 4 bytes integer
 *
 * Warc header line subject Uri length string provider
 */

WIPUBLIC warc_u32_t WHeader_getSubjectUriLen (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);


  unless (SUBJECT_URI)
     return (0);

  return (WString_getLength (SUBJECT_URI) );
}


/**
 * @param _self WHeader object
 * @param suri Subject URI string
 * @param len WARC Subject URI string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the subject URI of the WARC-record
 */

WIPUBLIC warc_bool_t WHeader_setSubjectUri (void * const _self,
                                            const warc_u8_t * suri,
                                            const warc_u32_t len)
{

  struct WHeader * const self = _self;
  /* preconditions */
  CASSERT (self);
  assert (suri);

  /*   unless (w_strcasestr(suri,"://")) */
  /*     return (WARC_TRUE);  */

  /* reset the text */
  unless (SUBJECT_URI)
     {
     SUBJECT_URI = bless (WString, suri, len);
     unless (SUBJECT_URI)
        return (WARC_TRUE);
     }
 else if (WString_setText (SUBJECT_URI, suri, len) )
    return (WARC_TRUE);

  return (WARC_FALSE);
}


/**
 * @param self WHeader object
 *
 * @return creation date constant string
 *
 * Returns the WARC-record's creation date
 */

WIPUBLIC const warc_u8_t * WHeader_getCreationDate (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WString_getText (CREATION_DATE) );
}


/**
 * @param _self WHeader object instance
 *
 * @return The creation date field string length
 * as a 4 bytes integer
 *
 * Warc Header line creation date field string length provider
 */

WIPUBLIC warc_u32_t WHeader_getCreationDateLen (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WString_getLength (CREATION_DATE) );
}



/**
 * @param _self WHeader object
 * @param cd creation date string
 * @param len WARC Creation Date string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the creation date of the WARC-record
 */


WIPUBLIC warc_bool_t WHeader_setCreationDate (void * const _self,
                                              const warc_u8_t * cd,
                                              const warc_u32_t len)
{

  struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);
  assert (cd);

  if (WString_setText (CREATION_DATE, cd, len) )
    return (WARC_TRUE);

  return (WARC_FALSE);
}


/**
 * @param _self WHeader object
 *
 * @return content type constant string
 *
 * Returns the WARC-record's content type
 */

WIPUBLIC const warc_u8_t * WHeader_getContentType (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (CONTENT_TYPE)
    return (NIL);

  return (WString_getText (CONTENT_TYPE));
}


/**
 * @param _self WHeader object instance
 *
 * @return The content type field string length
 * as a 4 bytes integer
 *
 * Warc Header line content type field string length provider
 */


WIPUBLIC warc_u32_t WHeader_getContentTypeLen (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (CONTENT_TYPE)
    return (0);

  return (WString_getLength (CONTENT_TYPE) );
}


/**
 * @param _self WHeader object
 * @param ct content type string
 * @param len WARC Content Type string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the WARC-record's content type
 */

WIPUBLIC warc_bool_t WHeader_setContentType (void * const _self,
                                             unsigned const char * ct,
                                             const warc_u32_t len)
{

  struct WHeader * const self = _self;
  /* preconditions */
  CASSERT (self);
  assert (ct);

  /*  unless (w_strcasestr(ct,"/")) */
  /*     return (WARC_TRUE); */

  /* reset the text */

  unless (CONTENT_TYPE)
     {
     CONTENT_TYPE = bless (WString, ct, len);
     unless (CONTENT_TYPE)
         return (WARC_TRUE);
     }
  else if (WString_setText (CONTENT_TYPE, ct, len) )
    return (WARC_TRUE);

  return (WARC_FALSE);
}


/**
 * @param _self WHeader object
 *
 * @return record id constant string
 *
 * Returns the WARC-record id.
 */

WIPUBLIC const warc_u8_t * WHeader_getRecordId (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WString_getText (RECORD_ID) );
}


/**
 * @param _self WHeader object instance
 *
 * @return The record is field string length
 * as a 4 bytes integer
 *
 * Warc Header line record id field string length provider
 */

WIPUBLIC warc_u32_t WHeader_getRecordIdLen (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (WString_getLength (RECORD_ID) );
}


/**
 * @param _self WHeader object
 * @param rid record id constant string
 * @param len WARC Record id string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the WARC-record id
 */

WPUBLIC warc_bool_t WHeader_setRecordId (void * const _self,
                                         const warc_u8_t * rid,
                                         const warc_u32_t len)
{

  struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);
  assert (rid);

  /*   unless(w_strcasestr(rid,"://")) */
  /*     return (WARC_TRUE); */

  /* reset the text */

  if (WString_setText (RECORD_ID, rid, len) )
    return (WARC_TRUE);

  return (WARC_FALSE);
}


/**
 * @param _self WHeader object
 *
 * @return concurrent record id constant string
 *
 * Returns the Record id which this one is concurrent to
 */

WIPUBLIC const warc_u8_t * WHeader_getConcurrentTo (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (CONC_TO)
     return (NIL);

  return (WString_getText (CONC_TO));
}


/**
 * @param _self WHeader object instance
 *
 * @return The concurrent record id field string length
 * as a 4 bytes integer
 *
 * Warc Header concurrent record id field string length provider
 */

WIPUBLIC warc_u32_t WHeader_getConcurrentToLen (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (CONC_TO)
     return (0);

  return (WString_getLength (CONC_TO));
}

/**
 * @param _self WHeader object
 * @param rid Warc Record id constant string
 * @param len WARC Record id string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the Warc Concurrent Record id
 */

WPUBLIC warc_bool_t WHeader_setConcurrentTo (void * const _self,
                                             const warc_u8_t * rid,
                                             const warc_u32_t len)
{

  struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);
  assert (rid);

  unless (CONC_TO)
    {
    CONC_TO = bless (WString, rid, len);
    unless (CONC_TO)
       return (WARC_FALSE);
    }

  else if (WString_setText (CONC_TO, rid, len) )
    return (WARC_TRUE);

  return (WARC_FALSE);
}


/**
 * @param _self WHeader object
 *
 * @return The string describing the block digest algorithm and value
 *
 * Returns the Block digest string value
 */

WIPUBLIC const warc_u8_t * WHeader_getBlockDigest (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (BLOCK_DIG)
    return (NIL);

  return (WString_getText (BLOCK_DIG));
}


/**
 * @param _self WHeader object instance
 *
 * @return The record Block digest string length
 * as a 4 bytes integer
 *
 * Warc Header Block Digest string length provider
 */

WIPUBLIC warc_u32_t WHeader_getBlockDigestLen (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (BLOCK_DIG)
    return (0);

  return (WString_getLength (BLOCK_DIG));
}



/**
 * @param _self WHeader object
 * @param digest Block Digest Constant string
 * @param len Block Digest string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the Block Digest string
 */

WPUBLIC warc_bool_t WHeader_setBlockDigest (void * const _self,
                                            const warc_u8_t * digest,
                                            const warc_u32_t len)
{

  struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);
  assert (digest);

  unless (BLOCK_DIG)
    {
    BLOCK_DIG = bless (WString, digest, len);
    unless (BLOCK_DIG)
       return (WARC_TRUE);
    }

  else if (WString_setText (BLOCK_DIG, digest, len))
    return (WARC_TRUE);

  return (WARC_FALSE);
}




/**
 * @param _self WHeader object
 *
 * @return The PayLoad Digest algorithm and value as a string
 *
 * Returns the WArc Record PayLoad Digest string
 */

WIPUBLIC const warc_u8_t * WHeader_getPayloadDigest (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (PAYLOAD_DIG)
    return (NIL);

  return (WString_getText (PAYLOAD_DIG));
}


/**
 * @param _self WHeader object instance
 *
 * @return The Payload digest string length
 * as a 4 bytes integer
 *
 * Warc Header Payload Digest field string length provider
 */

WIPUBLIC warc_u32_t WHeader_getPayloadDigestLen (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (PAYLOAD_DIG)
    return (0);

  return (WString_getLength (PAYLOAD_DIG));
}



/**
 * @param _self WHeader object
 * @param paydigest Payload Digest string
 * @param len Payload digest string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the WARC Header Payload Digest field
 */

WPUBLIC warc_bool_t WHeader_setPayloadDigest (void * const _self,
                                              const warc_u8_t * paydigest,
                                              const warc_u32_t len)
{

  struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);
  assert (paydigest);

  unless (PAYLOAD_DIG)
    {
    PAYLOAD_DIG = bless (WString, paydigest, len);
    unless (PAYLOAD_DIG)
       return (WARC_TRUE);
    }

  else if (WString_setText (PAYLOAD_DIG, paydigest, len))
    return (WARC_TRUE);

  return (WARC_FALSE);
}


/**
 * @param _self WHeader object
 *
 * @return The material Source ip addresss string
 *
 * Returns the WARC Record IP Address
 */

WIPUBLIC const warc_u8_t * WHeader_getIpAddress (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (IP_ADDRESS)
    return (NIL);

  return (WString_getText (IP_ADDRESS));
}


/**
 * @param _self WHeader object instance
 *
 * @return The Warc Record IP Address field string length
 * as a 4 bytes integer
 *
 * Warc Header IP Address  field string length provider
 */

WIPUBLIC warc_u32_t WHeader_getIpAddressLen (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (IP_ADDRESS)
    return (0);

return (WString_getLength (IP_ADDRESS));
}



/**
 * @param _self WHeader object
 * @param ip IP Ardess constant string
 * @param len The ip Ardess string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the WARC Record IP Address field
 */

WPUBLIC warc_bool_t WHeader_setIpAddress (void * const _self,
                                         const warc_u8_t * ip,
                                         const warc_u32_t len)
{

  struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);
  assert (ip);

  unless (IP_ADDRESS)
    {
    IP_ADDRESS = bless (WString, ip, len);
    unless (IP_ADDRESS)
       return (WARC_TRUE);
    }

  else if (WString_setText (IP_ADDRESS, ip, len))
    return (WARC_TRUE);

  return (WARC_FALSE);
}




/**
 * @param _self WHeader object
 *
 * @return The referenced record id string
 *
 * Returns the Referenced WARC Record Id
 */

WIPUBLIC const warc_u8_t * WHeader_getRefers (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (REFERS_TO)
    return (NIL);

  return (WString_getText (REFERS_TO));
}


/**
 * @param _self WHeader object instance
 *
 * @return The Refers_to field string length
 * as a 4 bytes integer
 *
 * Warc Header Refers_to field string length provider
 */

WIPUBLIC warc_u32_t WHeader_getRefersToLen (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (REFERS_TO)
    return (0);

  return (WString_getLength (REFERS_TO));
}



/**
 * @param _self WHeader object
 * @param ref referenced Warc Record id constant string
 * @param len referenced WARC Record id string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the WARC Record Refers_To Record id string
 */

WPUBLIC warc_bool_t WHeader_setRefersTo (void * const _self,
                                         const warc_u8_t * ref,
                                         const warc_u32_t len)
{

  struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);
  assert (ref);

  unless (REFERS_TO)
    {
    REFERS_TO = bless (WString, ref, len);
    unless (REFERS_TO)
      return (WARC_TRUE);
    }

  else if (WString_setText (REFERS_TO, ref, len) )
    return (WARC_TRUE);

  return (WARC_FALSE);
}


/**
 * @param _self WHeader object
 *
 * @return Truncation cause describing string
 *
 * Returns the WARC Record Truncated String
 */

WIPUBLIC const warc_u8_t * WHeader_getTruncated (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (TRUNCATED)
    return (NIL);

  return (WString_getText (TRUNCATED));
}


/**
 * @param _self WHeader object instance
 *
 * @return Truncated string length as a 4 bytes integer
 *
 * Warc Header Truncated field string length provider
 */

WIPUBLIC warc_u32_t WHeader_getTruncatedLen (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (TRUNCATED)
    return (0);

  return (WString_getLength (TRUNCATED));
}



/**
 * @param _self WHeader object
 * @param trunc record id constant string
 * @param len WARC Record id string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the WARC Record Truncation Field
 */

WPUBLIC warc_bool_t WHeader_setTruncated (void * const _self,
                                          const warc_u8_t * trunc,
                                          const warc_u32_t len)
{

  struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);
  assert (trunc);

  unless (TRUNCATED)
    {
    TRUNCATED = bless (WString, trunc, len);
    unless (TRUNCATED)
      return (WARC_TRUE); 
    }

  else if (WString_setText (TRUNCATED, trunc, len))
    return (WARC_TRUE);

  return (WARC_FALSE);
}


/**
 * @param _self WHeader object
 *
 * @return Related Warc Info Id string
 *
 * Returns the WARC info Record id related to this one
 */

WIPUBLIC const warc_u8_t * WHeader_getInfoId (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (INFO_ID)
    return (NIL);

  return (WString_getText (INFO_ID));
}


/**
 * @param _self WHeader object instance
 *
 * @return The WarcInfo Id field string length
 * as a 4 bytes integer
 *
 * Warc Header WarcInfo Record Id  string length provider
 */

WIPUBLIC warc_u32_t WHeader_getInfoIdLen (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (INFO_ID)
    return (0);

  return (WString_getLength (INFO_ID));
}

/**
 * @param _self WHeader object
 * @param idinfo Warcinfo Record id constant string
 * @param len Warcinfo Record id string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the WARC Record Related Warcinfo Record Id
 */

WPUBLIC warc_bool_t WHeader_setInfoId (void * const _self,
                                       const warc_u8_t * idinfo,
                                       const warc_u32_t len)
{

  struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);
  assert (idinfo);

  unless (INFO_ID)
    {
    INFO_ID = bless (WString, idinfo, len);
    unless (INFO_ID)
      return (WARC_TRUE);
    }

  else if (WString_setText (INFO_ID, idinfo, len))
    return (WARC_TRUE);

  return (WARC_FALSE);
}



/**
 * @param _self WHeader object
 *
 * @return Related Warc File name string
 *
 * Returns the WARC File name related to this record
 */

WIPUBLIC const warc_u8_t * WHeader_getFilename (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (FILE_NAME)
    return (NIL);

  return (WString_getText (FILE_NAME));
}


/**
 * @param _self WHeader object instance
 *
 * @return The Warc file name field string length
 * as a 4 bytes integer
 *
 * Warc Header Warc File name string length provider
 */

WIPUBLIC warc_u32_t WHeader_getFilenameLen (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (FILE_NAME)
    return (0);

  return (WString_getLength (FILE_NAME));
}

/**
 * @param _self WHeader object
 * @param fname Warc File name constant string
 * @param len Warc File name string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the WARC Record Related Warc File name
 */

WPUBLIC warc_bool_t WHeader_setFilename (void * const _self,
                                       const warc_u8_t * fname,
                                       const warc_u32_t len)
{

  struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);
  assert (fname);

  unless (FILE_NAME)
    {
    FILE_NAME = bless (WString, fname, len);
    unless (FILE_NAME)
      return (WARC_TRUE);
    }

  else if (WString_setText (FILE_NAME, fname, len))
    return (WARC_TRUE);

  return (WARC_FALSE);
}



/**
 * @param _self WHeader object
 *
 * @return The Profile field string
 *
 * Returns the WARC Record Profile
 */


WIPUBLIC const warc_u8_t * WHeader_getProfile (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (PROFILE)
    return (NIL);

  return (WString_getText (PROFILE));
}


/**
 * @param _self WHeader object instance
 *
 * @return The Record Profile field string length
 * as a 4 bytes integer
 *
 * Warc Header Profile field string length provider
 */

WIPUBLIC warc_u32_t WHeader_getProfileLen (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (PROFILE)
    return (0);

  return (WString_getLength (PROFILE));
}


/**
 * @param _self WHeader object
 * @param prof Record Profile string
 * @param len WARC Profile string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the WARC Record Profile field
 */

WPUBLIC warc_bool_t WHeader_setProfile (void * const _self,
                                        const warc_u8_t * prof,
                                        const warc_u32_t len)
{

  struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);
  assert (prof);

  unless (PROFILE)
    {
    PROFILE = bless (WString, prof, len);
    unless (PROFILE)
      return (WARC_TRUE);
    }

  else if (WString_setText (PROFILE , prof, len) )
    return (WARC_TRUE);

  return (WARC_FALSE);
}


/**
 * @param _self WHeader object
 *
 * @return The Payload type string
 *
 * Returns the WARC Record Payload Type field
 */

WIPUBLIC const warc_u8_t * WHeader_getPayloadType (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (PAYLOAD_TYPE)
     return (NIL);

  return (WString_getText (PAYLOAD_TYPE));
}


/**
 * @param _self WHeader object instance
 *
 * @return The Record Payload type field string length
 * as a 4 bytes integer
 *
 * Warc Header Payload Type field string length provider
 */

WIPUBLIC warc_u32_t WHeader_getPayloadTypeLen (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (PAYLOAD_TYPE)
     return (0);

  return (WString_getLength (PAYLOAD_TYPE));
}


/**
 * @param _self WHeader object
 * @param paytype Payload type string
 * @param len Payload type string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the WARC Record Payload type field
 */

WPUBLIC warc_bool_t WHeader_setPayloadType (void * const _self,
                                            const warc_u8_t * paytype,
                                            const warc_u32_t len)
{

  struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);
  assert (paytype);

  unless (PAYLOAD_TYPE)
   {
   PAYLOAD_TYPE = bless (WString, paytype, len);
   unless (PAYLOAD_TYPE)
      return (WARC_TRUE);
   }

  else if (WString_setText (PAYLOAD_TYPE, paytype, len) )
    return (WARC_TRUE);

  return (WARC_FALSE);
}


/**
 * @param _self WHeader object
 *
 * @return The SegmentOriginId field string
 *
 * Returns the WARC Record Id origin of this segment Record
 */

WIPUBLIC const warc_u8_t * WHeader_getSegmentOriginId (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (S_ORG_ID)
    return (NIL);

  return (WString_getText (S_ORG_ID));
}


/**
 * @param _self WHeader object instance
 *
 * @return The Origin Warc Record id field string length
 * as a 4 bytes integer
 *
 * Warc Header Origin Warc Record id field string length provider
 */

WIPUBLIC warc_u32_t WHeader_getSegmentOriginIdLen (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  unless (S_ORG_ID)
    return (0);

  return (WString_getLength (S_ORG_ID));
}


/**
 * @param _self WHeader object
 * @param segid Origin Warc Record id constant string
 * @param len Origin Warc Record id string length
 *
 * @return false if succseeds, true otherwise
 *
 * Sets the sgement WARC-Record Origin record id
 */

WPUBLIC warc_bool_t WHeader_setSegmentOriginId (void * const _self,
                                                const warc_u8_t * segid,
                                                const warc_u32_t len)
{

  struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);
  assert (segid);

  unless (S_ORG_ID)
   {
   S_ORG_ID = bless (WString, segid, len);
   unless (S_ORG_ID)
      return (WARC_TRUE);
   }

  else if (WString_setText (S_ORG_ID, segid, len))
    return (WARC_TRUE);

  return (WARC_FALSE);
}


/**
 * @param _self WHeader object
 *
 * @return The segment number field as a 32 bits unsigned integer
 *
 * Returns the Row of a segment Warc Record
 */

WIPUBLIC warc_u32_t WHeader_getSegmentNumber (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (S_NUMBER);
}

/**
 * @param _self WHeader object
 * @param snumber Segment number as a 32 bits unsigned integer
 *
 * @return False if it succeeds, True otherwise
 *
 * Sets the Segment number of a segment Warc Record
 */

WIPUBLIC warc_bool_t WHeader_setSegmentNumber (void * const _self, const warc_u64_t snumber)
{

  struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);
 
/*   if (snumber < 0) */
/*      return (WARC_TRUE); */

  S_NUMBER = snumber;
  return (WARC_FALSE);

}


/**
 * @param _self WHeader object
 *
 * @return The total segmented record size as a 32 bits unsigned integer
 *
 * Returns the total size of a segmented WARC-record (only in the last segment)
 */

WIPUBLIC warc_u32_t WHeader_getSegTotalLength (const void * const _self)
{

  const struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  return (S_TOTAL_LENGTH);
}

/**
 * @param _self WHeader object
 * @param stlength The total size as 32 bits unsigfned integer
 *
 * @return False if it succeeds, True otherwise
 *
 * Sets the Total size of segmented WARC-record (only in the last segment)
 */

WIPUBLIC warc_bool_t WHeader_setSegTotalLength (void * const _self, const warc_u64_t stlength)
{

  struct WHeader * const self = _self;

  /* preconditions */
  CASSERT (self);

  if (stlength <= 0)
     return (WARC_TRUE);

  S_TOTAL_LENGTH = stlength;

  return (WARC_FALSE);

}


/**
 * @param _self: WHeader object instance
 *
 * @return WList object if succeeds, NIL otherwise
 *
 * Returns the list of non predefined fields
 */

WIPUBLIC const void * WHeader_getAnvl (const void* const _self)
{
    const struct WHeader * const self = _self;

  /* Preconditions */
  CASSERT (self);

  return (OTHER_FIELDS);
}


/**
 * @param _self WHeader object instance
 * @param key the key of a non predefined named field as a const char *
 *
 * @return A valid field value correspnding to the key, NIL otherwise
 *
 * Warc Header non predefined named field value corrsponding to a key
 */

WIPUBLIC const warc_u8_t * WHeader_getAnvlValue (const void* const _self,
                                                 const warc_u8_t * key)
{

  const struct WHeader * const self = _self;
  const void * anvl   = NIL;
  warc_u32_t   i      = 0;
  warc_u32_t   lsize;

  /* Preconditions */
  CASSERT (self);

  unless (OTHER_FIELDS)
    return (NIL);

  unless (key)
    return (NIL);

  lsize = WList_size (OTHER_FIELDS);
  while (i < lsize)
    {
      anvl = WList_getElement (OTHER_FIELDS, i);
      unless (w_strcmp (WAnvl_getKey (anvl), key) )
        return (WAnvl_getValue (anvl) );
      else
        ++ i;
    }

  return (NIL);
}


/**
 * @param _self WHeader object instance
 * @param key the key of the named Field as a const char *
 * @param klen the key string length in bytes
 * @param value the value of the named field as a const char *
 * @param vlen the value string length in bytes
 *
 * @return False if succeeds, True otherwise
 *
 * WHeader new non predefined named field adding function
 */

WIPUBLIC warc_bool_t WHeader_addAnvl (void * _self,
                                      const warc_u8_t * key,
                                      const warc_u32_t klen,
                                      const warc_u8_t * value,
                                      const warc_u32_t vlen)
{

  struct WHeader * self = _self;
  void * anvl = NIL;

  /* Preconditions */
  CASSERT (self);

  unless (key)
  return (WARC_TRUE);

  unless (value)
  return (WARC_TRUE);

  anvl = bless (WAnvl, key, klen, value, vlen);

  unless (anvl)
  return (WARC_TRUE);

  return (WList_push (OTHER_FIELDS, anvl) );
}


/**
 * @param rectype: a record type as a string
 * 
 * @return The corrsponding code of the record type string
 *
 * Warc Record Types encoding function for WHeader object
 */

WPRIVATE warc_rec_t WHeader_getRecordTypeNumber (const warc_u8_t * rectype)
{
  /* preconditions */
  assert (rectype);

  if      (! w_strcmp (rectype, (warc_u8_t *) "warcinfo") )
    return (WARC_INFO_RECORD);
  else if (! w_strcmp (rectype, (warc_u8_t *) "response") )
    return (WARC_RESPONSE_RECORD);
  else if (! w_strcmp (rectype, (warc_u8_t *) "request") )
    return (WARC_REQUEST_RECORD);
  else if (! w_strcmp (rectype, (warc_u8_t *) "metadata") )
    return (WARC_METADATA_RECORD);
  else if (! w_strcmp (rectype, (warc_u8_t *) "revisit") )
    return (WARC_REVISIT_RECORD);
  else if (! w_strcmp (rectype, (warc_u8_t *) "conversion") )
    return (WARC_CONVERSION_RECORD);
  else if (! w_strcmp (rectype, (warc_u8_t *) "continuation") )
    return (WARC_CONTINUATION_RECORD);
  else if (! w_strcmp (rectype, (warc_u8_t *) "resource") )
    return (WARC_RESOURCE_RECORD);

  return (WARC_UNKNOWN_RECORD);
}

/**
 * @param initial_s: the initial string which contains spaces in its begining
 * @param initsize: the size of the initial string
 * @param new_s: the new generated string
 * 
 * @return the new string if succeeds, NIL otherwise
 *
 * Gives a string containing the characters of an other string
 * without the spaces in its beginbing
 */

WPRIVATE warc_u8_t * WHeader_jumpFirstSpaces (const warc_u8_t * initial_s, 
                                              warc_u32_t initsize, 
                                              warc_u8_t * new_s)
{
    warc_u32_t    i     = 0 ;
    warc_u32_t    j     = 0;

    while (((initial_s [i] == ' ') || (initial_s[i] == '\t')) && (i < initsize))
      ++ i;
    
    while (i < initsize)
      {
        new_s [j] = initial_s[i];
        i++, j++;
      }
    
    new_s [j] = '\0';
    
    return (new_s);
}



/**
 * @param date: the date string;
 * @param datele: the date string length
 *
 * @return True if succeeds, False otherwise
 * 
 * Checks if a date string is conform to the ISO norm
 */

WPRIVATE warc_bool_t WHeader_checkDate (const warc_u8_t * date, warc_u32_t datelen)
{
    warc_u32_t    i = 0;
    warc_u32_t    j = 0;

   if (datelen != 20)
      return (WARC_FALSE);

   i = datelen-1;

   if ((date [i] != 'z') && (date[i] != 'Z'))
      return (WARC_FALSE);

   i--;

   for (j = 0; j<3; j++)
      {
      if (! isdigit (date[i]) || (! isdigit (date[i-1])))
         return (WARC_FALSE);

     if ((j <2) && (date[i-2] != ':'))
          return (WARC_FALSE);

     else if ((j == 2) && ((date[i-2] != 't') && (date[i-2] != 'T')))
            return (WARC_FALSE);

      i = i-3;
      }

   for (j = 0; j<2; j++)
      {
      if ((! isdigit (date[i])) || (! isdigit(date[i-1])))
         return (WARC_FALSE);

      if (date[i-2] != '-')
         return (WARC_FALSE);

      i = i-3;
      }

   for (j =0; j<4; j++)
       {
       if (! isdigit (date [i]))
          return (WARC_FALSE);

       i--;
       }
return (WARC_TRUE);
}


/**
 * @param ipadd: the IP Address string
 *
 * @return True if correct, false otherwise
 *
 * Checks if an IP Address is correct
 */

WPRIVATE warc_bool_t  WHeader_checkIpAddress (const warc_u8_t * ipadd)
{
#define PORTMAX 65535 /* or whatever appropriate */

  warc_u32_t   rc;
  warc_u32_t   b1, b2, b3, b4, port = 0;
  warc_u8_t    c;

  /* Since this doesn't work with IPv6 addresses, just skip this check.
     wget's print_address will always generate a valid ip. */
  return WARC_TRUE;

  rc = sscanf ( (const char *) ipadd, "%3u.%3u.%3u.%3u:%u%c",
                & b1, & b2, & b3, & b4, & port, & c);

  if (rc != 4 && rc != 5)
    {
      w_fprintf (fprintf (stderr, "error> found IP address: %s\n", (char *) ipadd) );
      /* raise the flag error */
      WarcDebugMsg ("expecting a valid IP address: accept only IPv4 and IPv6 adresses");

      return   WARC_FALSE;
    }


  if ( (b1 | b2 | b3 | b4) > 255 || port > PORTMAX)
    {
      w_fprintf (fprintf (stderr, "error> found IP address: %s\n", (char *) ipadd) );
      /* raise the flag error */
      WarcDebugMsg ("expecting a valid IP address: IP range greater than 255 or port number greater than" PORTMAX);

      return WARC_FALSE;
    }

  if (strspn ( (const char *) ipadd, "0123456789.:") < w_strlen (ipadd))
    {
      w_fprintf (fprintf (stderr, "error> found IP address: %s\n", (char *) ipadd) );
      /* raise the flag error */
      WarcDebugMsg ("expecting a valid IP address: only \"0123456789.:\" are valid");
      
      return WARC_FALSE;
    }

  /* Valid IP address */
  return WARC_TRUE;
}


/**
 * @param digest;l the digest to check
 *
 * @return True if succeeds, False otherwise
 *
 * Checks if a Digest Format is correct
 */

WPRIVATE warc_bool_t WHeader_checkDigest (const warc_u8_t * digest)
{
    warc_u32_t    i    = 0;
    warc_u32_t    size = w_strlen (digest);

  while ((digest[i] != ':') && (i < size))
      i++;
 
  if (i == size)
    return (WARC_FALSE);

    i++;

  while (i < size)
    {
   
    if ((digest[i] <= 31) || (digest[i] == 127))
       return (WARC_FALSE);

    switch (digest[i])
      {
      case '(':
      case '\r':
      case '\n':
      case ')':
      case '<':
      case '>':
      case '@':
      case ',':
      case ';':
      case ':':
      case '\\':
      case '"':
      case '/':
      case '[':
      case ']':
      case '?':
      case '=':
      case '{':
      case '}':
      case ' ':
      case '\t': return (WARC_FALSE);

      default  : break;
      }
    i++;
    }

  return (WARC_TRUE);
}

/**
 * @param trunc: the trucnation to check
 *
 * @return True if succeed, False otherwise
 *
 * Checks the possible trucations causes
 */

WPRIVATE  warc_bool_t  WHeader_checkTruncated (const warc_u8_t * trunc)
{
if (w_strcmp (trunc, makeU ("length")))
   if (w_strcmp (trunc, makeU ("time")))
     if (w_strcmp (trunc, makeU ("disconnect")))
        if (w_strcmp (trunc, makeU ("unspecifed")))
            return WARC_FALSE;
 

return WARC_TRUE;
}


/**
 * @param fname: the filename to check
 * 
 * @return True if succeeds, False otherwise
 *
 * Checks if a file name is correct
 */

WPRIVATE warc_bool_t WHeader_checkFilename (const warc_u8_t * fname)
{
    warc_u32_t   i    = 0;
    warc_u32_t   size = w_strlen (fname);
    warc_u32_t   quotecount = 0;

    while (i < size)
      {
      if ((fname[i] <= 31) || (fname[i] == 127))
         return (WARC_FALSE);
      
      if (fname[i] == '"')
        {
        unless (quotecount)
           quotecount++;
        else
           quotecount = 0;
        }

      i++;
      }

  if (quotecount)
     return (WARC_FALSE);

  return (WARC_TRUE);
}


/**
 * @param _self: WHeadre object instance
 * @param infile: The input Warc File
 *
 * Fills The Warc Header object from the Header of a Warc Record in a Warc File
 */

WPUBLIC warc_bool_t WHeader_extractFromWarcFile (void * _self, FILE * infile)
{
    struct WHeader * self = _self;

    void          *   afsm = NIL       ;
    void          *   anvl = NIL       ;
    warc_bool_t       more = WARC_TRUE ;
    warc_u64_t        digital = 0      ;
    warc_u64_t        datalen = 0      ;
    warc_rec_t        rtype = WARC_UNKNOWN_RECORD;
    warc_u8_t     *   charbuf = NIL;

  /* Preconditions */
  CASSERT (self);

  /* testing the WARC Id Field */
  charbuf = wmalloc (WString_getLength (WARC_ID)+1);
    unless (charbuf)
      {
      WarcDebugMsg ("Can't Allocate memory for WARC_ID");
      return (WARC_TRUE);
      }
  digital = w_fread (charbuf, 1, WString_getLength (WARC_ID), infile);
  charbuf [digital] = '\0';

/*   if (w_strcmp (charbuf, WString_getText (WARC_ID))) */

  /* fprintf(stderr, "\n\n>>>>>> WARC VERSION ID is =====> >%s<\n", charbuf); */

  if ( w_checkCompatibleVersions (charbuf) )
     {
     WarcDebugMsg ("Incompatible Warc Version");
     wfree (charbuf);
     return (WARC_TRUE);
     }
  else /* force WARC_ID to be charbuf anyway */
    {
      if(WString_setText (WARC_ID, makeS(charbuf)))
        {
          WarcDebugMsg ("Cannot copy WARC_VERSION from file");
          wfree (charbuf);
          return (WARC_TRUE);
        }
    }

  /* jumping the CRLF separator */
  w_fread (charbuf, 2, 1, infile);
  wfree (charbuf);

  datalen = datalen + WString_getLength (WARC_ID) + 2;

  /* extracting otrher fields */
    do
    {
      /* init ANVL FSM */
      afsm = bless (WFsmANVL, infile);
      assert (afsm);

      unless (WFsmANVL_run (afsm) )
      {
        anvl = WFsmANVL_transform (afsm);

        if (anvl)
          {
          unless (w_strcmp (WAnvl_getKey(anvl), makeU("WARC-Type")))
             {
              charbuf = wmalloc (WAnvl_getValueLen (anvl) + 1);
              unless (charbuf)
                {
                 WarcDebugMsg ("Can't Allocate memory for working");
                 destroy (anvl);
                 destroy (afsm);
                 return (WARC_TRUE);
                }

              WHeader_jumpFirstSpaces (WAnvl_getValue (anvl), WAnvl_getValueLen (anvl), charbuf);

              unless ((rtype = WHeader_getRecordTypeNumber (charbuf)))
                {
                 WarcDebugMsg ("Unknown Record type found");
                 destroy (anvl);
                 destroy (afsm);
                 wfree (charbuf);
                 return (WARC_TRUE);
                }

              wfree (charbuf);
              if (WHeader_setRecordType (self, rtype))
                 {
                  WarcDebugMsg ("Bad Warc Type");
                  destroy (anvl);
                  destroy (afsm);
                  return (WARC_TRUE);
                 }

              datalen = datalen + WAnvl_getKeyLen (anvl) + WAnvl_getValueLen (anvl) +  3;
              destroy (anvl);
             }
        else unless (w_strcmp (WAnvl_getKey(anvl), makeU("WARC-Record-ID")))
             {
              charbuf = wmalloc (WAnvl_getValueLen (anvl) + 1);
              unless (charbuf)
                {
                 WarcDebugMsg ("Can't Allocate memory for working");
                 destroy (anvl);
                 destroy (afsm);
                 return (WARC_TRUE);
                }

              WHeader_jumpFirstSpaces (WAnvl_getValue (anvl), WAnvl_getValueLen (anvl), charbuf);

              if (WHeader_setRecordId (self, makeS(charbuf)))
                 {
                  WarcDebugMsg ("Bad Warc Record Id");
                  destroy (anvl);
                  destroy (afsm);
                  wfree (charbuf);
                  return (WARC_TRUE);
                  }
              wfree (charbuf);
              datalen = datalen + WAnvl_getKeyLen (anvl) + WAnvl_getValueLen (anvl) +  3;
              destroy (anvl);
             }
        else unless (w_strcmp (WAnvl_getKey(anvl), makeU("WARC-Date")))
             {
              charbuf = wmalloc (WAnvl_getValueLen (anvl) + 1);
              unless (charbuf)
                {
                 WarcDebugMsg ("Can't Allocate memory for working");
                 destroy (anvl);
                 destroy (afsm);
                 return (WARC_TRUE);
                }

              WHeader_jumpFirstSpaces (WAnvl_getValue (anvl), WAnvl_getValueLen (anvl), charbuf);

              unless (WHeader_checkDate (makeS(charbuf)))
                 {
                  WarcDebugMsg ("Incorrect Date format ");
                  destroy (anvl);
                  destroy (afsm);
                  wfree (charbuf);
                  return (WARC_TRUE);
                 }

              if (WHeader_setCreationDate (self, makeS (charbuf)))
                 {
                  WarcDebugMsg ("Bad Warc Date");
                  destroy (anvl);
                  destroy (afsm);
                  wfree (charbuf);
                  return (WARC_TRUE);
                 }

              wfree (charbuf);
              datalen = datalen + WAnvl_getKeyLen (anvl) + WAnvl_getValueLen (anvl) +  3;
              destroy (anvl);
             }
        else unless (w_strcmp (WAnvl_getKey(anvl), makeU("Content-Length")))
             {
              charbuf = wmalloc (WAnvl_getValueLen (anvl) + 1);
              unless (charbuf)
                {
                 WarcDebugMsg ("Can't Allocate memory for working");
                 destroy (anvl);
                 destroy (afsm);
                 return (WARC_TRUE);
                }

              WHeader_jumpFirstSpaces (WAnvl_getValue (anvl), WAnvl_getValueLen (anvl), charbuf);

              unless (w_check_digital_string (makeS(charbuf)))
                {
                 WarcDebugMsg ("Ivalid Content length string");
                 destroy (afsm);
                 wfree (charbuf);
                 destroy (anvl);
                 return (WARC_TRUE);
                }
              digital = 0;
              w_atou (makeS (charbuf), & digital);

              wfree (charbuf);
              if (WHeader_setContentLength (self, digital))
                 {
                  WarcDebugMsg ("Bad Content Length");
                  destroy (anvl);
                  destroy (afsm);
                  return (WARC_TRUE);
                 }
              datalen = datalen + WAnvl_getKeyLen (anvl) + WAnvl_getValueLen (anvl) +  3;
              destroy (anvl);
             }
        else unless (w_strcmp (WAnvl_getKey(anvl), makeU("Content-Type")))
             {
              charbuf = wmalloc (WAnvl_getValueLen (anvl) + 1);
              unless (charbuf)
                {
                 WarcDebugMsg ("Can't Allocate memory for working");
                 destroy (anvl);
                 destroy (afsm);
                 return (WARC_TRUE);
                }

              WHeader_jumpFirstSpaces (WAnvl_getValue (anvl), WAnvl_getValueLen (anvl), charbuf);

              if (WHeader_setContentType (self, makeS (charbuf)))
                 {
                  WarcDebugMsg ("Bad Warc Type");
                  destroy (anvl);
                  wfree (charbuf);
                  destroy (afsm);
                  return (WARC_TRUE);
                 }
              wfree (charbuf);
              datalen = datalen + WAnvl_getKeyLen (anvl) + WAnvl_getValueLen (anvl) +  3;
              destroy (anvl);
             }
        else unless (w_strcmp (WAnvl_getKey(anvl), makeU("WARC-Concurrent-To")))
             {
              charbuf = wmalloc (WAnvl_getValueLen (anvl) + 1);
              unless (charbuf)
                {
                 WarcDebugMsg ("Can't Allocate memory for working");
                 destroy (anvl);
                 destroy (afsm);
                 return (WARC_TRUE);
                }

              WHeader_jumpFirstSpaces (WAnvl_getValue (anvl), WAnvl_getValueLen (anvl), charbuf);

              if (WHeader_setConcurrentTo (self, makeS (charbuf)))
                 {
                  WarcDebugMsg ("Bad Concurrent Warc Record Id");
                  destroy (anvl);
                  destroy (afsm);
                  wfree (charbuf);
                  return (WARC_TRUE);
                  }
             wfree (charbuf);
              datalen = datalen + WAnvl_getKeyLen (anvl) + WAnvl_getValueLen (anvl) +  3;
             destroy (anvl);
             }
        else unless (w_strcmp (WAnvl_getKey(anvl), makeU("WARC-Block-Digest")))
             {
              charbuf = wmalloc (WAnvl_getValueLen (anvl) + 1);
              unless (charbuf)
                {
                 WarcDebugMsg ("Can't Allocate memory for working");
                 destroy (anvl);
                 destroy (afsm);
                 return (WARC_TRUE);
                }

              WHeader_jumpFirstSpaces (WAnvl_getValue (anvl), WAnvl_getValueLen (anvl), charbuf);

              unless (WHeader_checkDigest (charbuf))
                {
                  WarcDebugMsg ("Bad Digest format");
                  destroy (anvl);
                  destroy (afsm);
                  wfree (charbuf);
                  return (WARC_TRUE);
                }

              if (WHeader_setBlockDigest (self, makeS (charbuf)))
                 {
                  WarcDebugMsg ("Bad Digest string");
                  destroy (anvl);
                  destroy (afsm);
                  wfree (charbuf);
                  return (WARC_TRUE);
                  }
              wfree (charbuf);
              datalen = datalen + WAnvl_getKeyLen (anvl) + WAnvl_getValueLen (anvl) +  3;
              destroy (anvl);
             }
        else unless (w_strcmp (WAnvl_getKey(anvl), makeU("WARC-Payload-Digest")))
             {
              charbuf = wmalloc (WAnvl_getValueLen (anvl) + 1);
              unless (charbuf)
                {
                 WarcDebugMsg ("Can't Allocate memory for working");
                 destroy (anvl);
                 destroy (afsm);
                 return (WARC_TRUE);
                }

              WHeader_jumpFirstSpaces (WAnvl_getValue (anvl), WAnvl_getValueLen (anvl), charbuf);

              unless (WHeader_checkDigest (charbuf))
                {
                  WarcDebugMsg ("Bad Digest format");
                  destroy (anvl);
                  destroy (afsm);
                  wfree (charbuf);
                  return (WARC_TRUE);
                }

              if (WHeader_setPayloadDigest (self, makeS (charbuf)))
                 {
                  WarcDebugMsg ("Bad Digest string");
                  destroy (anvl);
                  destroy (afsm);
                  wfree (charbuf);
                  return (WARC_TRUE);
                  }

             wfree (charbuf);
              datalen = datalen + WAnvl_getKeyLen (anvl) + WAnvl_getValueLen (anvl) +  3;
             destroy (anvl);
             }
        else unless (w_strcmp (WAnvl_getKey(anvl), makeU("WARC-IP-Address")))
             {
              charbuf = wmalloc (WAnvl_getValueLen (anvl) + 1);
              unless (charbuf)
                {
                 WarcDebugMsg ("Can't Allocate memory for working");
                 destroy (anvl);
                 destroy (afsm);
                 return (WARC_TRUE);
                }

              WHeader_jumpFirstSpaces (WAnvl_getValue (anvl), WAnvl_getValueLen (anvl), charbuf);

              unless (WHeader_checkIpAddress (charbuf))
                 {
                  WarcDebugMsg ("Bad Ip address format");
                  destroy (anvl);
                  destroy (afsm);
                  wfree (charbuf);
                  return (WARC_TRUE);
                 }

              if (WHeader_setIpAddress (self, makeS (charbuf)))
                 {
                  WarcDebugMsg ("Bad Ip address string");
                  destroy (anvl);
                  destroy (afsm);
                  wfree (charbuf);
                  return (WARC_TRUE);
                  }

              wfree (charbuf);
              datalen = datalen + WAnvl_getKeyLen (anvl) + WAnvl_getValueLen (anvl) +  3;
              destroy (anvl);
             }
        else unless (w_strcmp (WAnvl_getKey(anvl), makeU("WARC-Refers-To")))
             {
              charbuf = wmalloc (WAnvl_getValueLen (anvl) + 1);
              unless (charbuf)
                {
                 WarcDebugMsg ("Can't Allocate memory for working");
                 destroy (anvl);
                 destroy (afsm);
                 return (WARC_TRUE);
                }

              WHeader_jumpFirstSpaces (WAnvl_getValue (anvl), WAnvl_getValueLen (anvl), charbuf);

              if (WHeader_setRefersTo (self, makeS (charbuf)))
                 {
                  WarcDebugMsg ("Bad Referd Warc Record Id");
                  destroy (anvl);
                  destroy (afsm);
                  wfree (charbuf);
                  return (WARC_TRUE);
                  }

              wfree (charbuf);
              datalen = datalen + WAnvl_getKeyLen (anvl) + WAnvl_getValueLen (anvl) +  3;
             destroy (anvl);
             }
        else unless (w_strcmp (WAnvl_getKey(anvl), makeU("WARC-Target-URI")))
             {
              charbuf = wmalloc (WAnvl_getValueLen (anvl) + 1);
              unless (charbuf)
                {
                 WarcDebugMsg ("Can't Allocate memory for working");
                 destroy (anvl);
                 destroy (afsm);
                 return (WARC_TRUE);
                }

              WHeader_jumpFirstSpaces (WAnvl_getValue (anvl), WAnvl_getValueLen (anvl), charbuf);

              if (WHeader_setSubjectUri (self, makeS (charbuf)))
                 {
                  WarcDebugMsg ("Bad Uri string");
                  destroy (anvl);
                  destroy (afsm);
                  wfree (charbuf);
                  return (WARC_TRUE);
                  }

               wfree (charbuf);
              datalen = datalen + WAnvl_getKeyLen (anvl) + WAnvl_getValueLen (anvl) +  3;
               destroy (anvl);
             }
        else unless (w_strcmp (WAnvl_getKey(anvl), makeU("WARC-Truncated")))
             {
              charbuf = wmalloc (WAnvl_getValueLen (anvl) + 1);
              unless (charbuf)
                {
                 WarcDebugMsg ("Can't Allocate memory for working");
                 destroy (anvl);
                 destroy (afsm);
                 return (WARC_TRUE);
                }

              WHeader_jumpFirstSpaces (WAnvl_getValue (anvl), WAnvl_getValueLen (anvl), charbuf);

              unless (WHeader_checkTruncated (charbuf))
                {
                  WarcDebugMsg ("Bad Truncation cause value");
                  destroy (anvl);
                  destroy (afsm);
                  wfree (charbuf);
                  return (WARC_TRUE);
                }

              if (WHeader_setTruncated (self, makeS (charbuf)))
                 {
                  WarcDebugMsg ("Bad Truncation cause string");
                  destroy (anvl);
                  destroy (afsm);
                  wfree (charbuf);
                  return (WARC_TRUE);
                  }

              wfree (charbuf);
              datalen = datalen + WAnvl_getKeyLen (anvl) + WAnvl_getValueLen (anvl) +  3;
              destroy (anvl);
             }
        else unless (w_strcmp (WAnvl_getKey(anvl), makeU("WARC-Warcinfo-ID")))
             {
              charbuf = wmalloc (WAnvl_getValueLen (anvl) + 1);
              unless (charbuf)
                {
                 WarcDebugMsg ("Can't Allocate memory for working");
                 destroy (anvl);
                 destroy (afsm);
                 return (WARC_TRUE);
                }

              WHeader_jumpFirstSpaces (WAnvl_getValue (anvl), WAnvl_getValueLen (anvl), charbuf);

              if (WHeader_setInfoId (self, makeS (charbuf)))
                 {
                  WarcDebugMsg ("Bad Refered Warc Info Record Id string");
                  destroy (anvl);
                  destroy (afsm);
                  wfree (charbuf);
                  return (WARC_TRUE);
                  }

             wfree (charbuf);
              datalen = datalen + WAnvl_getKeyLen (anvl) + WAnvl_getValueLen (anvl) +  3;
             destroy (anvl);
             }
        else unless (w_strcmp (WAnvl_getKey(anvl), makeU("WARC-Filename")))
             {
              charbuf = wmalloc (WAnvl_getValueLen (anvl) + 1);
              unless (charbuf)
                {
                 WarcDebugMsg ("Can't Allocate memory for working");
                 destroy (anvl);
                 destroy (afsm);
                 return (WARC_TRUE);
                }

              WHeader_jumpFirstSpaces (WAnvl_getValue (anvl),
                                       WAnvl_getValueLen (anvl), charbuf);

              unless (WHeader_checkFilename (charbuf))
                 {
                  WarcDebugMsg ("Bad File name Format");
                  destroy (anvl);
                  destroy (afsm);
                  wfree (charbuf);
                  return (WARC_TRUE);
                 }

              if (WHeader_setFilename (self, makeS (charbuf)))
                 {
                  WarcDebugMsg ("Bad File name string");
                  destroy (anvl);
                  destroy (afsm);
                  wfree (charbuf);
                  return (WARC_TRUE);
                  }

              wfree (charbuf);
              datalen = datalen + WAnvl_getKeyLen (anvl) + 
                WAnvl_getValueLen (anvl) +  3;

              destroy (anvl);
             }
        else unless (w_strcmp (WAnvl_getKey(anvl), makeU("WARC-Profile")))
             {
              charbuf = wmalloc (WAnvl_getValueLen (anvl) + 1);
              unless (charbuf)
                {
                 WarcDebugMsg ("Can't Allocate memory for working");
                 destroy (anvl);
                 destroy (afsm);
                 return (WARC_TRUE);
                }

              WHeader_jumpFirstSpaces (WAnvl_getValue (anvl), 
                                       WAnvl_getValueLen (anvl), charbuf);

              if (WHeader_setProfile (self, makeS (charbuf)))
                 {
                  WarcDebugMsg ("Bad Profile string");
                  destroy (anvl);
                  destroy (afsm);
                  wfree (charbuf);
                  return (WARC_TRUE);
                  }

              wfree (charbuf);
              datalen = datalen + WAnvl_getKeyLen (anvl) + WAnvl_getValueLen (anvl) +  3;
              destroy (anvl);
             }
        else unless (w_strcmp (WAnvl_getKey(anvl), makeU("WARC-Identified-Payload-Type")))
             {
              charbuf = wmalloc (WAnvl_getValueLen (anvl) + 1);
              unless (charbuf)
                {
                 WarcDebugMsg ("Can't Allocate memory for working");
                 destroy (anvl);
                 destroy (afsm);
                 return (WARC_TRUE);
                }

              WHeader_jumpFirstSpaces (WAnvl_getValue (anvl), WAnvl_getValueLen (anvl), charbuf);

              if (WHeader_setPayloadType (self, makeS (charbuf)))
                 {
                  WarcDebugMsg ("Bad payload type string");
                  destroy (anvl);
                  destroy (afsm);
                  wfree (charbuf);
                  return (WARC_TRUE);
                  }

              wfree (charbuf);
              datalen = datalen + WAnvl_getKeyLen (anvl) + WAnvl_getValueLen (anvl) +  3;
              destroy (anvl);
             }
        else unless (w_strcmp (WAnvl_getKey(anvl), makeU("WARC-Segment-Origin-ID")))
             {
              charbuf = wmalloc (WAnvl_getValueLen (anvl) + 1);
              unless (charbuf)
                {
                 WarcDebugMsg ("Can't Allocate memory for working");
                 destroy (anvl);
                 destroy (afsm);
                 return (WARC_TRUE);
                }

              WHeader_jumpFirstSpaces (WAnvl_getValue (anvl), WAnvl_getValueLen (anvl), charbuf);

              if (WHeader_setSegmentOriginId (self, makeS (charbuf)))
                 {
                  WarcDebugMsg ("Bad Origin Warc Record id  string");
                  destroy (anvl);
                  destroy (afsm);
                  wfree (charbuf);
                  return (WARC_TRUE);
                  }

              wfree (charbuf);
              datalen = datalen + WAnvl_getKeyLen (anvl) + WAnvl_getValueLen (anvl) +  3;
              destroy (anvl);
             }
        else unless (w_strcmp (WAnvl_getKey(anvl), makeU("WARC-Segment-Number")))
             {
              charbuf = wmalloc (WAnvl_getValueLen (anvl) + 1);
              unless (charbuf)
                {
                 WarcDebugMsg ("Can't Allocate memory for working");
                 destroy (anvl);
                 destroy (afsm);
                 return (WARC_TRUE);
                }

              WHeader_jumpFirstSpaces (WAnvl_getValue (anvl), WAnvl_getValueLen (anvl), charbuf);

              unless (w_check_digital_string (makeS (charbuf)))
                {
                 WarcDebugMsg ("Ivalid Segment Number string");
                 destroy (anvl);
                 destroy (afsm);
                 wfree (charbuf);
                 return (WARC_TRUE);
                }
              wfree (charbuf);

              digital = 0;
              w_atou (WAnvl_getValue (anvl), WAnvl_getValueLen (anvl), 
                      & digital);

              if (WHeader_setSegmentNumber (self, digital))
                 {
                  WarcDebugMsg ("Bad segment Number");
                  destroy (anvl);
                  destroy (afsm);
                  return (WARC_TRUE);
                 }

              datalen = datalen + WAnvl_getKeyLen (anvl) + WAnvl_getValueLen (anvl) +  3;
              destroy (anvl);
             }
        else unless (w_strcmp (WAnvl_getKey(anvl), makeU("WARC-Segment-Total-Length")))
             {
              charbuf = wmalloc (WAnvl_getValueLen (anvl) + 1);
              unless (charbuf)
                {
                 WarcDebugMsg ("Can't Allocate memory for working");
                 destroy (anvl);
                 destroy (afsm);
                 return (WARC_TRUE);
                }

              WHeader_jumpFirstSpaces (WAnvl_getValue (anvl), WAnvl_getValueLen (anvl), charbuf);

              unless (w_check_digital_string (makeS (charbuf)))
                {
                 WarcDebugMsg ("Ivalid Total Length string");
                 destroy (anvl);
                 destroy (afsm);
                 wfree (charbuf);
                 return (WARC_TRUE);
                }
              wfree (charbuf);

              digital = 0;
              w_atou (WAnvl_getValue (anvl), WAnvl_getValueLen (anvl), 
                      & digital);

              if (WHeader_setSegTotalLength (self, digital))
                 {
                  WarcDebugMsg ("Bad total Length Number");
                  destroy (anvl);
                  destroy (afsm);
                  return (WARC_TRUE);
                  }

              datalen = datalen + WAnvl_getKeyLen (anvl) + WAnvl_getValueLen (anvl) +  3;
              destroy (anvl);
             }
           else
             {
              datalen = datalen + WAnvl_getKeyLen (anvl) + WAnvl_getValueLen (anvl) +  3;
              WList_push (OTHER_FIELDS, anvl);
             }
          }
        else
          more = WARC_FALSE;
      }
      else
        {
          warc_u64_t off;
          w_ftell (infile, off);
          /* error when parsing the WARC header line */
          w_fprintf (fprintf (stderr , __FILE__ " (line %d) error in FSM state address %p, at offset %llu in the Warc file \n",
                              __LINE__, WFsmANVL_state (afsm), (long long unsigned int) off ) );

          destroy (afsm);
          return  (WARC_TRUE);
        }

      destroy (afsm);

    }

  while (more);

  datalen = datalen + 2 + CONT_LENGTH;

  if (WHeader_setDataLength (self, datalen))
     return (WARC_TRUE);

  return (WARC_FALSE);
}

/**
 * @param _self WHeader object instance
 *
 * @return none
 *
 * Free WHeader's object memory
 */

WPRIVATE void freeWHeader (void * _self)
{

  struct WHeader * self = _self;

  /* preconditions */
  CASSERT (self);

  if (WARC_ID)
    destroy (WARC_ID), WARC_ID = NIL;

  DATA_LENGTH = 0;

  RECORD_TYPE = WARC_UNKNOWN_RECORD;

  if (SUBJECT_URI)
    destroy (SUBJECT_URI), SUBJECT_URI = NIL;

  if (CREATION_DATE)
    destroy (CREATION_DATE), CREATION_DATE = NIL;

  if (CONTENT_TYPE)
    destroy (CONTENT_TYPE), CONTENT_TYPE = NIL;

  if (RECORD_ID)
    destroy (RECORD_ID), RECORD_ID = NIL;

  if (CONT_LENGTH)
     CONT_LENGTH = 0;

  if (CONC_TO)
     destroy (CONC_TO), CONC_TO = NIL;

  if (BLOCK_DIG)
     destroy (BLOCK_DIG), BLOCK_DIG = NIL;

  if (PAYLOAD_DIG)
     destroy (PAYLOAD_DIG), PAYLOAD_DIG = NIL;

  if (IP_ADDRESS)
     destroy (IP_ADDRESS), IP_ADDRESS = NIL;

  if (REFERS_TO)
     destroy (REFERS_TO), REFERS_TO = NIL;

  if (TRUNCATED)
     destroy (TRUNCATED), TRUNCATED = NIL;

  if (INFO_ID)
     destroy (INFO_ID), INFO_ID = NIL;

  if (FILE_NAME)
     destroy (FILE_NAME), FILE_NAME = NIL;

  if (PROFILE)
     destroy (PROFILE), PROFILE = NIL;

  if (PAYLOAD_TYPE)
      destroy (PAYLOAD_TYPE), PAYLOAD_TYPE = NIL;

  if (S_ORG_ID)
      destroy (S_ORG_ID), S_ORG_ID = NIL;

  if (S_NUMBER)
      S_NUMBER = 0;

  if (S_TOTAL_LENGTH)
     S_TOTAL_LENGTH = 0;


  if (OTHER_FIELDS)
     destroy (OTHER_FIELDS), OTHER_FIELDS = NIL;

}



/**
 * WHeader_constructor - create a new WDLine object instance
 *
 * @param _self WHeader class object
 * @param app constructor list parameters
 *
 * @return a valid WDLine object or NIL
 *
 * WARC Header Line constructor
 */

WPRIVATE void * WHeader_constructor (void * const _self, va_list * app)
{

  struct WHeader * const   self  = _self;

  const warc_rec_t         rt    = va_arg (* app, const warc_rec_t);

  const char             * rid   = va_arg (* app, const char *);
  const warc_u32_t         rid_l = va_arg (* app, const warc_u32_t);

  const char             * cd    = va_arg (* app, const char *);
  const warc_u32_t         cd_l  = va_arg (* app, const warc_u32_t);

  WARC_ID = bless (WString, makeS ((const warc_u8_t *) WARC_VERSION));
  unless (WARC_ID)
  {
    freeWHeader (self);
    return (NIL);
  }

  CONT_LENGTH = 0;

  RECORD_TYPE = rt;

  SUBJECT_URI = NIL;

  CREATION_DATE = bless (WString, cd, cd_l);
  unless (CREATION_DATE)
    {
     destroy (self);
     return (NIL);
    }

  CONTENT_TYPE = NIL;

  RECORD_ID = bless (WString, rid, rid_l);
  unless (RECORD_ID)
    {
     destroy (self);
     return (NIL);
    }

  OTHER_FIELDS = bless (WList);

  unless (OTHER_FIELDS)
    {
     destroy (self);
     return (NIL);
    }

  DATA_LENGTH = 0;

  CONC_TO = NIL;

  BLOCK_DIG = NIL;

  PAYLOAD_DIG = NIL;

  IP_ADDRESS = NIL;

  REFERS_TO = NIL;

  TRUNCATED = NIL;

  INFO_ID = NIL;

  FILE_NAME = NIL;

  PROFILE = NIL;

  PAYLOAD_TYPE = NIL;

  S_ORG_ID = NIL;

  S_NUMBER = 0;

  S_TOTAL_LENGTH = 0;


  return self;
}


/**
 * WHeader_destructor - delete an existing WString object
 *
 * @param _self WDLine object instance
 *
 * WARC Header Line destructor
 */

WPRIVATE void * WHeader_destructor (void * _self)
{
  freeWHeader (_self);

  return (_self);
}


/**
 * WARC WHeader class
 */

static const struct Class _WHeader =
  {
    sizeof (struct WHeader),
    SIGN,
    WHeader_constructor, WHeader_destructor
  };

const void * WHeader = & _WHeader;
