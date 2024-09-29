#include "shell.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstdio>
#include <cerrno>

using namespace std;

void trim(string &s)
{
    size_t start=s.find_first_not_of(' ');
    size_t end=s.find_last_not_of(' ');
    s=s.substr(start,end-start+1);
}

void removeQuotes(char* str)
{
    char* dst=str;
    char* src=str;
    while (*src=='"')
        src++;
    while(*src!='\0')
    {
        if(*src!='"')
            *dst++=*src;
        src++;
    }
    *dst='\0';
}

void handleRedirection(string &command,size_t pos,int flags)
{
    string comm;
    string filename;
    if(pos>0 && command[pos+1]=='>')
    {
        comm=command.substr(0,pos);
        filename=command.substr(pos+2);
    }
    else
    {
        comm=command.substr(0,pos);
        filename=command.substr(pos+1);
    }
    trim(comm);
    trim(filename);
    int len=comm.size();
    char *comm1=(char*)malloc(len);
    strcpy(comm1,comm.c_str());
    vector<char*> execArgs;
    char *token=strtok(comm1, " ");
    bool inQuotes=false;
    while(token!=nullptr)
    {
        removeQuotes(token);
        execArgs.push_back(token);
        if(strrchr(token,'"')!=nullptr)
            inQuotes=true;
        token=strtok(nullptr," ");
    }
    if(!execArgs.empty() && strcmp(execArgs[0],"cat")==0)
        execArgs[0]=(char*)"echo";
    execArgs.push_back(nullptr);
    int fd=open(filename.c_str(),flags,0644);
    if(fd<0)
    {
        perror("\nCould not open file");
        return;
    }
    pid_t pid=fork();
    if(pid==0)
    {
        if(dup2(fd,STDOUT_FILENO)<0)
        {
            perror("\nError duplicating file descriptor");
            exit(EXIT_FAILURE);
        }
        close(fd);
        if(execvp(execArgs[0],execArgs.data())==-1)
        {
            perror("\nError executing command");
            exit(EXIT_FAILURE);
        }
    }
    else if(pid>0)
    {
        close(fd);
        waitpid(pid,nullptr,0);
    }
    else
        perror("\nFork failed");
    free(comm1);
}

void handleInputRedirection(string &command,size_t pos)
{
    string comm=command.substr(0, pos);
    string filename=command.substr(pos + 1);
    trim(comm);
    trim(filename);
    vector<char*> execArgs;
    char *token1;
    int len=comm.size();
    if(len>0)
    {
        char *tok_str=(char*)malloc(len);
        strcpy(tok_str,comm.c_str());
        token1=strtok(tok_str," ");
        while(token1!=NULL)
        {
            execArgs.push_back(const_cast<char*>(token1));
            token1=strtok(NULL," ");
        }
    }
    execArgs.push_back(nullptr);
    int fd=open(filename.c_str(),O_RDONLY);
    if(fd<0)
    {
        perror("\nCould not open file");
        return;
    }
    pid_t pid=fork();
    if(pid==0)
    {
        dup2(fd,STDIN_FILENO);
        close(fd);
        if(execvp(execArgs[0],execArgs.data())==-1)
        {
            printf("\nError executing command.");
            exit(EXIT_FAILURE);
        }
    }
    else
        waitpid(pid, nullptr, 0);
    close(fd);
}

