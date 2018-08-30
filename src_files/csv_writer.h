#ifndef CSV_WRITER_H_

#define MAXCHARS 256

void create_header(FILE *fp);
void add_to_csv(char *domain, char *username, char *password, char **csv_string);
int check_valid_password(char *password);
int check_yes_no(char *yes_no);
int check_gen_or_create(char *gen_cre);
void generate_password(char *password);
void add_and_encrypt(char *filename, char *input_lines, Aes *aes, char *pass);

#define CSV_WRITER_H_
#endif
