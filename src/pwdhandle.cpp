#include "shell.h"
#include <cerrno>
#include <pwd.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <stdio.h>
#include <cstdio>
#include <cstring>
#include <limits.h>

using namespace std;

void handlepwd()
{
    char *pwdir1=getcwd(nullptr,0);
    if(pwdir1!=nullptr)
        cout<<"\n"<<pwdir1<<"\n";
    free(pwdir1);
}