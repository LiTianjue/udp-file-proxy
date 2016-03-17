/***********************
 *  Title  : UDP server
 *  Auther : andy
 *  Create : 2014-08-01
 *************************/

/*-----Change_Log-----------*/
/*		Date			Auther			Desc
 *		2014-10-13		andy		一个稳定的基础版本，去掉不需要的代码和注释
 *		2014-10-13		andy		修改为多线程的版本
 *---------------------------*/

#include <stdio.h>  
#include <stdlib.h>
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

/*--epoll--*/
#include <sys/epoll.h>

/*消息队列相关*/
#include <sys/msg.h>
#include <sys/ipc.h>

#include "Protocol.h"
#include "MY_UDP.h"
#include "Common.h"

#include "my_all.h"

/**--add log for Sudp--**/
#include "defs.h"
#include "liblog.h"

#include "inc/thread_process.h"

//#include "libdaemon.h"


#include "thread_process.h"



//char *LOG_PATH = NULL;
//char my_log[256] = {0};
//char app_name[32] = {0};
int t_sockfd;


static long long save;  //保存包序号

pid_t pid;				//用于创建和销毁临时文件
pthread_t tid;		    //写文件线程
int mqid_save;          //用于销毁消息队列
int mqid;               //消息队列
int test = 1 ;


static char *tcp_ip = NULL;
static char *udp_ip = NULL;
static int tcp_port;
static int udp_port;
static char *log_file = NULL; // 日志文件
static char *app_name = NULL;
// add by andy for udp message
static char *message_ip = NULL;
static int message_port = -1;

static char *dir1 = NULL;  //文件接收目录
static char *dir2 = NULL;  //文件存储目录

static int move_file = 0;   //是否将文件从接收目录移动到存储目录

int t_sockfd;	//tcp socket    //这个要改成udp的
int m_sockfd;   // message socket

#define DF_LOG_PATH "/usr/share/udp-file-proxy/log"
/********************************************/

static void Init_log(void);
static void my_exit1(void);
static void check_param(int argc,char **argv);
//Signal
static void signal_func(int sig);
static void signal_init();

//线程池
#include "threadpool.h"

int main(int argc,char **argv) 
{

    /**********************************/
	check_param(argc,argv);

    //创建线程池用于处理 文件传输完成之后的操作
    struct threadpool *pool;
    pool = threadpool_init(6,20);
    if(pool == NULL)
    {
        PRINTF(LEVEL_ERROR,"Create Thread pool Fail.");
        exit(-1);
    }


	if(atexit(my_exit1) != 0)
	{ 
		PRINTF(LEVEL_ERROR,"Atexit error : %s ",strerror(errno));
		exit(1);
	}


	signal_init();
	//要做信号的初始化


	int sockfd;
	struct sockaddr_in servaddr,client;
	int sin_size = sizeof(struct sockaddr_in);

//	Init_sockaddr_s(&servaddr,AF_INET,SERVER_IP,SERVER_PORT);
	Init_sockaddr_s(&servaddr,AF_INET,udp_ip,udp_port);	
	sockfd = Socket(AF_INET,SOCK_DGRAM,0);

	int opt = SO_REUSEADDR;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	/**--设置扩大系统接收缓冲区的大小提高UDP接收的性能--**/
	//int n = 512*1024;
	/*
	int n = 2048*1024;
	if(0 != setsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,&n,sizeof(n)))
		PRINTF(LEVEL_ERROR,"Setsockopt SO_RCVBUF error:",strerror(errno));
	*/
	int recLen = -1;
	int Optlen = 4;
	if(0 != getsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,&recLen,&Optlen))
		PRINTF(LEVEL_ERROR,"Getsockopt SO_RCVBUF error:",strerror(errno));
	PRINTF(LEVEL_INFORM,"getRecv len = %d\n",recLen);
	//printf("getRecv len = %d\n",recLen);

	if(bind(sockfd,(struct sockaddr *)&servaddr,sizeof(struct sockaddr)) == -1)  
	{
		PRINTF(LEVEL_ERROR,"Bind error:%s",strerror(errno));
		exit(-1);
	}

	int num;
	char msg[MAXDATASIZE] = {0};

	/************************************************/
	/*----消息队列和线程相关----*
	 */
	
	//pthread_t tid;
	struct msgbuf *msgbuf = NULL;

	int  oflag;
	int mqid;
	oflag = IPC_CREAT;
	oflag |= IPC_EXCL;
	key_t my_key;

	//这里的tmpfile通过getpid保证唯一性
	char tmp_file[64] = {0};
	FILE *fp;
	pid = getpid();
	sprintf(tmp_file,"/tmp/Sudp_%d",pid);
	fp =fopen(tmp_file,"w+");
	if(fp == NULL)
	{
		PRINTF(LEVEL_ERROR,"Open tmp file fail :%s",strerror(errno));
		exit(-1);
	}
	fclose(fp);
	PRINTF(LEVEL_INFORM,"create tmp file is %s\n",tmp_file);
	/*--------------------------------------------*/

	my_key = ftok(tmp_file,0);
	if(my_key == -1)
	{
		PRINTF(LEVEL_ERROR,"ftok error :%s\n",strerror(errno));
		exit(-1);
	}

	mqid = msgget(my_key, oflag);
	if(mqid == -1)
	{
		PRINTF(LEVEL_ERROR,"ftok error :%s\n",strerror(errno));
		exit(-1);
	}
	PRINTF(LEVEL_INFORM,"main mqid = %d : %s \n",mqid,strerror(errno));
	mqid_save = mqid;

