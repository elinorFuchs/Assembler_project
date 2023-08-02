cmake_minimum_required(VERSION 3.25)
project(Assembler_OpenU_project C)

set(CMAKE_C_STANDARD 90)

add_executable(Assembler_OpenU_project assembler.c assembler.h line_parser.c line_parser.h first_pass.c first_pass.h help_functions.c help_functions.h)
