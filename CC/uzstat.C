// Author V. Choutko 24-may-1996
 
#include <uzstat.h>
#include <iostream.h>
#include <iomanip.h>
#include <string.h>
AMSStat::AMSStat():AMSNodeMap(){
map(Timer);
}
AMSStat::~AMSStat(){
Timer.remove();
}
void AMSStat::book(char *name){
if( add(*(Timer.add(new AMSStatNode(name))))==-1)
cerr<<" AMSStat-Book-E-Name "<<name<<" already exists"<<endl;
}
void AMSStat::start(char *name){
AMSStatNode *p=(AMSStatNode*)getp(AMSID(name,0));
if(p){
geant tt;
TIMEX(tt);
p->_time=tt;
p->_startstop=1;
}
else cerr<<"AMSStat-Start-E-Name "<<name<<" does not exist"<<endl;
}
void AMSStat::stop(char *name){
AMSStatNode *p=(AMSStatNode*)getp(AMSID(name,0));
if(p){
  if(p->_startstop==1){
    p->_entry=p->_entry+1;
    geant tt;
    TIMEX(tt);
    number time=tt-p->_time;
    p->_time=tt;
    p->_sum=p->_sum+time;
    if(time > p->_max)p->_max=time;
    if(time < p->_min)p->_min=time;
    p->_startstop==0;
  }
  else cerr<<"AMSStat-Stop-W-NTSTRTD "<<name<<" was not started"<<endl;
}
else cerr<<"AMSStat-Stop-E-Name "<<name<<" does not exist"<<endl;
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
return stream <<setw(15)<<name<<" "<<setw(12)<<_entry<<" "<<setw(12)<<_min
<<" "<<setw(12)<<_sum/(_entry+1.e-20)<<" "<<setw(12)<<_max<<" "<<setw(12)<<_sum<<endl;
delete name;
}
