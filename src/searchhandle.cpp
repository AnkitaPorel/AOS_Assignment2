#include "shell.h"
#include <iostream>
#include <dirent.h>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

bool searchrec(string &dir_name,string &filename)
{
    DIR *dir=opendir(dir_name.c_str());
    if(dir==nullptr)
        return false;
    struct dirent *entry;
    while((entry=readdir(dir))!=nullptr)
    {
        string entryName=entry->d_name;
        if(entryName=="."||entryName=="..")
            continue;
        string fullpath=dir_name+"/" +filename;
        if(entryName==filename)
        {
            closedir(dir);
            return true;
        }
        struct stat entryinfo;
        if(stat(fullpath.c_str(),&entryinfo)==0 && S_ISDIR(entryinfo.st_mode))
        {
            if(searchrec(fullpath,filename))
            {
                closedir(dir);
                return true;
            }
        }
    }
    closedir(dir);
    return false;
}

void handlesearch(string &input)
{
    string root_dir="/";
    if(searchrec(root_dir,input))
        printf("\nTrue");
    else
        printf("\nFalse");
}

