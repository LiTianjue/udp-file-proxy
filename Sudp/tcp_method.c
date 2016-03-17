#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include <errno.h>

#include "liblog.h"
#include "defs.h"
#include "inc/MY_UDP.h"


int tcp_echo(char *uecho,int len ,int sockfd)
{
	if(-1 == send(sockfd,uecho,len,0))
	{
		PRINTF(LEVEL_ERROR,"Tcp echo error : %s\n",strerror(errno));
		close(sockfd);
		return -1;
	}

	PRINTF(LEVEL_INFORM,"TCP send %s\n",uecho);

	return 0;
}

int  udp_message(char *uecho,int len,int usock,struct sockaddr *sa)
{
    socklen_t sa_len = sizeof(struct sockaddr);
    Sendto(usock,uecho,len,0,sa,sa_len);

}
