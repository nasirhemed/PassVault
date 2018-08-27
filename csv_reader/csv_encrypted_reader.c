/*
 * CSV Reader: This program reads entries from a CSV file.
 * The labels will be provided as stdin and the output will
 * be printed to stdout
 *
 * Note: The CSV file is of the form Domain name, Username, Password
 * Usage: csv_reader file
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/pwdbased.h>

#define SALT_SIZE 8
#define MAXCHARS 256

/**
 * Given a char array and a CSV file pointer, set the string
 * to the cell the file pointer is currently at
 *
 * Precondition: File is a CSV file that is comma seperated
 */
void set_string(char password[], char **pointer_string) {
	char *str = password;
	int i = 0;
	char *p;
	for (p = *pointer_string; *p != ',' && *p != '\n' && *p != '\0';\
			p++) {
		str[i] = *p;
		i++;
	}
	str[i] = '\0';
	*pointer_string = ++p;
}

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
 * Decryptsr a file using AES
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
	ret = wc_PBKDF2(key, key, strlen((const char*)key), salt, SALT_SIZE, 4096,
			size, SHA256);
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
	printf("%s\n", (unsigned char *)output);
	if (check_if_valid_csv((char *)output) == -1) {
		return -1;
	}
	*out = (char *)output;

	/* closes the opened files and frees the memory*/
	memset(input, 0, aSize);
	memset(key, 0, size);
	free(input);
	free(key);
	fclose(inFile);
	wc_FreeRng(&rng);

	return 0;
}


/**
 * Given a domain_name, check if the name is there in
 * the CSV file. If it is then output the username and
 * password. Otherwise, output no domain found
 *
 * Example of a domain name could be like Facebook, Twitter, etc
 */
void check_domain(char *domain_name, char *csv_string) {

	char domain[MAXCHARS];
	char username[MAXCHARS];
	char password[MAXCHARS];

	char *pointer = csv_string;
	while (1) {
		set_string(domain, &pointer);

		if (strcmp(domain, domain_name) == 0) {
			set_string(username, &pointer);
			set_string(password, &pointer);
			printf("Domain name: "
					"%s\nUsername: %s\nPassword: %s\n",\
					domain, username, password);
			return;
		}
		else {
			while (*pointer != '\n' && *pointer != '\0') {
				pointer++;
			}
			if (*pointer == '\0') {
				printf("No such domain found\n");
				return;
			}
			pointer++;

		}

	}

}

/*
 * temporarily disables echoing in terminal for secure key input
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

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage: csv_reader file\n");
		exit(1);
	}
	FILE *fp = fopen(argv[1], "r");
	if (fp == NULL) {
		perror("fopen");
		exit(1);
	}
	Aes aes;
	char *domain_name = malloc(MAXCHARS);
	char *pass = malloc(256);
	char *out;

	NoEcho(pass, 256);
	if (AesDecrypt(&aes,(byte *) pass, 256, fp, &out) == -1) {
		fprintf(stderr, "Invalid Password\n");
		exit(1);
	}
	while (1) {

		printf("Enter a record the domain that you are looking for:");
		fgets(domain_name, MAXCHARS, stdin);
		domain_name[strlen(domain_name) - 1] = '\0';
		check_domain(domain_name, out);
	}
	free(out);
	exit(0);
}
