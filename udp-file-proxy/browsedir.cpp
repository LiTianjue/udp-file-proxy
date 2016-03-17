#include"browsedir.h"
using namespace std;


/*************************************************/
// PUBLIC
/*************************************************/
CBrowseDir::CBrowseDir()
{
	//cout << "call not NULL" << endl;
	//构造函数
}

CBrowseDir::CBrowseDir(const char *dir)
{
	//构造函数 指定开始目录为dir
	//1. 路径必须是绝对路径
	//2. 判断目录是否存在
	//3. 判断文件路经是否以/结尾
	
	cout << "call " << dir << endl;
     memset(root_dir,0,MAX_PATH_LEN);
	 int len = strlen(dir);
	 strcpy(root_dir,dir);
	 if(root_dir[len-1] == '/')
		 root_dir[len-1] = '\0';
	cout << "[root dir] " << root_dir << endl;
}

bool CBrowseDir::BeginBrowseDir(const char *filespes,int depth)
{
	if(access(root_dir,0))
		return false;

    return BrowseDir(root_dir,filespes,depth);
	//开始遍历，遍历到相应的文件和文件夹执行虚函数	
}

vector<string> CBrowseDir::BeginBrowseFilenames(const char *filespec ,int depth)
{
	//遍历获得文件列表
	return GetDirFilename(root_dir,filespec, depth);
}


/*************************************************/
// PROTECTED 
/*************************************************/
// 遍历目录，可能递归多次
bool CBrowseDir::BrowseDir(const char *dir, const char *filespec,int depth)
{
	if(depth < 1)
		return true;

	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;

	if((dp = opendir(dir)) == NULL)
	{
		//fprintf(stderr,"Cannot open directory:%s \n",dir);
		return false;
	}

	chdir(dir);
	while((entry = readdir(dp)) != NULL)
	{
		lstat(entry->d_name ,&statbuf);
		if(S_ISDIR(statbuf.st_mode)){
			//ignore. and ..
			if(strcmp(".",entry->d_name) == 0 ||
					strcmp("..",entry->d_name) == 0)
				continue;

			//printf("[dir] %s/%s\n",dir,entry->d_name);
			char nextdir[256] = {0};
			sprintf(nextdir,"%s/%s",dir,entry->d_name);
			
			BrowseDir(nextdir,NULL,depth-1);
			// 处理目录
			ProcessDir(entry->d_name,dir);
		}
		else
		{
			//printf("[file] %s/%s\n",dir,entry->d_name);

			char file[256] = {0};
			sprintf(file,"%s/%s",dir,entry->d_name);

			ProcessFile(file);
			//printf("%*s%s\n",depth," ",entry->d_name);
		}
	}
	// add by andy ,must close dir
	closedir(dp);
	chdir("..");
	
	return true;
}



// 获得一次遍历文件夹的文件结果
vector<string> CBrowseDir::GetDirFilename(const char *dir, const char *filespec,int depth)
{
	vector<string> filename_vector;
	filename_vector.clear();

	if(depth < 1)
		return  filename_vector;


	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;

	if((dp = opendir(dir)) == NULL)
	{
		//fprintf(stderr,"Cannot open directory:%s \n",dir);
		return  filename_vector;
	}

	chdir(dir);
	while((entry = readdir(dp)) != NULL)
	{
		lstat(entry->d_name ,&statbuf);
		if(S_ISDIR(statbuf.st_mode)){
			//ignore. and ..
			if(strcmp(".",entry->d_name) == 0 ||
					strcmp("..",entry->d_name) == 0)
				continue; 

			//printf("[dir] %s/%s\n",dir,entry->d_name);
			char nextdir[256] = {0};
			sprintf(nextdir,"%s/%s",dir,entry->d_name);
			
			// 处理目录
			vector<string>tmp = GetDirFilename(nextdir,filespec,depth-1);
            vector<string>::iterator it;
            for(it=tmp.begin();it < tmp.end();it++)
			{
				filename_vector.push_back(*it);
			}
		}
		else
		{
			//printf("[file] %s/%s\n",dir,entry->d_name);

			char file[256] = {0};
			sprintf(file,"%s/%s",dir,entry->d_name);

			filename_vector.push_back(file);
			//ProcessFile(file);
			//printf("%*s%s\n",depth," ",entry->d_name);
		}
	}
	chdir("..");
	
	return  filename_vector;
}

//被重载的函数
bool CBrowseDir::ProcessFile(const char *filename)
{
	//检查文件的读写权限 access
    cout << "[file] " << filename<<endl;
    return true;
}

void CBrowseDir::ProcessDir(const char *currentdir, const char *parentdir)
{
    cout << "[dir] " << currentdir<<endl;
    return;

}
/**********************************************/

ScriptExe::ScriptExe()
{
    is_err = false;

}

ScriptExe::ScriptExe(const char *dir)
{
	cout << "call " << dir << endl;
     memset(root_dir,0,MAX_PATH_LEN);
	 int len = strlen(dir);
	 strcpy(root_dir,dir);
	 if(root_dir[len-1] == '/')
		 root_dir[len-1] = '\0';
     cout << "[root dir] " << root_dir << endl;
    is_err = false;
}

void ScriptExe::SetDir(const char *dir)
{
     memset(root_dir,0,MAX_PATH_LEN);
     int len = strlen(dir);
     strcpy(root_dir,dir);
     if(root_dir[len-1] == '/')
         root_dir[len-1] = '\0';
}

bool ScriptExe::GetStatus()
{
    return is_err;
}

bool ScriptExe::SetStatus()
{
    is_err = false;
    return true;
}

bool ScriptExe::ProcessFile(const char *filename)
{
	if(access(filename,1))
	{
		//cout << "file " <<filename << "can not exe" << endl;
		return false;
	}
	int ret = system(filename);
    //cout << "do return [" << system(filename)<<"]" <<endl;
    if(ret != 0)
	{
        is_err= true;
		//cout << "exe : " <<filename << " --error---" << endl;
		return false;
	}

	return true;
}
