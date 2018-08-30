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
			if (NoEcho(password, MAXCHARS) != 0) {
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
			exit(0);
		}
	}

}
