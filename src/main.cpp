#include "shell.h"
#include <cstring>
#include <iostream>
#include <cstdio>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include <termios.h>

using namespace std;

pid_t bgprocess;
pid_t fgprocess=-1;

void enableRawMode(struct termios& orig_termios)
{
    struct termios raw;
    tcgetattr(STDIN_FILENO, &orig_termios);
    raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disableRawMode(struct termios& orig_termios)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

int main()
{
    struct termios orig_termios;
    enableRawMode(orig_termios);
    signal(SIGINT,sigintHandler);
    signal(SIGTSTP,sigtstpHandler);
    setHome();
    int i=-1;
    while(true)
    {
        vector<string> hist=loadhist();
        if(!hist.empty())
            i=hist.size()-1;
        string currDir=getCurrentDir();
        printf("\n%s",currDir.c_str());
        char ch;
        string userInput;
        while(true)
        {
            if(cin.get(ch))
            {
                if(ch=='\n')
                {
                    break;
                }
                else if(ch=='\x04')
                {
                    disableRawMode(orig_termios);
                    return 0;
                }
            }
            if(ch=='\033')
            {
                ch=getchar();
                if(ch=='[')
                {
                    ch=getchar();
                    if(ch=='A')
                    {
                        printf("\33[2K\r%s",currDir.c_str());
                        if(i>0 && i<hist.size())
                        {
                            printf("%s",hist[i].c_str());
                            userInput=hist[i];
                            i--;
                        }
                        else if(i==0)
                        {
                            printf("%s",hist[i].c_str());
                            userInput=hist[i];
                        }
                        else
                            continue;
                    }
                    continue;
                }
            }
            if(ch=='\t')
            {
                if(!userInput.empty())
                    autocomplete(userInput);
                else
                {
                    printf("\nNo autocomplete on empty string.");
                    continue;
                }
            }
            else if(ch==127)
            {
                if(!userInput.empty())
                {
                    userInput.pop_back();
                    cout<<"\b \b"<<flush;
                }
            }
            else
            {
                cout<<ch<<flush;
                userInput+=ch;
            }
        }
        char *token;
        vector<string> subtokens;
        int len=userInput.size();
        if(len>0)
        {
            char *comm=(char*)malloc(len);
            strcpy(comm,userInput.c_str());
            token=strtok(comm,";");
            while(token!=NULL)
            {
                subtokens.push_back(token);
                token=strtok(NULL,";");
            }
        }
        int n=subtokens.size();
        for(int x=0;x<n;x++)
        {
            execcommand(subtokens[x]);
            savehist(subtokens[x]);
        }
    }
    printf("\n");
    disableRawMode(orig_termios);
    return 0;
}