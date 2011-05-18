#include <iostream.h>
#include <fstream.h>
#include <algorithm>
#include <stdlib.h>
#include <unistd.h>
#include <string>
using namespace std;
extern "C" double Julian_Date_of_Epoch(double epoch);
extern "C" char KepCheck(char *line1, char* line2);
extern "C" int ISSLoad(const char *name, const char *line1, const char *line2);
extern "C" int ISSGTOD(float *r,float *t,float *p, float *v, float *vt, float *vp, float *grmedphi, double time);
int main(){

char amsdatadir[]="/afs/cern.ch/ams/Offline/AMSDataDir";
char httpdir[]="http://celestrak.com/NORAD/elements/stations.txt";
char *amsd=getenv("AMSDataDir");
if(!amsd || !strlen(amsd))amsd=amsdatadir;
char *httpd=getenv("AMSNORAD");
if(!httpd || !strlen(httpd))httpd=httpdir;

char spid[80];
pid_t pid=getpid();
sprintf(spid,"/tmp/tle.%d",pid);
string ofile=spid;
string wget="wget ";
wget+=httpd;
wget+=" -q -O ";
wget+=ofile.c_str();
int i=system(wget.c_str());
if(!i){

ifstream ftle;
ftle.clear();
ftle.open(ofile.c_str());
	char name[80], line1[80], line2[80];
	if (ftle){
		while (!ftle.eof()){
			/* Initialize variables */

			name[0]=0;
			line1[0]=0;
			line2[0]=0;

			/* Read element set */

			ftle.getline(name,75);
			ftle.getline(line1,75);
			ftle.getline(line2,75);
                        
			if (strstr(name,"ISS") && KepCheck(line1,line2) && !ftle.eof()){
				/* We found a valid TLE! */
                                     
string ifile=amsd;
ifile+="/v5.00/ISS_tlefile.txt";
bool found=false;
fstream ifbin;
ifbin.clear();
ifbin.open(ifile.c_str());
                         if(ifbin){
	char namei[80], line1i[80], line2i[80];
                           while(!ifbin.eof()){
			namei[0]=0;
			line1i[0]=0;
			line2i[0]=0;
			ifbin.getline(namei,75);
			ifbin.getline(namei,75);
			ifbin.getline(line1i,75);
			ifbin.getline(line2i,75);
                        if(!ifbin.eof()){
                          if(!strcmp(line1,line1i) && !strcmp(line2, line2i)){
                             found=true;
                             break;
                          }
                        }
                           }
                         }
                        if(!found){
                           int kb=-1;
                           int kw=0;
                           for( int k=1;k<strlen(line1);k++){
                            bool space=line1[k]!=' ' && line1[k-1]==' ';
                            if(space)kw++;
                            if(kw==2)kb=k;
                           }
                           if(kb>0){
                             double epoch=strtof(line1+kb,NULL);
                             double jul_epoch=Julian_Date_of_Epoch(epoch);
                             double daynum=jul_epoch-2444238.5;
                             double time=(daynum+3651.0)*86400;
                             ifbin.clear();
                             ifbin.seekg(0,ios::end);
                             int suc=ISSLoad(name,line1,line2);
                             if(suc){
                             unsigned int tme=time+0.5;
                             ifbin<<tme<<endl;
                             ifbin<<name<<endl;
                             ifbin<<line1<<endl;
                             ifbin<<line2<<endl;
                             ifbin.close();
                             float r,theta,phi,v,vtheta,vphi,grmedphi;
                             ISSGTOD(&r,&theta,&phi,&v,&vtheta,&vphi,&grmedphi,time+1000);
                             cout <<" GTOD "<<r<<" "<<theta<<" "<<phi<<" "<<v<<" "<<vtheta<<" "<<vphi<<" "<<grmedphi<<endl;                             
                             
                             return 0;
                             }
                             else return 3;
                        }
                        }
                }
            }
            }
            ftle.close();
            unlink(ofile.c_str());
                   
}
else{
 cerr<<"main-E-Problem "<<wget<<endl;
 return 1;
}


return 0;
}
