// Author V. Choutko 24-may-1996
 
#include <uzstat.h>
#include <iostream.h>
#include <iomanip.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
 extern "C" float etime_(float ar[]);

AMSStat::AMSStat():AMSNodeMap(){
map(Timer);
}
AMSStat::~AMSStat(){
print();
Timer.remove();
}
void AMSStat::book(char *name, int freq){
if( add(*(Timer.add(new AMSStatNode(name))))==-1)
cerr<<" AMSStat-Book-E-Name "<<name<<" already exists"<<endl;
Timer._freq=freq;
}
void AMSStat::start(char *name){
AMSStatNode *p=(AMSStatNode*)getp(AMSID(name,0));
if(p){
p->_time=HighResTime();
p->_startstop=1;
}
else cerr<<"AMSStat-Start-E-Name "<<name<<" does not exist"<<endl;
}
number AMSStat::check(char * name){
AMSStatNode *p=(AMSStatNode*)getp(AMSID(name,0));
 if(p){
   return HighResTime()-p->_time;
 }
 else return 0;
}
number AMSStat::stop(char *name){
AMSStatNode *p=(AMSStatNode*)getp(AMSID(name,0));
number time=0;
if(p){
  if(p->_startstop==1){
    p->_entry=p->_entry+1;
    number tt=HighResTime();
    time=tt-p->_time;
    p->_time=tt;
    p->_sum=p->_sum+time;
    if(time > p->_max)p->_max=time;
    if(time < p->_min)p->_min=time;
    p->_startstop==0;
  }
  else cerr<<"AMSStat-Stop-W-NTSTRTD "<<name<<" was not started"<<endl;
}
else cerr<<"AMSStat-Stop-E-Name "<<name<<" does not exist"<<endl;
return time;
}

void AMSStat::print(){
cout <<"           Name      Entries          Min      Average          Max"
<<"          Sum"<<endl;
AMSNodeMap::print(1);
}

ostream & AMSStatNode::print(ostream &stream) const{
char *name=new char[strlen(getname())+1];
strcpy(name,getname());
if(name && strlen(name)>15)name[14]='\0';

return _entry >0 ? stream <<setw(15)<<name<<" "<<setw(12)<<_entry*_freq<<" "<<setw(12)<<_min
 <<" "<<setw(12)<<_sum/(_entry+1.e-20)<<" "<<setw(12)<<_max<<" "<<setw(12)<<_sum*_freq<<endl:stream;
delete name;
}

extern "C" number HighResTime(){

 static float ar[2];
 static number ETimeLast;
 static timeval  TPSLast;
 static struct timezone  TZ;
 static timeval TPS;
 static integer init=0;

  const number TRes=0.002;

#ifdef __ALPHA__

if(init++ ==0){
gettimeofday(&TPSLast,&TZ);
ETimeLast=etime_(ar);
}
geant time=etime_(ar);
if(time -ETimeLast  > TRes){
gettimeofday(&TPSLast,&TZ);
ETimeLast=time;

return time;
}
else {
  //
  // Try to get more high res
  //
gettimeofday(&TPS,&TZ);
number ltime=
number(TPS.tv_sec-TPSLast.tv_sec)+1.e-6*(TPS.tv_usec-TPSLast.tv_usec);
TPSLast.tv_sec=TPS.tv_sec;
TPSLast.tv_usec=TPS.tv_usec;
if(ltime<= TRes )ETimeLast+=ltime;
else ETimeLast=time;
return ETimeLast;
}

#else
return etime_(ar);
#endif

}
