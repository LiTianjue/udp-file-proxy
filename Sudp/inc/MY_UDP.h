#ifndef __MY_UDP_H__
#define __MY_UDP_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

 /*----创建套接子-----*/
/* sockfd = Socket(AF_INET,SOCK_DGRAM,0)
 */
int Socket(int family,int type,int protocol);


 /*----初始化结构体 struct sockaddr_in----*/
/* 这个函数有两个版本，区别在于传进的ip是大整数还是字符串
 * Init_sockaddr(sockaddr_in结构体指针，协议簇，IP ,端口号)
 */
void Init_sockaddr(struct sockaddr_in *addr_struct,sa_family_t family,int32_t IP,int port);

void Init_sockaddr_s(struct sockaddr_in *addr_struct,sa_family_t family,char* IP,int port);



/*-----Bind-----*/
/* 绑定套接字和端口
 * Bind(套接字，结构体，结构体长度)
 */
void Bind(int fd,const struct sockaddr *sa,socklen_t salen);


/*----Recvfrom----*/
/* 封装了的recvfrom
 * 套接字，buf，接收长度，flags=0,发送端的sockaddr结构*，结构长度*
 * 最后两项都是指针，要事先准备好用来回填的,或者或者为NULL,表示不关心
 * 返回接收的字符个数
 */
ssize_t Recvfrom(int fd, void *ptr, size_t nbytes, int flags,struct sockaddr *sa, socklen_t *salenptr);


/*----Sendto----*/
/* 封装了的sendto
 * 套接字，buf,发送长度,flags=0,接收端sockaddr结构*，长度
 * 和Revcfrom 不同，最后一个参数不是指针
 */
void Sendto(int fd, const void *ptr, size_t nbytes, int flags,
		const struct sockaddr *sa, socklen_t salen);


/**************************************************************/
//为TCP套接字封装了其他一些必须的函数
//2014/07/31
//
void Listen(int fd,int listenq);

int Accept(int fd,struct sockaddr *sa,socklen_t *salenptr);

void Close(int fd);

void Connect(int fd, const struct sockaddr *sa,socklen_t salen);

pid_t Fork(void);








#endif
