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

   /* 打开源文件 */
   if((from_fd=open(src,O_RDONLY))==-1)
   {
     return -1;
   }

   /* 创建目的文件 */
   /* 使用了O_CREAT选项-创建文件,open()函数需要第3个参数,
      mode=S_IRUSR|S_IWUSR表示S_IRUSR 用户可以读 S_IWUSR 用户可以写*/
   if((to_fd=open(des,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR))==-1)
   {
     close(from_fd);
     return -1;
   }

   int ret =0;
   while(bytes_read=read(from_fd,buffer,BUFFER_SIZE))
   {
     /* 一个致命的错误发生了 */
     if((bytes_read==-1)&&(errno!=EINTR))
         break;
     else if(bytes_read>0)
     {
       ptr=buffer;
       while(bytes_write=write(to_fd,ptr,bytes_read))
       {
         /* 一个致命错误发生了 */
         if((bytes_write==-1)&&(errno!=EINTR))
         {
             ret = -1;
             break;
         }
         /* 写完了所有读的字节 */
         else if(bytes_write==bytes_read)
             break;
         /* 只写了一部分,继续写 */
         else if(bytes_write>0)
         {
           ptr+=bytes_write;
           bytes_read-=bytes_write;
         }
       }
       /* 写的时候发生的致命错误 */
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
