#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <time.h>
#include <uuid/uuid.h>
#include <warc.h>

#include "wget.h"
#include "wget_warc.h"

extern char *version_string;

static void *warc_current_wfile;
static char *warc_current_winfo_uuid_str;
static int  warc_current_file_number;

void warc_timestamp (char *timestamp)
{
  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = gmtime (&rawtime);
  strftime (timestamp, 21, "%Y-%m-%dT%H:%M:%SZ", timeinfo);
}

void warc_uuid_str (char *urn_str)
{
  uuid_t record_id;
  uuid_generate (record_id);
  char uuid_str[37];
  uuid_unparse (record_id, uuid_str);
  sprintf(urn_str, "<urn:uuid:%s>", uuid_str);
}

bool warc_filename_ends_with_warc (char *filename)
{
  char *lastfour;
  int len = strlen (filename);
  if (len < 4)
    return false;
  else
  {
    lastfour = &(filename[len - 4]);
    return (0 == strncasecmp (lastfour, "warc", 4));
  }
}

bool warc_start_new_file ()
{
  if (opt.warc_filename == 0)
    return false;

  if (warc_current_wfile != 0)
    destroy (warc_current_wfile);
  if (warc_current_winfo_uuid_str)
    free (warc_current_winfo_uuid_str);

  bool disableCompression = warc_filename_ends_with_warc(opt.warc_filename);
  
  warc_current_file_number++;

  int filename_length = strlen (opt.warc_filename);
  char *new_filename = opt.warc_filename;

  if (warc_current_file_number > 0)
  {
    new_filename = alloca (5 + (warc_current_file_number % 10) + filename_length);

    /* split at first . */
    char *suffix = strchr (opt.warc_filename, '.');
    if (suffix)
    {
      /* insert number before first . */
      strcpy (new_filename, opt.warc_filename);
      sprintf (new_filename + (suffix - opt.warc_filename), ".%d%s", warc_current_file_number, suffix);
    }
    else
    {
      /* append number */
      sprintf (new_filename, "%s.%d", opt.warc_filename, warc_current_file_number);
    }
  }

  warc_current_wfile = bless (WFile, new_filename, (1024 * 1024 * 1024), WARC_FILE_WRITER, (disableCompression ? WARC_FILE_UNCOMPRESSED : WARC_FILE_COMPRESSED_GZIP_BEST_COMPRESSION), ".");
  if (warc_current_wfile == 0)
  {
    printf(stderr, "Error opening WARC file.\n");
    return false;
  }

  /* Write warc-info record as the first record of the file. */
  /* We add the record id of this info record to the other records in the file. */
  warc_current_winfo_uuid_str = (char *) malloc (48);
  warc_uuid_str (warc_current_winfo_uuid_str);

  char warc_timestamp_str [21];
  warc_timestamp (warc_timestamp_str);

  void * infoWRecord = bless (WRecord);
  WRecord_setRecordType (infoWRecord, WARC_INFO_RECORD);
  WRecord_setContentType (infoWRecord, ((warc_u8_t *) "application/warc-fields"), w_strlen(((warc_u8_t *) "application/warc-fields")));
  WRecord_setDate (infoWRecord, ((warc_u8_t *) warc_timestamp_str), w_strlen(((warc_u8_t *) warc_timestamp_str)));
  WRecord_setRecordId (infoWRecord, ((warc_u8_t *) warc_current_winfo_uuid_str), w_strlen(((warc_u8_t *) warc_current_winfo_uuid_str)));
  WRecord_setFilename (infoWRecord, ((warc_u8_t *) new_filename), w_strlen(((warc_u8_t *) new_filename)));

  char winfo_header_string [300]; /* lazy */
  sprintf (winfo_header_string, "software: Wget/%s (%s)\r\nformat: WARC File Format 1.0\r\nconformsTo: http://bibnum.bnf.fr/WARC/WARC_ISO_28500_version1_latestdraft.pdf\r\nrobots: %s\r\n\r\n", version_string, OS_TYPE, (opt.use_robots ? "classic" : "off"));
  WRecord_setContentFromString (infoWRecord, ((warc_u8_t *) winfo_header_string), w_strlen(((warc_u8_t *) winfo_header_string)));

  /* Returns true on error. */
  if ( WFile_storeRecord (warc_current_wfile, infoWRecord) )
  {
    fprintf(stderr, "Error writing winfo record to WARC file.\n");
    destroy (infoWRecord);
    return false;
  }

  destroy (infoWRecord);

  return true;
}

bool warc_store_record (void * record)
{
  if (warc_current_wfile != 0)
  {
    /* Point to the current info record. */
    WRecord_setWarcInfoId (record, ((warc_u8_t *) warc_current_winfo_uuid_str), w_strlen(((warc_u8_t *) warc_current_winfo_uuid_str)));

    /* This will return true if writing failed. */
    if ( WFile_storeRecord (warc_current_wfile, record) )
    {
      /* A problem. Is the warc file full? */
      if ( ! WFile_isFull (warc_current_wfile) )
      {
        fprintf(stderr, "Error writing record to WARC file.\n");
        return false;
      }
      else
      {
        if (! warc_start_new_file ())
        {
          fprintf(stderr, "Could not open new WARC file.\n");
          return false;
        }

        if ( WFile_storeRecord (warc_current_wfile, record) )
        {
          fprintf(stderr, "Error writing record to WARC file.\n");
          return false;
        }
      }
    }

    return true;
  }
  else
  {
    fprintf(stderr, "Called warc_store_record without open WFile.\n");
    return false;
  }
}

void warc_init ()
{
  if (opt.warc_filename != 0)
  {
    warc_current_file_number = -1;
    if (! warc_start_new_file ())
    {
      fprintf(stderr, "Could not open WARC file.\n");
      opt.warc_filename = 0;
    }
  }
}

void warc_close ()
{
  if (warc_current_wfile != 0)
  {
    free (warc_current_winfo_uuid_str);
    destroy (warc_current_wfile);
  }
}

