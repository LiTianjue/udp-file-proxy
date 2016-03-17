#include "proxy_server.h"
//#include "cudp.h"
#include <iostream>
#include "mhp_lib/mhp_cmd_helper.h"

using namespace std;
class ProxyServer g_server;


void sig_chld(int signo)
{
    pid_t pid;
    int stat;

    pid = wait(&stat);
    //while(pid = waitpid(-1,&stat,WNOHANG) >0);

    //cout << " ### del app pid = " << pid <<endl;
    g_server.del_app(pid);
    return ;
}

/*

static void print_err_msg(unsigned err_code)
{
    switch(err_code)
    {
    case E_OK:
        printf("操作成功.\n");
        break;
    case E_EXIST:
        printf("应用已存在.\n");
        break;
    case E_NOTFOUND:
        printf("应用未找到.\n");
        break;
    case E_FULL:
        printf("传输队列满.\n");
        break;
    case E_BAD_CMD:
        printf("错误的指令.\n");
        break;
    default:
        printf("未知的错误.\n");
        break;
    }
}
*/



ProxyServer::ProxyServer()
{
}

void ProxyServer::Signal_Init()
{
    //注册信号处理函数处理sig_child
    //void sig_chld(int);
   if(SIG_ERR == signal(SIGCHLD,sig_chld))
       Log::Error("Init Signal Fail.");
   else
       Log::Info("Init Signal Success.");


}

int ProxyServer::handler_msg_c(bufferevent *bev, void *arg)
{
    int ret = 0;
    struct evbuffer *input = bufferevent_get_input(bev);
    struct evbuffer *output = bufferevent_get_output(bev);

    int read_len = evbuffer_get_length(input);
    char *msg = NULL;
    msg = (char *)malloc(read_len+1);
    memset(msg,'\0',read_len+1);

    //evbuffer_copyout(input,msg,read_len);
    evbuffer_remove(input,msg,read_len);

    //printf("Read Data[%d]\n",read_len);
    if(msg[1]>0)
    {
        Log::Debug("Recv Msg:[%s]\n",msg+2);
    }
    //evbuffer_add(output,msg,read_len);

    unsigned char cmd_type = msg[0];
    unsigned char msg_len = msg[1];
    Log::Debug("CMD : %02x  Len[%02x]\n",cmd_type,msg_len);

    if(read_len < 2)
    {
        //fprintf(stderr,"Error Message Format.\n");
        Log::Error("Error Message Format.");
        ret = E_FORMAT;

    }else if(read_len > 2 && msg_len !=(read_len -2) )
    {
        Log::Error("Error Message Format.\n");
        ret = E_FORMAT;
    }else{

        ret = RequestHandler(msg);
    }

    if(msg)
        free(msg);

    //echo result to java
    char rt[2] = {0,0};
    rt[0] = ret;
    evbuffer_add(output,rt,2);

    return ret;
}

