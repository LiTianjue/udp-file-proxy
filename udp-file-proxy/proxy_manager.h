#ifndef PROXY_MANAGER
#define PROXY_MANAGER

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

#include <string>
#include <map>

#include "config.h"

using namespace std;
extern int server_mode;



class ListenManager
{
public:
    ListenManager();
    ListenManager(string ip,int port);

protected:
    string m_ip;
    int m_port;

    struct event_base *base;
    struct evconnlistener *listener;
public:
    /*
    virtual void sockEvent(struct bufferevent* bev,short events,void *);
    */
    static void acceptErr(struct evconnlistener *listener,void *);
    static void accept_callbk(struct evconnlistener *listener,evutil_socket_t fd,struct sockaddr *sa,int td, void *arg);
    static void readSock(struct bufferevent* bev,void *);
    static void sockEvent(struct bufferevent* bev,short events,void *);


public:
    // 消息处理函数
    virtual int handler_msg_c(struct bufferevent *bev,void *arg);
    bool setManager(string ip,int port);
    bool listen();
    int run();
};

int handler_msg(struct bufferevent *bev,void *arg);




class MsgHelper:public CONF
{
public:
    MsgHelper(){}
    MsgHelper(const char *buffer)
    {
        Msg_parse(buffer);
    }
    
public:
    bool Msg_parse(const char *buffer)
    {
        return CONF_prase_line(buffer,",");
    }
    bool Msg_parse(const string &data)
    {
        return CONF_prase_line(data,",");
    }
    
    string Msg_get_string(const string &key)
    {
        return CONF_get_string(key);
    }
    int    Msg_get_num(const string &key)
    {
        return CONF_get_number(key);
    }
    bool   Msg_get_bool(const string &key)
    {
        return CONF_get_bool(key);
    }
    
    bool has_value()
    {
        if(m_section.empty())
            return false;
        return true;
    }

public:
    bool check_client_add_info();
    bool check_server_add_info();
private:
    map<string,string> m_section;
    
};




#endif // PROXY_MANAGER

