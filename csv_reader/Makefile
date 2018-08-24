FLAGS= -g -std=gnu99 

csv_reader: csv_reader.o
	gcc ${FLAGS} $^ -o $@
CSV_Writer: csv_writer.o
	gcc ${FLAGS} $^ -o $@ -lbsd
%.o: %.c
	gcc ${FLAGS} -c $< -lbsd 
clean:
	rm -f *.o csv_reader CSV_Writer
