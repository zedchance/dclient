#include <stdio.h>
#include <stdlib.h>
#include <openssl/md5.h>

char *md5(const char *str, int length) {
    // Data structure for the MD5 algorithm
    MD5_CTX c;

    // Array of bytes to hold the hash
    unsigned char digest[MD5_DIGEST_LENGTH];

    // String to hold the hex representation. Will be returned to caller.
    char *out = (char*)malloc(MD5_DIGEST_LENGTH * 2 + 1);

    // Initialize the data structure
    MD5_Init(&c);

    // Call MD5_Update with 512-byte chunks of the data. Or with the remaining
    // bytes if it's the last chunk.
    while (length > 0) {
        if (length > 512) {
            MD5_Update(&c, str, 512);
        } else {
            MD5_Update(&c, str, length);
        }
        length -= 512;
        str += 512;
    }

    // Finalize the hash and write it into the array.
    MD5_Final(digest, &c);

    // Convert array to hex, writing to the string that will be returned.
    int n;
    for (n = 0; n < MD5_DIGEST_LENGTH; ++n) {
        snprintf(&(out[n*2]), MD5_DIGEST_LENGTH*2, "%02x", (unsigned int)digest[n]);
    }

    // Terminate string with null character.
    out[MD5_DIGEST_LENGTH * 2] = '\0';

    return out;
}