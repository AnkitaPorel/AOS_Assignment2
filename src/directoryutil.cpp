#include "shell.h"
#include <pwd.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <cerrno>
#include <cstdio>
#include <iostream>
#include <limits.h>

using namespace std;

string homeDirectory;
string prevDirectory;
string USER_LOG="commandhistory.txt";

void setHome()
{
    char *pwdir=getcwd(nullptr,0);
    if(pwdir==nullptr)
    {
        perror("\nError in opening present working directory: ");
        return;
    }
    homeDirectory=string(pwdir);
    string fullPath=string(pwdir)+"/"+USER_LOG;
    USER_LOG=fullPath;
    prevDirectory=homeDirectory;
    int fd=open(USER_LOG.c_str(),O_RDWR | O_APPEND | O_CREAT,0644);
    if(fd<0)
    {
        perror("\nError in opening file");
    }
    else
        close(fd);
    free(pwdir);
}

string getCurrentDir()
{
    string user=getpwuid(getuid())->pw_name;
    char syst[256];
    if (gethostname(syst, sizeof(syst)) != 0)
    {
        perror("\nError getting hostname: ");
        exit(EXIT_FAILURE);
    }
    char pwdir2[8000];
    if(getcwd(pwdir2, sizeof(pwdir2))==nullptr)
    {
        perror("PWD error: ");
        exit(EXIT_FAILURE);
    }
    string presentdir(pwdir2);
    if(presentdir==homeDirectory)
        presentdir="~";
    else if(presentdir.find(homeDirectory)==0)
        presentdir.replace(0,homeDirectory.size(),"~");
    string prompt=user+"@"+syst+":"+presentdir+">";
    return prompt;
}