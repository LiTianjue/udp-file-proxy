
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "liblog.h"
#include "defs.h"

/**************************************/


/*-NO.1-----------------------------
 *  功能： 将一个字符串逆序
 *  说明 ： 这个函数只能将字符串逆序，不能逆序内存单元
 *-----------------------------*/

void char_exchange(char *str,int ll)
{
	char tmp;
	int len,i,j;
	len = ll;
	for(i = 0,j=len-1;i!=len/2; i++,j--) {
		tmp = str[i];
		str[i]=str[j];
		str[j] = tmp;
	}
}
/**************************************/

/*-NO.2-----------------------------
 *  功能： 获得一个文件的大小,单位 byte
 *  说明 ： 传入的字符串为文件的绝对路径
 *			不能获得文件夹的大小
 *-----------------------------*/
long GetFileSize(char *path)
{
	struct stat buf;
	int iRet = stat(path,&buf);
	if(iRet == -1)
		return 0;
	return buf.st_size;
}   

/**************************************/

/*-NO.3-----------------------------
 *  功能： 创建文件夹
 *  说明 : 传入的是包含文件名的绝对路径 
 *-----------------------------*/
int CreateDir(char *pDir)
{
	int i = 0;
	int iRet = 0;
	int iLen;
	char* pszDir;
	char *file_path = NULL;

	if(NULL == pDir)
	{
		return 0;
	}
	pszDir = strdup(pDir);
	iLen = strlen(pszDir);

	for(i = 0;;i++)
	{
		if(pszDir[i] != '\0')
		{
			if(pszDir[i] == '/')
			{
				file_path = pszDir + i;
			}
		}
		else{
			*file_path = '\0';
			break;
		}
	}

	for (i = 0;i < iLen;i ++)
	{
		if ((pszDir[i] == '/') && (i > 0))
		{   
			pszDir[i] = '\0';  
			if(access(pszDir,F_OK) < 0)
			{
				if(mkdir(pszDir,0755) < 0)
				{
					//perror("mkdir"); 
					PRINTF(LEVEL_ERROR,"Mkdir :%s \n",strerror(errno));
	 				return -1; 
	 			}
			}
			pszDir[i] = '/';  
		}  
	}

	iRet = mkdir(pszDir,0755);
	free(pszDir);  
	return iRet;  
}
/**************************************/

/*-NO.4-----------------------------
 *  功能： 写日志文件
 *  说明 : 传入的是要写进日志文件的内容，日志文件的绝对路径 
 *-----------------------------*/
void WriteSysLog(char *log_path,char *str)
{
	char buff[512];
	long MAXLEN = 10*1024*1024;		/*日志文件的大小最大为10M 超过就会清空*/
	time_t timep;
	FILE *fp = NULL;
	struct tm *p;

	time(&timep);
	p = localtime(&timep);
	memset(buff,0,sizeof(buff));
	sprintf(buff,"%d-%d-%d %d:%d:%d--->",(1900+p->tm_year),(1+p->tm_mon),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	strcat(buff,str);
	strcat(buff,"\r\n");

	fp = fopen(log_path,"r");
	if(fp == NULL)
	{
		fp = fopen(log_path,"w+");
	}
	else
	{
		fseek(fp,0,2);
		if(ftell(fp) >= MAXLEN)
		{
			fclose(fp);
			fp = fopen(log_path,"w+");
		}
		else
		{
			fclose(fp);
			fp = fopen(log_path,"a");
		}
	}

	fwrite(buff,1,strlen(buff),fp);
	fflush(fp);
	fsync(fileno(fp));
	fclose(fp);
}

