/*--------------------------------------
 * 改为 UDP 套接字用于发送文件名
 *------------------------------------*/

   THREAD_INFO *t_info;
   t_info = (THREAD_INFO *)malloc(sizeof(THREAD_INFO));
   Init_sockaddr_s(&(t_info->m_addr),AF_INET,message_ip,message_port);
   //这里不要创建套接字 ,工作线程里面去创建
   //t_info->m_sock = Socket(AF_INET,SOCK_DGRAM,0);
   t_info->pool = pool;
    /*
   if(t_info->m_sock < 0)
   {
       PRINTF(LEVEL_ERROR,"Create Msg Socket:%s\n",strerror(errno));
       exit(-1);
   }
   */
   t_info->mqid = mqid;
   t_info->move_file = move_file;
   t_info->dir1 = dir1;
   t_info->dir2 = dir2;
   t_info->app_name = app_name;
    /*-------------------------------*/


    //写文件线程
    if(1)   //可以测试只写消息队列
	{
		int ret = 0;
		fflush(NULL);
        //ret = pthread_create(&tid,NULL,&File_Process,(void *)mqid);
        ret = pthread_create(&tid,NULL,&File_Process,(void *)t_info);
        if(ret < 0)
		{
			PRINTF(LEVEL_ERROR,"pthread_create error :%s\n",strerror(errno));
			exit(-1);
		}
	}


/*--------------------------------------
 * 创建TCP 套接字用于发送文件名
 *------------------------------------*/
 #if 0
	struct sockaddr_in t_servaddr;
	t_sockfd = Socket(AF_INET,SOCK_STREAM,0);
	bzero(&t_servaddr,sizeof(t_servaddr));
	t_servaddr.sin_family = AF_INET;
	t_servaddr.sin_port = htons(tcp_port);
	Inet_pton(AF_INET,tcp_ip,&t_servaddr.sin_addr);

//	Connect(t_sockfd,(struct sockaddr *)&t_servaddr,sizeof(t_servaddr));
	//这里是不是需要做长连接的设置


	/*-------------------------------*/
#endif

	//这里会内存泄漏
	msgbuf = calloc(2048,sizeof(char));
	if(msgbuf == NULL)
	{
		PRINTF(LEVEL_ERROR,"calloc error :%s",strerror(errno));
		exit(-1);
	}

    save = 0;   //记录前一个包的包序号
	long long pack_nu = 0;

	int only = 1;
	while(1)
	{

		num = recvfrom(sockfd,msg,MAXDATASIZE,0,(struct sockaddr *)&client,&sin_size);
		if(num < 0)
		{
			PRINTF(LEVEL_ERROR,"Recvfrom error :%s",strerror(errno));
			//WriteSysLog(LOG_PATH,my_log);
			exit(-1);
		}
		if(only == 1)
		{
            //Connect(t_sockfd,(struct sockaddr *)&t_servaddr,sizeof(t_servaddr));
			only = 0;
		}

		memcpy(&pack_nu,msg+2,8);

		if(pack_nu == 1)
			save = 0;
		if((pack_nu != (save + 1)))
		{
            //这里需要通过通知端口发送丢包文件信息？
			PRINTF(LEVEL_ERROR,"lost package,Recv pack error :save=%lld,pack_nu=%lld\n",save,pack_nu);
		}
		save = pack_nu;

		memcpy(msgbuf->mtext,msg,num);
		msgbuf->mtype = (long)pack_nu;


		//if((msgsnd(mqid,msgbuf,num,0) == -1))
		if((msgsnd(mqid,msgbuf,num,IPC_NOWAIT) == -1))
		{
			PRINTF(LEVEL_ERROR,"Send msg error :%s\n",strerror(errno));
			if(msgsnd(mqid,msgbuf,num,0) < 0)
			{
				PRINTF(LEVEL_ERROR,"Send msg error:%s\n",strerror(errno));
			}
			//free(msgbuf);
			//exit(-1);
		}
	}
}



static void Init_log(void)
{
    if(log_file != NULL)
    {
        if(open_logfile(log_file) == R_ERROR)
        {
            PRINTF(LEVEL_ERROR,"Open logfile error.");
            free(log_file);
            //exit(-1);
        }
    }
    else {
        char m_path[256] = {0};
        sprintf(m_path,"%s/%s",DF_LOG_PATH,app_name);
        char c_dir[256] = {0};
        sprintf(c_dir,"%s/%s/%s",DF_LOG_PATH,app_name,"haha");
        //if(open_logfile_t(DF_LOG_PATH) == R_ERROR)
        CreateDir(c_dir);
        if(open_logfile_t(m_path) == R_ERROR)
        {
            PRINTF(LEVEL_ERROR,"Open logfile error.");
            free(log_file);
            //exit(-1);
        }
    }


}

