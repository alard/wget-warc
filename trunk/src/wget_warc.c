/* Utility functions for writing WARC files. */
#define _GNU_SOURCE

#include "wget.h"
#include "hash.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <uuid/uuid.h>
#include <libgen.h>
#include <tmpdir.h>
#include <sha1.h>
#include <base32.h>
#include <warc.h>

#include "wget_warc.h"

extern char *version_string;

/* Set by main in main.c */
extern char *program_argstring;


/* The log file (a temporary file that contains a copy
   of the wget log). */
static void *warc_log_fp;

/* The manifest file (a temporary file that contains the
   warcinfo uuid of every file in this crawl). */
static void *warc_manifest_fp;

/* The current WARC file (or NULL, if WARC is disabled).
   This is a pointer to a WFile object. */
static void *warc_current_wfile;

/* The current CDX file (or NULL, if CDX is disabled). */
static FILE *warc_current_cdx_file;

/* The record id of the warcinfo record of the current WARC file.  */
static char *warc_current_winfo_uuid_str;

/* The file name of the current WARC file. */
static char *warc_current_filename;

/* The serial number of the current WARC file.  This number is
   incremented each time a new file is opened and is used in the
   WARC file's filename. */
static int  warc_current_file_number;

/* The table of CDX records, if deduplication is enabled. */
struct hash_table * warc_cdx_dedup_table;



struct warc_cdx_record
{
  char *url;
  char *uuid;
  char digest[SHA1_DIGEST_SIZE];
};

static unsigned long
warc_hash_sha1_digest (const void *key)
{
  /* We just use some of the first bytes of the digest. */
  unsigned long v = 0;
  memcpy (&v, key, sizeof (unsigned long));
  return v;
}

static int
warc_cmp_sha1_digest (const void *digest1, const void *digest2)
{
  return !memcmp (digest1, digest2, SHA1_DIGEST_SIZE);
}




/* Helper functions to fill WRecord objects.
   The warctools library uses its own types for strings and integers.  */
#define WARC_WRAP_METHOD(name)                                          \
    static bool warc_##name(void *record, char *u8_string)              \
    {                                                                   \
      if (u8_string == NULL)                                            \
        return true;                                                    \
      warc_u8_t *wu8_string = (warc_u8_t *)u8_string;                   \
      return WRecord_##name(record, wu8_string, w_strlen(wu8_string));  \
    }

WARC_WRAP_METHOD (setTargetUri)
WARC_WRAP_METHOD (setContentType)
WARC_WRAP_METHOD (setRecordId)
WARC_WRAP_METHOD (setFilename)
WARC_WRAP_METHOD (setConcurrentTo)
WARC_WRAP_METHOD (setWarcInfoId)
WARC_WRAP_METHOD (setRefersTo)
WARC_WRAP_METHOD (setProfile)
WARC_WRAP_METHOD (setTruncated)

static bool
warc_setRecordType (void *record, const warc_rec_t t)
{
  return WRecord_setRecordType (record, t);
}

/* Set the date header of the WARC record to the given timestamp string.
   The current time will be used if timestamp is NULL. */
static bool
warc_setDate (void *record, char *timestamp)
{
  if (timestamp == NULL)
    {
      timestamp = alloca (21);
      warc_timestamp (timestamp);
    }
  return WRecord_setDate (record, (warc_u8_t *)timestamp, w_strlen ((warc_u8_t *)timestamp));
}

/* Set the ip address header of the WARC record, unless ip is NULL. */
static bool
warc_setIpAddress (void *record, ip_address *ip)
{
  bool result = true;
  if (ip != NULL)
  {
    warc_u8_t *ip_str = (warc_u8_t *)(strdup (print_address (ip)));
    result = WRecord_setIpAddress (record, ip_str, w_strlen (ip_str));
    free (ip_str);
  }
  return result;
}

/* Use the contents of file as the body of the WARC record.
   Note: calling  destroy (record)  will also close the file. */
static bool
warc_setContentFromFile (void *record, FILE *file)
{
  return WRecord_setContentFromFile (record, file);
}

/* Set the block digest header of the WARC record.
   sha1_digest is an SHA1 digest of SHA1_DIGEST_SIZE bytes. */
