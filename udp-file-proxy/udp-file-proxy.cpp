#include <iostream>
#include "Log.h"
#include "config.h"



//#define DEFAULT_CFG		"/home/andy/hot-standby/conf/hot-standby.cfg"

using namespace std;

class CONF g_conf;


static void set_log_level()
{
	string level = g_conf.CONF_get_string("log_level");
	if(level == "DEBUG")
	{
		Log::SetThreshold(Log::LOG_TYPE_DEBUG);
	}
	else if(level == "INFO")
	{
		Log::SetThreshold(Log::LOG_TYPE_INFO);
	}
	else if(level == "WARN")
	{
		Log::SetThreshold(Log::LOG_TYPE_WARN);
	}
	else if(level == "ERROR")
	{
		Log::SetThreshold(Log::LOG_TYPE_ERROR);
	}
	else if(level == "FATAL")
	{
		Log::SetThreshold(Log::LOG_TYPE_FATAL);
	}


	//Log::Debug("Init Log System OK.");
}



#include "proxy_manager.h"
#include "proxy_client.h"
#include "config.h"
#include "proxy_server.h"
#include "mhp_lib/mhp_cmd_helper.h"

//typedef map<string,APP_SERVER>::iterator app_t;
int server_mode = 0;

static int proxy_init(char *cfg)
{
    if(cfg)
    {
        g_conf.CONF_load(cfg);
    }else
        g_conf.CONF_load(DF_CFG);

    string log_file = g_conf.CONF_get_string("log_file");
    if(log_file.empty())
    {
        log_file = DF_LOG_FILE;
    }
    //初始化日志
    if(g_conf.CONF_get_string("log_to")=="STDOUT")
    {
        g_conf.CONF_print();
        Log::Initialise(log_file,LOG_STD_ONLY);
    }else
    {
        Log::Initialise(log_file,LOG_FILE_ONLY);
    }
    //设置日志等级
    set_log_level();

    Log::Debug("初始化配置文件和日志成功");


}



int main(int argc,char **argv)
{
    if(argc != 2)
        proxy_init(NULL);
    else
        proxy_init(argv[1]);

#ifdef DAEMON
	daemon(0,0);
	printf("deamon ok .\n");
#endif

    string m_ip = g_conf.CONF_get_string("manager_ip");
    int m_port = g_conf.CONF_get_number("manager_port");
    if(m_ip.empty() || m_port < 0 || m_port > 65535)
    {
        Log::Error("Missing Manager Address");
        exit(1);
    }

    if(g_conf.CONF_get_string("mode")=="SERVER")
    {
        Log::Debug("Run As Server");
        server_mode = SERVER_MODE;
        //初始化信号处理
        g_server.Signal_Init();

        g_server.setManager(m_ip,m_port);

        if(!g_server.listen())
            return -1;
        g_server.run();
    }
    else if(g_conf.CONF_get_string("mode")=="CLIENT")
    {
        Log::Debug("Run As Client");

        int speed = g_conf.CONF_get_number("max_speed");
        if(speed >0 && speed < 2000)
        {
            Log::Debug("Set Speed %d Mbps\n",speed);
            g_client.ClientSpeed.set_speed(speed);
        }

        g_client.setManager(m_ip,m_port);

        if(!g_client.listen())
        {
            Log::Error("Event listen Error.");
            return -1;
        }
        g_client.run();
    }

    Log::Info("udp-file-proxy Exit.");

    return 0;
}