static void my_exit1(void)
{
	int _err;
	//回收内存
	if(tcp_ip != NULL)
		free(tcp_ip);
	if(udp_ip != NULL)
		free(udp_ip);
	if(log_file != NULL)
		free(log_file);
    if(message_ip != NULL)
        free(message_ip);
    if(dir1 != NULL)
        free(dir1);
    if(dir2 != NULL)
        free(dir2);
    if(app_name != NULL)
        free(app_name);

	//处理消息队列
	char tmp_file[64] ={0};
	sprintf(tmp_file,"/tmp/Sudp_%d",getpid());
    printf("rm tmp_file is %s\n",tmp_file);
	
	if(msgctl(mqid_save,IPC_RMID,NULL) != 0)
		PRINTF(LEVEL_ERROR,"Msgctl Remove: %s\n",strerror(errno));
	if(remove(tmp_file)!=0)
		PRINTF(LEVEL_ERROR,"Remove tmpfile %s fail: %s\n",tmp_file,strerror(errno));
    //清理线程池？
}

static void check_param(int argc,char **argv)
{
    /*
     * argv[1] server_ip
     * argv[2] server_port
     * argv[3] message_ip
     * argv[4] message_port
     * argv[5] dir1
     * argv[6] dir2
     * argv[7] move_file
     * argv[8] app_name
     * argv[8]
     * argv[9] debug_level
     *
     */

    if(argc < 8)
    {
        fprintf(stderr,"Bad Param !!!\n");
        exit(-1);
    }
    /*
    if(argc > 8)
    {
        log_file = strdup(argv[8]);
        Init_log();
    }
    if(argc > 9)
    {
        // debug mode
        PRINTF(LEVEL_INFORM,"Run as Debug mode !!!\n");
        int level = atoi(argv[9]);
        liblog_level(level);
    }
    */

    //sscanf(argv[1],"%[^:]:%d",udp_ip,&udp_port);
    //sscanf(argv[2],"%[^:]:%d",tcp_ip,&tcp_port);

    udp_ip = strdup(argv[1]);
    udp_port = atoi(argv[2]);
    message_ip = strdup(argv[3]);
    message_port = atoi(argv[4]);
    dir1 = strdup(argv[5]);
    dir2 = strdup(argv[6]);
    app_name = strdup(argv[8]);

    if(!strcmp(argv[7],"true"))
    {
        move_file = 1;
    }

    Init_log();
    liblog_level(1);

    if(1)
        liblog_level(1);

    PRINTF(LEVEL_INFORM,"move file string %s\n",argv[7]);
    PRINTF(LEVEL_INFORM,"udp [%s:%d]\n",udp_ip,udp_port);
    PRINTF(LEVEL_INFORM,"msg [%s:%d]\n",message_ip,message_port);
    PRINTF(LEVEL_INFORM,"dir1:%s\n",dir1);
    PRINTF(LEVEL_INFORM,"dir2:%s\n",dir2);
    PRINTF(LEVEL_INFORM,"app_name:%s\n",app_name);
    PRINTF(LEVEL_INFORM,"move_file:%d\n",move_file);
}


static void signal_init()
{
	int sig ;
	sig = SIGINT;
	if(SIG_ERR == signal(sig,signal_func))
	{
		PRINTF(LEVEL_ERROR,"%s signal[%d] failed.\n",__func__,sig);
	}
	sig = SIGTERM;
	if(SIG_ERR == signal(sig,signal_func))
	{
		PRINTF(LEVEL_ERROR,"%s signal[%d] failed.\n",__func__,sig);
	}

	sig = SIGPIPE;
	if(SIG_ERR == signal(sig,signal_func))
	{
		PRINTF(LEVEL_ERROR,"%s signal[%d] failed.\n",__func__,sig);
	}

	sig = SIGHUP;
	if(SIG_ERR == signal(sig,signal_func))
	{
		PRINTF(LEVEL_ERROR,"%s signal[%d] failed.\n",__func__,sig);
	}
}

static void signal_func(int sig)
{
	switch(sig)
	{
		case SIGINT:
		case SIGTERM:
			PRINTF(LEVEL_INFORM,"signal [%d],exit.\n",sig);
			exit(0);
			break;
		case SIGPIPE:
			PRINTF(LEVEL_INFORM,"signal [%d],SIGPIPE.\n",sig);
			break;
		case SIGHUP:
			PRINTF(LEVEL_INFORM,"signal [%d],SIGHUP.\n",sig);
			break;
		default:
			PRINTF(LEVEL_INFORM,"signal [%d] not support.\n",sig);
			break;
	}
}