static bool
warc_setBlockDigest (void *record, unsigned char *sha1_digest)
{
  if (sha1_digest != NULL)
    {
      unsigned char sha1_base32[BASE32_LENGTH(SHA1_DIGEST_SIZE) + 1 + 5]; // "sha1:" + digest + "\0"
      base32_encode ((char *) sha1_digest, SHA1_DIGEST_SIZE, (char *) sha1_base32 + 5, BASE32_LENGTH(SHA1_DIGEST_SIZE) + 1);
      memcpy (sha1_base32, "sha1:", 5);
      sha1_base32[BASE32_LENGTH(SHA1_DIGEST_SIZE) + 5] = '\0';
      return WRecord_setBlockDigest (record, sha1_base32, w_strlen (sha1_base32));
    }
  return true;
}

/* Set the payload digest header of the WARC record.
   sha1_digest is an SHA1 digest of SHA1_DIGEST_SIZE bytes. */
static bool
warc_setPayloadDigest (void *record, unsigned char *sha1_digest)
{
  if (sha1_digest != NULL)
    {
      unsigned char sha1_base32[BASE32_LENGTH(SHA1_DIGEST_SIZE) + 1 + 5]; // "sha1:" + digest + "\0"
      base32_encode ((char *) sha1_digest, SHA1_DIGEST_SIZE, (char *) sha1_base32 + 5, BASE32_LENGTH(SHA1_DIGEST_SIZE) + 1);
      memcpy (sha1_base32, "sha1:", 5);
      sha1_base32[BASE32_LENGTH(SHA1_DIGEST_SIZE) + 5] = '\0';
      return WRecord_setPayloadDigest (record, sha1_base32, w_strlen (sha1_base32));
    }
  return true;
}


/* warc_sha1_stream_with_payload is a modified copy of sha1_stream
   from gnulib/sha1.c.  This version calculates two digests in one go.

   Compute SHA1 message digests for bytes read from STREAM.  The
   digest of the complete file will be written into the 16 bytes
   beginning at RES_BLOCK.
   
   If payload_offset >= 0, a second digest will be calculated of the
   portion of the file starting at payload_offset and continuing to
   the end of the file.  The digest number will be written into the
   16 bytes beginning ad RES_PAYLOAD.  */
static int
warc_sha1_stream_with_payload (FILE *stream, void *res_block, void *res_payload, long int payload_offset)
{
#define BLOCKSIZE 32768

  struct sha1_ctx ctx_block;
  struct sha1_ctx ctx_payload;
  long int pos;
  size_t sum;

  char *buffer = malloc (BLOCKSIZE + 72);
  if (!buffer)
    return 1;

  /* Initialize the computation context.  */
  sha1_init_ctx (&ctx_block);
  if (payload_offset >= 0)
    sha1_init_ctx (&ctx_payload);

  pos = 0;

  /* Iterate over full file contents.  */
  while (1)
    {
      /* We read the file in blocks of BLOCKSIZE bytes.  One call of the
         computation function processes the whole buffer so that with the
         next round of the loop another block can be read.  */
      size_t n;
      sum = 0;

      /* Read block.  Take care for partial reads.  */
      while (1)
        {
          n = fread (buffer + sum, 1, BLOCKSIZE - sum, stream);

          sum += n;
          pos += n;

          if (sum == BLOCKSIZE)
            break;

          if (n == 0)
            {
              /* Check for the error flag IFF N == 0, so that we don't
                 exit the loop after a partial read due to e.g., EAGAIN
                 or EWOULDBLOCK.  */
              if (ferror (stream))
                {
                  free (buffer);
                  return 1;
                }
              goto process_partial_block;
            }

          /* We've read at least one byte, so ignore errors.  But always
             check for EOF, since feof may be true even though N > 0.
             Otherwise, we could end up calling fread after EOF.  */
          if (feof (stream))
            goto process_partial_block;
        }

      /* Process buffer with BLOCKSIZE bytes.  Note that
                        BLOCKSIZE % 64 == 0
       */
      sha1_process_block (buffer, BLOCKSIZE, &ctx_block);
      if (payload_offset >= 0 && payload_offset < pos)
        {
          /* At least part of the buffer contains data from payload. */
          int start_of_payload = payload_offset - (pos - BLOCKSIZE);
          if (start_of_payload <= 0)
            /* All bytes in the buffer belong to the payload. */
            start_of_payload = 0;

          /* Process the payload part of the buffer.
             Note: we can't use  sha1_process_block  here even if we
             process the complete buffer.  Because the payload doesn't
             have to start with a full block, there may still be some
             bytes left from the previous buffer.  Therefore, we need
             to continue with  sha1_process_bytes.  */
          sha1_process_bytes (buffer + start_of_payload, BLOCKSIZE - start_of_payload, &ctx_payload);
        }
    }

 process_partial_block:;

  /* Process any remaining bytes.  */
  if (sum > 0)
    {
      sha1_process_bytes (buffer, sum, &ctx_block);
      if (payload_offset >= 0 && payload_offset < pos)
        {
          /* At least part of the buffer contains data from payload. */
          int start_of_payload = payload_offset - (pos - sum);
          if (start_of_payload <= 0)
            /* All bytes in the buffer belong to the payload. */
            start_of_payload = 0;

          /* Process the payload part of the buffer. */
          sha1_process_bytes (buffer + start_of_payload, sum - start_of_payload, &ctx_payload);
        }
    }

  /* Construct result in desired memory.  */
  sha1_finish_ctx (&ctx_block,   res_block);
  if (payload_offset >= 0)
    sha1_finish_ctx (&ctx_payload, res_payload);
  free (buffer);
  return 0;

#undef BLOCKSIZE
}


