#include "shell.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <dirent.h>
#include <unistd.h>
#include <algorithm>

using namespace std;

vector<string> getmatch(string ip)
{
    vector<string> match;
    char currDir[4096];
    getcwd(currDir,sizeof(currDir));
    DIR *dir=opendir(currDir);
    if(dir)
    {
        struct dirent *entry;
        while((entry=readdir(dir))!=nullptr)
        {
            string name=entry->d_name;
            if(name.find(ip)==0)
                match.push_back(name);
        }
        closedir(dir);
    }
    sort(match.begin(),match.end());
    return match;
}

void autocomplete(string &ip)
{
    char *mod_ip=new char[ip.size()+1];
    strcpy(mod_ip,ip.c_str());
    char *tok=strtok(mod_ip," ");
    char *lasttok=tok;
    while(tok!=nullptr)
    {
        lasttok=tok;
        tok=strtok(nullptr," ");
    }
    vector<string> match=getmatch(lasttok);
    if(match.size()==1)
    {
        size_t pos=ip.rfind(lasttok);
        if(pos!=string::npos)
        {
            ip.replace(pos,strlen(lasttok),match[0]);
            printf("\n%s",ip.c_str());
        }
    }
    else if(match.size()>1)
    {
        printf("\n");
        for(string mat:match)
            printf("%s ",mat.c_str());
        printf("\n");
    }
    else
        printf("\nNothing found on autocomplete.");
}