#include "config.h"

#include <fstream>
#include <iostream>

#include <stdlib.h>
#include <errno.h>
#include <string.h>

using namespace std;

bool IsSpace(char c)
{
    if (' ' == c || '\t' == c)
        return true;
    return false;
}

bool IsCommentChar(char c)
{
    switch(c) {
    case COMMENT_CHAR:
        return true;
    default:
        return false;
    }
}

void Trim(string & str)
{
    if (str.empty()) {
        return;
    }
    int i, start_pos, end_pos;
    for (i = 0; i < str.size(); ++i) {
        if (!IsSpace(str[i])) {
            break;
        }
    }
    if (i == str.size()) { // 全部是空白字符串
        str = "";
        return;
    }
    
    start_pos = i;
    
    for (i = str.size() - 1; i >= 0; --i) {
        if (!IsSpace(str[i])) {
            break;
        }
    }
    end_pos = i;
    
    str = str.substr(start_pos, end_pos - start_pos + 1);
}

bool AnalyseLine(const string & line, string & key, string & value)
{
    if (line.empty())
        return false;
    int start_pos = 0, end_pos = line.size() - 1, pos;
    if ((pos = line.find(COMMENT_CHAR)) != -1) {
        if (0 == pos) {  // 行的第一个字符就是注释字符
            return false;
        }
        end_pos = pos - 1;
    }
    string new_line = line.substr(start_pos, start_pos + 1 - end_pos);  // 预处理，删除注释部分
    
    if ((pos = new_line.find('=')) == -1)
        return false;  // 没有=号
        
    key = new_line.substr(0, pos);
    value = new_line.substr(pos + 1, end_pos + 1- (pos + 1));
    
    Trim(key);
    if (key.empty()) {
        return false;
    }
    Trim(value);
    return true;
}

//字符串分割函数
std::vector<std::string> split(std::string str,std::string pattern)
{
  std::string::size_type pos;
  std::vector<std::string> result;
  str+=pattern;//扩展字符串以方便操作
  int size=str.size();

  for(int i=0; i<size; i++)
  {
    pos=str.find(pattern,i);
    if(pos<size)
    {
      std::string s=str.substr(i,pos-i);
      result.push_back(s);
      i=pos+pattern.size()-1;
    }
  }
  return result;
}

bool ReadConfig(const string & filename, map<string, string> & m)
{
    m.clear();
    ifstream infile(filename.c_str());
    if (!infile) {
        cout << "file open error:" << filename << endl;
        perror("open");
        return false;
    }
    string line, key, value;
    while (getline(infile, line)) {
        if (AnalyseLine(line, key, value)) {
            m[key] = value;
        }
    }
    
    infile.close();
    return true;
}

void PrintConfig(const map<string, string> & m)
{
    map<string, string>::const_iterator mite = m.begin();
    for (; mite != m.end(); ++mite) {
        cout << mite->first << "=" << mite->second << endl;
    }
}




/*******************************************************/

CONF::CONF()
{
    m_section.clear();
}


CONF::~CONF()
{
}

bool CONF::CONF_load(const string &filename)
{
	m_section.clear();
	ifstream infile(filename.c_str());
	if(!infile) {
        cout << "file open error" <<filename << endl;
        perror("open");
        return false;
	}
	string line ,key,value;
	while (getline(infile,line)) {
		if (AnalyseLine(line,key,value))
		{
			m_section[key] = value;
		}
	}
}

void CONF::CONF_print()
{
	map<string,string>::const_iterator mite = m_section.begin();
	for(;mite != m_section.end();++mite) {
		cout << mite->first << "=" <<mite->second << endl;
    }
}

bool CONF::CONF_prase_line(const char *data, string pattern)
{
    m_section.clear();
    string line_ex(data);

    std::vector<std::string> result=split(line_ex,pattern);
    //std::cout<<"The result:"<<std::endl;

    string key,value;
    for(int i=0; i<result.size(); i++)
    {
        if(AnalyseLine(result[i],key,value))
        {
            m_section[key] = value;
        }
        //std::cout<<result[i]<<std::endl;
    }

    if(result.size() <=0)
        return false;

    return true;
}

bool CONF::CONF_prase_line(const string &data, string pattern)
{
    m_section.clear();

    std::vector<std::string> result=split(data,pattern);

    //std::cout<<"The result:"<<std::endl;
    string key,value;
    for(int i=0; i<result.size(); i++)
    {
        //std::cout<<result[i]<<std::endl;
        if(AnalyseLine(result[i],key,value))
        {
            m_section[key] = value;
        }
    }
    if(result.size() <=0)
        return false;

    return true;
}

bool CONF::CONF_get_bool(const string &key)
{
    map<string,string >::iterator l_it;;
    l_it=m_section.find(key);
    if(l_it==m_section.end())
    {
        //cout<<"we do not find" << section <<endl;
        //return string("");
        return false;
    }
    else
    {
        //cout<<"we find " << section <<endl;
        bool ret = ((l_it->second)=="true")?true:false;

        return ret;

        //return l_it->second;
    }
}

string CONF::CONF_get_string(const string &section)
{
	map<string,string >::iterator l_it;; 
	l_it=m_section.find(section);
	if(l_it==m_section.end())
	{
		//cout<<"we do not find" << section <<endl;
		return string("");
	}
	else 
	{
		//cout<<"we find " << section <<endl;
		return l_it->second;
	}
}

int CONF::CONF_get_number(const string &section)
{
	map<string,string >::iterator l_it;; 
	l_it=m_section.find(section);
	if(l_it==m_section.end())
	{
		//cout<<"we do not find" << section <<endl;
		//return string("");
		return -1;
	}
	else 
	{
		//cout<<"we find " << section <<endl;
		int number = atoi( (l_it->second).c_str() );
		if (errno == ERANGE) //可能是std::errno
		{
			return -1;
		}
		else if (errno == EINVAL)
		{
			//不能转换成一个数字
			return -1;
		}
		return number;

		//return l_it->second;
	}
}




