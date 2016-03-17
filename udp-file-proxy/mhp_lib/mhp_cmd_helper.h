#ifndef MHP_CMD_HELPER_H
#define MHP_CMD_HELPER_H

#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

using namespace std;


class MHP_cmd_helper
{
public:
    MHP_cmd_helper();
public:
    static int run_system(string cmd_line);
    static int run_system_demon(string cmd_line);

    static int run_exe(string cmd_line,string arg);
    static int run_exe(const char *path, const char *cmd_name, const char *arg);
    static int run_exev(const char *path, const char *arg[]);
};

#endif // MHP_CMD_HELPER_H

