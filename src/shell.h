#ifndef SHELL_H
#define SHELL_H

#include <string>
#include <vector>
#include <sys/types.h>

void setHome();
std::string getCurrentDir();
void execcommand(std::string &command);
void handlecd(std::string &arg);
void handlepwd();
void handleecho(std::string &arg);
void handlels(std::vector<std::string> &args);
void pinfo(int pid);
void handlesearch(std::string &input);
void autocomplete(std::string &ip);
void savehist(std::string &userInput);
void sigintHandler(int sig_num);
void sigtstpHandler(int sig_num);
void execpipeline(std::vector<std::vector<std::string>> &commands);
void handlehistory(int num);
std::vector<std::string> loadhist();

extern std::string homeDirectory;
extern std::string prevDirectory;
extern pid_t fgprocess;
extern pid_t bgprocess;
extern std::string USER_LOG;

#endif