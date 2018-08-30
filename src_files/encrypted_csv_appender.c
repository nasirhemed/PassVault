/** CSV Writer
 * This program writes CSV entries of the form domain_name, username, password
 * to the CSV file provided by the argument. If there is no argument provided
 * then, it creates a CSV file and writes to it.
 * Usage: CSV_Writer <file>
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
	FILE *fp;
	if (argc != 2) {
		fprintf(stderr, "%s\n", "Usage:\n encrypted_csv_appender filename");
		exit(1);
	}
	fp = fopen(argv[1], "r");
	if (fp == NULL) {
		perror("fopen");
		exit(1);
	}
	Aes aes, toEncrypt;
	char *pass = malloc(256);
	char *out;
	char decrypt_pass[MAXCHARS];
	NoEcho(pass, 256);
	strcpy(decrypt_pass, pass);
	if (AesDecrypt(&aes,(byte *) pass, 256, fp, &out) == -1) {
		fprintf(stderr, "Invalid Password\n");
		exit(1);
	}

	appender_function(&out, &toEncrypt, decrypt_pass, argv[1]);
	free(out);
	exit(0);

}
