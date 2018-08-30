#ifndef CSV_READER_H_
#define CSV_READER_H_

#ifndef MAXCHARS
#define MAXCHARS 256
#endif

void set_string(char password[], char **pointer_string);
void check_domain(char *domain_name, char *csv_string);
void read_inputs(char *domain_name, char *out);

#endif
