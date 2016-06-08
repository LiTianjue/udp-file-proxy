#include "proxy_client.h"
#include "cudp.h"
#include <iostream>
using namespace std;
class ProxyClient g_client;


void print_err_msg(unsigned err_code)
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

ProxyClient::ProxyClient()
{
    //applist->list = NULL;
    //MUTEX_SETUP(applist->lock);
    applist = NULL;
}

int ProxyClient::handler_msg_c(bufferevent *bev, void *arg)
{
    struct evbuffer *input = bufferevent_get_input(bev);
    struct evbuffer *output = bufferevent_get_output(bev);

    int ret = 0;
    int read_len = evbuffer_get_length(input);
    char *msg = NULL;
    msg = (char *)malloc(read_len+1);
    memset(msg,'\0',read_len+1);

    //evbuffer_copyout(input,msg,read_len);
    evbuffer_remove(input,msg,read_len);

    //printf("Read Data[%d]\n",read_len);
    if(msg[1]>0)
    {
        //printf("msg:[%s]\n",msg+2);
    }
    //evbuffer_add(output,msg,read_len);

    unsigned char cmd_type = msg[0];
    unsigned char msg_len = msg[1];
    Log::Debug("CMD : %02x  Len[%02x],Msg:%s\n",cmd_type,msg_len,msg+2);

    if(read_len < 2)
    {
        Log::Error("Error Message Format.\n");
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

    Log::Debug("CMD Exe return %02x\n",rt[0]);

    return ret;

}


int ProxyClient::RequestHandler(char *msg_ex)
{

    uint8_t  cmd_type = msg_ex[0];
    uint8_t  msg_len = msg_ex[1];


    char *msg = NULL;

    if(msg_len != 0)
        msg= msg_ex+2;
    else
        msg= NULL;

    //printf("Handler Register.\n");
    class MsgHelper helper;

    int ret = 0;
    if(cmd_type == CMD_REGISTER)
    {
        helper.Msg_parse(msg);
        helper.CONF_print();



        APP_INFO *new_app = NULL;
        //new_app = (APP_INFO*)malloc(sizeof(APP_INFO));
        new_app = new APP_INFO;
        if(new_app == NULL)
        {
            Log::Error("Malloc new App fail:%s\n",strerror(errno));
            //注册失败
            return E_REGISTER;
        }

        //string name = helper.Msg_get_string("app_name");

        new_app->app_name = helper.Msg_get_string("app_name");
        new_app->server_ip = helper.Msg_get_string("server_ip");
        new_app->server_port = helper.Msg_get_num("server_port");
        new_app->delect_file = helper.Msg_get_bool("delect_file");
        new_app->max_line = helper.Msg_get_num("max_file");
        new_app->pack_t = g_conf.CONF_get_number("pack_t");
        new_app->list_c.max_count = new_app->max_line;
        ret = Add_App(new_app);
       // Log::Debug("Add app return %d\n",ret);

        //print_err_msg(ret);

        return ret;
    }
    else if(cmd_type == CMD_ADD_FILE)   //添加文件
    {
        helper.Msg_parse(msg);
        //helper.CONF_print();

        string app_name = helper.Msg_get_string("app_name");
        string file_name = helper.Msg_get_string("file_name");
        bool delect_later = helper.Msg_get_bool("delect");

        ret = Add_Send_File(app_name,file_name,delect_later);

        Log::Debug("add File return %d\n",ret);

        //print_err_msg(ret);

        return ret;
    }
    else if(cmd_type == CMD_DELECT) //删除应用
    {
        helper.Msg_parse(msg);
        string app_name = helper.Msg_get_string("app_name");

        ret = Del_App(app_name,0);
        return ret;

    }

        return 0;
}


int ProxyClient::Add_App(APP_INFO *info)
{
    APP_INFO *tmp;
    APP_INFO *per;
    // may need lock
    tmp = applist;

    if(applist == NULL)
    {
        applist = info;
    }
    else
    {
        while(tmp->next != NULL)
        {
            if(tmp->app_name == info->app_name)
                return  E_EXIST;    //应用已经存在
            tmp = tmp->next;
        }
        if(tmp->app_name == info->app_name)
            return  E_EXIST;    //应用已经存在

        tmp->next = info;
    }

    info->next = NULL;

    // 让传输线程先跑起来
    pthread_t tid;
    if(pthread_create(&tid,NULL,file_thread,(void*)info))
    {
        //perror("pthread_create");
        Log::Error("Create App Thread fail:%s\n",strerror(errno));
        //MUTEX_CLEANUP(params->lock);
        tmp->next = NULL;
        free(info);
        return E_THREAD;
    }
    info->tid = tid;

    return E_OK;
}

int ProxyClient::Del_App(string appname, int opt)
{
    APP_INFO *per;
    APP_INFO *tmp;
    per = applist;
    tmp = applist;
    if(applist == NULL)
        return E_NOTFOUND;
   // per = applist->list;
   // tmp = applist->list->next;
    while(tmp != NULL)
    {
        if(tmp->app_name == appname)
        {
            //删除
            // head
            if(per == tmp)
            {
                //查找到的节点是头节点
                if(tmp->next == NULL)   //只剩头节点
                    applist = NULL;
                else
                {
                    applist= tmp->next;
                }
            }
            else
            {
                per->next = tmp->next;
            }

            if(opt == DEL_NOW)
                pthread_cancel(tmp->tid);
            else if(opt == DEL_LAST_ONE || opt == DEL_LATER)
                tmp->stat = S_CANCEL;

            return E_OK;
        }
        per = tmp;
        tmp = tmp->next;
    }

    if(tmp == NULL)
        return E_NOTFOUND;  //应用不存在
}


int ProxyClient::Add_Send_File(string appname, string filename, bool delect)
{
    int ret = 0;
    APP_INFO *tmp;
    //tmp = applist->list->next;
    tmp = applist;
    if(tmp == NULL)
    {
        return E_NOTFOUND;  //应用为空
    }
    while(tmp->next != NULL && tmp->app_name != appname)
    {
        tmp = tmp->next;
    }


    //找到应用名
    if(tmp->app_name == appname)
    {
        /*
        MUTEX_LOCK(tmp->list->lock);
        if(tmp->list->list_count +1 >= MAX_FILE)
        {
            MUTEX_UNLOCK(tmp->list->lock);
            return E_FULL;  //  队列满，不能添加
        }
        */
        ret = tmp->list_c.append_file(filename,delect);
        if(ret != E_OK)
        {
            return E_FULL;
        }
        /*
        SEND_DATA *new_data = NULL;
        new_data = (SEND_DATA)malloc(SEND_DATA);

        new_data->filename = filename;
        new_data->delect = delect;
        new_data->per  = NULL;

        new_data->next = tmp->list->in;
        tmp->list->in->per = new_data;
        tmp->list->in = new_data;       // 插入头部
        tmp->list->list_count += 1;


        MUTEX_UNLOCK(tmp->list->lock);
        */
        return E_OK;
    }else
        return E_NOTFOUND;
}







SpeedManger::SpeedManger()
{
    MUTEX_SETUP(speed_lock);
    max_speed = MAX_SPEED;
    current_speed = 0;
    app_count = 0;
    run_count = 0;

    base_speed = max_speed/10;
}

SpeedManger::SpeedManger(int speed)
{
    MUTEX_SETUP(speed_lock);
    max_speed = speed;

    current_speed = 0;
    app_count = 0;
    run_count = 0;

    base_speed = max_speed/10;
}

int SpeedManger::query_speed()
{
    int ret_speed = 0;
    MUTEX_LOCK(speed_lock);
    int idle_speed = max_speed - current_speed; //  获取剩余可用的流量

    //取走剩余流量的一半
    if(idle_speed > max_speed/2) // 单个应用的最大流量不能超过最大流量的一半
    {
        ret_speed = idle_speed/2 -base_speed; //留有余量
    }else
    {
        ret_speed = idle_speed/2;
    }

    current_speed += ret_speed;

    MUTEX_UNLOCK(speed_lock);

    return ret_speed;
}


int SpeedManger::return_speed(int speed)
{
    MUTEX_LOCK(speed_lock);
    current_speed -= speed; //  归还流量
    if(current_speed < 0)
        current_speed = 0;
    MUTEX_UNLOCK(speed_lock);
}

void SpeedManger::inc_app()
{
       MUTEX_LOCK(speed_lock);
       app_count +=1;
       MUTEX_UNLOCK(speed_lock);
}

void SpeedManger::dec_app()
{
       MUTEX_LOCK(speed_lock);
       app_count -=1;
       MUTEX_UNLOCK(speed_lock);
}

void SpeedManger::inc_run()
{
       MUTEX_LOCK(speed_lock);
       run_count += 1;
       MUTEX_UNLOCK(speed_lock);

}

void SpeedManger::dec_run()
{
       MUTEX_LOCK(speed_lock);
       run_count -= 1;
       MUTEX_UNLOCK(speed_lock);
}

int SpeedManger::set_speed(int speed)
{
   max_speed = speed;
}







// SendList
SendList::SendList()
{
    stat = S_IDLE;
    MUTEX_SETUP(lock);
    in = NULL;
    out = NULL;
    max_count = MAX_FILE;
    list_count = 0;
    //data = NULL;
}

void SendList::set_stat(int st)
{
   MUTEX_LOCK(lock);
   stat = st;
   MUTEX_UNLOCK(lock);
}

int SendList::get_stat()
{
   MUTEX_LOCK(lock);
   int st = stat;
   MUTEX_UNLOCK(lock);

   return st;
}

int SendList::append_file(string filename, bool delect)
{
    MUTEX_LOCK(lock);
    if(list_count >= max_count)
    {
        MUTEX_UNLOCK(lock);
        return E_FULL;
    }
    SEND_DATA *new_data = NULL;
    //new_data = (SEND_DATA*)malloc(sizeof(SEND_DATA));
    new_data  = new SEND_DATA;
    new_data->delect = delect;
    new_data->filename = filename;
    new_data->per = NULL;

    if(in==NULL && out==NULL)   //空队列
    {
        new_data->next = NULL;
        out = new_data;
        in = new_data;
        list_count +=1;
    }else                       //非空队列
    {
        new_data->next = in;
        in->per = new_data;
        in = new_data;
        list_count +=1;
    }
    if(stat == S_IDLE)
    {
        stat = S_BUSY;
    }

    //printf("list cout = %d\n",list_count);

    MUTEX_UNLOCK(lock);

    return E_OK;
}

SEND_DATA* SendList::get_next()
{
    MUTEX_LOCK(lock);
    if(list_count == 0)
    {
        stat = S_IDLE;
        MUTEX_UNLOCK(lock);
        return NULL;
    }

    SEND_DATA *ret_data = NULL;
    if(out != NULL)
    {
        ret_data = out;
        out = out->per;
        if(out == NULL) //取出来之后队列就空了
        {
            in = NULL;
            stat = S_IDLE;
        }
        list_count -= 1;

    }

    MUTEX_UNLOCK(lock);

    return ret_data;
}




