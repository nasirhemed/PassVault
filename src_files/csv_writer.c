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

void add_and_encrypt(char *filename, char *input_lines, Aes *aes, char *pass) {
					FILE *out_file = fopen(filename, "w");
					AesEncrypt(aes, (byte *) pass, 256, input_lines, out_file);
					// Signature (Aes *aes, byte *pass, int size, char *inputLines, FILE *outFile)
	//			add_and_encrypt(file, out, &aes, pass);

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
