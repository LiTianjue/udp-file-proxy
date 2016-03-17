#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


#define BUFFER_SIZE 2048
int mhp_file_helper_copy_ex(const char *src,const char *des)
{
   int from_fd,to_fd;
   int bytes_read,bytes_write;
   char buffer[BUFFER_SIZE];
   char *ptr;

   /* ��Դ�ļ� */
   if((from_fd=open(src,O_RDONLY))==-1)
   {
     return -1;
   }

   /* ����Ŀ���ļ� */
   /* ʹ����O_CREATѡ��-�����ļ�,open()������Ҫ��3������,
      mode=S_IRUSR|S_IWUSR��ʾS_IRUSR �û����Զ� S_IWUSR �û�����д*/
   if((to_fd=open(des,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR))==-1)
   {
     close(from_fd);
     return -1;
   }

   int ret =0;
   while(bytes_read=read(from_fd,buffer,BUFFER_SIZE))
   {
     /* һ�������Ĵ������� */
     if((bytes_read==-1)&&(errno!=EINTR))
         break;
     else if(bytes_read>0)
     {
       ptr=buffer;
       while(bytes_write=write(to_fd,ptr,bytes_read))
       {
         /* һ�������������� */
         if((bytes_write==-1)&&(errno!=EINTR))
         {
             ret = -1;
             break;
         }
         /* д�������ж����ֽ� */
         else if(bytes_write==bytes_read)
             break;
         /* ֻд��һ����,����д */
         else if(bytes_write>0)
         {
           ptr+=bytes_write;
           bytes_read-=bytes_write;
         }
       }
       /* д��ʱ�������������� */
       if(bytes_write==-1)
       {
           ret = -1;
           break;
       }
     }
   }
   close(from_fd);
   close(to_fd);
   return ret;
}

int main(int argc,char *argv[])
{
	int ret =0;
	ret = mhp_file_helper_copy_ex(argv[1],argv[2]);
	if(ret != 0)
		perror("copy:");

	return 0;
}
