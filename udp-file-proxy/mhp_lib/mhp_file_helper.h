#ifndef MHP_FILE_HELPER_H
#define MHP_FILE_HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


// [1] 重命名文件不能跨磁盘,没有拷贝
int mhp_file_helper_rename(const char *src,const char *des);

// [2] 重命名文件(移动文件)，可以跨磁盘，有拷贝过程
int mhp_file_helper_move(const char *src,const char *des);

// [3] 拷贝文件
int mhp_file_helper_copy(const char *src,const char *des);


#endif // MHP_FILE_HELPER_H

