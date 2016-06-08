#ifndef PROXY_CLIENT_H
#define PROXY_CLIENT_H

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

#include <string>
#include <map>
#include "lock.h"
#include "proxy_proto.h"

#include "proxy_manager.h"

using namespace std;



void print_err_msg(unsigned char *err_code);
extern class ProxyClient g_client;

#define QUERY_SPEED  (g_client.ClientSpeed.query_speed())
#define RETUEN_SPEED(sped)  (g_client.ClientSpeed.return_speed(sped))

class SpeedManger
{
public:
    SpeedManger();
    SpeedManger(int speed);

private:
    MUTEX_TYPE speed_lock;
    int app_count;
    int run_count;

    int max_speed;
    int current_speed;
    int base_speed;

public:
    int query_speed();
    int return_speed(int speed);

    void inc_app();     //增加减少app数量
    void dec_app();

    void inc_run();     //增加减少run线程数量
    void dec_run();
public:
    int set_speed(int speed);

};



typedef struct send_data
{
    string filename;
    bool delect;
    struct send_data *per;
    struct send_data *next;
}SEND_DATA;


typedef struct send_list
{
    SEND_DATA *in;
    SEND_DATA *out;
    int stat;

    MUTEX_TYPE lock;    //添加删除传输文件,先进先出

    int list_count;

    SEND_DATA *data;

}SEND_LIST;


// SendList
class SendList
{
public:
    SendList();

private:
    int stat;
    MUTEX_TYPE lock;
    SEND_DATA *in;
    SEND_DATA *out;

    int list_count;
    SEND_DATA *data;

public:
    // 注意ABBA死锁
    int max_count;
    void set_stat(int st);
    int get_stat();
    int append_file(string filename,bool delect);
    SEND_DATA* get_next();



};





typedef struct app_info
{
    pthread_t tid;
    int stat;               // 线程状态

    string app_name;        //应用名
    string server_ip;       //目标地址
    int server_port;        //目标端口

    bool delect_file;       //传输完成后删除文件
    int max_line;           //最大传输队列

    int u_sock;             //udp传输端口

    //MUTEX_TYPE lock;
    struct send_list *list; //传输队列

    class SendList list_c;

    struct app_info *next;  //下一个应用的指针

    // add by andy from config file 02-26
    int max_speed;
    int pack_t;


}APP_INFO;

typedef struct app_list
{
    struct app_info *list;
    //MUTEX_TYPE lock;    //添加删除应用锁，貌似不需要

}APP_LIST;      // 头指针

#if 0
class ProxyClient
{
public:
    ProxyClient();
public:
    class ListenManager manger;
    //APP_LIST *applist;
    APP_INFO *applist;
    //APP_INFO head;

public:
    SpeedManger ClientSpeed;

    int RequestHandler(uint8_t cmd_type,uint8_t msg_len,uint8_t *msg);
    int RequestHandler(uint8_t *msg);

public:
    int Add_App(APP_INFO *info);
    int Del_App(string appname,int opt);

    int Add_Send_File(string appname,string filename,bool delect);
};
#endif

class ProxyClient:public ListenManager
{
public:
    ProxyClient();
public:
    class ListenManager manger;
    //APP_LIST *applist;
    APP_INFO *applist;
    //APP_INFO head;

public:
    SpeedManger ClientSpeed;
    virtual int handler_msg_c(struct bufferevent *bev,void *arg);

    //int RequestHandler(uint8_t cmd_type,uint8_t msg_len,uint8_t *msg);
    int RequestHandler(char *msg_ex);

public:
    int Add_App(APP_INFO *info);
    int Del_App(string appname,int opt);

    int Add_Send_File(string appname,string filename,bool delect);
};



#endif // PROXY_CLIENT_H

