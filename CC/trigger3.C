#include <trigger1.h>
#include <trigger3.h>
#include <event.h>
#include <mccluster.h>
#include <amsdbc.h>
#include <tofrec.h>
#include <antirec.h>
#include <trrawcluster.h>
#include <ntuple.h>

void TriggerAuxLVL3::fill(integer crate){


  //
  // fillTracker
  //

{

  if(!AMSJob::gethead()->isReconstruction()){
  // AddNoise first ( a bit conservative)
  int i,j;
  integer ierr=0;
  integer nn=0;
  int16u drp,strip,va,side;
  geant d=0;
  for(i=0;i<2;i++){
   geant xn=LVL3SIMFFKEY.NoiseProb[i]*NTRHDRP/2*TKDBc::NStripsDrp(1,i);
   POISSN(xn,nn,ierr);
   for(j=0;j<nn;j++){
    side=i;
    drp=RNDM(d)*NTRHDRP;
    if(drp >= NTRHDRP)drp=NTRHDRP-1;
    strip=RNDM(d)*TKDBc::NStripsDrp(1,i);
    if(strip >= TKDBc::NStripsDrp(1,i))strip=TKDBc::NStripsDrp(1,i)-1;
    AMSTrIdSoft idd(crate,drp,i,strip);
    if(!idd.dead()){ 
     if(maxtr-_ltr > 4){
      _ptr[_ltr]=1;   // length + max strp
      _ptr[_ltr+1]=idd.gethaddr();
      _ptr[_ltr+2]=1000;
      _ptr[_ltr+3]=1000;
      _ltr+=4;
     }    
     else return;
    }
    }  
  }
 }  
   AMSTrRawCluster *ptr=(AMSTrRawCluster*)AMSEvent::gethead()->
   getheadC("AMSTrRawCluster",crate);
   while (ptr){
    _ltr+=ptr->lvl3format(_ptr+_ltr,maxtr-_ltr);
    ptr=ptr->next();
   }

}
}

TriggerAuxLVL3::TriggerAuxLVL3(){
_ltr=0;
_ctr=0;
_ptr[0]=0;

}


int16 *  TriggerAuxLVL3::readtracker(integer begin){
if(begin)_ctr=0;
else if (_ctr < _ltr)_ctr=_ctr+(((_ptr[_ctr])&63)+3);
return _ctr < _ltr ? _ptr+_ctr : 0;
}

  TriggerExpertLVL3 * TriggerExpertLVL3::pExpert=0;
  integer TriggerLVL3::_TOFPattern[SCMXBR][SCMXBR];
  integer TriggerLVL3::_TOFStatus[SCLRS][SCMXBR];
  integer TriggerLVL3::_TOFOr[SCLRS][SCMXBR];
  integer TriggerLVL3::_TrackerStatus[NTRHDRP2];
  integer TriggerLVL3::_TrackerAux[NTRHDRP][ncrt];
  integer TriggerLVL3::_TOFAux[SCLRS][SCMXBR];
  integer TriggerLVL3::_NTOF[SCLRS];
  geant TriggerLVL3::_TOFCoo[SCLRS][SCMXBR][3];
  geant TriggerLVL3::_TrackerCoo[NTRHDRP][ncrt][3];
 geant TriggerLVL3::_TrackerDir[NTRHDRP][ncrt];
  geant TriggerLVL3::_TrackerCooZ[maxlay];
  integer TriggerLVL3::_TrackerDRP2Layer[NTRHDRP][ncrt];
  number TriggerLVL3::_stripsize=0.0055;
integer TriggerLVL3::_TrackerOtherTDR[NTRHDRP][ncrt];



TriggerLVL3::TriggerLVL3():_AntiTrigger(0),_TOFTrigger(0),
_TrackerTrigger(0),_NPatFound(0),_Time(0),_NTrHits(0),_TrEnergyLoss(0){
  
  // Ctor

_Residual[0]=0;
_Residual[1]=0;
_Pattern[0]=-1;
_Pattern[1]=-1;
VZERO(_NTOF,SCLRS);
VZERO(_TrackerAux,AMSTrIdSoft::ndrp());
VZERO(_nhits,sizeof(_nhits)/sizeof(integer));

}


 TriggerLVL3::TriggerLVL3(integer tra, integer tof, integer anti,integer ntr, integer npat,
                          integer pat[], number res[], number time, number eloss):_AntiTrigger(anti),
                          _TOFTrigger(tof),_TrackerTrigger(tra),_NPatFound(npat),_Time(time),_TrEnergyLoss(eloss), _NTrHits(ntr){
  
  // Ctor

_Residual[0]=res[0];
_Residual[1]=res[1];
_Pattern[0]=pat[0];
_Pattern[1]=pat[1];
VZERO(_NTOF,SCLRS);
VZERO(_TrackerAux,AMSTrIdSoft::ndrp());
VZERO(_nhits,sizeof(_nhits)/sizeof(integer));

}





