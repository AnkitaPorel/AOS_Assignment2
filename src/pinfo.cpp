#include "shell.h"
#include <iostream>
#include <unistd.h>
#include <limits.h>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <sstream>
#include <cstdlib>

using namespace std;

void pinfo(int pid)
{
    string pidStr=to_string(pid);
    string procPath="/proc/"+pidStr;
    string statFilePath=procPath+"/stat";
    int statfd=open(statFilePath.c_str(),O_RDONLY);
    if(statfd==-1)
    {
        perror("\nError in opening process path: ");
        return;
    }
    char buff[4096];
    ssize_t bytesRead=read(statfd,buff,sizeof(buff)-1);
    if(bytesRead==-1)
    {
        perror("\nError in reading: ");
        close(statfd);
        return;
    }
    buff[bytesRead]='\0';
    string status;
    char *token=strtok(buff," ");
    for(int i=0;i<3;i++)
        token=strtok(nullptr," ");
    if(token)
        status=token;
    close(statfd);
    string statmFilePath=procPath+"/statm";
    int statmFd=open(statmFilePath.c_str(),O_RDONLY);
    if(statmFd==-1)
    {
        perror("\nError: ");
        return;
    }
    bytesRead=read(statmFd,buff,sizeof(buff)-1);
    if(bytesRead==-1)
    {
        perror("\nError: ");
        close(statmFd);
        return;
    }
    string memory;
    token=strtok(buff," ");
    if(token)
        memory=token;
    close(statmFd);
    char exePath[PATH_MAX];
    ssize_t len=readlink((procPath + "/exe").c_str(), exePath, sizeof(exePath) - 1);
    if(len==-1)
    {
        perror("\nError: ");
        return;
    }
    exePath[len]='\0';
    cout<<"\nPID -- "<<pid<<endl;
    cout<<"Process Status -- "<<status<<endl;
    cout<<"Memory -- " << memory <<" {Virtual Memory}"<<endl;
    cout<<"Executable Path -- "<<exePath<<endl;
}