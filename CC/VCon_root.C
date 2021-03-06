#include "VCon_root.h"

VCon_root::VCon_root(const char * cc){
  if(!cc) sprintf(contname,"empty");
  else
  sprintf(contname,"%s",cc);
  ev=AMSEventR::Head();
  _prevel=0;
}

VCon* VCon_root::GetCont(const char * name){
  if(
     strstr(name,"TrMCCluster")||
     strstr(name,"TrCluster")||
     strstr(name,"TrRawCluster")||
     strstr(name,"TrRecHit")||
     strstr(name,"TrTrack")||
     strstr(name,"AMSTRDRawHit")||
     strstr(name,"AMSTRDHSegment")||
     strstr(name,"AMSTRDHTrack")||
     strstr(name,"Vtx") ||
     strstr(name,"AMSTOFClusterH") ||
     strstr(name,"AMSBetaH") ||
     strstr(name,"AMSEcalH")
     ){
    sprintf(contname,"%s",name);
    return       (VCon*)(this);
    }
  else return 0;
}

TrElem* VCon_root::first(){_prevel=0;return getelem(0);}
TrElem* VCon_root::next(){return getelem(++_prevel);}

void VCon_root::removeEl(TrElem* aa, integer res)
{
  // remove next element !!!
  if(!ev)  return ;
#ifdef _PGTRACK_
  if( strstr(contname,"TrMCCluster")){
    int index=(aa)?getindex(aa)+1:0;
    vector<TrMCClusterR>::iterator it=ev->TrMCCluster().begin();
    for(int ii=0;ii<index;ii++) it++;
    ev->TrMCCluster().erase(it);
  }
  if( strstr(contname,"TrCluster")){
    int index=(aa)?getindex(aa)+1:0;
    vector<TrClusterR>::iterator it=ev->TrCluster().begin();
    for(int ii=0;ii<index;ii++) it++;
    ev->TrCluster().erase(it);
  }
  if( strstr(contname,"TrRawCluster")){
    int index=(aa)?getindex(aa)+1:0;
    vector<TrRawClusterR>::iterator it=ev->TrRawCluster().begin();
    for(int ii=0;ii<index;ii++) it++;
    ev->TrRawCluster().erase(it);
  }
  if( strstr(contname,"TrRecHit")){
    int index=(aa)?getindex(aa)+1:0;
    vector<TrRecHitR>::iterator it=ev->TrRecHit().begin();
    for(int ii=0;ii<index;ii++) it++;
    ev->TrRecHit().erase(it);
  }
  if( strstr(contname,"TrTrack")){
    int index=(aa)?getindex(aa)+1:0;
    vector<TrTrackR>::iterator it=ev->TrTrack().begin();
    for(int ii=0;ii<index;ii++) it++;
    ev->TrTrack().erase(it);
  }
  if( strstr(contname,"Vtx")){
    int index=(aa)?getindex(aa)+1:0;
    vector<VertexR>::iterator it=ev->Vertex().begin();
    for(int ii=0;ii<index;ii++) it++;
    ev->Vertex().erase(it);
  }
#endif
  if( strstr(contname,"AMSTRDHSegment")){
    int index=(aa)?getindex(aa)+1:0;
    vector<TrdHSegmentR>::iterator it=ev->TrdHSegment().begin();
    for(int ii=0;ii<index;ii++) it++;
    ev->TrdHSegment().erase(it);
  }
  if( strstr(contname,"AMSTRDHTrack")){
    int index=(aa)?getindex(aa)+1:0;
    vector<TrdHTrackR>::iterator it=ev->TrdHTrack().begin();
    for(int ii=0;ii<index;ii++) it++;
    ev->TrdHTrack().erase(it);
  }
  if( strstr(contname,"AMSTRDRawHit")){
    int index=(aa)?getindex(aa)+1:0;
    vector<TrdRawHitR>::iterator it=ev->TrdRawHit().begin();
    for(int ii=0;ii<index;ii++) it++;
    ev->TrdRawHit().erase(it);
  }
  if( strstr(contname,"AMSTOFClusterH")){
    int index=(aa)?getindex(aa)+1:0;
    vector<TofClusterHR>::iterator it=ev->TofClusterH().begin();
    for(int ii=0;ii<index;ii++) it++;
    ev->TofClusterH().erase(it);
  }
  if( strstr(contname,"AMSBetaH")){
    int index=(aa)?getindex(aa)+1:0;
    vector<BetaHR>::iterator it=ev->BetaH().begin();
    for(int ii=0;ii<index;ii++) it++;
    ev->BetaH().erase(it);
  }
  if( strstr(contname,"AMSEcalH")){
    int index=(aa)?getindex(aa)+1:0;
    vector<EcalHR>::iterator it=ev->EcalH().begin();
    for(int ii=0;ii<index;ii++) it++;
    ev->EcalH().erase(it);
  }

}


