#ifndef BROWSEDIR
#define BROWSEDIR

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
 #include <string.h>
 #include <sys/stat.h>
 #include <dirent.h>

#include <iostream>
#include <vector>
#define MAX_PATH_LEN    1024


using namespace std;
// 遍历文件文件夹的基类
class CBrowseDir
{
protected:
	 //根路径
	//string m_szInitDir;
    char root_dir[MAX_PATH_LEN];

public:
	//构造函数 检查路径是否存在，尾部没有/要添加/结尾
    CBrowseDir();	//目录为当前路径
    CBrowseDir(const char *dir);	//根路径为指定路径

    //bool SetInitDir(const char *dir);
	// 开始遍历
    bool BeginBrowseDir(const char *filespes,int depth);
	// 遍历获取文件名
    vector<string> BeginBrowseFilenames(const char *filespec,int depth);

protected:
    bool BrowseDir(const char *dir ,const char *filespec,int depth);
    vector<string> GetDirFilename(const char *dir,const char *filespec,int depth);

	//遍历到文件的操作方法
    virtual bool ProcessFile(const char *filename);
	//遍历到目录的操作方法
    virtual void ProcessDir(const char *currentdir ,const char *parentdir);
};


/******************************************************/
//派生出一个用于执行脚本的类
class ScriptExe : public CBrowseDir
{
public:
    ScriptExe();
    ScriptExe(const char *dir);	//根路径为指定路径
    void SetDir(const char *dir);
    bool GetStatus();
    bool SetStatus();
//重写虚函数
protected:
	bool is_err;	
	
	virtual bool ProcessFile(const char *filename);
};



#endif // BROWSEDIR

