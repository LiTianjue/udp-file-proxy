#include "mhp_cmd_helper.h"

using namespace std;

MHP_cmd_helper::MHP_cmd_helper()
{

}


int MHP_cmd_helper::run_system(string cmd_line)
{
     system(cmd_line.c_str());
}


int MHP_cmd_helper::run_system_demon(string cmd_line)
{
    string cmd =  cmd_line + " &";
    run_system(cmd);
}




int MHP_cmd_helper::run_exe(string cmd_line,string arg)
{
   // return run_exe(cmd_line.c_str(),arg.c_str());
}

int MHP_cmd_helper::run_exe(const char *path,const char *cmd_name, const char *arg)
{
    pid_t pid = fork();
    switch(pid)
    {
    case -1:
        perror("fork failed");
        return -1;
        break;
    case 0:
        //这是在子进程中，启动进程
        printf("exe start cmd\n");
        //execlp("ps", "ps", "au", 0);
        execl(path,cmd_name,arg,(char *)0);
        //execlp(exe_name,NULL);
        // 有返回表示没有起来，会变成僵尸进程
        perror("Exec Error");
        exit(1);
        break;
    default:
        //这是在父进程中，输出相关提示信息
        printf("Parent, ps Done\n");
        break;
    }
    return pid;
}

int MHP_cmd_helper::run_exev(const char *path,const char *arg[])
{
    pid_t pid = fork();

    switch(pid)
    {
    case -1:
        perror("fork failed");
        return -1;
        break;
    case 0:
        printf("exe start cmd\n");

        execv(path,(char **)arg);
        perror("Exec Error");
        exit(1);
        break;
    default:
        printf("Parent, ps Done\n");
        break;
    }

    return pid;

}






