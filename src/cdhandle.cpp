#include "shell.h"
#include <string>
#include <cerrno>
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <pwd.h>

using namespace std;

void handlecd(string &arg)
{
    char cwd[1024];
    if(getcwd(cwd,sizeof(cwd))==nullptr)
    {
        perror("\nError in getting current working directory: ");
        return;
    }
    string currDirectory=cwd;
    if(arg==".")
        return;
    else if(arg=="..")
    {
        if(chdir("..")!=0)
        {
            perror("\nError in changing to previous directory: ");
            return;
        }
    }
    else if(arg=="-")
    {
        if(prevDirectory.empty())
        {
            perror("\nNo previous directory. You are in home");
            return;
        }
        if(chdir(prevDirectory.c_str())!=0)
            perror("\nPrevious directory");
    }
    else if(arg=="~")
    {
        if(homeDirectory.empty())
        {
            struct passwd *pw=getpwuid(getuid());
            homeDirectory=pw->pw_dir;
        }
        if(chdir(homeDirectory.c_str())!=0)
            perror("\nError in changing to home directory: ");
    }
    else
    {
        if(chdir(arg.c_str())!=0)
            perror("\nDirectory path does not exist: ");
    }
    prevDirectory=currDirectory;
}