/* Sets the digest headers of the record.
   This method will calculate the block digest and, if payload_offset >= 0,
   will also calculate the payload digest of the payload starting at the
   provided offset.  */
void
warc_calc_digests (void *record, FILE *file, long payload_offset)
{
  if (opt.warc_digests_enabled)
    {
      /* Calculate the block and payload digests. */
      unsigned char sha1_res_block[SHA1_DIGEST_SIZE];
      unsigned char sha1_res_payload[SHA1_DIGEST_SIZE];

      rewind (file);
      if (warc_sha1_stream_with_payload (file, sha1_res_block, sha1_res_payload, payload_offset) == 0)
        {
          warc_setBlockDigest (record, sha1_res_block);

          if (payload_offset >= 0)
            {
              warc_setPayloadDigest (record, sha1_res_payload);
            }
        }
    }
}


/* Fills timestamp with the current time and date.
   The UTC time is formatted following ISO 8601, as required
   for use in the WARC-Date header.
   The timestamp will be 21 characters long. */
void
warc_timestamp (char *timestamp)
{
  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = gmtime (&rawtime);
  strftime (timestamp, 21, "%Y-%m-%dT%H:%M:%SZ", timeinfo);
}

/* Fills urn_str with a UUID in the format required
   for the WARC-Record-Id header.
   The string will be 47 characters long. */
void
warc_uuid_str (char *urn_str)
{
  uuid_t record_id;
  uuid_generate (record_id);
  char uuid_str[37];
  uuid_unparse (record_id, uuid_str);
  sprintf(urn_str, "<urn:uuid:%s>", uuid_str);
}

/* Write a warcinfo record to the current file.
   Updates warc_current_winfo_uuid_str. */
bool
warc_write_warcinfo_record (char * filename)
{
  /* Write warc-info record as the first record of the file. */
  /* We add the record id of this info record to the other records in the file. */
  warc_current_winfo_uuid_str = (char *) malloc (48);
  warc_uuid_str (warc_current_winfo_uuid_str);

  char *filename_copy, *filename_basename;
  filename_copy = strdup (filename);
  filename_basename = basename (filename_copy);

  void * infoWRecord = bless (WRecord);
  warc_setRecordType (infoWRecord, WARC_INFO_RECORD);
  warc_setContentType (infoWRecord, "application/warc-fields");
  warc_setDate (infoWRecord, NULL);
  warc_setRecordId (infoWRecord, warc_current_winfo_uuid_str);
  warc_setFilename (infoWRecord, filename_basename);

  /* Create content.  (The warc-tools library would create a temporary file
     anyway, so we can just as well do it here.)  */
  FILE * warc_tmp = warc_tempfile ();
  if (warc_tmp == NULL)
    {
      destroy (infoWRecord);
      free (filename_copy);
      return false;
    }

  fprintf (warc_tmp, "software: Wget/%s (%s)\r\n", version_string, OS_TYPE);
  fprintf (warc_tmp, "format: WARC File Format 1.0\r\n");
  fprintf (warc_tmp, "conformsTo: http://bibnum.bnf.fr/WARC/WARC_ISO_28500_version1_latestdraft.pdf\r\n");
  fprintf (warc_tmp, "robots: %s\r\n", (opt.use_robots ? "classic" : "off"));
  fprintf (warc_tmp, "wget-arguments: %s\r\n", program_argstring);
  /* Add the user headers, if any. */
  if (opt.warc_user_headers)
    {
      int i;
      for (i = 0; opt.warc_user_headers[i]; i++)
        fprintf (warc_tmp, "%s\r\n", opt.warc_user_headers[i]);
    }
  fprintf(warc_tmp, "\r\n");

  warc_setContentFromFile (infoWRecord, warc_tmp);

  warc_calc_digests (infoWRecord, warc_tmp, -1);

  /* Returns true on error. */
  if ( WFile_storeRecord (warc_current_wfile, infoWRecord, NULL) )
    {
      logprintf (LOG_NOTQUIET, _("Error writing winfo record to WARC file.\n"));
      destroy (infoWRecord);
      free (filename_copy);
      return false;
    }

  destroy (infoWRecord);
  free (filename_copy);

  /* destroy will have closed warc_tmp. */

  return true;
}