void execpipeline(vector<vector<string>> &commands)
{
    int numCommands=commands.size();
    int pipefd[2],input_fd=0;
    pid_t pid;
    for(int i=0;i<numCommands;i++)
    {
        size_t pos;
        int fd=-1;
        int flags=0;
        for(auto &cmd:commands[i])
        {
            if((pos=cmd.find(">>"))!=string::npos)
            {
                flags=O_WRONLY | O_CREAT | O_APPEND;
                fd=open(cmd.substr(pos + 2).c_str(),flags,0644);
                cmd=cmd.substr(0,pos);
            }
            else if((pos=cmd.find(">"))!=string::npos)
            {
                flags = O_WRONLY | O_CREAT | O_TRUNC;
                fd = open(cmd.substr(pos + 1).c_str(), flags, 0644);
                cmd = cmd.substr(0, pos);
            }
        }
        if(i<numCommands-1)
        {
            if(pipe(pipefd)==-1)
            {
                perror("\nPipe failed");
                return;
            }
        }
        pid=fork();
        if(pid<0)
        {
            perror("\nFork failed");
            return;
        }
        else if(pid==0)
        {
            if(input_fd!=0)
            {
                dup2(input_fd,STDIN_FILENO);
                close(input_fd);
            }
            if(i<numCommands-1)
            {
                dup2(pipefd[1],STDOUT_FILENO);
                close(pipefd[1]);
            } else if(fd!=-1)
            {
                dup2(fd,STDOUT_FILENO);
                close(fd);
            }
            close(pipefd[0]);
            vector<char*> execArgs;
            for(string &arg:commands[i])
                execArgs.push_back(const_cast<char*>(arg.c_str()));
            execArgs.push_back(nullptr);
            if(execvp(execArgs[0],execArgs.data())==-1)
            {
                perror("\nExecution failed");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            close(pipefd[1]);
            input_fd=pipefd[0];
            waitpid(pid,nullptr,0);
        }
    }
}

void execcommand(string &command)
{
    if(command.back()=='&')
    {
        command.pop_back();
        vector<string> subtokens;
        stringstream ss(command);
        string token;
        while(ss>>token)
            subtokens.push_back(token);
        vector<char*> execArgs;
        for(const auto &subtoken:subtokens)
            execArgs.push_back(const_cast<char*>(subtoken.c_str()));
        execArgs.push_back(nullptr);
        bgprocess=fork();
        if(bgprocess<0)
        {
            perror("\nError in creating fork(): ");
            return;
        }
        else if(bgprocess==0)
        {
            if(execvp(execArgs[0],execArgs.data())==-1)
            {
                printf("\nError executing command.");
                return;
            }
        }
        else
            printf("\nPID: %d",bgprocess);
        return;
    }
    vector<vector<string>> commands;
    size_t pipePos=command.find('|');
    if(pipePos!=string::npos)
    {
        vector<string> pipedCommands;
        stringstream ss(command);
        string segment;
        while(getline(ss,segment,'|'))
        {
            vector<string> commandParts;
            stringstream commandStream(segment);
            string token;
            while (commandStream >> token)
            {
                commandParts.push_back(token);
            }
            commands.push_back(commandParts);
        }
        execpipeline(commands);
        return;
    }
    size_t pos;
    if((pos=command.find(">>"))!=string::npos)
    {
        handleRedirection(command, pos, O_WRONLY | O_CREAT | O_APPEND);
        return;
    }
    else if((pos=command.find(">"))!=string::npos)
    {
        handleRedirection(command, pos, O_WRONLY | O_CREAT | O_TRUNC);
        return;
    }
    else if((pos=command.find('<'))!=string::npos)
    {
        handleInputRedirection(command,pos);
        return;
    }
    char *token;
    vector<string> subtokens;
    int len=command.size();
    if(len>0)
    {
        char *comm=(char*)malloc(len);
        strcpy(comm,command.c_str());
        token=strtok(comm," ");
        while(token!=NULL)
        {
            subtokens.push_back(token);
            token=strtok(NULL," ");
        }
    }
    if(subtokens[0]=="cd")
    {
        string cdArg=(subtokens.size()>1?subtokens[1]:"~");
        handlecd(cdArg);
    }
    else if(subtokens[0]=="pwd")
        handlepwd();
    else if(subtokens[0]=="ls")
    {
        vector<string> args(subtokens.begin()+1,subtokens.end());
        if(args.empty())
        {
            vector<string> defaultArgs{"."};
            handlels(defaultArgs);
        }
        else
            handlels(args);
    }
    else if(subtokens[0]=="search")
    {
        string searchArg=subtokens.size()>1?subtokens[1]:"";
        handlesearch(searchArg);
    }
    else if(subtokens[0]=="history")
        handlehistory(subtokens.size()>1?stoi(subtokens[1]):20);
    else if(subtokens[0]=="pinfo")
        pinfo(subtokens.size()>1?stoi(subtokens[1]):getpid());
    else if(subtokens[0]=="echo")
    {
        string echoArg;
        if(subtokens.size()>1)
        {
            echoArg=subtokens[1];
            for(size_t i=2;i<subtokens.size();i++)
                echoArg+=" "+subtokens[i];
        }
        else
            echoArg="";
        if(!echoArg.empty() && echoArg.front()=='"')
            echoArg.erase(0,1);
        if(!echoArg.empty() && echoArg.back()=='"')
            echoArg.pop_back();
        handleecho(echoArg);
    }
    else if(subtokens[0]=="exit")
    {
        exit(EXIT_SUCCESS);
    }
    else
    {
        vector<char*> execArgs;
        char *token1;
        int len=command.size();
        if(len>0)
        {
            char *tok_str=(char*)malloc(len);
            strcpy(tok_str,command.c_str());
            token1=strtok(tok_str," ");
            while(token1!=NULL)
            {
                execArgs.push_back(const_cast<char*>(token1));
                token1=strtok(NULL," ");
            }
        }
        execArgs.push_back(nullptr);
        pid_t pid1 = fork();
        if(pid1<0) 
        {
            perror("\nFork failed");
            return;
        }
        else if(pid1==0)
        {
            if(execvp(execArgs[0],execArgs.data())==-1)
            {
                perror("\nExecution failed");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            int status;
            waitpid(pid1,&status,0);
            if(WIFEXITED(status))
            {
                int exitStatus=WEXITSTATUS(status);
                if(exitStatus!=0)
                printf("\nCommand exited with status %d\n", exitStatus);
            }
        }
    }
}