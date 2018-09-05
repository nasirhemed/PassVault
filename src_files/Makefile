
CC=gcc
CFLAGS=-Wall
LIBS= -lwolfssl -lbsd
OUT=CSV_Writer csv_reader aes-file-encrypt encrypted_csv_reader\
    encrypted_csv_appender vault

vault: vault.o encryption_algorithms.o csv_writer.o csv_reader.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)
encrypted_csv_appender: encrypted_csv_appender.o encryption_algorithms.o csv_writer.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)
encrypted_csv_reader: encrypted_csv_reader.o encryption_algorithms.o csv_reader.o csv_writer.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)
%.o: %.c
	$(CC) ${CFLAGS} -c $< $(LIBS)
aes-file-encrypt: aes-file-encrypt.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f *.o $(OUT)