/* Opens a new WARC file.
   If META is true, generates a filename ending with 'meta.warc.gz'.
   
   This method will:
   1. close the current WARC file (if there is one);
   2. increment warc_current_file_number;
   3. open a new WARC file;
   4. write the initial warcinfo record.

   Returns true on success, false otherwise.
   */
static bool
warc_start_new_file (bool meta)
{
  if (opt.warc_filename == NULL)
    return false;

  if (warc_current_wfile != NULL)
    destroy (warc_current_wfile);
  if (warc_current_winfo_uuid_str)
    free (warc_current_winfo_uuid_str);
  if (warc_current_filename)
    free (warc_current_filename);

  warc_current_file_number++;

  int base_filename_length = strlen (opt.warc_filename);
  /* filename format:  base + "-" + 5 digit serial number + ".warc.gz" */
  char *new_filename = malloc (base_filename_length + 1 + 5 + 8 + 1);
  warc_current_filename = new_filename;

  char *extension = (opt.warc_compression_enabled ? "warc.gz" : "warc");

  /* If max size is enabled, we add a serial number to the file names. */
  if (meta)
    sprintf (new_filename, "%s-meta.%s", opt.warc_filename, extension);
  else if (opt.warc_maxsize > 0)
    sprintf (new_filename, "%s-%05d.%s", opt.warc_filename, warc_current_file_number, extension);
  else
    sprintf (new_filename, "%s.%s", opt.warc_filename, extension);

  logprintf (LOG_VERBOSE, _("Opening WARC file %s.\n\n"), quote (new_filename));

  wfile_comp_t compression = (opt.warc_compression_enabled ? WARC_FILE_COMPRESSED_GZIP_BEST_COMPRESSION : WARC_FILE_UNCOMPRESSED);

  /* Find the temporary directory. */
  char tmpdir_filename[100];
  if (path_search (tmpdir_filename, 100, opt.warc_tempdir, "wget", true) == -1)
    return false;
  char *warc_tmpdir = dirname (tmpdir_filename);

  /* Open the WARC file. */
  warc_current_wfile = bless (WFile, new_filename, opt.warc_maxsize, WARC_FILE_WRITER, compression, warc_tmpdir);
  if (warc_current_wfile == NULL)
    {
      logprintf (LOG_NOTQUIET, _("Error opening WARC file %s.\n"), quote (new_filename));
      return false;
    }

  if (! warc_write_warcinfo_record (new_filename))
    return false;

  /* Add warcinfo uuid to manifest. */
  if (warc_manifest_fp)
    fprintf (warc_manifest_fp, "%s\n", warc_current_winfo_uuid_str);

  return true;
}

/* Opens the CDX file for output. */
static bool
warc_start_cdx_file ()
{
  int filename_length = strlen (opt.warc_filename);
  char *cdx_filename = alloca (filename_length + 4 + 1);
  memcpy (cdx_filename, opt.warc_filename, filename_length);
  memcpy (cdx_filename + filename_length, ".cdx", 5);
  warc_current_cdx_file = fopen (cdx_filename, "a+");
  if (warc_current_cdx_file == NULL)
    return false;

  /* Print the CDX header.
   *
   * a - original url
   * b - date
   * m - mime type
   * s - response code
   * k - new style checksum
   * r - redirect
   * M - meta tags
   * V - compressed arc file offset
   * g - file name
   * u - record-id
   */
  fprintf (warc_current_cdx_file, " CDX a b a m s k r M V g u\n");
  fflush (warc_current_cdx_file);

  return true;
}

/* Loads the CDX file from opt.warc_cdx_dedup_filename and fills
   the warc_cdx_dedup_table. */
