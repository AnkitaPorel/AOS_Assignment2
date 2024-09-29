#include "shell.h"
#include <cstring>

using namespace std;

void handleecho(string &arg)
{
    printf("\n%s",arg.c_str());
}