void TriggerLVL3::init(){
 int i,j;

  // TOF

    for(i=0;i<SCMXBR;i++){
      for(j=0;j<SCLRS;j++)_TOFStatus[j][i]=0;
    }

  if(LVL3FFKEY.UseTightTOF){
    char fnam[256]="";
    strcpy(fnam,AMSDATADIR.amsdatadir);
    strcat(fnam,"TOFLVL3.");
    strcat(fnam,AMSJob::gethead()->getsetup());
    ifstream iftxt(fnam,ios::in);
    if(!iftxt){
     cerr <<"TriggerLVL3::init-F-Error open file "<<fnam<<endl;
     exit(1);
    }
    for(i=0;i<SCMXBR;i++){
      for(j=0;j<SCMXBR;j++)iftxt>>_TOFPattern[j][i];
    }
    for(i=0;i<SCLRS;i++){
      for(j=0;j<SCMXBR;j++)iftxt>>_TOFOr[i][j];
    }
    for(i=0;i<SCLRS;i++){
      for(j=0;j<SCMXBR;j++)iftxt>>_TOFStatus[i][j];
    }
    if(iftxt.eof() ){
      cerr<< "TriggerLVL3::init-F-Unexpected EOF"<<endl;
      exit(1);
    }
  }
  else{
    for(i=0;i<SCMXBR;i++){
      for (j=0;j<SCMXBR;j++)_TOFPattern[i][j]=1;
      for(j=0;j<SCLRS;j++)_TOFOr[j][i]=1;
    }
  }

    for(i=0;i<SCMXBR;i++){
      for(j=0;j<SCLRS;j++){
        AMSgvolume *ptr=AMSJob::gethead()->getgeomvolume(AMSID("TOFS",100*(j+1)+i+1));
        if( ptr){
         AMSPoint loc(0,0,0);
         AMSPoint global=ptr->loc2gl(loc);
         for(int k=0;k<3;k++)_TOFCoo[j][i][k]=global[k];         
        }
        else cerr <<"TriggerLVL3-Init-S-TOFVolumeNotFound"<<AMSID("TOFS",100*(j+1)+i+1);
      }
    }  
   
  
   

  // Tracker

    for(i=0;i<AMSTrIdSoft::ndrp();i++){
     _TrackerStatus[i]=0;
    }


    for (i=0;i<TKDBc::nlay();i++){
      _TrackerCooZ[i]=0;
      int nsf=0;
      for(int j=0;j<TKDBc::nlad(i+1);j++){
        for(int k=0;k<TKDBc::nsen(i+1,j+1);k++){
         AMSTrIdGeom id(i+1,j+1,k+1,0,0);
         AMSgvolume *ptr=AMSJob::gethead()->getgeomvolume(id.crgid());
         if( ptr){
          AMSPoint loc(0,0,0);
          AMSPoint global=ptr->loc2gl(loc);
          _TrackerCooZ[i]+=global[2];
          nsf++;         
         }
        }
      }
      if(nsf==0){
        cerr <<"Trigger3::Init-S-no sensors found for layer "<<i+1<<endl;
        //exit(1);
      }
      else{
          _TrackerCooZ[i]=_TrackerCooZ[i]/nsf;
#ifdef __AMSDEBUG__
          cout <<"Trigger3::Init-I-"<<nsf<<" sensors found for layer "<<i+1<<endl;
#endif
      }
    }
    for(i=0;i<ntdr;i++){
       for( int k=0;k<AMSTrIdSoft::ncrates();k++){
        AMSTrIdSoft id(k,i,1,0);
        if(!id.dead()){         
             _TrackerCoo[i][k][0]=0;         
             _TrackerCoo[i][k][1]=0;
             _TrackerCoo[i][k][2]=0;
             _TrackerDir[i][k]=0;
              int nfs=0;
              int half=id.gethalf(); 
              int a= half==0?0:TKDBc::nhalf(id.getlayer(),id.getdrp());
              int b= half==0?TKDBc::nhalf(id.getlayer(),id.getdrp()):
                          TKDBc::nsen(id.getlayer(),id.getdrp());
/*
              if(a>=b){
               // warning force dead
               cout<<"TriggerLVL3::Init-W-DummyLadderFound "<<id.getdrp()<<" "<<id.getlayer()<<endl;
               id.kill();
               _TrackerCoo[i][k][0]=0;
               _TrackerCoo[i][k][1]=0;
               _TrackerCoo[i][k][2]=0;
               _TrackerDir[i][k]=0;
               break;
              }
*/
              for(int l=a;l<b;l++){
              AMSTrIdGeom idg(id.getlayer(),id.getdrp(),l+1,0,0); 
              AMSgvolume *ptr=AMSJob::gethead()->getgeomvolume(idg.crgid());
              if( ptr){
                AMSPoint loc(0,-ptr->getpar(1),0);
                AMSPoint global=ptr->loc2gl(loc);
                _TrackerCoo[i][k][0]+=global[0];
                _TrackerCoo[i][k][1]+=global[1];
                _TrackerCoo[i][k][2]+=global[2];
                _TrackerDir[i][k]+=ptr->getnrmA(1,1);
                nfs++;
              }
          }
          if(nfs==0){
             cerr <<"Trigger3::Init-S-no sensors found for DRP "<<i+1<<" crate "<<k<<" "<<id.getlayer()<<" "<<id.getdrp()<<endl;
             //exit(1);
          }
          else{
#ifdef __AMSDEBUG__
          cout <<"Trigger3::Init-I-"<<nfs<<" sensors found for DRP "<<i+1<<" crate "<<k<<" "<<id.getlayer()<<" "<<id.getdrp()<<endl;
#endif
          _TrackerCoo[i][k][0]=_TrackerCoo[i][k][0]/nfs;
          _TrackerCoo[i][k][1]=_TrackerCoo[i][k][1]/nfs;
          _TrackerCoo[i][k][2]=_TrackerCoo[i][k][2]/nfs;
          _TrackerDir[i][k]=_TrackerDir[i][k]/nfs;
          }
        }
        else {
          _TrackerCoo[i][k][0]=0;
          _TrackerCoo[i][k][1]=0;
          _TrackerCoo[i][k][2]=0;
          _TrackerDir[i][k]=0;

        }
       }
    }
    int k;
    for (i=0;i<ntdr;i++){
      for(k=0;k<AMSTrIdSoft::ncrates();k++){
       AMSTrIdSoft id(k,i,1,0);
       if(!id.dead())_TrackerDRP2Layer[i][k]=id.getlayer()-1;
       else _TrackerDRP2Layer[i][k]=-1;
#ifdef __AMSDEBUG__
       cout <<"_TrackerDRP2Layer[i][k] "<<i<<" "<<k<<" "<<_TrackerDRP2Layer[i][k]<<endl;
#endif      
      } 
   }

    for (i=0;i<NTRHDRP;i++){
      for(k=0;k<AMSTrIdSoft::ncrates();k++){
       AMSTrIdSoft idx(k,i,0,0);
       if(!idx.dead()){
         AMSTrIdSoft idy(idx.getlayer(),idx.getdrp(),idx.gethalf(),1,0);
         _TrackerOtherTDR[i][k]=idy.gettdr();
      }
       else _TrackerOtherTDR[i][k]=-1;
      } 
   }

    // Now print it in the some file

#ifdef __AMSDEBUG__
    {

    char fnam[256]="lvl3.constants";
    ofstream oftxt(fnam,ios::out);
    if(oftxt){
      oftxt<<endl;
      oftxt << "_TOFPattern[i][14]"<<endl;
      int i,j,k;
      for(i=0;i<SCMXBR;i++){
        oftxt <<"i "<<i<<" ";
        for(j=0;j<SCMXBR;j++)oftxt <<_TOFPattern[i][j]<<" ";
        oftxt <<endl;
      }    
      oftxt<<endl;
      oftxt<<endl;
      oftxt << "_TOFOr[i][14]"<<endl;
      for(i=0;i<SCLRS;i++){
        oftxt <<"i "<<i<<" ";
        for(j=0;j<SCMXBR;j++)oftxt <<_TOFOr[i][j]<<" ";
        oftxt <<endl;
      }    
      oftxt<<endl;
      oftxt << "_TOFStatus[i][14]"<<endl;
      for(i=0;i<4;i++){
        oftxt <<"i "<<i<<" ";
        for(j=0;j<SCMXBR;j++)oftxt <<_TOFStatus[i][j]<<" ";
        oftxt <<endl;
      }    
      oftxt<<endl;
      oftxt << "_TOFCoo[i][j][3]"<<endl;
      for(i=0;i<4;i++){
        oftxt <<"i "<<i<<endl;
        for(j=0;j<SCMXBR;j++){
         oftxt <<"j "<<j<<" ";
         for(k=0;k<3;k++)oftxt <<_TOFCoo[i][j][k]<<" ";
         oftxt <<endl;
        }
        oftxt <<endl;
      }  
    

      oftxt << "_Trackerstatus[64]"<<endl;
      for(i=0;i<AMSTrIdSoft::ndrp();i++){
       oftxt<<_TrackerStatus[i]<<" ";
      }
      oftxt<<endl;
      oftxt<<endl;

      oftxt << "_TrackerCooZ[nl]"<<endl;
      for(i=0;i<TKDBc::nlay();i++){
       oftxt<<_TrackerCooZ[i]<<" ";
      }
      oftxt<<endl;
      oftxt<<endl;

      oftxt << "_TrackerCoo[i][j][3]"<<endl;
      for(i=0;i<NTRHDRP;i++){
        oftxt <<"i "<<i<<endl;
        for(j=0;j<AMSTrIdSoft::ncrates();j++){
         oftxt <<"j "<<j<<" ";
         for(k=0;k<3;k++)oftxt <<_TrackerCoo[i][j][k]<<" ";
         oftxt <<endl;
        }
        oftxt <<endl;
      }  
      oftxt<<endl;

      oftxt << "_TrackerDir[i][2]"<<endl;
      for(i=0;i<NTRHDRP;i++){
        oftxt <<"i "<<i<<endl;
         for(k=0;k<AMSTrIdSoft::ncrates();k++)oftxt <<_TrackerDir[i][k]<<" ";
         oftxt <<endl;
      }

      oftxt<<endl;


      oftxt << "_TrackerDRP2Layer[i][2]"<<endl;
      for(i=0;i<NTRHDRP;i++){
        oftxt <<"i "<<i<<endl;
         for(k=0;k<AMSTrIdSoft::ncrates();k++)oftxt <<_TrackerDRP2Layer[i][k]<<" ";
         oftxt <<endl;
        }
        
    
      oftxt<<endl;

      oftxt << "_TrackerOtherTDR[i][2]"<<endl;
      for(i=0;i<NTRHDRP;i++){
         oftxt <<"i "<<i<<endl;
         for(k=0;k<AMSTrIdSoft::ncrates();k++)oftxt <<_TrackerOtherTDR[i][k]<<" ";
         oftxt <<endl;
      }
    




    }
    }
#endif
    // Expert
    TriggerExpertLVL3::pExpert= new 
     TriggerExpertLVL3(LVL3EXPFFKEY.NEvents,LVL3EXPFFKEY.ToBad,
                       LVL3EXPFFKEY.TryAgain,LVL3EXPFFKEY.Range);
    #ifdef __AMSDEBUG__
       assert( TriggerExpertLVL3::pExpert != NULL);
    #endif

    
}
 