int VCon_root::getnelem(){
  if(!ev)  return -1;
#ifdef _PGTRACK_
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
  if( strstr(contname,"Vtx"))
    return ev->NVertex();
#endif
  if( strstr(contname,"AMSTRDHSegment"))
    return ev->NTrdHSegment();
  if( strstr(contname,"AMSTRDHTrack"))
    return ev->NTrdHTrack();
  if( strstr(contname,"AMSTRDRawHit"))
    return ev->NTrdRawHit();
  if( strstr(contname,"AMSTOFClusterH"))
    return ev->NTofClusterH();
  if( strstr(contname,"AMSBetaH"))
    return ev->NBetaH();
  if( strstr(contname,"AMSEcalH"))
    return ev->NEcalH();

  return 0;
}

void VCon_root::eraseC(){
 if(!ev)  return ;
#ifdef _PGTRACK_
  if( strstr(contname,"TrMCCluster")) {
    ev->fHeader.TrMCClusters = 0;
    ev->TrMCCluster().clear();
  }
  if( strstr(contname,"TrCluster")) {
    ev->fHeader.TrClusters = 0;
    ev->TrCluster().clear();
  } 
  if( strstr(contname,"TrRawCluster")) {
    ev->fHeader.TrRawClusters = 0;
    ev->TrRawCluster().clear();
  }
  if( strstr(contname,"TrRecHit")) {
    ev->fHeader.TrRecHits = 0;
    ev->TrRecHit().clear();
  }
  if( strstr(contname,"TrTrack")) {
    ev->fHeader.TrTracks = 0;
    ev->TrTrack().clear();
  }
  if( strstr(contname,"Vtx")) {
    ev->fHeader.Vertexs = 0;
    ev->Vertex().clear();
  }
#endif
  if( strstr(contname,"AMSTRDHSegment")) {
    ev->fHeader.TrdHSegments = 0;
    ev->TrdHSegment().clear();
  }
  if( strstr(contname,"AMSTRDHTrack")) {
    ev->fHeader.TrdHTracks = 0;
    ev->TrdHTrack().clear();
  }
  if( strstr(contname,"AMSTRDRawHit")) {
    ev->fHeader.TrdRawHits = 0;
    ev->TrdRawHit().clear();
  }
  if( strstr(contname,"AMSTOFClusterH")) {
    ev->fHeader.TofClusterHs = 0;
    ev->TofClusterH().clear();
  }
  if( strstr(contname,"AMSBetaH")) {
    ev->fHeader.BetaHs = 0;
    for(unsigned int ii=0;ii<ev->NCharge();ii++)  {ev->pCharge(ii)->setBetaH(-1);}
    for(unsigned int ii=0;ii<ev->NParticle();ii++){ev->pParticle(ii)->setBetaH(-1);}
    ev->BetaH().clear();
    ev->TofChargeH().clear();
  }
  if( strstr(contname,"AMSEcalH")) {
    ev->fHeader.EcalHs = 0;
    ev->EcalH().clear();
  }
}

