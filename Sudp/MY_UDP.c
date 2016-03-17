#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>	/*htonl htons*/
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

#include "liblog.h"
#include "defs.h"



/*----创建套接子-----*/
int Socket(int family,int type,int protocol)
{
	int n;

	n = socket(family,type,protocol);
	PRINTF(LEVEL_INFORM,"create socket success :%d\n",n);
	if(n < 0)
		PRINTF(LEVEL_ERROR,"Create socket error :%s\n ",strerror(errno));

	return(n);
}

/*----初始化结构体 struct sockaddr_in----*/
/* 对于UDP服务器: init_sockaddr(sa_in,,INADDR_ANY,SERVER_PORT)
 * 这个函数有两个版本，其中
 *  init_sockaddr_s() 中的ip 参数为点分十进制的地址字串指针
 * */
/*----将点分十进制的字符串转换为大整数----*/
void
Inet_pton(int family, const char *strptr, void *addrptr)
{
	int     n;

	if ( (n = inet_pton(family, strptr, addrptr)) < 0)
		PRINTF(LEVEL_ERROR,"inet_pton error for %s ",strerror(errno));  /* errno set */
	else if (n == 0)
		PRINTF(LEVEL_ERROR,"inet_pton error for %s", strptr); /* errno not set */
	/* nothing to return */
}
/*----1----*/
void Init_sockaddr(struct sockaddr_in *addr_struct,sa_family_t family,int32_t IP,int port)
{
	bzero(addr_struct,sizeof(struct sockaddr_in));
	addr_struct->sin_family = family;
	addr_struct->sin_addr.s_addr = htonl(IP);
	addr_struct->sin_port = htons(port);
}
/*---2----*/
void Init_sockaddr_s(struct sockaddr_in *addr_struct,sa_family_t family,char * IP,int port)
{
	bzero(addr_struct,sizeof(struct sockaddr_in));
	addr_struct->sin_family = family;
	addr_struct->sin_port = htons(port);
	Inet_pton(family, IP, &(addr_struct->sin_addr));
}


/*-----Bind-----*/
void Bind(int fd,const struct sockaddr *sa,socklen_t salen)
{
	if(bind(fd,sa,salen) < 0)
		PRINTF(LEVEL_ERROR,"bind error :%s\n",strerror(errno));
}

/*----Recvfrom----*/
ssize_t 
Recvfrom(int fd, void *ptr, size_t nbytes, int flags,
		struct sockaddr *sa, socklen_t *salenptr)
{
	ssize_t     n;
	if ( (n = recvfrom(fd, ptr, nbytes, flags, sa, salenptr)) < 0)
		PRINTF(LEVEL_ERROR,"recvfrom error :%s\n",strerror(errno));
	return(n);
}
/*----Sendto----*/
void
Sendto(int fd, const void *ptr, size_t nbytes, int flags,
		const struct sockaddr *sa, socklen_t salen)
{       
	if (sendto(fd, ptr, nbytes, flags, sa, salen) != (ssize_t)nbytes)
		PRINTF(LEVEL_ERROR,"sendto error %s\n",strerror(errno));
}



/*********************************************************/
/*----TCP----相关的代码----*/
void Listen(int fd,int listenq)
{
	if(listen(fd,listenq) < 0)
	{
		PRINTF(LEVEL_ERROR,"Listern error %s\n",strerror(errno));
		exit(-1);
	}
}

int Accept(int fd,struct sockaddr *sa,socklen_t *salenptr)
{
	int		n;

again:
	if((n = accept(fd,sa,salenptr)) < 0) {
		//可能要处理某些特殊的错误
		//if(errno == E... || error == E..)

		PRINTF(LEVEL_ERROR,"Accept error %s\n",strerror(errno));
	}
	return (n);
}

void Close(int fd)
{
	if(close(fd) == -1)
		perror("close error");
}

void Connect(int fd, const struct sockaddr *sa,socklen_t salen)
{
	if(connect(fd,sa,salen) < 0)
		PRINTF(LEVEL_ERROR,"Connect error %s\n",strerror(errno));
}


pid_t Fork(void)
{
	pid_t	pid;

	if((pid = fork()) == -1)
		perror("fork error");
	return(pid);
}

				                          
