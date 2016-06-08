/***********************************
 * Title : JNI code for SendFile Client 
 * Author: LiTianjue
 * Date  : 2014-11-05
 ***********************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/time.h>

#include <iostream>

#include "proxy_proto.h"
#include "proxy_client.h"

//#define MAXLINE 1024


//init socket
static int Inet_pton(int family, const char *strptr, void *addrptr);
static int Init_sockaddr_s(struct sockaddr_in *addr_struct,sa_family_t family,const char * IP,int port);


static int Sendto(int fd, const void *ptr, size_t nbytes, int flags,const struct sockaddr *sa, socklen_t salen);
static int SendPack(const char *file_path,int sockfd,const struct sockaddr *pservadddr,socklen_t servlen,int pack_t,int wait_t);
static long GetFileSize(const char *path);

#if 0
/*--main method--*/
int SendFile(const char *udp_ip,int udp_port,int pack_t,int wait_t ,const char *file_name)
{
	//
	int sock;
	struct sockaddr_in servaddr;
	Init_sockaddr_s(&servaddr,AF_INET,udp_ip,udp_port);
	sock = socket(AF_INET,SOCK_DGRAM,0);
	if(sock <0){
		return E_SOCKET;
	}
	//fprintf(stderr,"file name %s\n",file_name);

	int ret = SendPack(file_name,sock,(struct sockaddr *)&servaddr,sizeof(servaddr),pack_t,wait_t);

	if(ret == S_OK)
		return S_OK;
	
	return ret;
}
/*----------------------------------------------------*/


static int SendPack(const char *file_path,int sockfd,const struct sockaddr *pservaddr,socklen_t servlen,int pack_t,int wait_t)
{
	int fd,ret;
	int rlen;

	char *msg = NULL;
	int msg_len;

	char recv_buff[MAXLINE] ={0};

	/*----UDP 包结构----**/
	char version = '\017';
	char protocol = '\014';
	long long pack_nu = 0;
	long long file_size;
	int file_name_length = strlen(file_path);
	char *file_name = NULL;
	//int pack_length;
	//char *pack;

	file_size = GetFileSize(file_path);
	//fprintf(stderr,"### filesize = %lld\n",file_size);
	if(file_size == E_OPEN)
	{
		goto _err1; /*just return E_OPEN*/
	}

	msg_len = 1 + 1 + 8 + 8 + 4 +file_name_length + 4 + MAXLINE;
	
	//fprintf(stderr,"### msg_len = %d\n",msg_len);
	msg = malloc(msg_len);
	file_name = strdup(file_path);

	//初始化部分发送包的数据结构
	memcpy(msg,&version,1);

	memcpy(msg +10,&file_size,8);
	memcpy(msg + 18,&file_name_length,4);
	memcpy(msg + 22,file_name,file_name_length);

	if(!(fd = open(file_name,O_RDONLY)))
	{
		//perror("open file ");
		ret = E_OPEN;
		goto _err2;	/*close fd free msg & file_name and return E_OPEN */
	}

	int len_a = 22 + file_name_length + 4;
	int send_len;
	int send_count = 0;

	while((rlen = read(fd,recv_buff,MAXLINE)) != -1)
	{
		if(rlen == 0)
			break;

		if((send_count == 0)&&((send_count+rlen) == file_size))
			protocol = '\013';
		else if(send_count == 0)
			protocol = '\014';
		else if(send_count+rlen == file_size)
			protocol = '\016';
		else if(send_count + rlen < file_size)
			protocol = '\015';

		send_count +=rlen;

		memcpy(msg +1,&protocol,1);

		pack_nu++;

		memcpy(msg +1,&protocol,1);

		//pack_length = rlen;
		memcpy(msg +2,&pack_nu,8);
		memcpy(msg + 22 + file_name_length,&rlen,4);
		memcpy(msg + 22  + file_name_length +4,recv_buff,rlen);

		send_len = len_a + rlen;
		
		ret = Sendto(sockfd,msg,send_len,0,pservaddr,servlen);
		if(ret < 0)
		{
			goto _err2;
		}
		if(pack_nu == 1)
		{
			usleep(wait_t);
			//处理策略?
		}
		if((pack_nu % pack_t) == 0)
			usleep(wait_t);
	}
	if(rlen == -1)
	{
		ret = E_READ;
		goto _err2;
	}
	
	close(sockfd);
	close(fd);
	free(msg);
	free(file_name);
	return S_OK;

_err2:
	close(sockfd);
	close(fd);
	if(msg != NULL)
		free(msg);
	if(file_name != NULL)
		free(file_name);
	return ret;
_err1:
	close(sockfd);
	return E_OPEN;

}
#endif

