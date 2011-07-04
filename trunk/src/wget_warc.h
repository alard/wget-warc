#include <warc.h>

void warc_init ();
void warc_close ();
void warc_timestamp (char *timestamp);
void warc_uuid_str (char *id_str);
bool warc_store_record (void * record);

bool warc_setTargetUri(void *record, char *u8_uri);
bool warc_setContentType(void *record, char *u8_type);
bool warc_setDate(void *record, char *u8_date);
bool warc_setRecordId(void *record, char *u8_uuid);
bool warc_setFilename(void *record, char *u8_filename);
bool warc_setConcurrentTo(void *record, char *u8_to);
bool warc_setContentFromString(void *record, char *u8_content);
bool warc_setWarcInfoId(void *record, char *u8_infoId);
bool warc_setContentFromFileName(void *record, char *u8_filename);
bool warc_setContentFromFile(void *record, FILE *file);
bool warc_setRecordType(void *record, const warc_rec_t t);

FILE * warc_tempfile();

