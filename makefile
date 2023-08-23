assembler: assembler.ob line_parser.ob pre_assembler.ob first_pass.ob second_pass.ob help_functions.ob
	gcc -ansi -g -Wall -pedantic -g  assembler.ob line_parser.ob pre_assembler.ob first_pass.ob second_pass.ob help_functions.ob -o assembler -lm
assembler.ob: assembler.c assembler.h
	gcc -c -ansi -g -Wall -pedantic assembler.c -o assembler.ob
line_parser.ob: line_parser.c line_parser.h
	gcc -c -ansi -g -Wall line_parser.c -o line_parser.ob
pre_assembler.ob: pre_assembler.c pre_assembler.h
	gcc -c -ansi -g -Wall -pedantic pre_assembler.c -o pre_assembler.ob
first_pass.ob: first_pass.c first_pass.h
	gcc -c -ansi -g -Wall first_pass.c -o first_pass.ob
second_pass.ob: second_pass.c second_pass.h
	gcc -c -ansi -g -Wall second_pass.c -o second_pass.ob
help_functions.ob: help_functions.c line_parser.h
	gcc -c -ansi -g -Wall help_functions.c -o help_functions.ob
