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
#define MAXCHARS 256

int check_yes_no(char *yes_no) {
	int valid = strlen(yes_no) == 1 && (toupper(yes_no[0]) == 'Y'\
			|| toupper(yes_no[0]) == 'N') ? 0 : 1;
	return valid;

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
	}
	if (fp == NULL) {
		perror("fopen");
		exit(1);
	}
	char yes_no[MAXCHARS];
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
		if (getpassword(password, fp, &oflags, &nflags) != 0) {
			fprintf(stderr, "There was an error\n");
			exit(1);
		}
	//	add_to_csv(domain, username, password, fp);
		printf("Would you like to add another username/password?\
				\n(1) Y\n(2) N\n");
		fgets(yes_no, MAXCHARS, stdin);
		yes_no[strlen(yes_no) - 1] = '\0';
		while (check_yes_no(yes_no)) {
			printf("Enter a valid option: \n(1) Y\n(2) N\n");
			fgets(yes_no, MAXCHARS, stdin);
			yes_no[strlen(yes_no) - 1] = '\0';
		}
		if (toupper(yes_no[0]) == 'N') {
			break;
		}
	}

}
