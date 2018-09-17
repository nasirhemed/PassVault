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
#include "encrypt_decrypt.h"
#include "csv_writer.h"

#define INTLENGTH 10


/**
 * Given a string, store the standard input in the
 * string "string". If EOF is entered, free out and exit with
 * exit status 1
 */
void get_string(char *string, char *out) {
	if (fgets(string, MAXCHARS, stdin) == NULL) {
		printf("\n");
		if (out != NULL) {
			memset(out, 0, strlen(out));
			free(out);
		}
		exit(1);
	}
	string[strlen(string) - 1] = '\0';
}

/**
 * Option c from ./everything
 * This function creates a new encrypted CSV file for the user
 * It asks for the username and password verification
 */
void create_csv_file() {
	char *username = malloc(MAXCHARS);
	char *password = malloc(MAXCHARS);
	char *password_verification = malloc(MAXCHARS);
	char *out;
	Aes aes;
	char *path;
	if ((path = getenv("PassFolders")) == NULL) {
		perror("getenv");
		exit(1);
	}
	strcpy(username, path);
	username[strlen(username)] = '/';
	printf("Enter your username: ");
	get_string(username + strlen(username), NULL);
	NoEcho(password, KEYSIZE);
	NoEcho(password_verification, KEYSIZE);
	while (strcmp(password, password_verification) != 0) {
		printf("Please try again.");
		NoEcho(password, KEYSIZE);
		NoEcho(password_verification, KEYSIZE);
	}
	create_header(&out);
	add_and_encrypt(username, out, &aes, password);
	free (username); free(password_verification);
}

/**
 * Option -g from ./everything
 *
 * This function takes in the username and password
 * and decrypts the password file to an actual csv file
 */
void generate_csv_file() {
	FILE *inFile;
	FILE *outFile;
	char *username = malloc(MAXCHARS);
	char *password = malloc(KEYSIZE);
	char *output_path = malloc(MAXCHARS);
	Aes aes_struct;
	char *path;
	if ((path = getenv("PassFolders")) == NULL) {
		perror("getenv");
		exit(1);
	}
	strcpy(username, path);
	username[strlen(username)] = '/';
	printf("Enter your username: ");
	get_string(username + strlen(username), NULL);
	NoEcho(password, KEYSIZE);
	inFile = fopen(username, "r"); //TODO: Make changes with path variable
	printf("Enter the path to store the csv file: ");
	get_string(output_path, NULL);
	outFile = fopen(output_path, "w");
	decrypt_file(&aes_struct, (byte *)password, KEYSIZE, inFile, outFile);
	free(username); free(output_path);
	exit(0);
}

/**
 * Option w from ./everything
 *
 * This function takes the username and the password
 * If successful login, the user will be able to add passwords
 * to his csv file
 */
void add_to_csv_file() {
	char *username = malloc(MAXCHARS);
	char *password = malloc(MAXCHARS);
	Aes aes, toEncrypt;
	char *password_copy = malloc(MAXCHARS);
	char *out;
	FILE *inFile;
	char *path;
	if ((path = getenv("PassFolders")) == NULL) {
		perror("getenv");
		exit(1);
	}
	strcpy(username, path);
	username[strlen(username)] = '/';
	printf("Enter your username: ");
	get_string(username + strlen(username), NULL);
	NoEcho(password, KEYSIZE);
	strcpy(password_copy, password);
	if ((inFile = fopen(username, "r")) == NULL) {
		fprintf(stderr, "Error: Username doesn't exist\n");
		exit(1);
	}
	if (AesDecrypt(&aes, (byte *)password, KEYSIZE, inFile, &out) == -1) {
		fprintf(stderr, "Invalid Password\n");
		exit(1);
	}
	appender_function(&out, &toEncrypt, password_copy, username);
}

/**
 * This function takes in domain, username and asks to generate or create
 * password. It then stores the credentials in the encrypted CSV file
 */
void appender_function(char **out, Aes *toEncrypt,\
	char *decrypt_pass, char *file) {
	char yes_no[MAXCHARS];
	char generate_create[MAXCHARS];
	char domain[MAXCHARS];
	char username[MAXCHARS];
	char password[MAXCHARS];
	while (1) {
		printf("Please enter the domain name: ");
		get_string(domain, *out);
		printf("Please enter your username: ");
		get_string(username, *out);
		printf("Would you like to generate a password or enter your "
				"own password?\n(G) Generate\n(C) Create\n");
		get_string(generate_create, *out);
		while (check_gen_or_create(generate_create)) {
			printf("Enter a valid option:\n(G) Generate\n(C) "
					"Create");
			get_string(generate_create, *out);

		}
		if (toupper(generate_create[0]) == 'G') {
			generate_password(password);
		}
		else {
			if (NoEcho(password, MAXCHARS) != 0) {
				fprintf(stderr, "There was an error\n");
				exit(1);
			}
		}
		add_to_csv(domain, username, password, out);
		printf("Would you like to add another username/password?"
				"\n(Y) Yes\n(N) No\n");
		get_string(yes_no, *out);
		while (check_yes_no(yes_no)) {
			printf("Enter a valid option: \n(Y) Yes\n(N) No\n");
			get_string(yes_no, *out);
		}
		if (toupper(yes_no[0]) == 'N') {
			add_and_encrypt(file, *out, toEncrypt, decrypt_pass);
			exit(0);
		}
	}
}

/**
 * Given a string, check if it could be a valid password.
 * i.e: If a string has less than ten characters or the following:
 *
 * If it has at least 1 digit, 1 special character, 3 lowercase letters
 * and 3 upper case letters
 */
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

/**
 * Given a string, check whether it is "Y" or "N" (Case doesn't matter)
 */
int check_yes_no(char *yes_no) {
	int valid = strlen(yes_no) == 1 && (toupper(yes_no[0]) == 'Y'\
			|| toupper(yes_no[0]) == 'N') ? 0 : 1;
	return valid;

}
/**
 * Given a string, check whether it is "G" or "C" (Case doesn't matter)
 */
int check_gen_or_create(char *gen_cre) {
	int valid = strlen(gen_cre) == 1 && \
		    (toupper(gen_cre[0]) == 'G' || toupper(gen_cre[0]) == 'C')\
		    ? 0 : 1;
	return valid;
}

/**
 * Generate a valid password based on the length of the password.
 * The length of the password will be taken from the standard input
 */
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

/**
 * Given a filename, csv input lines, Aes struct and the password, encrypt the
 * csv input lines and store it in a file "filename"
 */
void add_and_encrypt(char *filename, char *input_lines, Aes *aes, char *pass) {
	FILE *out_file = fopen(filename, "w");
	if (out_file == NULL) {
		fprintf(stderr, "FILE COULD NOT BE OPENED\n");
		perror("fopen");
		exit(1);
	}
	AesEncrypt(aes, (byte *) pass, 256, input_lines, out_file);

}

/**
 * Given a domain name, username, password, add it to the csv_string
 */
void add_to_csv(char *domain, char *username, char *password,\
		char **csv_string) {
	int total = strlen(*csv_string) + strlen(domain) + strlen(username)\
		    + strlen(password) + 4;
	*csv_string = realloc(*csv_string, total);
	char *csv = *csv_string;
	strcat(csv, "\n");
	strcat(csv, domain);
	strcat(csv, ",");
	strcat(csv, username);
	strcat(csv, ",");
	strcat(csv, password);
}

/**
 * Given a uninitialzed string, create a csv_header
 * Domain name,Username,Password
 */
void create_header(char **out) {
	*out = malloc(30);
	strcat(*out, "Domain name,Username,Password");
	(*out)[29] = '\0';
}
