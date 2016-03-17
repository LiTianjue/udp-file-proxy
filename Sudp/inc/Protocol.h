#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

//#define USE_JNI		0
//需要全局的变量也写在这里好了

//#define LOG_PATH	"/root/SUDP_"

#define S_OK		0		/*发送成功*/
#define S_ERR		-1		/*发送文件失败*/
#define E_OPEN		-2		/*获取文件大小或打开文件失败*/
#define E_INIT		-3		/*端口的初始化失败*/
#define E_SENDTO	-4		/*发送数据包失败*/
#define E_SOCKET	-5		/*创建套接字失败*/
#define E_READ		-6		/*读取文件失败*/




#define PT_ONLY     '\013'
#define PT_START    '\014'
#define PT			'\015'
#define PT_END      '\016'

#define MAXLINE			1024
#define MAX_BUFF        1024*2
#define MAX_PACK        1024
#define MAXDATASIZE     1024*2


/**--缓冲区的大小--**/
/*
#define BUFF_SIZE       2048
#define BUFF_COUNT      8

*/
/*缓冲区的数据结构*/
/*
typedef struct _buf_list
{
	int pack_num;
	int is_used;
	int buff_size;
	char buff[BUFF_SIZE];
}recv_buff;
*/

#define RUN_PATH        "/run/shm"

/*消息队列的结构*/
#define MAXMSG      (8192 + sizeof(long))
#define WAIT_MSG    8

struct msgbuf {
	long mtype;
	char mtext[1];
};

typedef struct __MyPthread {
	int msqid;
	key_t key;
} MyPthread;


#endif
