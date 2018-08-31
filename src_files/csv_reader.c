#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "csv_reader.h"
#include "encrypt_decrypt.h"


/**
 * Given a char array and a CSV string pointer, set the string
 * to the cell the string pointer is currently at
 * Precondition: String is of type CSV that is comma seperated
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


/**
 * Given a domain_name, check if the name is there in
 * the CSV string. If it is then output the username and
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

void read_csv_file() {
	char *username = malloc(MAXCHARS);
	char *password = malloc(MAXCHARS);
	Aes aes;
	char *domain_name = malloc(MAXCHARS);
	char *out;
	FILE *inFile;
	printf("Enter your username: ");
	get_string(username, NULL);
	NoEcho(password, KEYSIZE);
	if ((inFile = fopen(username, "r")) == NULL) {
		fprintf(stderr, "Error: Username doesn't exist\n");
		exit(1);
	}	
	AesDecrypt(&aes, (byte *)password, KEYSIZE, inFile, &out);
	read_inputs(domain_name, out);
	free(domain_name); free(out); free(username); 
}

/**
	* This function reads inputs from stdin and prints
	* whether the record is in the string out
	*/
void read_inputs(char *domain_name, char *out) {

	while (1) {

		printf("Enter a record the domain that you are looking for:"
				"(Press Enter or EOF if you want to skip): "
		      );

		if (fgets(domain_name, MAXCHARS, stdin) == NULL ||\
				domain_name[0] == '\n'){
			printf("\n");
			break;
		}
		domain_name[strlen(domain_name) - 1] = '\0';
		printf("%s\n", domain_name);
		check_domain(domain_name, out);
	}
}

void view_all_passwords(char *out) {
	printf("%s\n", out);
}

void generate_password_file(char *out, char *filename) {
	FILE *fp = fopen(filename, "w");
	int size = strlen(out);
	fwrite(out, 1, size, fp);

}