void TriggerLVL3::addtof(int16 plane, int16 paddle){
  if(plane >=0 && plane < SCLRS && paddle>=0 && paddle <SCMXBR && 
  _TOFStatus[plane][paddle] ==0){
   _TOFAux[plane][_NTOF[plane]]=paddle;
   _NTOF[plane]=_NTOF[plane]+1;
  }

}
  integer TriggerLVL3::tofok(){
     _TOFTrigger=0;
    int i,j;
    int ntof=0;
      for(i=0;i<_NTOF[0];i++){
        for(j=0;j<_NTOF[SCLRS-1];j++){
          if(_TOFPattern[_TOFAux[0][i]][_TOFAux[SCLRS-1][j]]){
              goto out;
          }
        }
      }
     _TOFTrigger=-1;
      return 0;
out:
    for (i=0;i<SCLRS;i++){
      if((_NTOF[i]> 2) || (_NTOF[i] == 0 && (i == 0 || i==SCLRS-1)))return 0;
            
      if(_NTOF[i]==2){
        if(abs(_TOFAux[i][0]-_TOFAux[i][1]) > 1)return 0;
      }
      if(_NTOF[i])ntof++;    
    }
    if (ntof < LVL3FFKEY.MinTOFPlanesFired)     return 0;
    _TOFTrigger=1;
    return 1;
  }




  void TriggerLVL3::preparetracker(){
   geant cooup=0;
   geant coodown=0;
   int i;
   for(i=0;i<_NTOF[0];i++)cooup+=_TOFCoo[0][_TOFAux[0][i]][1];
   cooup=cooup/_NTOF[0];   
   for(i=0;i<_NTOF[SCLRS-1];i++)coodown+=_TOFCoo[SCLRS-1][_TOFAux[SCLRS-1][i]][1];
   coodown=coodown/_NTOF[SCLRS-1];   
   for(i=0;i<TKDBc::nlay();i++){
     geant coo=coodown+(cooup-coodown)/(_TOFCoo[0][0][2]-_TOFCoo[SCLRS-1][0][2])*
     (_TrackerCooZ[i]-_TOFCoo[SCLRS-1][0][2]);
     _lowlimitY[i]=-LVL3FFKEY.TrTOFSearchReg+coo;
     _upperlimitY[i]=LVL3FFKEY.TrTOFSearchReg+coo;
   }  
  if(_NTOF[1]== 0 || _NTOF[2]==0){
    for(i=0;i<TKDBc::nlay();i++){
     _lowlimitX[i]=-1;
     _upperlimitX[i]=+1;
    }
  }
  else {
   cooup=0;
   coodown=0;
   for(i=0;i<_NTOF[1];i++)cooup+=_TOFCoo[1][_TOFAux[1][i]][0];
   cooup=cooup/_NTOF[1];   
   for(i=0;i<_NTOF[2];i++)coodown+=_TOFCoo[2][_TOFAux[2][i]][0];
   coodown=coodown/_NTOF[2];   
   for(i=0;i<TKDBc::nlay();i++){
     geant coo=coodown+(cooup-coodown)/(_TOFCoo[1][0][2]-_TOFCoo[2][0][2])*
     (_TrackerCooZ[i]-_TOFCoo[2][0][2]);
     _lowlimitX[i]=-LVL3FFKEY.TrTOFSearchReg+coo;
     _upperlimitX[i]=LVL3FFKEY.TrTOFSearchReg+coo;
   }  

  }
  }





  void TriggerLVL3::build(){
    if(LVL3FFKEY.RebuildLVL3==1){
      AMSEvent::gethead()->getC("TriggerLVL3",0)->eraseC();
    }
    // Shuttle    
     AMSgObj::BookTimer.start("LVL3");
     TriggerLVL1 * plvl1= 
       (TriggerLVL1*)AMSEvent::gethead()->
       getheadC("TriggerLVL1",0);
     if(plvl1){
       int16 * ptr;
       number tt1,tt2;
       TriggerAuxLVL3 aux[ncrt];
       for(int icrt=0;icrt<AMSTrIdSoft::ncrates();icrt++)aux[icrt].fill(icrt);
       int idum;
       TriggerLVL3 *plvl3=0;
       tt1=HighResTime();
       for(idum=0;idum<LVL3FFKEY.NRep;idum++){
        delete plvl3;
        plvl3 = new TriggerLVL3();  
    //
    // Start here the lvl3 algorithm
    //    

    //
    // first TOF Part
    //
    int i,j;
    for(i=0;i<SCLRS;i++){
      for(j=0;j<SCMXBR;j++){
        if(plvl1->checktofpattand(i,j) ||
            (_TOFOr[i][j] && plvl1->checktofpattor(i,j))){
          plvl3->addtof(i,j);
        }
      }
    }

    if(plvl3->tofok() == 0) goto formed;



  //
  // now Tracker Part
  //   
  plvl3->preparetracker();
  integer crate;
  for(crate=0;crate<AMSTrIdSoft::ncrates();crate++){
   ptr=aux[crate].readtracker(1);  
  while(ptr){
     integer drp,va,strip,side;
     AMSTrRawCluster::lvl3CompatibilityAddress
     (ptr[1],strip,va,side,drp);
     if(side == 0  && 
         _TrackerStatus[_TrackerOtherTDR[drp][crate]+crate*NTRHDRP] == 0)
         _TrackerAux[_TrackerOtherTDR[drp][crate]][crate]=1;
   ptr = aux[crate].readtracker();    
  }
  }
  for(crate=0;crate<AMSTrIdSoft::ncrates();crate++){
  ptr=aux[crate].readtracker(1);  
  while(ptr){
     integer drp,va,strip,side;
     AMSTrRawCluster::lvl3CompatibilityAddress
      (ptr[1],strip,va,side,drp);
     if(side != 0 && (LVL3FFKEY.NoK || _TrackerAux[drp][crate])){
      integer layer=_TrackerDRP2Layer[drp][crate];
#ifdef __AMSDEBUG__
      if(layer<0){
       cerr<<"TriggerLVL3::build-S-wronglayer "<<layer<<" "<<drp<<" "<<crate<<" "<<ptr[1]<<" "<<strip<<" "<<side<<endl;
       goto next;
      }
#endif      
      integer num = ((*ptr)&63);
      if(abs(LVL3FFKEY.SeedThr)>0 ){
        if(((*((int16u*)ptr)>>6) & 63) <abs(LVL3FFKEY.SeedThr) && ((*((int16u*)ptr)>>6) & 63)>0)goto next;
        int count=0;
        for(int k=2;k<num+3;k++){
          if(*(ptr+k)>=16)count++;
          else if(count)count--;
          if(count>=2)break;
        }
        if(count<2  )goto next;
      }
      else{
        // set failsafe cluster def > 1 strip || ( > 2 && two adj to max >=0)
        integer nmax= (*(int16u*)ptr)>>6;
        if(nmax == 0 || nmax==num){
          // probably reduced mode
          if(num == 0)goto next;
        }  
        else {
          // Probably normal mode
          if(*(ptr+1+nmax)< 16 && *(ptr+3+nmax) < 16)goto next;
        } 
      }
              float coo=0;
            float ss=0;
            float amp=0;
            for(i=0;i<num+1;i++){
             integer s2;
             switch (strip+i) {
              case 0:
               s2=0;
               break;
              case 639:
               s2=1282;
               break;
              default:
               s2=(strip+i)*2+2;
             }
             ss+= ((int)*(ptr+i+2)) * s2;
             amp+=((int)*(ptr+i+2));
  
            }
            if(amp)ss=ss/amp;

     
      coo=_TrackerCoo[drp][crate][1]+
      _TrackerDir[drp][crate]*_stripsize*(0.5+ss);
      if(AMSFFKEY.Debug){
       cout << "Trigger3  s/n*8 "<<((*((int16u*)ptr)>>6)&63)<<" lay "<<layer<<" strip "<<strip<<" coo "<<coo<<endl;
      }
      if (coo > plvl3->getlowlimitY(layer) && coo < plvl3->getupperlimitY(layer)){
        if((_TrackerCoo[drp][crate][0]<0 && plvl3->getlowlimitX(layer) < 0) ||
           (_TrackerCoo[drp][crate][0]>0 && plvl3->getupperlimitX(layer) > 0) ){
           if(!(plvl3->addnewhit(coo,amp,layer,drp+crate*NTRHDRP))){
           plvl3->TrackerTrigger()=2;
           goto formed;
           }             
        }
      }
     }
     next:
     ptr = aux[crate].readtracker();    
  }
 }
  plvl3->fit(idum);

 formed:
  //TriggerExpertLVL3::pExpert->update(plvl3);        
  tt2=HighResTime();
       }
//       if((plvl3->TrackerTriggerS())%32 >= LVL3FFKEY.Accept || 
//          (LVL3FFKEY.Accept>31 && plvl3->TrackerTriggerS()>=LVL3FFKEY.Accept)){ 
       if(plvl3->TrackerTriggerS()>=LVL3FFKEY.Accept){ 
         plvl3->settime(tt2-tt1);
         AMSEvent::gethead()->addnext(AMSID("TriggerLVL3",0),plvl3);
       }
       else delete plvl3;
       AMSgObj::BookTimer.stop("LVL3");

     }
    }


  










