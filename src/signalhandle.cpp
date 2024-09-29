#include "shell.h"
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

void sigintHandler(int sig_num)
{
    signal(SIGINT,sigintHandler);
}

void sigtstpHandler(int sig_num)
{
    signal(SIGTSTP,sigtstpHandler);
}