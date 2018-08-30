#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <ctype.h>
#include <bsd/stdlib.h>

#include "encrypt_decrypt.h"

int GenerateKey(RNG* rng, byte* key, int size, byte* salt, int pad)
{
	int ret;

	ret = wc_RNG_GenerateBlock(rng, salt, SALT_SIZE);
	if (ret != 0)
		return -1020;

	if (pad == 0)
		salt[0] = 0;

	/* stretches key */
	ret = wc_PBKDF2(key, key, strlen((const char*)key), salt, SALT_SIZE, 4096,
			size, SHA256);
	if (ret != 0)
		return -1030;

	return 0;
}

/*
 * Encrypts a file using AES
 */
 // Signature (Aes *aes, byte *pass, int size, char *inputLines, FILE *outFile)
int AesEncrypt(Aes* aes, byte* key, int size, char *inputLines, FILE* outFile)
{
	RNG     rng;
	byte    iv[AES_BLOCK_SIZE];
	byte*   input;
	byte*   output;
	byte    salt[SALT_SIZE] = {0};

	int     i = 0;
	int     ret = 0;
	int     inputLength;
	int     length;
	int     padCounter = 0;

	// fseek(inFile, 0, SEEK_END);
	inputLength = strlen(inputLines);
	// fseek(inFile, 0, SEEK_SET);

	length = inputLength + 1;
	/* pads the length until it evenly matches a block / increases pad number*/
	while (length % AES_BLOCK_SIZE != 0) {
		length++;
		padCounter++;
	}

	input = malloc(length);
	output = malloc(length);

	ret = wc_InitRng(&rng);
	if (ret != 0) {
		printf("Failed to initialize random number generator\n");
		return -1030;
	}

	/* reads from inFile and wrties whatever is there to the input array */
	// ret = fread(input, 1, inputLength, inFile);
	strncpy((char *)input, inputLines, strlen(inputLines));

	for (i = inputLength; i < length; i++) {
		/* padds the added characters with the number of pads */
		input[i] = padCounter;
	}

	ret = wc_RNG_GenerateBlock(&rng, iv, AES_BLOCK_SIZE);
	if (ret != 0)
		return -1020;

	/* stretches key to fit size */
	ret = GenerateKey(&rng, key, size, salt, padCounter);
	if (ret != 0)
		return -1040;

	/* sets key */
	ret = wc_AesSetKey(aes, key, AES_BLOCK_SIZE, iv, AES_ENCRYPTION);
	if (ret != 0)
		return -1001;

	/* encrypts the message to the ouput based on input length + padding */
	ret = wc_AesCbcEncrypt(aes, output, input, length);
	if (ret != 0)
		return -1005;

	/* writes to outFile */
	fwrite(salt, 1, SALT_SIZE, outFile);
	fwrite(iv, 1, AES_BLOCK_SIZE, outFile);
	fwrite(output, 1, length, outFile);

	/* closes the opened files and frees the memory*/
	memset(input, 0, length);
	memset(output, 0, length);
	memset(key, 0, size);
	memset(inputLines, 0, strlen(inputLines));
	free(inputLines);
	free(input);
	free(output);
	// free(key);
	// fclose(inFile);
	fclose(outFile);
	wc_FreeRng(&rng);

	return ret;
}


/**
 * Given a string, check if the header is of the valid CSV file:
 * 	Domain name,Username,Password
 * Return -1 if the file does not the header and 0 otherwise
 */
int check_if_valid_csv(char *string) {
	char toCheck[] = "Domain name,Username,Password";
	for (int i = 0; i < strlen(toCheck); i++) {
		if (toCheck[i] != string[i]) {
			return -1;
		}
	}
	return 0;
}

/*
 * Given a file and its key, decrypt the file and store the contents in
 * a string that is allocated in the heap. Make sure the string is freed
 * (char *out). The file is assumed to be an encrypted CSV file. If the key
 * is not correct, this function return -1. If there are any other errors,
 * -10** is returned. If successful, this function returns 0
 */
int AesDecrypt(Aes* aes, byte* key, int size, FILE* inFile, char **out)
{
	RNG     rng;
	byte    iv[AES_BLOCK_SIZE];
	byte*   input;
	byte*		output = (byte *) *out;
	byte    salt[SALT_SIZE] = {0};

	int     i = 0;
	int     ret = 0;
	int     length;
	int     aSize;

	fseek(inFile, 0, SEEK_END);
	length = ftell(inFile);
	fseek(inFile, 0, SEEK_SET);
	aSize = length;

	input = malloc(aSize);
	output = malloc(aSize);

	wc_InitRng(&rng);

	/* reads from inFile and wrties whatever is there to the input array */
	ret = fread(input, 1, length, inFile);
	if (ret == 0) {
		printf("Input file does not exist.\n");
		return -1010;
	}
	for (i = 0; i < SALT_SIZE; i++) {
		/* finds salt from input message */
		salt[i] = input[i];
	}
	for (i = SALT_SIZE; i < AES_BLOCK_SIZE + SALT_SIZE; i++) {
		/* finds iv from input message */
		iv[i - SALT_SIZE] = input[i];
	}

	/* replicates old key if keys match */
	ret = wc_PBKDF2(key, key, strlen((const char*)key), salt, SALT_SIZE,
			4096, size, SHA256);
	if (ret != 0)
		return -1050;

	/* sets key */
	ret = wc_AesSetKey(aes, key, AES_BLOCK_SIZE, iv, AES_DECRYPTION);
	if (ret != 0)
		return -1002;

	/* change length to remove salt/iv block from being decrypted */
	length -= (AES_BLOCK_SIZE + SALT_SIZE);
	for (i = 0; i < length; i++) {
		/* shifts message: ignores salt/iv on message*/
		input[i] = input[i + (AES_BLOCK_SIZE + SALT_SIZE)];
	}
	/* decrypts the message to output based on input length + padding*/
	ret = wc_AesCbcDecrypt(aes, output, input, length);
	if (ret != 0)
		return -1006;

	if (salt[0] != 0) {
		/* reduces length based on number of padded elements */
		length -= output[length-1];
	}
	output[length - 1] = '\0';
	// printf("%s\n", (unsigned char *)output);
	if (check_if_valid_csv((char *)output) == -1) {
		memset(input, 0, aSize);
		memset(key, 0, size);
		free(input);
		free(key);
		free(output);
		fclose(inFile);
		return -1;
	}
	*out = (char *)output;
	printf("%s\n", *out);

	/* closes the opened files and frees the memory*/
	memset(input, 0, aSize);
	memset(key, 0, size);
	// memset(output, 0, aSize);
	// free(output);
	free(input);
	free(key);
	fclose(inFile);
	wc_FreeRng(&rng);

	return 0;
}

/*
 * Temporarily disable displaying the characters on the terminal to allow
 * the user to securely enter the password for decrypting the file.
 */
int NoEcho(char* key, int size)
{
	struct termios oflags, nflags;

	/* disabling echo */
	tcgetattr(fileno(stdin), &oflags);
	nflags = oflags;
	nflags.c_lflag &= ~ECHO;
	nflags.c_lflag |= ECHONL;

	if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
		printf("Error\n");
		return -1060;
	}

	printf("Unique Password: ");
	fgets(key, size, stdin);
	key[strlen(key) - 1] = 0;
	printf("\n");
	/* restore terminal */
	if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
		printf("Error\n");
		return -1070;
	}
	return 0;
}