void TriggerLVL3::_writeEl(){
  LVL3Ntuple* lvl3N = AMSJob::gethead()->getntuple()->Get_lvl3();

  if (lvl3N->Nlvl3>=MAXLVL3) return;

// Fill the ntuple
  lvl3N->TOFTr[lvl3N->Nlvl3]=_TOFTrigger;
  lvl3N->AntiTr[lvl3N->Nlvl3]=_AntiTrigger;
  lvl3N->TrackerTr[lvl3N->Nlvl3]=_TrackerTrigger;
  lvl3N->NTrHits[lvl3N->Nlvl3]=_NTrHits;
  lvl3N->NPatFound[lvl3N->Nlvl3]=_NPatFound;
  lvl3N->Residual[lvl3N->Nlvl3][0]=_Residual[0];
  lvl3N->Residual[lvl3N->Nlvl3][1]=_Residual[1];
  lvl3N->Pattern[lvl3N->Nlvl3][0]=_Pattern[0];
  lvl3N->Pattern[lvl3N->Nlvl3][1]=_Pattern[1];
  lvl3N->Time[lvl3N->Nlvl3]=_Time;
  lvl3N->ELoss[lvl3N->Nlvl3]=_TrEnergyLoss;
  lvl3N->Nlvl3++;

}


integer TriggerLVL3::addnewhit(geant coo, geant amp, integer layer, integer drp){
  if( _nhits[layer]>= maxtrpl)return 0; 
  _coo[layer][_nhits[layer]]=coo;
  _eloss[layer][_nhits[layer]]=amp;
  _drp[layer][_nhits[layer]]=drp;
  _nhits[layer]=_nhits[layer]+1;
  
  return 1;
  }


