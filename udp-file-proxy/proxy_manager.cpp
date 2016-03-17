#include "proxy_manager.h"
#include <stdlib.h>
#include "proxy_client.h"
#include "proxy_server.h"
using namespace std;


ListenManager::ListenManager()
{

}

ListenManager::ListenManager(string ip, int port)
{
    m_ip = ip;
    m_port = port;
}

void ListenManager::acceptErr(evconnlistener *listener, void *)
{
    //获取base
    struct event_base *base = evconnlistener_get_base(listener);
    //获取错误字符
    int err = EVUTIL_SOCKET_ERROR();

    fprintf(stderr,"Get Error %d (%s)\n",err,evutil_socket_error_to_string(err));

    //服务是不能退出的
    event_base_loopexit(base,NULL);
}

void ListenManager::accept_callbk(struct evconnlistener *listener,evutil_socket_t fd,struct sockaddr *sa,int td, void *arg)
{
     //重要，获取base
    struct event_base *base = evconnlistener_get_base(listener);

    //获取client address
    //struct sockaddr_in *clientsa = (sockaddr_in*) sa;

    //下面是 bufferevent 的技术
    struct bufferevent *bev = bufferevent_socket_new(base,fd,BEV_OPT_CLOSE_ON_FREE);

    //可以对这个bev设置：read_callbk,write_callbk,event_callbk

    bufferevent_setcb(bev,readSock,NULL,sockEvent,NULL);
    bufferevent_enable(bev,EV_READ | EV_WRITE);
}

void ListenManager::readSock(struct bufferevent *bev, void *)
{
    //evutil_socket_t fd = bufferevent_getfd(bev);
    //struct evbuffer *input = bufferevent_get_input(bev);
    //struct evbuffer *output = bufferevent_get_output(bev);

    //evbuffer_add_printf(output,"appname=%s,ip=1.1.1.1","sslvpn");
    //evbuffer_add_buffer();

    //evbuffer_add_buffer(output,input);  //假设是个echo 服务
    //基于套接字的没有实现我了个fuck
    //bufferevent_flush(bev,EV_READ | EV_WRITE,BEV_FINISHED);
    //bufferevent_free(bev);
    //int read_len = evbuffer_get_length(input);
    //char *msg;
    //msg = (char *)malloc(read_len);
    //evbuffer_copyout(input,msg,read_len);

    //printf("Read Data[%s]\n",msg);
    //evbuffer_add(output,msg,read_len);
    /*
    if(handler_msg(bev,NULL) < 0)
    {
        bufferevent_free(bev);
    }
    */


    if(server_mode == CLIENT_MODE)
    {
        if(g_client.handler_msg_c(bev,NULL) < 0)
        {
            bufferevent_free(bev);
        }
    }
    else if(server_mode == SERVER_MODE)
    {
        if(g_server.handler_msg_c(bev,NULL) < 0)
        {
            bufferevent_free(bev);
        }

    }



    /*
    if(this->handler_msg_c(bev,NULL) < 0)
    {
        bufferevent_free(bev);
    }
    */

}

void ListenManager::sockEvent(struct bufferevent *bev, short events, void *)
{
      //出错处理
    printf("sock Event \n");
    if(events & BEV_EVENT_ERROR)
    {
        perror("Error form bufferevent");
    }
    if(events & (BEV_EVENT_EOF | BEV_EVENT_ERROR))
    {
        //获得socket
        //int sockfd = bufferevent_getfd(bev);

        //printf("Free dev.\n");
        bufferevent_free(bev);
    }
}

int ListenManager::handler_msg_c(bufferevent *bev, void *arg)
{
    // 默认是 client的处理函数
   //return handler_msg(bev,arg);
}

bool ListenManager::setManager(string ip, int port)
{
    m_ip = ip;
    m_port = port;
    base = event_base_new();
    if(!base)
    {
        Log::Error("New Manager base Error.");
        return false;
    }
    Log::Debug("New event base success.");
    return true;

}

bool ListenManager::listen()
{
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(m_port);

    listener = evconnlistener_new_bind(base,this->accept_callbk,NULL,
                                       LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE,
                                       10,
                                       (struct sockaddr *)&addr,sizeof(addr));

    if(!listener)
    {
        //perror("listen:");
        Log::Error("Listen Error:%s",strerror(errno));
        return false;
    }

    evconnlistener_set_error_cb(listener,acceptErr);
    return true;
}

int ListenManager::run()
{
    if(base)
        event_base_dispatch(base);

}

int handler_msg(bufferevent *bev, void *arg)
{
    struct evbuffer *input = bufferevent_get_input(bev);
    struct evbuffer *output = bufferevent_get_output(bev);

    int read_len = evbuffer_get_length(input);
    char *msg;
    msg = (char *)malloc(read_len+1);
    memset(msg,'\0',read_len+1);

    //evbuffer_copyout(input,msg,read_len);
    evbuffer_remove(input,msg,read_len);

    printf("Read Data[%d]\n",read_len);
    if(msg[1]>0)
    {
        printf("msg:[%s]\n",msg+2);
    }
    //evbuffer_add(output,msg,read_len);

    unsigned char cmd_type = msg[0];
    unsigned char msg_len = msg[1];
    printf("CMD : %02x  Len[%02x]\n",cmd_type,msg_len);

    if(read_len < 2)
    {
        fprintf(stderr,"Error Message Format.\n");
        free(msg);
        return -1;
    }else if(read_len > 2 && msg_len !=(read_len -2) )
    {
        fprintf(stderr,"Error Message Format.\n");
        free(msg);
        return -1;
    }
    //msg内存泄露
    /*
    if(server_mode == 0)
        ret = g_client.RequestHandler(msg);
    */

 #if 0
    if(0)
    {
        int i = 0;
        for(i = 0 ;i < read_len;i++)
        {
            printf("%02x ",msg[i]);
        }
        printf("\n");
    }

    unsigned char retval = CMD_E_OK;
    char *info = NULL;
    int info_len = 0;
    switch(cmd_type)
    {
    case CMD_REGISTER:
        {
            printf("call for register.\n");
            
        }
        // retval = server_register(msg+2,msg_len)
        break;
    case CMD_DELECT:
        // retval = server_delect(msg+2,msg_len)
        break;
    case CMD_QUERY:
        // retval = server_query(info,&info_len)
        if(retval == CMD_E_OK)
        {
            if(info_len > 0)
            evbuffer_add(output,info,info_len);
            if(info)
                free(info);
            if(msg)
                free(msg);
            return 0;
        }
        break;
    default:
        retval = CMD_E_CMD;
        //evbuffer_add(output,"Unknow CMD",11);
        break;
    }
    evbuffer_add(output,&retval,1);
    if(msg)
        free(msg);
#endif
}





bool MsgHelper::check_client_add_info()
{

}

bool MsgHelper::check_server_add_info()
{
    if(Msg_get_string("app_name").empty() ||
            Msg_get_string("server_ip").empty() ||
            Msg_get_string("message_ip").empty() ||
            Msg_get_string("dir1").empty() ||
            Msg_get_string("dir2").empty())
    {
        printf("配置信息不全.\n");
        return false;
    }
    int port1 = Msg_get_num("server_port");
    int port2 = Msg_get_num("message_port");
    if(port1 < 0 || port1 > 65535)
    {
        printf("服务端口号错误.\n");
        return false;
    }
    if(port2 < 0 || port2 > 65535)
    {
        printf("通知端口号错误.\n");
        return false;
    }
    return true;
}