bool
warc_load_cdx_dedup_file ()
{
#define CDX_FIELDSEP " \t\r\n"
  FILE *f = fopen (opt.warc_cdx_dedup_filename, "r");
  if (f == NULL)
    return false;

  int field_num_original_url = -1;
  int field_num_checksum = -1;
  int field_num_record_id = -1;

  char *lineptr = NULL;
  size_t n = 0;
  size_t line_length;

  /* The first line should contain the CDX header.
     Format:  " CDX x x x x x"
     where x are field type indicators.  For our purposes, we only
     need 'a' (the original url), 'k' (the SHA1 checksum) and
     'u' (the WARC record id). */
  line_length = getline (&lineptr, &n, f);
  if (line_length != -1)
    {
      /* Parse the CDX header. */
      char *token;
      char *save_ptr;
      token = strtok_r (lineptr, CDX_FIELDSEP, &save_ptr);
      
      if (token != NULL && strcmp (token, "CDX") == 0)
        {
          int field_num = 0;
          while (token != NULL)
            {
              token = strtok_r (NULL, CDX_FIELDSEP, &save_ptr);
              if (token != NULL)
                {
                  switch (token[0])
                    {
                    case 'a':
                      field_num_original_url = field_num;
                      break;
                    case 'k':
                      field_num_checksum = field_num;
                      break;
                    case 'u':
                      field_num_record_id = field_num;
                      break;
                    }
                }
              field_num++;
            }
        }
    }

  /* If the file contains all three fields, read the complete file. */
  if (field_num_original_url == -1
      || field_num_checksum == -1
      || field_num_record_id == -1)
    {
      if (field_num_original_url == -1)
        logprintf (LOG_NOTQUIET, _("CDX file does not list original urls. (Missing column 'a'.)\n"));
      if (field_num_checksum == -1)
        logprintf (LOG_NOTQUIET, _("CDX file does not list checksums. (Missing column 'k'.)\n"));
      if (field_num_record_id == -1)
        logprintf (LOG_NOTQUIET, _("CDX file does not list record ids. (Missing column 'u'.)\n"));
    }
  else
    {
      /* Initialize the table. */
      warc_cdx_dedup_table = hash_table_new (1000, warc_hash_sha1_digest, warc_cmp_sha1_digest);

      do
        {
          line_length = getline (&lineptr, &n, f);
          if (line_length != -1)
            {
              char *original_url = NULL;
              char *checksum = NULL;
              char *record_id = NULL;

              char *token;
              char *save_ptr;
              token = strtok_r (lineptr, CDX_FIELDSEP, &save_ptr);

              /* Read this line to get the fields we need. */
              int field_num = 0;
              while (token != NULL)
                {
                  char **val;
                  if (field_num == field_num_original_url)
                    val = &original_url;
                  else if (field_num == field_num_checksum)
                    val = &checksum;
                  else if (field_num == field_num_record_id)
                    val = &record_id;
                  else
                    val = NULL;

                  if (val != NULL)
                    *val = strdup (token);

                  token = strtok_r (NULL, CDX_FIELDSEP, &save_ptr);
                  field_num++;
                }

              if (original_url != NULL && checksum != NULL && record_id != NULL)
                {
                  /* For some extra efficiency, we decode the base32 encoded
                     checksum value.  This should produce exactly SHA1_DIGEST_SIZE
                     bytes.  */
                  size_t checksum_l;
                  char * checksum_v;
                  base32_decode_alloc (checksum, strlen (checksum), &checksum_v, &checksum_l);
                  free (checksum);

                  if (checksum_v != NULL && checksum_l == SHA1_DIGEST_SIZE)
                    {
                      /* This is a valid line with a valid checksum. */
                      struct warc_cdx_record * rec = malloc (sizeof (struct warc_cdx_record));
                      rec->url = original_url;
                      rec->uuid = record_id;
                      memcpy (rec->digest, checksum_v, SHA1_DIGEST_SIZE);
                      hash_table_put (warc_cdx_dedup_table, rec->digest, rec);
                      free (checksum_v);
                    }
                  else
                    {
                      free (original_url);
                      if (checksum_v != NULL)
                        free (checksum_v);
                      free (record_id);
                    }
                }
            }
        }
      while (line_length != -1);

      /* Print results. */
      int nrecords = hash_table_count (warc_cdx_dedup_table);
      logprintf (LOG_VERBOSE, ngettext ("Loaded %d record from CDX.\n\n",
                                        "Loaded %d records from CDX.\n\n", nrecords),
                              nrecords);
    }

  fclose (f);
#undef CDX_FIELDSEP

  return true;
}

/* Writes the record (a WRecord pointer) to the current WARC file.
   If the WARC file is full, the function will open a new file.
   Returns true if the writing was successful, false otherwise. */
