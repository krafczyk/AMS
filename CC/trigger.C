#include <trigger.h>
#include <event.h>
#include <mccluster.h>
void Trigger::build(){
  // Trigger mode 1 : 3 out of 4 TOF layers fired + Anti < 0.2 MeV
  int i;
  integer tof=0;
  for(i=0;i<4;i++){
    if((AMSEvent::gethead()->getC("AMSTOFCluster",i))->getnelem() )tof++;
  }

#if 1
  // change to 0 when xxx writes a proper code
  AMSAntiMCCluster* pcl=(AMSAntiMCCluster*)AMSEvent::gethead()->
       getheadC("AMSAntiMCCluster",0);
  number sumanti=0;
  while(pcl){
   sumanti+=pcl->edep*1000;
   pcl=pcl->next();
  }
#else
          // xxx code here
#endif
  if (tof >=3 && sumanti < 0.2)    // Hardwired here
AMSEvent::gethead()->addnext(AMSID("Trigger",0),new Trigger(1));
 

}
