#include "GET_NEW_FILE.h"
GET_NEW_FILE::GET_NEW_FILE(string dir):fdir(dir){
	fpre_time=time(NULL);
}
GET_NEW_FILE::GET_NEW_FILE(string dir,time_t pre_time):fdir(dir),fpre_time(pre_time){
	
}
GET_NEW_FILE::~GET_NEW_FILE(){
}
int GET_NEW_FILE::get_new(vector<string> new_files){
	struct dirent *entry;
  	DIR *dp;
	int i=0;
/*
	map<unsigned char,string> T;
	T[DT_UNKNOWN]="DT_UNKNOWN";
	T[DT_REG]="DT_REG";
	T[DT_DIR]="DT_DIR";
	T[DT_FIFO]="DT_FIFO";
	T[DT_SOCK]="DT_SOCK";
	T[DT_CHR]="DT_CHR";
	T[DT_BLK]="DT_BLK";
*/
	dp=opendir(fdir.c_str());
	struct stat fstat;
	
	if(dp==NULL){
		perror("opendir");
		return -1;
	}
	stat(fdir.c_str(),&fstat);
	if(fstat.st_mtime==fpre_time){
		return 0;
	}
	while((entry=readdir(dp))){
		cout<<entry->d_name<<endl;
		stat(entry->d_name,&fstat);
		if(fstat.st_mtime>fpre_time&&entry->d_fileno!=DT_DIR){
			if(strstr(entry->d_name,".root")!=NULL){
				new_files.push_back(entry->d_name);
				i++;
			}		
		}
	}	
	closedir(dp);
	return i;
}

