/**线程处理函数的实现**/
/* Auther : Andy 
 * Date   : 2014-08-09
 ***********************/

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

#include <sys/msg.h>
#include <sys/ipc.h>  

#include "Protocol.h"

#include "MY_UDP.h"
#include "Common.h"
//#include "log.h"

#include "my_all.h"


/*add log for thread*/
#include "liblog.h"
#include "defs.h"
#include "inc/thread_process.h"
#include "file_helper.h"
#include <libgen.h>

static ECHO_INFO share_info;

void* Recv_Finish(void *args)
{
    //printf("\n#########################\n");
    char filename[256] = {0};
    sprintf(filename,"%s",(char *)args);
    //printf("Save File to :[%s]\n",filename);

    char src[256] = {0};
    char des[256] = {0};
    struct sockaddr_in mm_addr;

    MUTEX_LOCK(share_info.lock);

    //strcpy(src,share_info.dir1);
    //strcpy(des,share_info.dir2);

    mm_addr = share_info.m_addr;

    if(share_info.move_file > 0)  //需要拷贝文件
    {
        //printf("需要拷贝移动文件.\n");
        //printf("src:%s\n",src);
        //printf("save:%s\n",(char *)args);

        //strcpy(des,share_info.dir2);

        sprintf(des,"%s/%s/%s",share_info.dir2,share_info.app_name,basename(filename));
        CreateDir(des);
        MUTEX_UNLOCK(share_info.lock);
        int ret = 0;
        ret = file_helper_move(filename,des);
        MUTEX_LOCK(share_info.lock);
        if(ret != 0)
        {
            // log ??
        }
    }else
    {
        strcpy(des,filename);
        //printf("不需要拷贝移动文件.\n");
    }

    printf("Save File:%s\n",des);
    //udp_message;
    int sock = share_info.m_sock;
    char echo_msg[256] = {0};
    echo_msg[0] = 0x00;
    echo_msg[1] = strlen(des);
    memcpy(echo_msg+2,des,strlen(des));

    //udp_message(des,strlen(des),sock,(struct sockaddr*)(&mm_addr));
    udp_message(echo_msg,strlen(des)+2,sock,(struct sockaddr*)(&mm_addr));

    MUTEX_UNLOCK(share_info.lock);


    //printf("#########################\n");

}

void *File_Process(void *arg)
{
	pthread_detach(pthread_self());

    // add by andy for more info
    THREAD_INFO *tinfo = (THREAD_INFO *)arg;

    //mqid = (int)arg;
    mqid = tinfo->mqid;
    struct threadpool *pool = tinfo->pool;

	PRINTF(LEVEL_INFORM,"thread mqid = %d \n",mqid);
	//fprintf(stderr,"--->test = %d \n",test);

    //init thread share info
    MUTEX_SETUP(share_info.lock);
    share_info.dir1 = strdup(tinfo->dir1);
    share_info.dir2 = strdup(tinfo->dir2);
    share_info.app_name = strdup(tinfo->app_name);
    share_info.move_file = tinfo->move_file;
    share_info.m_addr = tinfo->m_addr;

    share_info.m_sock = Socket(AF_INET,SOCK_DGRAM,0);
	int fd;
	FILE *fp;
	struct msgbuf *buff = malloc(MAXMSG);
	int n , ret;

	mode_t fdmode = (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	int wlen;

	long long pack_total;
	long long pack_saved; 
	long long pack_move; 


	char *msg = NULL;

	//char save_path[512] ={0};
	//add by andy ,fix bug tcp_echo err ,2014-12-12
	char save_path[1024] ={0};

	/**UDP 数据包结构**/
	char protocol;

	long long file_size;
	int file_name_length;
	char *file_name;
	int pack_length;
	char *pack;

again:	
	pack_saved = 0;
	pack_move = 1;
	pack_total = 10;

	//这句可能是多余的	
	memset(buff,0,MAXMSG);	


	while(1)
	{	
		if((n = msgrcv(mqid,buff,MAXMSG,pack_move,IPC_NOWAIT)) < 0)
		//if((n = msgrcv(mqid,buff,MAXMSG,pack_move,0)) < 0)
		{
			n = msgrcv(mqid,buff,MAXMSG,0,0);
			if(n < 0)
			{
				PRINTF(LEVEL_ERROR,"thread msgrcv error :%s\n",strerror(errno));
				//WriteSysLog(LOG_PATH,log);
				goto err_end;
				continue;
			}
			pack_move = buff->mtype;
		}

		pack_move ++;

		msg = buff->mtext;

		if((pack_saved == 0)||(pack_move == 2))
		{
			if((pack_move == 2)&&(pack_saved !=0))
			{
				PRINTF(LEVEL_ERROR,"A file is incomplete because client stop transmite by accident\n");
				// write log

				pack_saved = 0;
				if(fp != NULL)
				{
					fclose(fp);
					fp = NULL;
				}
			}

			memcpy(&file_size,msg+10,8);
			pack_total = file_size/1024;
			if(file_size%1024 != 0)
				pack_total+=1;

			memcpy(&file_name_length,msg+18,4);
			file_name = (char *)malloc(file_name_length+1);
			memcpy(file_name,msg+22,file_name_length);
			*(file_name+file_name_length) = '\0';
            // usr dir install of true path ,andy,2015-03-07
            //sprintf(save_path,RUN_PATH);
            //strcat(save_path,file_name);
            sprintf(save_path,"%s/%s/",tinfo->dir1,tinfo->app_name);
            strcat(save_path,basename(file_name));

            //memcpy(&pack_length,msg+22+file_name_length,4);

			pack = msg +22 +file_name_length +4;

			ret = CreateDir(save_path);
			if(ret < 0)
			{
				;
			}
			fp = fopen(save_path,"w+");
			if(fp == NULL)
			{
				PRINTF(LEVEL_ERROR,"Open %s error:%s",file_name,strerror(errno)); 
 				goto err_end;
			}
		}


        memcpy(&pack_length,msg+22+file_name_length,4);

        int n = fwrite(pack,sizeof(char),pack_length,fp);
		if(n != pack_length)
		{
			PRINTF(LEVEL_ERROR,"Write %s error :%s\n",file_name,strerror(errno));
			goto err_end;
		}

		pack_saved++;				/**写文件成功，记录**/

		//检查文件大小
		if(pack_total == pack_saved )
		{
            //PRINTF(LEVEL_ERROR,"Last Pack Size :%d\n",pack_length);
			fclose(fp);
			fp = NULL;

            threadpool_add_job(pool,Recv_Finish,save_path);
            //tcp_echo(save_path,strlen(save_path),t_sockfd);
            free(file_name);

			goto again;
		}
		else if(pack_total == (pack_move -1))
		{
			PRINTF(LEVEL_ERROR,"## lost package ## File %s is incomplete\n",file_name);

			fclose(fp);

			fp = NULL;
			//remove(save_path);
            //tcp_echo(save_path,strlen(save_path),t_sockfd);
			free(file_name);
			goto again;
		}


	}

err_end:	
	free(buff);
	free(file_name);
	exit(-1);
	return NULL;
}
