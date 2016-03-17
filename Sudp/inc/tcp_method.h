#ifndef __TCP_METHOD_H__
#define __TCP_METHOD_H__
#include <sys/types.h>

int tcp_echo(char *echo,int len,int sockfd);
int  udp_message(char *uecho,int len,int usock,struct sockadrr *sa);


#endif
