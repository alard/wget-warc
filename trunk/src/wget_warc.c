#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <time.h>
#include <uuid/uuid.h>
#include <warc.h>

#include "wget.h"
#include "wget_warc.h"

static void *warc_current_wfile;
static char *warc_current_winfo_uuid_str;

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

void warc_store_record (void * record)
{
  if (warc_current_wfile != 0)
  {
    WFile_storeRecord (warc_current_wfile, record);
  }
  else
  {
    fprintf(stderr, "Called warc_store_record without open WFile.\n");
  }
}

void warc_init ()
{
  if (opt.warc_filename != 0)
  {
    bool disableCompression = warc_filename_ends_with_warc(opt.warc_filename);
    warc_current_wfile = bless (WFile, opt.warc_filename, (1024 * 1024 * 1024), WARC_FILE_WRITER, (disableCompression ? WARC_FILE_UNCOMPRESSED : WARC_FILE_COMPRESSED_GZIP_BEST_COMPRESSION), ".");

    /* Write warc-info record as the first record of the file. */
    warc_current_winfo_uuid_str = (char *) malloc (48);
    warc_uuid_str (warc_current_winfo_uuid_str);
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

