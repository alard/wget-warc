#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <time.h>
#include <uuid/uuid.h>
#include <warc.h>

#include "wget.h"
#include "wget_warc.h"

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
  if (opt.warc_filename != 0)
  {
    bool disableCompression = warc_filename_ends_with_warc(opt.warc_filename);
    void * w = bless (WFile, opt.warc_filename, (1024 * 1024 * 1024), WARC_FILE_WRITER, (disableCompression ? WARC_FILE_UNCOMPRESSED : WARC_FILE_COMPRESSED_GZIP_BEST_COMPRESSION), ".");
    WFile_storeRecord (w, record);
    destroy (w);
  }
}

