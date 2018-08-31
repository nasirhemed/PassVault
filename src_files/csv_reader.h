#ifndef CSV_READER_H_
#define CSV_READER_H_

#ifndef MAXCHARS
#define MAXCHARS 256
#endif

void set_string(char password[], char **pointer_string);
void check_domain(char *domain_name, char *csv_string);
void read_inputs(char *domain_name, char *out);
void view_all_passwords(char *out);
void generate_password_file(char *out, char *filename);
void read_csv_file();
void get_string(char *string, char *out);

#endif
