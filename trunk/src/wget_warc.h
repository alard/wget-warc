/* Declarations of WARC helper methods. */
#ifndef WGET_WARC_H
#define WGET_WARC_H

void warc_init ();
void warc_close ();
void warc_timestamp (char *timestamp);
void warc_uuid_str (char *id_str);

FILE * warc_tempfile ();

bool warc_write_request_record (char *url, char *timestamp_str, char *request_uuid, FILE *body);
bool warc_write_response_record (char *url, char *timestamp_str, char *request_uuid, FILE *body);

#endif /* WGET_WARC_H */