static bool
warc_store_record (void * record, unsigned long int * offset)
{
  if (warc_current_wfile != 0)
    {
      /* If the WARC file is full, start a new file. */
      if ( WFile_isFull (warc_current_wfile) )
        {
          if (! warc_start_new_file (false))
            {
              logprintf (LOG_NOTQUIET, _("Could not open new WARC file.\n"));
              return false;
            }
        }

      /* Point to the current info record. */
      warc_setWarcInfoId (record, warc_current_winfo_uuid_str);

      /* This will return true if writing failed. */
      if ( WFile_storeRecord (warc_current_wfile, record, offset) )
        {
          logprintf (LOG_NOTQUIET, _("Error writing record to WARC file.\n"));
          return false;
        }

      return true;
    }
  else
    {
      logprintf (LOG_NOTQUIET, _("Called warc_store_record without open WFile.\n"));
      return false;
    }
}

/* Initializes the WARC writer (if opt.warc_filename is set).
   This should be called before any WARC record is written. */
void
warc_init ()
{
  if (opt.warc_filename != NULL)
    {
      if (opt.warc_cdx_dedup_filename != NULL)
        {
          if (! warc_load_cdx_dedup_file ())
            {
              logprintf (LOG_NOTQUIET,
                         _("Could not read CDX file %s for deduplication.\n"),
                         quote (opt.warc_cdx_dedup_filename));
              exit(1);
            }
        }

      warc_manifest_fp = warc_tempfile ();
      if (warc_manifest_fp == NULL)
        {
          logprintf (LOG_NOTQUIET, _("Could not open temporary WARC manifest file.\n"));
          exit(1);
        }

      if (opt.warc_keep_log)
        {
          warc_log_fp = warc_tempfile ();
          if (warc_log_fp == NULL)
            {
              logprintf (LOG_NOTQUIET, _("Could not open temporary WARC log file.\n"));
              exit(1);
            }
          log_set_warc_log_fp (warc_log_fp);
        }

      warc_current_file_number = -1;
      if (! warc_start_new_file (false))
        {
          logprintf (LOG_NOTQUIET, _("Could not open WARC file.\n"));
          exit(1);
        }

      if (opt.warc_cdx_enabled)
        {
          if (! warc_start_cdx_file ())
            {
              logprintf (LOG_NOTQUIET, _("Could not open CDX file for output.\n"));
              exit(1);
            }
        }
    }
}

/* Writes metadata (manifest, configuration, log file) to the WARC file. */
void
warc_write_metadata ()
{
  /* If there are multiple WARC files, the metadata should be written to a separate file. */
  if (opt.warc_maxsize > 0)
    warc_start_new_file (true);

  char manifest_uuid [48];
  warc_uuid_str (manifest_uuid);

  fflush (warc_manifest_fp);
  warc_write_resource_record (manifest_uuid,
                              "metadata://gnu.org/software/wget/warc/MANIFEST.txt",
                              NULL, NULL, NULL, "text/plain",
                              warc_manifest_fp, -1);
  /* warc_write_resource_record has closed warc_manifest_fp. */

  FILE * warc_tmp_fp = warc_tempfile ();
  if (warc_tmp_fp == NULL)
    {
      logprintf (LOG_NOTQUIET, _("Could not open temporary WARC file.\n"));
      exit(1);
    }
  fflush (warc_tmp_fp);
  fprintf (warc_tmp_fp, "%s\n", program_argstring);

  warc_write_resource_record (manifest_uuid,
                              "metadata://gnu.org/software/wget/warc/wget_arguments.txt",
                              NULL, NULL, NULL, "text/plain",
                              warc_tmp_fp, -1);
  /* warc_write_resource_record has closed warc_tmp_fp. */

  if (warc_log_fp != NULL)
    {
      warc_write_resource_record (NULL,
                                  "metadata://gnu.org/software/wget/warc/wget.log",
                                  NULL, manifest_uuid, NULL, "text/plain",
                                  warc_log_fp, -1);
      /* warc_write_resource_record has closed warc_log_fp. */

      warc_log_fp = NULL;
      log_set_warc_log_fp (NULL);
    }
}

/* Finishes the WARC writing.
   This should be called at the end of the program. */
void
warc_close ()
{
  if (warc_current_wfile != NULL)
    warc_write_metadata ();

  if (warc_current_wfile != NULL)
    {
      free (warc_current_winfo_uuid_str);
      destroy (warc_current_wfile);
    }
  if (warc_current_cdx_file != NULL)
    fclose (warc_current_cdx_file);
  if (warc_log_fp != NULL)
    {
      fclose (warc_log_fp);
      log_set_warc_log_fp (NULL);
    }
}

