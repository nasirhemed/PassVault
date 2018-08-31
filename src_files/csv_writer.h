#ifndef CSV_WRITER_H_

#define MAXCHARS 256

void create_header(char *out);
void add_to_csv(char *domain, char *username, char *password, char **csv_string);
int check_valid_password(char *password);
int check_yes_no(char *yes_no);
int check_gen_or_create(char *gen_cre);
void generate_password(char *password);
void add_and_encrypt(char *filename, char *input_lines, Aes *aes, char *pass);
void get_string(char *string, char *out);
void appender_function(char **out, Aes *toEncrypt, char *decrypt_pass, \
		char *file);
void generate_csv_file(char *username, char *password);

#define CSV_WRITER_H_
#endif
