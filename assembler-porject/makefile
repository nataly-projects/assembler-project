CC = gcc 
CFLAGS = -ansi -Wall -pedantic 
GLOBAL = globals.h 
EXE_DEPS = assembler.o code.o first_pass.o instructions.o table.o utils.o  second_pass.o write_output.o

assembler: $(EXE_DEPS) $(GLOBAL)
	$(CC) -g $(EXE_DEPS) $(CFLAGS) -lm -o $@

assembler.o: assembler.c $(GLOBAL)
	$(CC) -c assembler.c $(CFLAGS) -o $@

first_pass.o: first_pass.c first_pass.h $(GLOBAL)
	$(CC) -c first_pass.c $(CFLAGS) -o $@

table.o: table.c table.h $(GLOBAL)
	$(CC) -c table.c $(CFLAGS) -o $@

utils.o: utils.c instructions.h $(GLOBAL)
	$(CC) -c utils.c $(CFLAGS) -lm -o $@

instructions.o: instructions.c instructions.h $(GLOBAL_DEPS)
	$(CC) -c instructions.c $(CFLAGS) -o $@

code.o: code.c code.h $(GLOBAL_DEPS)
	$(CC) -c code.c $(CFLAGS) -o $@

second_pass.o: second_pass.c second_pass.h $(GLOBAL_DEPS)
	$(CC) -c second_pass.c $(CFLAGS) -o $@

write_output.o: write_output.c write_output.h $(GLOBAL_DEPS)
	$(CC) -c write_output.c $(CFLAGS) -o $@

clean:
	rm -rf *.o