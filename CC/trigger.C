#include <trigger.h>
#include <event.h>
#include <mccluster.h>
void Trigger::build(){
  // Trigger mode 1 : TRIGFKEY.ntof out of 4 TOF layers fired + Anti < TRIGFFKEY.anti MeV
  int i;
  integer tof=0;
  for(i=0;i<4;i++){
    if((AMSEvent::gethead()->getC("AMSTOFCluster",i))->getnelem() )tof++;
  }

#if 1
  // change to 0 when xxx writes a proper code
  AMSAntiMCCluster* pcl=(AMSAntiMCCluster*)AMSEvent::gethead()->
       getheadC("AMSAntiCluster",0);
  number sumanti=0;
  while(pcl){
   sumanti+=pcl->getedep()*1000;
   pcl=pcl->next();
  }
#else
          // xxx code here
#endif
  if (tof >=TRIGFFKEY.ntof && sumanti < TRIGFFKEY.anti)    // Hardwired here
AMSEvent::gethead()->addnext(AMSID("Trigger",0),new Trigger(1));
 

}
