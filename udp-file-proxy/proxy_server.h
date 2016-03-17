#ifndef PROXY_SERVER_H
#define PROXY_SERVER_H

#include <arpa/inet.h>
#include <errno.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/thread.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>

#include <sys/types.h>
#include <sys/wait.h>


#include <string>
#include <map>
#include "lock.h"
#include "proxy_manager.h"
#include "proxy_proto.h"

using namespace std;

extern class ProxyServer g_server;

//调用kill的时候要处理sigchld信号
void sig_chld(int signo);

class APP_SERVER
{
public:
    APP_SERVER(){;}      //无参构造函数

    //缺拷贝函数

public:
    //int pid;
    //string name;
    string dir1;
    string dir2;
    bool move;

public:
    string name;          // 应用名
    int pid ;             // 应用进程号
    bool b_active;        // 应用是否运行

public:
    class MsgHelper regsiter_info;

};

typedef map<string,APP_SERVER>::iterator app_t;

/*************************************************/

class ProxyServer:public ListenManager
{
public:
    ProxyServer();

public:
    void Signal_Init();

    //含一个链表结构保存应用信息（使用C++容器）

public:
    virtual int  handler_msg_c(struct bufferevent *bev,void *arg);
    int RequestHandler(char *msg_ex);

private:
    map<string,APP_SERVER> mp_app;
public:
    int add_app(APP_SERVER s);
    int del_app(string app_name);
    int del_app(int pid);
};



#endif // PROXY_CLIENT_H

