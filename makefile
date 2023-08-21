assembler: assembler.o line_parser.o pre_assembler.o first_pass.o second_pass.o help_functions.o
	gcc -ansi -Wall -pedantic -g  assembler.o line_parser.o pre_assembler.o first_pass.o second_pass.o help_functions.o -o assembler
assembler.o: assembler.c assembler.h
	gcc -g -c -ansi -Wall -pedantic assembler.c -o assembler.o
line_parser.o: line_parser.c line_parser.h
	gcc -g -c -ansi -Wall line_parser.c -o line_parser.o
pre_assembler.o: pre_assembler.c pre_assembler.h
	gcc -g -c -ansi -Wall -pedantic pre_assembler.c -o pre_assembler.o
first_pass.o: first_pass.c first_pass.h
	gcc -g -c -ansi -Wall first_pass.c -o first_pass.o
second_pass.o: second_pass.c second_pass.h
	gcc -g -c -ansi -Wall second_pass.c -o second_pass.o
help_functions.o: help_functions.c line_parser.h
	gcc -g -c -ansi -Wall help_functions.c -o help_functions.o