TrElem* VCon_root::getelem(int ii){
 if(!ev)  return 0;
#ifdef _PGTRACK_
  if( strstr(contname,"TrMCCluster"))
    return (TrElem*) ev->pTrMCCluster(ii);
  if( strstr(contname,"TrCluster"))
    return  (TrElem*) ev->pTrCluster(ii);
  if( strstr(contname,"TrRawCluster"))
    return  (TrElem*) ev->pTrRawCluster(ii);
  if( strstr(contname,"TrRecHit"))
    return  (TrElem*) ev->pTrRecHit(ii);
  if( strstr(contname,"TrTrack"))
    return  (TrElem*) ev->pTrTrack(ii);
  if( strstr(contname,"Vtx"))
    return  (TrElem*) ev->pVertex(ii);
#endif
  if( strstr(contname,"AMSTRDRawHit"))
    return  (TrElem*) ev->pTrdRawHit(ii);
  if( strstr(contname,"AMSTRDHSegment"))
    return  (TrElem*) ev->pTrdHSegment(ii);
  if( strstr(contname,"AMSTRDHTrack"))
    return  (TrElem*) ev->pTrdHTrack(ii);

  if( strstr(contname,"AMSTOFClusterH"))
    return  (TrElem*) ev->pTofClusterH(ii);
  if( strstr(contname,"AMSBetaH"))
    return  (TrElem*) ev->pBetaH(ii);
  if( strstr(contname,"AMSEcalH"))
    return  (TrElem*) ev->pEcalH(ii);
 
  return 0;
}

void  VCon_root::addnext(TrElem* aa){
   if(!ev)  return ;
#ifdef _PGTRACK_
  if( strstr(contname,"TrMCCluster")) {
    ev->TrMCCluster().push_back(*(TrMCClusterR*)aa);
    delete (TrMCClusterR*)aa;
  }
  if( strstr(contname,"TrCluster")) {
    ev->TrCluster().push_back(*(TrClusterR*)aa);
    delete (TrClusterR*)aa;
  }
  if( strstr(contname,"TrRawCluster")) {
    ev->TrRawCluster().push_back(*(TrRawClusterR*)aa);
    delete (TrRawClusterR*)aa;
  }
  if( strstr(contname,"TrRecHit")) {
    ev->TrRecHit().push_back(*(TrRecHitR*)aa);
    delete (TrRecHitR*)aa;
  }
  if( strstr(contname,"TrTrack")) {
    ev->TrTrack().push_back(*(TrTrackR*)aa);
    delete (TrTrackR*)aa;
  }
  if( strstr(contname,"Vtx")) {
    ev->Vertex().push_back(*(VertexR*)aa);
    delete (VertexR*)aa;
  }
#endif
  if( strstr(contname,"AMSTRDHSegment"))
    ev->TrdHSegment().push_back(*(TrdHSegmentR*)aa);
  if( strstr(contname,"AMSTRDHTrack"))
    ev->TrdHTrack().push_back(*(TrdHTrackR*)aa);
  if( strstr(contname,"AMSTRDRawHit"))
    ev->TrdRawHit().push_back(*(TrdRawHitR*)aa);

  if( strstr(contname,"AMSTOFClusterH")){
    ev->TofClusterH().push_back(*(TofClusterHR*)aa);
    ev->fHeader.TofClusterHs++;
    delete (TofClusterHR*)aa;
  }
  if( strstr(contname,"AMSBetaH")){
    ev->BetaH().push_back(*(BetaHR*)aa);
    ev->fHeader.BetaHs++;
    delete (BetaHR*)aa;
  }
  if( strstr(contname,"AMSEcalH")){
    ev->EcalH().push_back(*(EcalHR*)aa);
    ev->fHeader.EcalHs++;
    delete (EcalHR*)aa;
  }
}