void TriggerLVL3::fit(integer idum){
  //
  // Here  LVL3 Tracker Algorithm ( extremely stupid but very effective)
  //
  int i,j,k,l,ic,n1,n2,n3,n4;
  //
  // suppress splitted hits
  //
  for(i=0;i<TKDBc::nlay();i++){
   AMSsortNAGa(_coo[i],_nhits[i]);
   for(j=_nhits[i]-1;j>0;j--){
     if(_coo[i][j]-_coo[i][j-1] < LVL3FFKEY.Splitting){
       for(k=j;k<_nhits[i]-1;k++){
        _coo[i][k]=_coo[i][k+1];
       }
       (_nhits[i])--;
     }
   }
  }

  _NTrHits=_nhits[0]+_nhits[1]+_nhits[2]+_nhits[3]+_nhits[4]+_nhits[5];
  if(_NTrHits > LVL3FFKEY.TrMaxHits){
   _TrackerTrigger=2;
   return;
  }   

  if(_NTrHits < 3)ic=TKDBc::nlay()-2;
  else if (_NTrHits > TKDBc::nlay())ic=0;
  else ic=TKDBc::nlay()-_NTrHits;
  for (i=ic;i<TKDBc::nlay()-2;i++){      // Start from 4
    for( j=TKDBc::patd(i);j<TKDBc::patd(i+1);j++){
      if(TKDBc::patallowFalseX(j)){ 
      for(k=0;k<_nhits[TKDBc::patconf3(j,0)];k++){
         coou=_coo[TKDBc::patconf3(j,0)][k];
         amp[0]=_eloss[TKDBc::patconf3(j,0)][k];
         for(l=0;l<_nhits[TKDBc::patconf3(j,TKDBc::patpoints(j)-1)];l++){
            cood=_coo[TKDBc::patconf3(j,TKDBc::patpoints(j)-1)][l];
            amp[TKDBc::patpoints(j)-1]=_eloss[TKDBc::patconf3(j,TKDBc::patpoints(j)-1)][l];
            b=_TrackerCooZ[TKDBc::patconf3(j,TKDBc::patpoints(j)-1)]-
            _TrackerCooZ[TKDBc::patconf3(j,0)];
            zmean=_TrackerCooZ[TKDBc::patconf3(j,TKDBc::patpoints(j)-1)]+
            _TrackerCooZ[TKDBc::patconf3(j,0)];
            factor=1/fabs(b);
            s=sqrt((cood-coou)*(cood-coou)+b*b);
            a=(cood-coou);
            dscr=s*Discriminator(_NTrHits-TKDBc::patpoints(j));
            if(!_Level3Searcher(1,j))goto done;
         }
      }
     }
    }
    if(_NPatFound)break;
  }
  done:

   if( _NPatFound  == 0)_TrackerTrigger=3;
   else if ( _NPatFound >2)_TrackerTrigger = 4;
   else if (_NPatFound ==1){
    if(_Residual[0] > LVL3FFKEY.TrMinResidual )_TrackerTrigger=1;
    else _TrackerTrigger=7;
   }
   else {
    if(_Residual[0] > LVL3FFKEY.TrMinResidual && _Residual[1] > LVL3FFKEY.TrMinResidual )
    _TrackerTrigger=1;
    else if(_Residual[0] <= LVL3FFKEY.TrMinResidual && _Residual[1] <= LVL3FFKEY.TrMinResidual )
    _TrackerTrigger=7;
    else _TrackerTrigger=4;

   }
   if(_TrEnergyLoss > LVL3FFKEY.TrHeavyIonThr*8)_TrackerTrigger+= 8;
     
}

