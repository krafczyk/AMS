#include <iostream>
#include <math.h>
#include <string>
#include <fstream>
using namespace std;
bool file_exists (const std::string& name);

int main(int argc, char* argv[]){
int y_min=2013;
int y_max=2015;
int d_min=1;
int d_max=366;
char tmp[1024];
  switch (argc) {
   case 1:
//  today only
    time_t timenow;
    time(&timenow);
    strftime(tmp,80,"%Y",gmtime(&timenow));
    y_min=atol(tmp);
    y_max=atol(tmp);
    strftime(tmp,80,"%j",gmtime(&timenow));
    d_min=atol(tmp);
    d_max=atol(tmp);
    break;
   break;
   default:
   break;
} 
 for( int i=1;i<argc;i++){
   if(strstr(argv[i],"yesterday")){
    time_t timenow;
    time(&timenow);
    timenow-=86400;
    strftime(tmp,80,"%Y",gmtime(&timenow));
    y_min=atol(tmp);
    y_max=atol(tmp);
    strftime(tmp,80,"%j",gmtime(&timenow));
    d_min=atol(tmp);
    d_max=atol(tmp);
   }
   else if(strstr(argv[i],"lastweek")){
    time_t timenow;
    time(&timenow);
    strftime(tmp,80,"%Y",gmtime(&timenow));
    y_max=atol(tmp);
    strftime(tmp,80,"%j",gmtime(&timenow));
    d_max=atol(tmp);
    timenow-=86400*7;
    strftime(tmp,80,"%Y",gmtime(&timenow));
    y_min=atol(tmp);
    strftime(tmp,80,"%j",gmtime(&timenow));
    d_min=atol(tmp);
    if(d_min>d_max)d_min=d_max;
   }
   else if(strstr(argv[i],"lastmonth")){
    time_t timenow;
    time(&timenow);
    strftime(tmp,80,"%Y",gmtime(&timenow));
    y_max=atol(tmp);
    strftime(tmp,80,"%j",gmtime(&timenow));
    d_max=atol(tmp);
    timenow-=86400*31;
    strftime(tmp,80,"%Y",gmtime(&timenow));
    y_min=atol(tmp);
    strftime(tmp,80,"%j",gmtime(&timenow));
    d_min=atol(tmp);
    if(d_min>d_max)d_min=d_max;
   }
   else{
     cerr<<"main-F-AvailableOptions are --lastweek --yesterday --lastmonth NULL==today"<<endl;
     return 1;
   }
 }
char *ad=getenv("AMSDataDir");
if(!ad){
cerr<<"main-F-AMSDataDirNotDefinedExiting"<<endl;
return 1;
}
string input=ad;
string output=ad;
output+="/altec/";
input+="/isssa/USGNC_PS_Pointing_LVLH_Att_Quatrn/";
string fnamei;
string fnameo;
string prefix="ISS_USGNC_PS_Pointing_LVLH_Att_Quatrn_";
string prefixo="ISS_ATT_EULR_LVLH-";
string suffix="-24H.csv";
for(int y=y_min;y<=y_max;y++){
 for(int d=d_min;d<=d_max;d++){
   char tmp[80];
   char tmpo[80];
   sprintf(tmp,"%u_%03u",y,d);
   sprintf(tmpo,"%u-%03u",y,d);
   fnamei=input;
   fnamei+=prefix;
   fnamei+=tmp;
   fnamei+=suffix;
   fnameo=output;
   fnameo+=prefixo;
   fnameo+=tmpo;
   fnameo+=suffix;
   ifstream si;
   si.clear();
   si.open(fnamei.c_str());
   if(si){
    if(file_exists(fnameo)){
       si.close();
       continue;
    }
    ofstream so;
    so.clear();
    so.open(fnameo.c_str());
    if(so){
      while(si.good() && !si.eof()){
        char line[2500];
        si.getline(line,2499);
        if(isdigit(line[0])){
         char *pch;
         pch=strtok(line,",");
         if(!pch)continue;
         string outline=pch;
         outline+=",";
         double q[4];
         pch=strtok(NULL,",");
         if(!pch)continue;
         if(!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.')continue;
         q[0]=atof(pch); 
         pch=strtok(NULL,",");
         if(!pch)continue;
         if(!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.')continue;
         q[1]=atof(pch); 
         pch=strtok(NULL,",");
         if(!pch)continue;
         if(!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.')continue;
         q[2]=atof(pch); 
         pch=strtok(NULL,"\n");
         if(!pch)continue;
         if(!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.')continue;
         q[3]=atof(pch); 
         double qq=sqrt(q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3]);
         for(int i=0;i<4;i++)q[i]/=qq; 
         double ss=180/3.1415926;
         double r=atan2(2*(q[0]*q[1]+q[2]*q[3]),1-2*(q[1]*q[1]+q[2]*q[2]))*ss;;
         double p=asin(2*(q[0]*q[2]-q[3]*q[1]))*ss;
         double y=atan2(2*(q[0]*q[3]+q[1]*q[2]),1-2*(q[2]*q[2]+q[3]*q[3]))*ss;
         char stmp[2500];
         sprintf(stmp,"%s%f7,%f7,%f7\n",outline.c_str(),y,p,r);
         so<<stmp;
       }
      }
    }
    else{
     cerr<<"-F-UnabletoOpenO-"<<fnameo<<endl;
     exit(1); 
    }
    so.close();
    cout<<"-I-Created"<<fnameo<<endl;
   }
   else{ 
    cerr<<"-W-UnabletoOpenI-"<<fnamei<<endl;

   }
 }
}





return 0;
}
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>

bool file_exists (const std::string& name) {
  struct stat64 buffer;   
  return (stat64 (name.c_str(), &buffer) == 0); 
}