int  VCon_root::getindex(TrElem* aa){
 if(!ev)  return 0;
#ifdef _PGTRACK_
 if( strstr(contname,"TrMCCluster"))
   for(unsigned int ii=0;ii<ev->NTrMCCluster();ii++)
     if(ev->pTrMCCluster(ii)==aa) return ii;
  if( strstr(contname,"TrCluster"))
    for(unsigned int ii=0;ii<ev->NTrCluster();ii++)
      if(ev->pTrCluster(ii)==aa) return ii;
  if( strstr(contname,"TrRawCluster"))
   for(unsigned int ii=0;ii<ev->NTrRawCluster();ii++)
     if(ev->pTrRawCluster(ii)==aa) return ii;
  if( strstr(contname,"TrRecHit"))
   for(unsigned int ii=0;ii<ev->NTrRecHit();ii++)
     if(ev->pTrRecHit(ii)==aa) return ii;
  if( strstr(contname,"TrTrack"))
   for(unsigned int ii=0;ii<ev->NTrTrack();ii++)
     if(ev->pTrTrack(ii)==aa) return ii;
  if( strstr(contname,"Vtx"))
   for(unsigned int ii=0;ii<ev->NVertex();ii++)
     if(ev->pVertex(ii)==aa) return ii;
#endif
  if( strstr(contname,"AMSTRDHSegment"))
   for(unsigned int ii=0;ii<ev->NTrdHSegment();ii++)
     if(ev->pTrdHSegment(ii)==aa) return ii;
  if( strstr(contname,"AMSTRDHTrack"))
   for(unsigned int ii=0;ii<ev->NTrdHTrack();ii++)
     if(ev->pTrdHTrack(ii)==aa) return ii;
  if( strstr(contname,"AMSTRDRawHit"))
   for(unsigned int ii=0;ii<ev->NTrdRawHit();ii++)
     if(ev->pTrdRawHit(ii)==aa) return ii;

  if( strstr(contname,"AMSTOFClusterH"))
   for(unsigned int ii=0;ii<ev->NTofClusterH();ii++)
     if(ev->pTofClusterH(ii)==aa) return ii;
  if( strstr(contname,"AMSBetaH"))
    for(unsigned int ii=0;ii<ev->NBetaH();ii++)
      if(ev->pBetaH(ii)==aa) return ii;
  if( strstr(contname,"AMSEcalH"))
    for(unsigned int ii=0;ii<ev->NEcalH();ii++)
      if(ev->pEcalH(ii)==aa) return ii;

  return 0;
}