void TriggerLVL3::settime(number time){
   _Time=time/LVL3FFKEY.NRep;
}
  
integer TriggerLVL3::_UpdateOK(geant s,  geant res[], geant amp[],integer pat){
  int i;
  geant maxa;
  if(_NPatFound > 1){
     _NPatFound++;
     return 0;
  }
  else {
     int n=(TKDBc::patpoints(pat)-2);
     for(i=0;i<n;i++)_Residual[_NPatFound]+=res[i];
     _Residual[_NPatFound]=_Residual[_NPatFound]/n/s;     
     _Pattern[_NPatFound]=pat;
     if(_NPatFound++==0){
      maxa=0;
      for(i=0;i<TKDBc::patpoints(pat);i++){
        if(amp[i]>maxa)maxa=amp[i];
        _TrEnergyLoss+=amp[i];       
      }
      _TrEnergyLoss=(_TrEnergyLoss-maxa)/(TKDBc::patpoints(pat)-1);
     }
   return 1;
  }


}



void TriggerLVL3::builddaq(integer i, integer n, int16u *p){
  TriggerLVL3 *ptr=(TriggerLVL3*)AMSEvent::gethead()->
  getheadC("TriggerLVL3",i);
  *p=getdaqid(i);
  if(ptr){
    integer toftr=ptr->_TOFTrigger;
    if(toftr==-1)toftr=255;
    uinteger tra=ptr->TrackerTriggerS();
    *(p+1)=int16u(tra);
    *(p+2)=(toftr) | ((ptr->_AntiTrigger)<<8);
    int16 res=int16(ptr->_Residual[0]*1000);
    *(p+3)=int16u(res);
    res=int16(ptr->_Residual[1]*1000);
    *(p+4)=int16u(res);
    *(p+5)=(ptr->_NPatFound) | ((ptr->_NTrHits)<<4);
    *(p+6)= (ptr->_Pattern[0]) |( (ptr->_Pattern[1])<<6);
    *(p+7)=int16u(ptr->_TrEnergyLoss);
  }
  else {
    cerr <<"TriggerLVL3::builddaq-E-No Trigger for "<<i<<endl;
  }
}

