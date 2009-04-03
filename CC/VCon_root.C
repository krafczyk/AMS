#include "VCon_root.h"


VCon_root::VCon_root(char * cc){
  if(!cc) sprintf(contname,"empty");
  else
  sprintf(contname,"%s",cc);
  ev=AMSEventR::Head();
}

VCon* VCon_root::GetCont(char * name){
  if(
     strstr(name,"TrMCCluster")||
     strstr(name,"TrCluster")||
     strstr(name,"TrRawCluster")||
     strstr(name,"TrRecHit")||
     strstr(name,"TrTrack")
     )
    return (VCon*)(new VCon_root(name));
  else return 0;

}

void VCon_root::removeEl(AMSlink* aa, integer res)
{
  // remove next element !!!
  if(!ev)  return ;
  if( strstr(contname,"TrMCCluster")){
    int index=getindex(aa);
    vector<AMSTrMCCluster>::iterator it=ev->TrMCCluster().begin();
    for(int ii=0;ii<index;ii) it++;
    ev->TrMCCluster().erase(it);
  }
  if( strstr(contname,"TrCluster")){
    int index=getindex(aa);
    vector<AMSTrCluster>::iterator it=ev->TrCluster().begin();
    for(int ii=0;ii<index;ii) it++;
    ev->TrCluster().erase(it);
  }
   
  if( strstr(contname,"TrRawCluster")){
    int index=getindex(aa);
    vector<AMSTrRawCluster>::iterator it=ev->TrRawCluster().begin();
    for(int ii=0;ii<index;ii) it++;
    ev->TrRawCluster().erase(it);
  }

  
  if( strstr(contname,"TrRecHit")){
    int index=getindex(aa);
    vector<AMSTrRecHit>::iterator it=ev->TrRecHit().begin();
    for(int ii=0;ii<index;ii) it++;
    ev->TrRecHit().erase(it);
  }
   
  if( strstr(contname,"TrTrack")){
    int index=getindex(aa);
    vector<AMSTrTrack>::iterator it=ev->TrTrack().begin();
    for(int ii=0;ii<index;ii) it++;
    ev->TrTrack().erase(it);
  }
   
  
}


int VCon_root::getnelem(){
  if(!ev)  return -1;
  if( strstr(contname,"TrMCCluster"))
    return ev->NTrMCCluster();
  if( strstr(contname,"TrCluster"))
    return ev->NTrCluster();
  if( strstr(contname,"TrRawCluster"))
    return ev->NTrRawCluster();
  if( strstr(contname,"TrRecHit"))
    return ev->NTrRecHit();
  if( strstr(contname,"TrTrack"))
    return ev->NTrTrack();
}

void VCon_root::eraseC(){
 if(!ev)  return ;
  if( strstr(contname,"TrMCCluster"))
    return ev->TrMCCluster().clear();
  if( strstr(contname,"TrCluster"))
    return ev->TrCluster().clear();
  if( strstr(contname,"TrRawCluster"))
    return ev->TrRawCluster().clear();
  if( strstr(contname,"TrRecHit"))
    return ev->TrRecHit().clear();
  if( strstr(contname,"TrTrack"))
    return ev->TrTrack().clear();
}

AMSlink* VCon_root::getelem(int ii){
 if(!ev)  return 0;
  if( strstr(contname,"TrMCCluster"))
    return (AMSlink*) ev->pTrMCCluster(ii);
  if( strstr(contname,"TrCluster"))
    return  (AMSlink*) ev->pTrCluster(ii);
  if( strstr(contname,"TrRawCluster"))
    return  (AMSlink*) ev->pTrRawCluster(ii);
  if( strstr(contname,"TrRecHit"))
    return  (AMSlink*) ev->pTrRecHit(ii);
  if( strstr(contname,"TrTrack"))
    return  (AMSlink*) ev->pTrTrack(ii);
}

void  VCon_root::addnext(AMSlink* aa){
   if(!ev)  return ;
  if( strstr(contname,"TrMCCluster"))
    ev->TrMCCluster().push_back(*(TrMCClusterR*)aa);
  if( strstr(contname,"TrCluster"))
    ev->TrCluster().push_back(*(TrClusterR*)aa);
  if( strstr(contname,"TrRawCluster"))
    ev->TrRawCluster().push_back(*(TrRawClusterR*)aa);
  if( strstr(contname,"TrRecHit"))
    ev->TrRecHit().push_back(*(TrRecHitR*)aa);
  if( strstr(contname,"TrTrack"))
    ev->TrTrack().push_back(*(TrTrackR*)aa);
}


int  VCon_root::getindex(AMSlink* aa){
 if(!ev)  return 0;
 if( strstr(contname,"TrMCCluster"))
   for(int ii=0;ii<ev->NTrMCCluster();ii++)
     if(ev->pTrMCCluster(ii)==aa) return ii;
   
 
  if( strstr(contname,"TrCluster"))
    for(int ii=0;ii<ev->NTrCluster();ii++)
      if(ev->pTrCluster(ii)==aa) return ii;
  
  if( strstr(contname,"TrRawCluster"))
   for(int ii=0;ii<ev->NTrRawCluster();ii++)
     if(ev->pTrRawCluster(ii)==aa) return ii;

  if( strstr(contname,"TrRecHit"))
   for(int ii=0;ii<ev->NTrRecHit();ii++)
     if(ev->pTrRecHit(ii)==aa) return ii;

  if( strstr(contname,"TrTrack"))
   for(int ii=0;ii<ev->NTrTrack();ii++)
     if(ev->pTrTrack(ii)==aa) return ii;

}