void  VCon_root::exchangeEl(TrElem* el1, TrElem* el2) {
  int i1=-1;
  int i2=-1;
  if (!ev) return;
  if ( (el1==el2)||(!el1)||(!el2) ) return;
#ifdef _PGTRACK_
  if (strstr(contname,"TrMCCluster")) {
    for(unsigned int ii=0;ii<ev->NTrMCCluster();ii++) {
      if (ev->pTrMCCluster(ii)==el1) i1=ii;
      if (ev->pTrMCCluster(ii)==el2) i2=ii;
    }
    if ( (i1<0)||(i2<0) ) return;
    swap(ev->TrMCCluster().at(i1),ev->TrMCCluster().at(i2));
  }
  if (strstr(contname,"TrCluster")) {
    for(unsigned int ii=0;ii<ev->NTrCluster();ii++) {
      if (ev->pTrCluster(ii)==el1) i1=ii;
      if (ev->pTrCluster(ii)==el2) i2=ii;
    }
    if ( (i1<0)||(i2<0) ) return;
    swap(ev->TrCluster().at(i1),ev->TrCluster().at(i2));
  }
  if (strstr(contname,"TrRawCluster")) {
    for(unsigned int ii=0;ii<ev->NTrRawCluster();ii++) {
      if (ev->pTrRawCluster(ii)==el1) i1=ii;
      if (ev->pTrRawCluster(ii)==el2) i2=ii;
    }
    if ( (i1<0)||(i2<0) ) return;
    swap(ev->TrRawCluster().at(i1),ev->TrRawCluster().at(i2));
  }
  if (strstr(contname,"TrRecHit")) {
    for(unsigned int ii=0;ii<ev->NTrRecHit();ii++) {
      if (ev->pTrRecHit(ii)==el1) i1=ii;
      if (ev->pTrRecHit(ii)==el2) i2=ii;
    }
    if ( (i1<0)||(i2<0) ) return;
    swap(ev->TrRecHit().at(i1),ev->TrRecHit().at(i2));
  }
  if (strstr(contname,"TrTrack")) {
    for(unsigned int ii=0;ii<ev->NTrTrack();ii++) {
      if (ev->pTrTrack(ii)==el1) i1=ii;
      if (ev->pTrTrack(ii)==el2) i2=ii;
    }
    if ( (i1<0)||(i2<0) ) return;
    swap(ev->TrTrack().at(i1),ev->TrTrack().at(i2));
  }
  if (strstr(contname,"Vtx")) {
    for(unsigned int ii=0;ii<ev->NVertex();ii++) {
      if (ev->pVertex(ii)==el1) i1=ii;
      if (ev->pVertex(ii)==el2) i2=ii;
    }
    if ( (i1<0)||(i2<0) ) return;
    swap(ev->Vertex().at(i1),ev->Vertex().at(i2));
  }
#endif
  if (strstr(contname,"AMSTRDHSegment")) {
    for(unsigned int ii=0;ii<ev->NTrdHSegment();ii++) {
      if (ev->pTrdHSegment(ii)==el1) i1=ii;
      if (ev->pTrdHSegment(ii)==el2) i2=ii;
    }
    if ( (i1<0)||(i2<0) ) return;
    swap(ev->TrdHSegment().at(i1),ev->TrdHSegment().at(i2));
  }
  if (strstr(contname,"AMSTRDHTrack")) {
    for(unsigned int ii=0;ii<ev->NTrdHTrack();ii++) {
      if (ev->pTrdHTrack(ii)==el1) i1=ii;
      if (ev->pTrdHTrack(ii)==el2) i2=ii;
    }
    if ( (i1<0)||(i2<0) ) return;
    swap(ev->TrdHTrack().at(i1),ev->TrdHTrack().at(i2));
  }
  if (strstr(contname,"AMSTRDRawHit")) {
    for(unsigned int ii=0;ii<ev->NTrdRawHit();ii++) {
      if (ev->pTrdRawHit(ii)==el1) i1=ii;
      if (ev->pTrdRawHit(ii)==el2) i2=ii;
    }
    if ( (i1<0)||(i2<0) ) return;
    swap(ev->TrdRawHit().at(i1),ev->TrdRawHit().at(i2));
  }

  if (strstr(contname,"AMSTOFClusterH")) {
    for(unsigned int ii=0;ii<ev->NTofClusterH();ii++) {
      if (ev->pTofClusterH(ii)==el1) i1=ii;
      if (ev->pTofClusterH(ii)==el2) i2=ii;
    }
    if ( (i1<0)||(i2<0) ) return;
    swap(ev->TofClusterH().at(i1),ev->TofClusterH().at(i2));
  }
  if (strstr(contname,"AMSBetaH")) {
    for(unsigned int ii=0;ii<ev->NBetaH();ii++) {
      if (ev->pBetaH(ii)==el1) i1=ii;
      if (ev->pBetaH(ii)==el2) i2=ii;
    }
    if ( (i1<0)||(i2<0) ) return;
    swap(ev->BetaH().at(i1),ev->BetaH().at(i2));
  }
  if (strstr(contname,"AMSEcalH")) {
    for(unsigned int ii=0;ii<ev->NEcalH();ii++) {
      if (ev->pEcalH(ii)==el1) i1=ii;
      if (ev->pEcalH(ii)==el2) i2=ii;
    }
    if ( (i1<0)||(i2<0) ) return;
    swap(ev->EcalH().at(i1),ev->EcalH().at(i2));
  }


}