/* Creates a temporary file for writing WARC output.
   The temporary file will be created in opt.warc_tempdir.
   Returns the pointer to the temporary file, or NULL. */
FILE *
warc_tempfile ()
{
  char filename[100];
  if (path_search (filename, 100, opt.warc_tempdir, "wget", true) == -1)
    return NULL;

  int fd = mkstemp (filename);
  if (fd < 0)
    return NULL;

  if (unlink (filename) < 0)
    return NULL;

  return fdopen (fd, "wb+");
}

/* Writes a request record to the WARC file.
   url  is the target uri of the request,
   timestamp_str  is the timestamp of the request (generated with warc_timestamp),
   concurrent_to_uuid  is the uuid of the request (generated with warc_uuid_str),
   body  is a pointer to a file containing the request headers and body.
   ip  is the ip address of the server (or NULL),
   Calling this function will close body.
   Returns true on success, false on error. */
bool
warc_write_request_record (char *url, char *timestamp_str, char *concurrent_to_uuid, ip_address *ip, FILE *body, long int payload_offset)
{
  void * requestWRecord = bless (WRecord);
  warc_setRecordType (requestWRecord, WARC_REQUEST_RECORD);
  warc_setTargetUri (requestWRecord, url);
  warc_setContentType (requestWRecord, "application/http;msgtype=request");
  warc_setDate (requestWRecord, timestamp_str);
  warc_setRecordId (requestWRecord, concurrent_to_uuid);
  warc_setIpAddress (requestWRecord, ip);
  warc_setContentFromFile (requestWRecord, body);

  warc_calc_digests (requestWRecord, body, payload_offset);

  bool result = warc_store_record (requestWRecord, NULL);

  destroy (requestWRecord);

  /* destroy has also closed body. */

  return result;
}

/* Writes a response record to the WARC file.
   url  is the target uri of the request/response,
   timestamp_str  is the timestamp of the request that generated this response
                  (generated with warc_timestamp),
   concurrent_to_uuid  is the uuid of the request for that generated this response
                 (generated with warc_uuid_str),
   ip  is the ip address of the server (or NULL),
   body  is a pointer to a file containing the response headers and body.
   mime_type  is the mime type of the response body (will be printed to CDX),
   response_code  is the HTTP response code (will be printed to CDX),
   redirect_location  is the contents of the Location: header, or NULL (will be printed to CDX),
   Calling this function will close body.
   Returns true on success, false on error. */
