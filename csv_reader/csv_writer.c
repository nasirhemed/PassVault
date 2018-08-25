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
#define MAXCHARS 256
#define INTLENGTH 10

int check_valid_password(char *password) {
	if (strlen(password) == 0) {
		return 1;
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
			password[i] = random;
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

void add_to_csv(char *domain, char *username, char *password, FILE *fp) {
	fputs(domain, fp);
	fputc(',', fp);
	fputs(username, fp);
	fputc(',', fp);
	fputs(password, fp);
	fputc('\n', fp);
}

void create_header(FILE *fp) {
	fputs("Domain name", fp);
	fputc(',', fp);
	fputs("Username", fp);
	fputc(',', fp);
	fputs("Password", fp);
	fputc('\n', fp);
}

int main(int argc, char **argv) {
	struct termios oflags, nflags;
	char file[MAXCHARS];
	FILE *fp;
	if (argc > 1) {
		strncpy(file, argv[1], MAXCHARS -1);
		fp = fopen(file, "a");
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
		add_to_csv(domain, username, password, fp);
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
			fclose(fp);
			exit(0);
		}
	}

}