void TriggerLVL3::buildraw(integer n, int16u *p){
  integer ic=checkdaqid(*p)-1;
  {
    AMSContainer *ptr=AMSEvent::gethead()->getC("TriggerLVL3",ic);
   if(ptr)ptr->eraseC();
   else cerr <<"TriggerLVL3::buildraw-S-NoContainer"<<endl;
  }

 integer tra,tof,anti,ntr,npat,pat[2];
 number res[2],time,eloss;
 time=0;
 res[1]=0;
 pat[1]=-1;
 //tra=(*(p+1)%32);   // tmp probg
 tra=(*(p+1));   // tmp probg
 tof=(*(p+2))&255;
 if(tof==255)tof=-1;
 anti=((*(p+2))>>8)&255;
 res[0]=int16(*(p+3))/1000.;
 res[1]=int16(*(p+4))/1000.;
 pat[0]=((*(p+6)))&63;
 pat[1]=((*(p+6))>>6)&63;
 if(pat[0]==63)pat[0]=-1;
 if(pat[1]==63)pat[1]=-1;
 npat=((*(p+5)))&15;
 ntr=((*(p+5))>>4)&255;
 eloss=int16(*(p+7));
 //tmp fix
 if(eloss==0 && n>8)eloss=int16(*(p+8));
 //  for(int k=0;k<8;k++){
 //    cout <<k<<" "<<*(p+k)<<endl;
 //   }
 
if(tra >= LVL3FFKEY.Accept ){
 // swap 1 <-> 3
  if(tra%8==1)tra=tra+2; 
  else if(tra%8==3)tra=tra-2; 
  AMSEvent::gethead()->addnext(AMSID("TriggerLVL3",ic), new
 TriggerLVL3( tra,  tof,  anti, ntr,  npat,
  pat,  res,  time, eloss));
}
}

integer TriggerLVL3::TrackerTriggerS(){
 // swap 1 <-> 3
 if(_TrackerTrigger%8==1)return _TrackerTrigger+2; 
 else if(_TrackerTrigger%8==3)return _TrackerTrigger-2; 
 else return _TrackerTrigger; 
}




TriggerExpertLVL3::TriggerExpertLVL3(integer countermax, integer tobad,
                   integer tryagain, geant badrange[NDSTR][2]):
  _CounterMax(countermax),_ToBadSwitch(tobad),_TryAgainSwitch(tryagain){
  int i,j;
  for(i=0;i<NDSTR;i++){
   for(j=0;j<2;j++)_BadRange[i][j]=badrange[i][j];
  }
  _Counter=0;
  if(_CounterMax <=0 )_CounterMax=1024;
  _Relative[0]=1;
  _Relative[1]=0;
  _Relative[2]=1;
  for(j=0;j<AMSTrIdSoft::ndrp()/2*2;j++)_DRPBad[j]=0;
  for(j=0;j<AMSTrIdSoft::ndrp()/2*2;j++)_DRPOff[j]=0;
  for(i=0;i<NDSTR;i++){
   _Mean[i]=0;
   _Sigma[i]=0;
   _XCount[i]=0;
   for(j=0;j<AMSTrIdSoft::ndrp()/2*2;j++)_Distributions[i][j]=0;
  }  
  for(j=0;j<AMSTrIdSoft::ndrp()/2*2;j++)_Distributions[NDSTR][j]=0;
}

void TriggerExpertLVL3::update(const TriggerLVL3 *  plvl3){
  int i,j;
integer where;
static integer local[NTRHDRP2];
if(plvl3->_TrackerTrigger==1)where=0;
else if(plvl3->_TrackerTrigger==2 || plvl3->_TrackerTrigger==4)where=1;
else if(plvl3->_TrackerTrigger==7)where=2;
else return;
  // Update Here
  for(i=0;i<AMSTrIdSoft::ndrp();i++)local[i]=0;
  for(i=0;i<TKDBc::nlay();i++){
    for(j=0;j<plvl3->_nhits[i];j++)local[plvl3->_drp[i][j]]=1;
  }

  for(i=0;i<AMSTrIdSoft::ndrp()/2*2;i++){
   if(local[i])_Distributions[where][i]++;
  }
_Counter=(_Counter+1)%_CounterMax;
if(_Counter == 0)analyse(plvl3);
}