int ProxyServer::RequestHandler(char *msg_ex)
{
    //cout << "Handler Server Request" << endl;
    uint8_t cmd_type = msg_ex[0];

    uint8_t  msg_len = msg_ex[1];

    char *msg = NULL;

    if(msg_len != 0)
        msg= msg_ex+2;
    else
        msg= NULL;

    if(cmd_type == CMD_REGISTER)
    {
        /*
        app_name 		: 应用名		(app_name=app1)
        server_ip 		: 文件服务地址 (server_ip=1.1.1.1)
        server_port		: 文件服务端口 (server_port=8000)
        message_ip		: 完成通知地址(message_ip=12.12.12.1)
        message_port	: 完成通知端口 (message_port = 9999)
        dir1	    	: 文件接收目录1（dir1=/run/shm/app1）
        dir2			: 文件存储目录 2(dir2=/data/save_path/)
        move			: 是否将目录1的文件移动到目录2 （move=false）
        */
        Log::Debug("注册新应用");

        APP_SERVER new_server;
        new_server.regsiter_info.Msg_parse(msg);
        if(!new_server.regsiter_info.check_server_add_info())
        {
            Log::Debug("添加应用失败");
            return E_BAD_CMD;   //参数错误
        }else
        {
            //检查应用是否已经存在
            string app_name = new_server.regsiter_info.CONF_get_string("app_name");
            if(mp_app.count(app_name) !=0 )
            {
                Log::Debug("应用"+app_name+"已存在");
                //cout << "pid:" << mp_app[app_name].pid << endl;
                return E_EXIST;
            }
            else
            {
                int ret = add_app(new_server);
                if(ret < 0)
                {
                    return E_REGISTER;
                }
                //这里最好再做一次检查
                // kill(ret,0);
                mp_app[app_name] = new_server;
                mp_app[app_name].pid = ret;
                Log::Debug("注册应用："+app_name+" 完成");
            }


        }

    }
    else if(cmd_type == CMD_DELECT)
    {
        Log::Debug("删除应用");
        MsgHelper delect_msg;
        delect_msg.Msg_parse(msg);
        string app_name = delect_msg.Msg_get_string("app_name");
        if(app_name.empty())
            return E_BAD_CMD;
        else
            return del_app(app_name);

    }
    return 0;
}

int ProxyServer::add_app(APP_SERVER s)
{
    const char *path = "/usr/share/udp-file-proxy/bin/Sudp";
    const char *cmd = "Sudp";
    const char *app_name = s.regsiter_info.Msg_get_string("app_name").c_str();

    const char *server_ip = s.regsiter_info.Msg_get_string("server_ip").c_str();
    const char *server_port = s.regsiter_info.Msg_get_string("server_port").c_str();
    const char *message_ip = s.regsiter_info.Msg_get_string("message_ip").c_str();
    const char *message_port = s.regsiter_info.Msg_get_string("message_port").c_str();

    const char *dir1 = s.regsiter_info.Msg_get_string("dir1").c_str();
    const char *dir2 = s.regsiter_info.Msg_get_string("dir2").c_str();

    bool b_move = s.regsiter_info.Msg_get_bool("move");


    Log::Debug("# App INFO:");
    Log::Debug("# app_name:%s",app_name);
    Log::Debug("# server  %s:%s",server_ip,server_port);
    Log::Debug("# message %s:%s",message_ip,message_port);
    Log::Debug("# dir1:%s",dir1);
    Log::Debug("# dir2:%s",dir2);
    Log::Debug("# move_file:%s",b_move?"true":"false");

    pid_t pid = fork();
    switch(pid)
    {
    case -1:
        Log::Error("Fork Error:%s",strerror(errno));
        return -1;
        break;
    case 0:
        if(b_move)
            execl(path,cmd,server_ip,server_port,message_ip,message_port,dir1,dir2,"true",app_name,(char *)0);
        else
            execl(path,cmd,server_ip,server_port,message_ip,message_port,dir1,dir2,"false",app_name,(char *)0);
        Log::Error("Run UDP Server Fail:%s",strerror(errno));
        exit(1);
        break;
    default:
        Log::Info("Run UDP Server Done. pid = %d",pid);
        break;
    }
    return pid;
}

int ProxyServer::del_app(string app_name)
{
   if(mp_app.count(app_name) == 0)
   {
       Log::Debug("应用"+app_name+"不存在");
       return E_NOTFOUND;
   }
   else{
        app_t app;
        app=mp_app.find(app_name);
        int pid = mp_app[app_name].pid;
        mp_app.erase(app);
        //int ret = kill(pid,9);
        int ret = kill(pid,15);
        //cout << "kill " << pid << " return "<<ret <<endl;

        return E_OK;
   }
}

int ProxyServer::del_app(int pid)
{
    //遍历
    app_t it;
    for(it=mp_app.begin();it!=mp_app.end();++it)
    {
        //cout<<"key: "<<it->first<<endl;
        if(it->second.pid == pid)
        {
            mp_app.erase(it);
            return E_OK;
        }
    }
    return  E_NOTFOUND;
}






