#include <iostream>
#include <stdio.h>
#include <dirent.h>
#include "sys/stat.h"
#include <vector>
#include <map>
#include <time.h>
#include <string>
#include "string.h"
#include <unistd.h>



using namespace std;
#ifndef _GET_NEW_FILE_H_
#define _GET_NEW_FILE_H_
class GET_NEW_FILE{
public:
	GET_NEW_FILE(string dir);
	GET_NEW_FILE(string dir, time_t pre_time);
	~GET_NEW_FILE();
	//-1 : directory does not exist
	//0  : no new files
	//other: number of new files
	int get_new(vector<string> new_files);
	void set_dir(string dir){ fdir=dir; };
	string get_dir(){ return fdir;}
	void set_time(time_t t){ fpre_time=t;};
private:
	string fdir;
	time_t fpre_time;
};
#endif