static int SendPack_ex(const char *file_path,int sockfd,const struct sockaddr *pservaddr,socklen_t servlen,int pack_t)
{
    int wait_t = 0;
    int queryed = 0;
    int len_a = 0;
    int send_len = 0;
    int send_count = 0;

    int fd,ret;
    int rlen;

    char *msg = NULL;
    int msg_len;

    char recv_buff[MAXLINE] ={0};

    /*----UDP 包结构----**/
    char version = '\017';
    char protocol = '\014';
    long long pack_nu = 0;
    long long file_size;
    int file_name_length = strlen(file_path);
    char *file_name = NULL;
    //int pack_length;
    //char *pack;

    file_size = GetFileSize(file_path);
    //fprintf(stderr,"### filesize = %lld\n",file_size);
    if(file_size < 0)
    {
        goto _err1_ex; /*just return E_OPEN*/
    }

    msg_len = 1 + 1 + 8 + 8 + 4 +file_name_length + 4 + MAXLINE;

    //fprintf(stderr,"### msg_len = %d\n",msg_len);
    msg = (char *)malloc(msg_len);
    file_name = strdup(file_path);

    //初始化部分发送包的数据结构
    memcpy(msg,&version,1);

    memcpy(msg +10,&file_size,8);
    memcpy(msg + 18,&file_name_length,4);
    memcpy(msg + 22,file_name,file_name_length);

    if(!(fd = open(file_name,O_RDONLY)))
    {
        perror("open file ");
        Log::Error("Can not openfile %s:%s\n",file_name,strerror(errno));
        ret = -1;
        goto _err2_ex;	/*close fd free msg & file_name and return E_OPEN */
    }

    len_a = 22 + file_name_length + 4;

    while((rlen = read(fd,recv_buff,MAXLINE)) != -1)
    {
        if(rlen == 0)
            break;

        if((send_count == 0)&&((send_count+rlen) == file_size))
            protocol = 0x13;
        else if(send_count == 0)
            protocol = 0x14;
        else if(send_count+rlen == file_size)
        {
            protocol = 0x16;
        }
        else if(send_count + rlen < file_size)
            protocol = 0x15;

        send_count +=rlen;

        memcpy(msg+1,&protocol,1);

        pack_nu++;

        memcpy(msg +1,&protocol,1);

        //pack_length = rlen;
        memcpy(msg +2,&pack_nu,8);
        memcpy(msg + 22 + file_name_length,&rlen,4);
        memcpy(msg + 22  + file_name_length +4,recv_buff,rlen);

        send_len = len_a + rlen;

        // qurey speed here
        if(pack_nu == 1 || (pack_nu % pack_t)==0 )
        {
            if(queryed != 0)
            {
                RETUEN_SPEED(queryed);
                queryed = 0;
            }
            queryed = QUERY_SPEED;
            //wait_t = QUERY_SPEED;
            wait_t = 8016*pack_t / queryed;
            //printf("queryed = %d Mbps ,wait_t = %d\n",queryed,wait_t);
        }
        /*
        if(protocol == '\016')
        {
            int t_len = 0;
            memcpy(&t_len,msg+22 +file_name_length,4);
            printf("\n Last Pack %d\n  ",t_len);
        }

        */
        ret = Sendto(sockfd,msg,send_len,0,pservaddr,servlen);
        if(ret < 0)
        {
            goto _err2_ex;
        }
        if(pack_nu == 1)
        {
            usleep(wait_t);
            //处理策略?
        }
        if((pack_nu % pack_t) == 0)
            usleep(wait_t);
    }
    if(rlen == -1)
    {
        ret = -1;
        goto _err2_ex;
    }
    if(queryed != 0)
        RETUEN_SPEED(queryed);

    //close(sockfd);
    close(fd);
    free(msg);
    free(file_name);
    return 0;

_err2_ex:
    //close(sockfd);
    close(fd);
    if(msg != NULL)
        free(msg);
    if(file_name != NULL)
        free(file_name);

    if(queryed != 0)
        RETUEN_SPEED(queryed);

    return ret;
_err1_ex:
    //close(sockfd);
    if(queryed != 0)
        RETUEN_SPEED(queryed);
    return -1;

}










