#ifndef _GET_CONFIG_H_
#define _GET_CONFIG_H_
 
#include <string>
#include <map>
#include <vector>
using namespace std;
 
#define COMMENT_CHAR '#'
 
bool ReadConfig(const string & filename, map<string, string> & m);
void PrintConfig(const map<string, string> & m);

class CONF
{
public:
	CONF();
	~CONF();

	bool CONF_load(const string &filename);
	string CONF_get_string(const string & key);
	int CONF_get_number(const string &key);
	void CONF_print();

    // add for msg prase
    bool CONF_prase_line(const char *data, string pattern);
    bool CONF_prase_line(const string &data, string pattern);
    bool CONF_get_bool(const string &key);
	

private:
	//CONF_free();


	map<string,string> m_section;


};
#endif
