/** CSV Writer
 * This program writes CSV entries of the form domain_name, username, password
 * to the CSV file provided by the argument. If there is no argument provided
 * then, it creates a CSV file and writes to it.
 * Usage: CSV_Writer <file>(optional)
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <ctype.h>
#include <bsd/stdlib.h>
#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/pwdbased.h>
#define MAXCHARS 256
#define INTLENGTH 10
#define SALT_SIZE 8

int check_valid_password(char *password) {
	if (strlen(password) == 0) {
		return 1;
	}
	else if (sizeof(password) < 10) {
		return 0;
	}
	int num = 0;
	int spec = 0;
	int lcase = 0;
	int ucase = 0;

	for (int i = 0; i < strlen(password); i++) {
		if (password[i] >= 65 && password[i] <= 90) {
			ucase++;
		}
		else if (password[i] >= 97 && password[i] <= 122) {
			lcase++;
		}
		else if (password[i] >= 48 && password[i] <= 57) {
			num++;
		}
		else {
			spec++;
		}
	}
	int valid = num > 1 && spec > 1 && lcase > 3 && ucase > 3 ? 0 : 1;
	return valid;

}

int check_yes_no(char *yes_no) {
	int valid = strlen(yes_no) == 1 && (toupper(yes_no[0]) == 'Y'\
			|| toupper(yes_no[0]) == 'N') ? 0 : 1;
	return valid;

}

int check_gen_or_create(char *gen_cre) {
	int valid = strlen(gen_cre) == 1 && \
		    (toupper(gen_cre[0]) == 'G' || toupper(gen_cre[0]) == 'C')\
		    ? 0 : 1;
	return valid;
}

void generate_password(char *password) {
	int max_length;
	char number_input[INTLENGTH];
	printf("Enter the maximum length of your password. (Press enter if you"
		" don't want to specify maximum charaters): ");
	fgets(number_input, INTLENGTH, stdin);
	if (number_input[0] == '\n') {
		max_length = arc4random_uniform(10) + 10;
	}
	else {
		max_length = strtol(number_input, NULL, 10);
	}
	int i;
	password[0] = '\0';
	int random;
	while (check_valid_password(password)) {
		i = 0;
		while (i < max_length) {
			random = arc4random_uniform(94) + 33;
			password[i] = random == 44 ? random + 1: random;
			i++;
		}
		password[i] = '\0';
		printf("Your password is %s\n", password);

	}
}
int getpassword(char *password, FILE *fp, struct termios *oflags, \
		struct termios *nflags) {
	tcgetattr(fileno(stdin), oflags);
	*nflags = *oflags;
	nflags->c_lflag &= ~ECHO;
	nflags->c_lflag |= ECHONL;

	if (tcsetattr(fileno(stdin), TCSANOW, nflags) != 0) {
		perror("tcsetattr");
		exit(1);
	}
	printf("Enter your password: ");
	fgets(password, MAXCHARS, stdin);
	password[strlen(password) - 1] = '\0';
	printf("\n");

	if (tcsetattr(fileno(stdin), TCSANOW, oflags) != 0) {
		perror("tcsetattr");
		exit(1);
	}
	return 0;
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

	length = inputLength;
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
	printf("%s is my password for encryption\n", (char *)key);
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
	// memset(key, 0, size);
	free(input);
	free(output);
	// free(key);
	// fclose(inFile);
	fclose(outFile);
	wc_FreeRng(&rng);

	return ret;
}



void add_and_encrypt(char *filename, char *input_lines, Aes *aes, char *pass) {
					FILE *out_file = fopen(filename, "w");
					AesEncrypt(aes, (byte *) pass, 256, input_lines, out_file);
					// Signature (Aes *aes, byte *pass, int size, char *inputLines, FILE *outFile)
	//			add_and_encrypt(file, out, &aes, pass);

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

void add_to_csv(char *domain, char *username, char *password, char **csv_string) {
	int total = strlen(*csv_string) + strlen(domain) + strlen(username)\
		    + strlen(password) + 4;
	printf("BEFORE:\n\n%s\n\n", *csv_string);
	*csv_string = realloc(*csv_string, total);
	char *csv = *csv_string;
	strcat(csv, "\n");
	strcat(csv, domain);
	strcat(csv, ",");
	strcat(csv, username);
	strcat(csv, ",");
	strcat(csv, password);
	printf("AFTER:\n\n%s\n\n", *csv_string)	;
}

void create_header(FILE *fp) {
	fputs("Domain name", fp);
	fputc(',', fp);
	fputs("Username", fp);
	fputc(',', fp);
	fputs("Password", fp);
	fputc('\n', fp);
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

int main(int argc, char **argv) {
	struct termios oflags, nflags;
	char file[MAXCHARS];
	FILE *fp;
	if (argc > 1) {
		strncpy(file, argv[1], MAXCHARS -1);
		fp = fopen(file, "r");
	}
	else {
		printf("Enter the name of file you want to create: ");
		fgets(file, MAXCHARS, stdin);
		file[strlen(file) - 1] = '\0';
		fp = fopen(file, "w");
		create_header(fp);
	}
	if (fp == NULL) {
		perror("fopen");
		exit(1);
	}
	Aes aes;
	char *pass = malloc(256);
	char *out;
	char decrypt_pass[MAXCHARS];
	NoEcho(pass, 256);
	strcpy(decrypt_pass, pass);
	if (AesDecrypt(&aes,(byte *) pass, 256, fp, &out) == -1) {
		fprintf(stderr, "Invalid Password\n");
		exit(1);
	}

	char yes_no[MAXCHARS];
	char generate_create[MAXCHARS];
	char domain[MAXCHARS];
	char username[MAXCHARS];
	char password[MAXCHARS];
	while (1) {
		printf("Please enter the domain name: ");
		fgets(domain, MAXCHARS, stdin);
		domain[strlen(domain) - 1] = '\0';
		printf("Please enter your username: ");
		fgets(username, MAXCHARS, stdin);
		username[strlen(username) - 1] = '\0';
		printf("Would you like to generate a password or enter your "
				"own password?\n(G) Generate\n(C) Create\n");
		fgets(generate_create, MAXCHARS, stdin);
		generate_create[strlen(generate_create) - 1] = '\0';
		while (check_gen_or_create(generate_create)) {
			printf("Enter a valid option:\n(G) Generate\n(C) "
					 "Create");
			fgets(generate_create, MAXCHARS, stdin);
			generate_create[strlen(generate_create) - 1] = '\0';

		}
		if (toupper(generate_create[0]) == 'G') {
			generate_password(password);
		}
		else {
			if (getpassword(password, fp, &oflags, &nflags) != 0) {
				fprintf(stderr, "There was an error\n");
				exit(1);
			}
		}
		add_to_csv(domain, username, password, &out);
		printf("Would you like to add another username/password?"
				"\n(Y) Yes\n(N) No\n");
		fgets(yes_no, MAXCHARS, stdin);
		yes_no[strlen(yes_no) - 1] = '\0';
		while (check_yes_no(yes_no)) {
			printf("Enter a valid option: \n(Y) Yes\n(N) No\n");
			fgets(yes_no, MAXCHARS, stdin);
			yes_no[strlen(yes_no) - 1] = '\0';
		}
		if (toupper(yes_no[0]) == 'N') {
			Aes lol;
			add_and_encrypt(file, out, &lol, decrypt_pass);
			free(out);
			exit(0);
		}
	}

}
