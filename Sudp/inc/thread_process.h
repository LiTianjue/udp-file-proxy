#ifndef __THREAD_PROCESS_H__
#define __THREAD_PROCESS_H__

#include "../threadpool.h"
#include "../lock.h"

extern select_timer(int sec,int usec);

extern void *File_Process(void *arg);

typedef struct _thread_info
{
    int mqid;                   //消息队列描述符
    int m_sock;                 //发送消息用的套接子
    struct sockaddr_in m_addr;  //消息接收的地址

    int move_file;              //文件是否需要被拷贝

    struct threadpool *pool;
    char msg_ip[32];
    int msg_port;

    char *app_name;
    char *dir1;
    char *dir2;
}THREAD_INFO;

typedef struct echo_info
{
    MUTEX_TYPE lock;
    char *app_name;
    char *dir1;
    char *dir2;
    struct sockaddr_in m_addr;
    int move_file;
    int m_sock;
}ECHO_INFO;

#endif
