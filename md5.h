/*
 * Create an MD5 hex digest from a string.
 * 33-byte string is malloc'd by this function. The caller must free it.
 * When compiling, link to:  -l crypto
 */
char * md5(const char *str, int length);
char * md5(const char *file_name, int length);