/***********************************
 * Some Other important method
 **********************************/
// 1. GetFileSize
static long GetFileSize(const char *path)
{
	struct stat buf;
	int iRet = stat(path,&buf);
	if(iRet == -1)
        return -1;
	return buf.st_size;
}

// 2. Init_sockaddr 
static int Inet_pton(int family, const char *strptr, void *addrptr)
{
	int     n;
	if ( (n = inet_pton(family, strptr, addrptr)) < 0)
        return -1;
	else if (n == 0)
        return -1;
    return 0;
}
static int Init_sockaddr_s(struct sockaddr_in *addr_struct,sa_family_t family,const char * IP,int port)
{
	bzero(addr_struct,sizeof(struct sockaddr_in));
	addr_struct->sin_family = family;
	addr_struct->sin_port = htons(port);
	int iRet = 0;
	iRet = Inet_pton(family, IP, &(addr_struct->sin_addr));
	return iRet;
}

// 3. Sendto
static int Sendto(int fd, const void *ptr, size_t nbytes, int flags,
				const struct sockaddr *sa, socklen_t salen)
{       
		if (sendto(fd, ptr, nbytes, flags, sa, salen) != (ssize_t)nbytes){
			perror("sendto error");
            return -1;
		}
        return 0;
}

void *file_thread(void *arg)
{

    //add for debug
    static FILE *fp_out = NULL;
    if(fp_out == NULL)
        fp_out = fopen("/usr/share/udp-file-proxy/send_result.log","w+");



    APP_INFO *info = (APP_INFO*)arg;
    //准备socket和目标地址结构
    info->tid = pthread_self();
    int sock;
    struct sockaddr_in serveraddr;
    Init_sockaddr_s(&serveraddr,AF_INET,info->server_ip.c_str(),info->server_port);

    sock = socket(AF_INET,SOCK_DGRAM,0);
    if(sock < 0){
       perror("socket");
       fprintf(stderr,"file_thread exit.\n");
       return NULL;
    }
    bool del_file = info->delect_file;
    //SEND_LIST *list = info->list;
    //从链表中取出数据
    int pack_t = 0;
    if(info->pack_t < 0 || info->pack_t > 1000)
    {
        pack_t = PACK_T;
    }else
    {
        pack_t = info->pack_t;
    }
    while(1) // 服务永不退出外部指令退出
    {
        if(info->list_c.get_stat()==S_IDLE)
        {
            //printf("没有文件需要传输.\n");
            //sleep(1);
			usleep(200);
            continue;
        }
        else if(info->list_c.get_stat() == S_BUSY)
        {
            //printf("即将传输文件.\n");
            SEND_DATA* data = info->list_c.get_next();
            std::cout << "get filename :" << data->filename << std::endl;

            if(data == NULL)
            {
                 Log::Error("Get empty Data");
                //sleep(1);
				usleep(100);
                continue;
            }

            const char *file = data->filename.c_str();
            bool delect = data->delect;

            struct timeval start, end;
            float size;
#if 1
            size = GetFileSize(file);
            if(size <=0)
            {
                 Log::Error("Can not access file %s:%s\n",file,strerror(errno));
                delete data;
                continue;
                //文件错误
            }

            gettimeofday( &start, NULL );
            //int ret = SendPack_ex(file,sock,(struct sockaddr *)&serveraddr,sizeof(serveraddr),PACK_T);
            int ret = SendPack_ex(file,sock,(struct sockaddr *)&serveraddr,sizeof(serveraddr),pack_t);
            if(ret == 0)
            {
                 if(delect)
                {
                     //delect file;
                    if(unlink(file)<0){
                        printf("unlink error !\n");
                    }
                }
                gettimeofday( &end, NULL );
                float timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec -start.tv_usec;

                printf("文件传输完毕[%d] .\n",ret);
                printf("用时 %.02f us,文件大小 %f K\n",timeuse,size/1000);
                float speed = (size/1024/1024)/(timeuse/1000/1000);
                printf("speed : %.02f M/s\n",speed);
            }
#endif
            //add for debug
            //sleep(3);
            fprintf(fp_out,"%s  result:%d\n",file,ret);
            fflush(fp_out);

           delete data;




        }
        else if(info->list_c.get_stat()==S_CANCEL)
        {
            close(sock);
            break;
        }
        else
        {
            printf("未知的状态.\n");
            sleep(2);
            continue;
        }
    }
}













