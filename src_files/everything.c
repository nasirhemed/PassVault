#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <ctype.h>
#include <bsd/stdlib.h>

#include "encrypt_decrypt.h"
#include "csv_writer.h"
#include "csv_reader.h"

int main(int argc, char **argv) {
	char options;
	if((options = getopt(argc, argv, "gcrw")) == -1 || options == '?') {
		fprintf(stderr, "Usage\neverything -g -c -r -w\n");
		exit(1);
	}
	printf("The option chosen was %c\n", options);
	switch(options) {
		case 'g':
			generate_csv_file();
			break;
		case 'c':
			break;
		case 'r':
			break;
		case 'w':
			break;
		}
	exit(0);
}

