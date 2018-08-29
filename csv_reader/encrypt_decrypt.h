#ifndef ENCRYPT_DECRYPT_H
#define ENCRYPT_DECRYPT_H
#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/pwdbased.h>

#define MAXCHARS 256
#define KEYSIZE 256
#define SALT_SIZE 8

int AesEncrypt(Aes *aes, byte *key, int size, char *input_lines, FILE *outfile);
int AesDecrypt(Aes *aes, byte *key, int size, FILE *inFile, char **out);
int GenerateKey(RNG *rng, byte *key, int size, byte *salt, int pad);
int NoEcho(char* key, int size);
int check_if_valid_csv(char *string);
#endif
