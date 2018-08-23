FLAGS= -g -std=gnu99

csv_reader: csv_reader.o
	gcc ${FLAGS} $^ -o $@
%.o: %.c
	gcc ${FLAGS} -c $< 
clean:
	rm -f *.o csv_reader
