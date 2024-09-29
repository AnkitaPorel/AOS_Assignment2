#include "shell.h"
#include <cstring>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <iostream>
#include <sys/stat.h>
#include <time.h>

using namespace std;

void lsdirectory(const string &dir,bool showall,bool perm)
{
    DIR* dr=opendir(dir.c_str());
    if(dr==nullptr)
    {
        perror("\nError in opening directory: ");
        return;
    }
    struct dirent* entry;
    while((entry=readdir(dr))!=nullptr)
    {
        string name=entry->d_name;
        if (!showall && name[0]=='.') 
            continue;
        string fullPath=dir+"/"+name;
        struct stat fileStat;
        if(stat(fullPath.c_str(),&fileStat)==-1)
        {
            perror("\nError: ");
            continue;
        }
        if(perm)
        {
            cout<<((S_ISDIR(fileStat.st_mode))?"d":"-");
            cout<<((fileStat.st_mode & S_IRUSR)?"r":"-");
            cout<<((fileStat.st_mode & S_IWUSR)?"w":"-");
            cout<<((fileStat.st_mode & S_IXUSR)?"x":"-");
            cout<<((fileStat.st_mode & S_IRGRP)?"r":"-");
            cout<<((fileStat.st_mode & S_IWGRP)?"w":"-");
            cout<<((fileStat.st_mode & S_IXGRP)?"x":"-");
            cout<<((fileStat.st_mode & S_IROTH)?"r":"-");
            cout<<((fileStat.st_mode & S_IWOTH)?"w":"-");
            cout<<((fileStat.st_mode & S_IXOTH)?"x":"-");
            cout<<" "<<fileStat.st_nlink;
            struct passwd* pw=getpwuid(fileStat.st_uid);
            cout<<" "<<(pw?pw->pw_name:to_string(fileStat.st_uid));
            struct group *grp=getgrgid(fileStat.st_gid);
            cout<<" "<<(grp?grp->gr_name:to_string(fileStat.st_gid));
            cout <<" "<<fileStat.st_size;
            char timeBuf[80];
            struct tm *timeinfo=localtime(&fileStat.st_mtime);
            strftime(timeBuf, sizeof(timeBuf), "%b %d %H:%M",timeinfo);
            cout<<" "<<timeBuf;
        }
        cout<<" "<<name<<endl;
    }
    closedir(dr);
}

void handlels(vector<string> &args)
{
    bool showall=false,perm=false;
    vector<string> directory;
    for(const string &arg:args)
    {
        if(arg[0]=='-')
        {
            for(char flag:arg.substr(1))
            {
                if(flag=='a')
                    showall=true;
                else if(flag=='l')
                    perm=true;
            }
        }
        else
        {
            if(arg[0]=='~')
                directory.push_back(homeDirectory+arg.substr(1));
            else
                directory.push_back(arg);
        }
    }
    if(directory.empty())
        directory.push_back(".");
    for(const string &dir:directory)
    {
        printf("\n%s : \n",dir.c_str());
        lsdirectory(dir,showall,perm);
        printf("\n");
    }
}