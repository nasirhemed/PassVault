/*
 * CSV Reader: This program reads entries from an encrypted CSV file.
 * The labels will be provided as stdin and the output will
 * be printed to stdout
 *
 * Note: The CSV file is assumed to be of the form
 * Domain name, Username, Password.
 * The program will exit if the wrong password is entered.
 * Usage: csv_encrypted_reader encrypted_file
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "encrypt_decrypt.h"
#include "csv_reader.h"


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
	char *pass = malloc(KEYSIZE);
	char *out;

	NoEcho(pass, KEYSIZE);
	if (AesDecrypt(&aes,(byte *) pass, KEYSIZE, fp, &out) == -1) {
		fprintf(stderr, "Invalid Password\n");
		memset(domain_name, 0, MAXCHARS);
		free(domain_name);
		exit(1);
	}
	read_inputs(domain_name, out);
	memset(out, 0, strlen(out));
	memset(domain_name, 0, MAXCHARS);
	free(domain_name);
	free(out);
	exit(0);
}