void TriggerExpertLVL3::analyse(const TriggerLVL3 *  plvl3){
  // analyse here
  int i,j;
  for(i=0;i<AMSTrIdSoft::ndrp()/2*2;i++){
   for(j=0;j<NDSTR;j++)_Distributions[NDSTR][i]+=_Distributions[j][i];
  }
  for (i=0;i<NDSTR;i++){
    if(_Relative[i]){
      for(j=0;j<AMSTrIdSoft::ndrp()/2*2;j++){
        if(_Distributions[NDSTR][j] != 0){ 
         _Distributions[i][j]=_Distributions[i][j]/_Distributions[NDSTR][j];
        }
      }
    }
  }
  for (i=0;i<NDSTR;i++){
     for(j=0;j<AMSTrIdSoft::ndrp()/2*2;j++){
      if(_Distributions[NDSTR][j] != 0){ 
       _Mean[i]+=_Distributions[i][j];
       _Sigma[i]+=_Distributions[i][j]*_Distributions[i][j];
       _XCount[i]++;
      }
     }
     if(_XCount[i] !=0){
      _Mean[i]=_Mean[i]/_XCount[i];
      _Sigma[i]=sqrt(_Sigma[i]/_XCount[i]-_Mean[i]*_Mean[i]);
     }
  }
  for(j=0;j<AMSTrIdSoft::ndrp();j++){
    if(plvl3->_TrackerStatus[j] == 0){
      integer bad=0;
      for(i=0;i<NDSTR;i++){
        if(_Distributions[i][j] < _Mean[i]-_Sigma[i]*_BadRange[i][0] || 
           _Distributions[i][j] > _Mean[i]+_Sigma[i]*_BadRange[i][1]){
           bad++;
           cerr <<"TriggerExpertLVL3-I-DRP "<<j<<" out of limits "<<
             _Mean[i]<<" "<<i<<" "<<_Mean[i]-_Sigma[i]*_BadRange[i][0]<<" "
                <<_Distributions[i][j]<<
             " "<<_Mean[i]+_Sigma[i]*_BadRange[i][1]<<endl;
        }
      }
      if(bad)_DRPBad[j]++;
      else _DRPBad[j]=0;
    }
    else _DRPOff[j]++;
    if(_DRPBad[j]>=_ToBadSwitch){
     _DRPBad[j]=0;
     plvl3->_TrackerStatus[j]=1;
     cerr <<"TriggerExpertLVL3-W-DRP "<<j<<" declared bad"<<endl;
    }
    if(_DRPOff[j]>_TryAgainSwitch){
     _DRPOff[j]=0;
     plvl3->_TrackerStatus[j]=0;
     cout <<" TriggerExpertLVL3-W-DRP "<<j<<" brought back to on"<<endl;
    }
  }

  for(i=0;i<NDSTR;i++){
   _Mean[i]=0;
   _Sigma[i]=0;
   _XCount[i]=0;
   for(j=0;j<AMSTrIdSoft::ndrp()/2*2;j++)_Distributions[i][j]=0;
  }  
  for(j=0;j<AMSTrIdSoft::ndrp()/2*2;j++)_Distributions[NDSTR][j]=0;


}


void TriggerExpertLVL3::print(){
}

integer TriggerLVL3::checkdaqid(int16u id){
int i;
for(i=0;i<getmaxblocks();i++){
 if(id==getdaqid(i)){
  return i+1;
 }
}
return 0;
}



integer TriggerLVL3::calcdaqlength(integer i){
  TriggerLVL3 *ptr=(TriggerLVL3*)AMSEvent::gethead()->
  getheadC("TriggerLVL3",i);
  if(ptr)return 8;
  else return 0;
}

void TriggerLVL3::_printEl(ostream & stream){

  stream << " Lvl3Output " << _TrackerTrigger<<" "<<_TOFTrigger<<" "<<
    _AntiTrigger<<" res "<<_Residual[0]<<" "<<_Residual[1]<<" "<<_NPatFound <<
    " pat "<<_Pattern[0]<<" "<<_Pattern[1]<<" eloss "<<_TrEnergyLoss<<" nhits "<<_NTrHits<<endl;
 for(int j=0;j<TKDBc::nlay();j++){
  for(int i=0;i<_nhits[j];i++){
    stream<<"Lvl3-Layer "<<j+1<<" hit "<<i<<" "<<_coo[j][i]<<endl;    
  }
 }
}

integer TriggerLVL3::LVL3OK(){
  if(_TrackerTrigger ==0  || _TrackerTrigger ==2 ||
     (TRFITFFKEY.FastTracking>1 && _TrackerTrigger ==3) ||
     (TRFITFFKEY.FastTracking>2 && _TrackerTrigger <8 )  ||
     (TRFITFFKEY.FastTracking>3 && _TrackerTrigger <32 ) 
     )return 0;
  else return 1;
}

AMSLVL3Error::AMSLVL3Error(char * name){
  if(name){
    integer n=strlen(name)+1;
    if(n>255)n=255;
    strncpy(msg,name,n);
  }
}
char * AMSLVL3Error::getmessage(){return msg;}


integer TriggerLVL3::_Level3Searcher(int call, int j){
//              cout <<"Searcher "<<call<<" "<<TKDBc::patconf3(j,call)<<" "<<_nhits[TKDBc::patconf3(j,call)]<<" "<<j<<endl;
             for(int n1=0;n1<_nhits[TKDBc::patconf3(j,call)];n1++){
               r=a*(_TrackerCooZ[TKDBc::patconf3(j,call)]-_TrackerCooZ[TKDBc::patconf3(j,0)])+
               b*(coou-_coo[TKDBc::patconf3(j,call)][n1]);
                 if(fabs(r) < (1-fabs(2*_TrackerCooZ[TKDBc::patconf3(j,call)]-zmean)*factor)*dscr){
                   resid[call-1]=r;
                   amp[call]=_eloss[TKDBc::patconf3(j,call)][n1];
                   if(TKDBc::patpoints(j) > call+2){
                     return _Level3Searcher(++call,j);
                   }
                   else{
//                     cout << "qqs "<<j<<endl;
                     return _UpdateOK(s,resid,amp,j);
                   }
                }
              }
              return -1;
              
}
