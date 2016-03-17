#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>


int main()
{
	char dir1[128] = "/usr/share/udp-file-proxy/log/";
	char dir2[128] = "/usr/share/udp-file-proxy/log/";
	/*
	printf("dirname:\n");
	char *tmp = dirname(dir1);
	char *tmp2 = dirname(dir2);
	printf("%s.\n",tmp);
	printf("%s.\n",tmp2);
	printf("\n");
	
	printf("basename:\n");
	char *tmp3 = dirname(dir1);
	char *tmp4 = dirname(dir2);
	printf("%s.\n",tmp3);
	printf("%s.\n",tmp4);
	*/
	printf("clean name : %s\n",dir1);
	/*
	printf("dir name %s\n",dirname(dir1));
	printf("dir name %s\n",dirname(dir1));
	printf("dir name %s\n",dirname(dir1));
	*/
	printf("base name %s\n",basename(dir1));
	printf("base name %s\n",basename(dir1));
	printf("base name %s\n",basename(dir1));

	printf("base name %s\n",basename(dir2));

	
	printf("clean name :%s\n",dir1);
	printf("clean name :%s\n",dir2);
	
}
