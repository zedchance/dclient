#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>

/*
 * MD5 function for files already on disk
 */
char * md5(const char *file_name, int length) 
{
    // Data structure for the MD5 algorithm
    MD5_CTX c;

    // Array of bytes to hold the hash
    unsigned char digest[MD5_DIGEST_LENGTH];

    // String to hold the hex representation. Will be returned to caller.
    char *out = (char*)malloc(MD5_DIGEST_LENGTH * 2 + 1);

    // Initialize the data structure
    MD5_Init(&c);
    
    // Open FILE
    FILE *f = fopen(file_name, "r");
    if (!f)
    {
        fprintf(stderr, "Can't open %s for reading\n", file_name);
        exit(1);
    }

    // Call MD5_Update with 512-byte chunks of the data. Or with the remaining
    // bytes if it's the last chunk.
    char chunk[512];
    while (length > 0)
    {
        fread(chunk, sizeof(unsigned char), 512, f);
        if (length > 512)
        {
            MD5_Update(&c, chunk, 512);
        }
        else
        {
            MD5_Update(&c, chunk, length);
        }
        length -= 512;
    }

    // Finalize the hash and write it into the array.
    MD5_Final(digest, &c);

    // Convert array to hex, writing to the string that will be returned.
    int n;
    for (n = 0; n < MD5_DIGEST_LENGTH; ++n)
    {
        snprintf(&(out[n*2]), MD5_DIGEST_LENGTH*2, "%02x", (unsigned int)digest[n]);
    }

    // Terminate string with null character.
    out[MD5_DIGEST_LENGTH * 2] = '\0';

    // Close file and return sum
    fclose(f);
    return out;
}