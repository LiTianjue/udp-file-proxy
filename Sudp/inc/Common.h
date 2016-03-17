#ifndef __COMMON_H__
#define __COMMON_H__

/*将一个字符串原地逆序,只对char字符串有效*/
void char_exchange(char *str,int ll);

/*获得一个文件的大小单位 byte*/
long GetFileSize(char *path);

/*创建文件夹（传入的字串包含文件名）*/
int CreateDir(char *pDir);

/*写日志文件 传入路径和要写的字串*/
void WriteSysLog(char *log_path,char * str);

#endif
