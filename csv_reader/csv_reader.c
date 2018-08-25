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
#define MAXCHARS 256 

/**
 * Given a char array and a CSV file pointer, set the string
 * to the cell the file pointer is currently at
 *
 * Precondition: File is a CSV file that is comma seperated
 */
void set_string(char password[], FILE* file) {
	char *str = password;
	int i = 0;
	int c;
	while ((c = fgetc(file)) != '\n' && c != EOF && c != ',') {
		str[i] = c;
		i++;
	}
	str[i] = '\0';
}


/**
 * Given a domain_name, check if the name is there in 
 * the CSV file. If it is then output the username and 
 * password. Otherwise, output no domain found
 *
 * Example of a domain name could be like Facebook, Twitter, etc
 */
void check_domain(char *domain_name, FILE *file) {

	char domain[MAXCHARS];
	char username[MAXCHARS];
	char password[MAXCHARS];

	int character;
	fseek(file, 0, SEEK_SET);
	fgets(domain, MAXCHARS, file);
	while (1) {
		memset(domain, '\0', MAXCHARS);
		set_string(domain, file);

		if (strcmp(domain, domain_name) == 0) {
			set_string(username, file);
			set_string(password, file);
			printf("Domain name: %s\nUsername: %s\nPassword: %s\n", domain, username, password);
			return;
		}
		else {
			while ((character= fgetc(file)) != '\n' && character != EOF);
			if (character== EOF) {
				printf("No such domain found\n");
				return;
			}

		}

	}

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

	char domain_name[MAXCHARS];
	while (1) {

		printf("Enter a record the domain that you are looking for:");	
		fgets(domain_name, MAXCHARS, stdin);
		domain_name[strlen(domain_name) - 1] = '\0';
		check_domain(domain_name, fp);
	}
	exit(0);
}