bool
warc_write_response_record (char *url, char *timestamp_str, char *concurrent_to_uuid, ip_address *ip, FILE *body, long int payload_offset, char *mime_type, int response_code, char *redirect_location)
{
  char response_uuid [48];
  warc_uuid_str (response_uuid);

  void * responseWRecord = bless (WRecord);
  warc_setTargetUri (responseWRecord, url);
  warc_setDate (responseWRecord, timestamp_str);
  warc_setRecordId (responseWRecord, response_uuid);
  warc_setConcurrentTo (responseWRecord, concurrent_to_uuid);
  warc_setIpAddress (responseWRecord, ip);
  warc_setContentType (responseWRecord, "application/http;msgtype=response");

  if (opt.warc_digests_enabled)
    {
      /* Calculate the block and payload digests. */
      unsigned char sha1_res_block[SHA1_DIGEST_SIZE];
      unsigned char sha1_res_payload[SHA1_DIGEST_SIZE];

      rewind (body);
      if (warc_sha1_stream_with_payload (body, sha1_res_block, sha1_res_payload, payload_offset) == 0)
        {
          /* Decide (based on url + payload digest) if we have seen this
             data before. */
          if (warc_cdx_dedup_table != NULL)
            {
              /* Find a CDX record with this payload digest. */
              char *key;
              struct warc_cdx_record *rec_existing;
              hash_table_get_pair (warc_cdx_dedup_table, sha1_res_payload, &key, &rec_existing);

              if (rec_existing != NULL && !strcmp (rec_existing->url, url))
                {
                  /* Found an existing record. */
                  logprintf (LOG_VERBOSE, _("Found exact match in CDX file. Saving revisit record to WARC.\n"));

                  /* Make the record a revisit record. */
                  warc_setRecordType (responseWRecord, WARC_REVISIT_RECORD);
                  warc_setProfile (responseWRecord, "http://netpreserve.org/warc/1.0/revisit/identical-payload-digest");
                  warc_setRefersTo (responseWRecord, rec_existing->uuid);
                  warc_setTruncated (responseWRecord, "length");

                  /* Remove the payload from the file. */
                  if (payload_offset > 0)
                    {
                      if (ftruncate (fileno (body), payload_offset) == -1)
                        return false;
                    }

                  warc_setContentFromFile (responseWRecord, body);

                  /* Digests: payload refers to the old payload, but the block
                     digest must be calculated based on the truncated block. */
                  warc_setPayloadDigest (responseWRecord, sha1_res_payload);
                  sha1_stream (body, sha1_res_block);
                  warc_setBlockDigest (responseWRecord, sha1_res_block);

                  bool result = warc_store_record (responseWRecord, NULL);
                  destroy (responseWRecord);
                  return result;
                }
            }
        }

      warc_setBlockDigest (responseWRecord, sha1_res_block);
      warc_setPayloadDigest (responseWRecord, sha1_res_payload);
    }

  /* Not a revisit, just store the record. */
  warc_setContentFromFile (responseWRecord, body);
  warc_setRecordType (responseWRecord, WARC_RESPONSE_RECORD);

  unsigned long int offset;
  bool result = warc_store_record (responseWRecord, &offset);

  if (result && opt.warc_cdx_enabled)
    {
      /* Add this record to the CDX. */
      /* Transform the timestamp. */
      char timestamp_str_cdx [15];
      memcpy (timestamp_str_cdx     , timestamp_str     , 4); /* "YYYY" "-" */
      memcpy (timestamp_str_cdx +  4, timestamp_str +  5, 2); /* "mm"   "-" */
      memcpy (timestamp_str_cdx +  6, timestamp_str +  8, 2); /* "dd"   "T" */
      memcpy (timestamp_str_cdx +  8, timestamp_str + 11, 2); /* "HH"   ":" */
      memcpy (timestamp_str_cdx + 10, timestamp_str + 14, 2); /* "MM"   ":" */
      memcpy (timestamp_str_cdx + 12, timestamp_str + 17, 2); /* "SS"   "Z" */
      timestamp_str_cdx[14] = '\0';
      
      /* Rewrite the checksum. */
      char *checksum = (char *) WRecord_getPayloadDigest (responseWRecord);
      if (checksum != NULL)
        checksum += 5; /* Skip the "sha1:" */
      else
        checksum = "-";

      if (mime_type == NULL || strlen(mime_type) == 0)
        mime_type = "-";
      if (redirect_location == NULL || strlen(redirect_location) == 0)
        redirect_location = "-";

      /* Print the CDX line. */
      fprintf (warc_current_cdx_file, "%s %s %s %s %d %s %s - %ld %s %s\n", url, timestamp_str_cdx, url, mime_type, response_code, checksum, redirect_location, offset, warc_current_filename, response_uuid);
      fflush (warc_current_cdx_file);
    }

  destroy (responseWRecord);

  /* destroy has also closed body. */

  return result;
}

/* Writes a resource record to the WARC file.
   resource_uuid  is the uuid of the resource (or NULL),
   url  is the target uri of the resource,
   timestamp_str  is the timestamp (generated with warc_timestamp),
   concurrent_to_uuid  is the uuid of the request for that generated this resource
                 (generated with warc_uuid_str) or NULL,
   ip  is the ip address of the server (or NULL),
   content_type  is the mime type of the body (or NULL),
   body  is a pointer to a file containing the resource data.
   Calling this function will close body.
   Returns true on success, false on error. */
bool
warc_write_resource_record (char *resource_uuid, char *url, char *timestamp_str, char *concurrent_to_uuid, ip_address *ip, char *content_type, FILE *body, long int payload_offset)
{
  if (resource_uuid == NULL)
    {
      resource_uuid = alloca (48);
      warc_uuid_str (resource_uuid);
    }

  void * resourceWRecord = bless (WRecord);
  warc_setRecordType (resourceWRecord, WARC_RESOURCE_RECORD);
  warc_setTargetUri (resourceWRecord, url);
  warc_setDate (resourceWRecord, timestamp_str);
  warc_setRecordId (resourceWRecord, resource_uuid);
  if (content_type == NULL)
    content_type = "application/octet-stream";
  warc_setContentType (resourceWRecord, content_type);
  warc_setConcurrentTo (resourceWRecord, concurrent_to_uuid);
  warc_setIpAddress (resourceWRecord, ip);
  warc_setContentFromFile (resourceWRecord, body);

  warc_calc_digests (resourceWRecord, body, payload_offset);

  bool result = warc_store_record (resourceWRecord, NULL);

  destroy (resourceWRecord);

  /* destroy has also closed body. */

  return result;
}

