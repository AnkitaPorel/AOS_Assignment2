Assignment 2

Instructions to Execute:
Step 1: Command to compile the Makefile- make
Step 2: To run the program shell- ./my_program

User Defined Header File- shell.h

CPP Files-

autocompletehandle.cpp		-> For tab autocomplete
cdhandle.cpp			-> For cd command
commandexec.cpp			-> For command execution and parsing of commands
directoryutil.cpp		-> For initializing home directory and getting current directory
echohandle.cpp			-> For handling echo command
historyhandle.cpp		-> For handling history command
lshandle.cpp			-> For handling ls command
main.cpp			-> For running the main loop of taking user input
pinfo.cpp			-> For handling pinfo command
pwdhandle.cpp			-> For handling pwd command
searchhandle.cpp		-> For handling search command
signalhandle.cpp		-> For handling ctrl+C, ctrl+D and ctrl+Z signals


Instructions for Using the Shell-
To terminate shell- exit
To log out of shell- ctrl+d
To push any currently running foreground job to the background- ctrl+z
To interrupt any currently running foreground job- ctrl+c
