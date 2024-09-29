#include "shell.h"
#include <deque>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <sys/stat.h>
#include <fstream>
#include <iostream>

using namespace std;

void savehist(string &userInput)
{
    deque<string> history;
    int fd=open(USER_LOG.c_str(),O_RDWR | O_CREAT , 0644);
    if(fd<0)
    {
        perror("\nError in creating user log file: ");
        return;
    }
    char buff[4096];
    ssize_t sizeread;
    string remaining;
    while((sizeread=read(fd,buff,sizeof(buff)-1))>0)
    {
        buff[sizeread]='\0';
        string command(buff);
        command=remaining+command;
        size_t pos;
        while((pos=command.find('\n'))!=string::npos)
        {
            history.push_back(command.substr(0,pos));
            command.erase(0,pos+1);
        }
        remaining=command;
    }
    if(!remaining.empty())
        history.push_back(remaining);
    if(history.size()>=20)
    {
        while(history.size()>=20)
        {
            history.pop_front();
        }
    }
    history.push_back(userInput);
    ftruncate(fd,0);
    lseek(fd,0,SEEK_SET);
    for(const string &cmd : history)
    {
        write(fd,cmd.c_str(),cmd.size());
        write(fd,"\n",1);
    }
    close(fd);
}

vector<string> loadhist()
{
    vector<string> history;
    char buff[4096];
    ssize_t sizeread;
    int fd=open(USER_LOG.c_str(),O_RDONLY);
    if(fd<0)
    {
        perror("\nError opening history file");
        return history;
    }
    string remaining;
    while((sizeread=read(fd,buff,sizeof(buff)-1))>0)
    {
        buff[sizeread]='\0';
        string command(buff);
        command=remaining+command;
        size_t pos;
        while((pos=command.find('\n'))!=string::npos)
        {
            history.push_back(command.substr(0,pos));
            command.erase(0,pos+1);
        }
        remaining=command;
    }
    if(sizeread<0)
        cout<<"\nError reading from history file: " << endl;
    if(!remaining.empty())
        history.push_back(remaining);
    if(close(fd)<0)
        perror("\nError closing history file");
    return history;
}

void handlehistory(int num)
{
    vector<string> hist=loadhist();
    int n=hist.size();
    int count=min(num,n);
    for(int i=0;i<count;i++)
        printf("\n%s",hist[i].c_str());
}