// AMS common blocks method file
// Objectivity version Oct 08, 1996 ak
// Oct 14, 1996. ak. don't compare ccffkey run
//
// Last Edit : Oct 16, 1996. ak.
//

#include <typedefs.h>
#include <commonsD.h>
//#include <commons.C>

AMScommonsD::AMScommonsD() {

tofmcffkeyD   = TOFMCFFKEY;
tofrecffkeyD  = TOFRECFFKEY;
ctcmcffkeyD   = CTCMCFFKEY;
ctcgeomffkeyD = CTCGEOMFFKEY;
ctcrecffkeyD  = CTCRECFFKEY;
trmcffkeyD    = TRMCFFKEY;
betafitffkeyD = BETAFITFFKEY;
trfitffkeyD   = TRFITFFKEY;
ccffkeyD      = CCFFKEY;
trclffkeyD    = TRCLFFKEY;

}

ooStatus AMScommonsD::CmpConstants() {

//
// this function is awful, but how can I compare structures, not classes
//
 ooStatus rstatus = oocSuccess;
 integer  i;

  cout<<"AMScommonsD::CmpConstants -I- started "<<endl;
  cout<<"AMScommonsD::CmpConstants -I- TOFMCFFKEY"<<endl;
     if (tofmcffkeyD.TimeSigma != TOFMCFFKEY.TimeSigma) 
      cout<<"tofmcffkeyD.TimeSigma "<<tofmcffkeyD.TimeSigma<<" "
          <<TOFMCFFKEY.TimeSigma<<endl;
     if (tofmcffkeyD.padl != TOFMCFFKEY.padl) 
      cout<<"tofmcffkeyD.padl "<<tofmcffkeyD.padl<<" "
          <<TOFMCFFKEY.padl<<endl;
     if (tofmcffkeyD.Thr  != TOFMCFFKEY.Thr) 
      cout<<"tofmcffkeyD.Thr  "<<tofmcffkeyD.Thr<<" "
          <<TOFMCFFKEY.Thr<<endl;
     if (tofmcffkeyD.edep2ph != TOFMCFFKEY.edep2ph) 
      cout<<"tofmcffkeyD.edep2ph "<<tofmcffkeyD.edep2ph<<" "
          <<TOFMCFFKEY.edep2ph<<endl;
     if (tofmcffkeyD.pmqeff  != TOFMCFFKEY.pmqeff)  
      cout<<"tofmcffkeyD.pmqeff  "<<tofmcffkeyD.pmqeff<<" "
          <<TOFMCFFKEY.pmqeff<<endl;
     if (tofmcffkeyD.trtspr  != TOFMCFFKEY.trtspr)  
      cout<<"tofmcffkeyD.trtspr  "<<tofmcffkeyD.trtspr<<" "
          <<TOFMCFFKEY.trtspr<<endl;
     if (tofmcffkeyD.fladctb != TOFMCFFKEY.fladctb) 
      cout<<"tofmcffkeyD.fladctb "<<tofmcffkeyD.fladctb<<" "
          <<TOFMCFFKEY.fladctb<<endl;
     if (tofmcffkeyD.shaptb  != TOFMCFFKEY.shaptb)  
      cout<<"tofmcffkeyD.shaptb  "<<tofmcffkeyD.shaptb<<" "
          <<TOFMCFFKEY.shaptb<<endl;
     if (tofmcffkeyD.di2anr  != TOFMCFFKEY.di2anr)  
      cout<<"tofmcffkeyD.di2anr  "<<tofmcffkeyD.di2anr<<" "
          <<TOFMCFFKEY.di2anr<<endl;
     if (tofmcffkeyD.shrtim  != TOFMCFFKEY.shrtim)  
      cout<<"tofmcffkeyD.shrtim  "<<tofmcffkeyD.shrtim<<" "
          <<TOFMCFFKEY.shrtim<<endl;
     if (tofmcffkeyD.shftim  != TOFMCFFKEY.shftim)  
      cout<<"tofmcffkeyD.shftim  "<<tofmcffkeyD.shftim<<" "
          <<TOFMCFFKEY.shftim<<endl;
     if (tofmcffkeyD.strrat  != TOFMCFFKEY.strrat)  
      cout<<"tofmcffkeyD.strrat  "<<tofmcffkeyD.strrat<<" "
          <<TOFMCFFKEY.strrat<<endl;
     if (tofmcffkeyD.trigtb  != TOFMCFFKEY.trigtb)  
      cout<<"tofmcffkeyD.trigtb  "<<tofmcffkeyD.trigtb<<" "
          <<TOFMCFFKEY.trigtb<<endl;
     if (tofmcffkeyD.mcprtf  != TOFMCFFKEY.mcprtf)  
      cout<<"tofmcffkeyD.mcprtf  "<<tofmcffkeyD.mcprtf<<" "
          <<TOFMCFFKEY.mcprtf<<endl;

  for (i=0; i<2; i++) {
    if (tofmcffkeyD.accdel[i]    != TOFMCFFKEY.accdel[i])  
      cout<<"tofmcffkeyD.accdel "<<tofmcffkeyD.accdel[i]<<" "
          <<TOFMCFFKEY.accdel[i]<<endl;
    if (tofmcffkeyD.trlogic[i]   != TOFMCFFKEY.trlogic[i]) 
      cout<<"tofmcffkeyD.trlogic "<<tofmcffkeyD.trlogic[i]<<" "
          <<TOFMCFFKEY.trlogic[i]<<endl;
    if (tofmcffkeyD.accdelmx[i]  != TOFMCFFKEY.accdelmx[i]) 
      cout<<"tofmcffkeyD.accdelmx "<<tofmcffkeyD.accdelmx[i]<<" "
          <<TOFMCFFKEY.accdelmx[i]<<endl;
  }     

  for (i=0; i<4; i++) {
    if (tofmcffkeyD.tdcbin[i]    != TOFMCFFKEY.tdcbin[i]) {
      cout<<"tofmcffkeyD.tdcbin[ "<<i<<"] "<<tofmcffkeyD.tdcbin[i]<<" "
          <<TOFMCFFKEY.tdcbin[i]<<endl;
   }
  }

  for (i=0; i<5; i++) {
    if (tofmcffkeyD.daqthr[i]    != TOFMCFFKEY.daqthr[i]) {
      cout<<"tofmcffkeyD.daqthr[ "<<i<<"] "<<tofmcffkeyD.daqthr[i]<<" "
          <<TOFMCFFKEY.daqthr[i]<<endl;
   }
  }

  for (i=0; i<10; i++) {
    if (tofmcffkeyD.daqpwd[i]    != TOFMCFFKEY.daqpwd[i]) {
      cout<<"tofmcffkeyD.daqpwd "
          <<tofmcffkeyD.daqpwd[i]<<" "<<TOFMCFFKEY.daqpwd[i]<<endl;
   }
  }

tofrec:
  cout<<"AMScommonsD::CmpConstants -I- TOFRECFFKEY  "<<endl;
     if (tofrecffkeyD.Thr1 != TOFRECFFKEY.Thr1) 
      cout<<"tofrecffkeyD.Thr1 "<<tofrecffkeyD.Thr1<<" "
                                <<TOFRECFFKEY.Thr1<<endl;
     if (tofrecffkeyD.ThrS != TOFRECFFKEY.ThrS) 
      cout<<"tofrecffkeyD.ThrS "<<tofrecffkeyD.ThrS<<" "
                                <<TOFRECFFKEY.ThrS<<endl;
ctcgeom:
     if (
      (ctcgeomffkeyD.wallth != CTCGEOMFFKEY.wallth) ||
      (ctcgeomffkeyD.agap   != CTCGEOMFFKEY.agap) ||
      (ctcgeomffkeyD.wgap   != CTCGEOMFFKEY.wgap) ||
      (ctcgeomffkeyD.wlsth  != CTCGEOMFFKEY.wlsth) ||
      (ctcgeomffkeyD.supzc  != CTCGEOMFFKEY.supzc) ||
      (ctcgeomffkeyD.aegden != CTCGEOMFFKEY.aegden) ||
      (ctcgeomffkeyD.wlsden != CTCGEOMFFKEY.wlsden) ||
      (ctcgeomffkeyD.nblk   != CTCGEOMFFKEY.nblk)   ||
      (ctcgeomffkeyD.nwls   != CTCGEOMFFKEY.nwls) ) {
      rstatus = oocError;
      cout<<"AMScommonsD::CmpConstants -E- CTCGEOMFFKEY  are different "
           <<" and will be set to DBASE values"<<endl;
      CTCGEOMFFKEY = ctcgeomffkeyD;
     if (ctcgeomffkeyD.wallth != CTCGEOMFFKEY.wallth) 
       cout<<ctcgeomffkeyD.wallth<<" "<<CTCGEOMFFKEY.wallth<<endl;
     if (ctcgeomffkeyD.agap   != CTCGEOMFFKEY.agap) 
       cout<<ctcgeomffkeyD.agap<<" "<<CTCGEOMFFKEY.agap<<endl;
     if (ctcgeomffkeyD.wgap   != CTCGEOMFFKEY.wgap) 
       cout<<ctcgeomffkeyD.wgap<<" "<<CTCGEOMFFKEY.wgap<<endl;
     if (ctcgeomffkeyD.wlsth  != CTCGEOMFFKEY.wlsth) 
       cout<<ctcgeomffkeyD.wlsth<<" "<<CTCGEOMFFKEY.wlsth<<endl;
     if (ctcgeomffkeyD.supzc  != CTCGEOMFFKEY.supzc) 
       cout<<ctcgeomffkeyD.supzc<<" "<<CTCGEOMFFKEY.supzc<<endl;
     if (ctcgeomffkeyD.aegden != CTCGEOMFFKEY.aegden) 
       cout<<ctcgeomffkeyD.aegden<<" "<<CTCGEOMFFKEY.aegden<<endl;
     if (ctcgeomffkeyD.wlsden != CTCGEOMFFKEY.wlsden) 
       cout<<ctcgeomffkeyD.wlsden<<" "<<CTCGEOMFFKEY.wlsden<<endl;
     if (ctcgeomffkeyD.nblk   != CTCGEOMFFKEY.nblk)   
       cout<<ctcgeomffkeyD.nblk<<" "<<CTCGEOMFFKEY.nblk<<endl;
     if (ctcgeomffkeyD.nwls   != CTCGEOMFFKEY.nwls) 
       cout<<ctcgeomffkeyD.nwls<<" "<<CTCGEOMFFKEY.nwls<<endl;
      goto ctcmc;
  }
  for (i=0; i<3; i++) {
    if (
         (ctcgeomffkeyD.agsize[i]    != CTCGEOMFFKEY.agsize[i]) ||
         (ctcgeomffkeyD.hcsize[i]    != CTCGEOMFFKEY.hcsize[i]) ) {
        cout<<"AMScommonsD::CmpConstants -W- CTCGEOMFFKEY  are different "
            <<" and will be set to DBASE values"<<endl;
        CTCGEOMFFKEY = ctcgeomffkeyD;
         if (ctcgeomffkeyD.agsize[i]    != CTCGEOMFFKEY.agsize[i]) 
          cout<<ctcgeomffkeyD.agsize[i]<<" "<<CTCGEOMFFKEY.agsize[i]<<endl;
         if (ctcgeomffkeyD.hcsize[i]    != CTCGEOMFFKEY.hcsize[i]) 
          cout<<ctcgeomffkeyD.hcsize[i]<<" "<<CTCGEOMFFKEY.hcsize[i]<<endl;
        goto ctcmc;
    }
  }

ctcmc:
  cout<<"AMScommonsD::CmpConstants -I- CTCMCFFKEY"<<endl;
  for (i=0; i<2; i++) {
     if (ctcmcffkeyD.Refraction[i]    != CTCMCFFKEY.Refraction[i]) 
        cout<<"ctcmcffkeyD.Refraction["<<i<<"] "
            <<ctcmcffkeyD.Refraction[i]<<" "<<CTCMCFFKEY.Refraction[i]<<endl;
     if (ctcmcffkeyD.AbsLength[i]     != CTCMCFFKEY.AbsLength[i])  
        cout<<"ctcmcffkeyD.AbsLength["<<i<<"] "
            <<ctcmcffkeyD.AbsLength[i]<<" "<<CTCMCFFKEY.AbsLength[i]<<endl;
     if (ctcmcffkeyD.Path2PhEl[i]     != CTCMCFFKEY.Path2PhEl[i]) 
        cout<<"ctcmcffkeyD.Path2PhEl["<<i<<"] "
            <<ctcmcffkeyD.Path2PhEl[i]<<" "<<CTCMCFFKEY.Path2PhEl[i]<<endl;
    
  }


  cout<<"AMScommonsD::CmpConstants -I- CTCRECFFKEY"<<endl;
    if (ctcrecffkeyD.Thr1 != CTCRECFFKEY.Thr1) 
      cout<<"ctcrecffkeyD.Thr1 "<<ctcrecffkeyD.Thr1<<" "
                                <<CTCRECFFKEY.Thr1<<endl;
    if (ctcrecffkeyD.ThrS != CTCRECFFKEY.ThrS) 
      cout<<"ctcrecffkeyD.ThrS "<<ctcrecffkeyD.ThrS<<" "
                                <<CTCRECFFKEY.ThrS<<endl;

  cout<<"AMScommonsD::CmpConstants -I- TRMCFFKEY"<<endl;
    if  (trmcffkeyD.alpha != TRMCFFKEY.alpha) 
      cout<<"alpha "<<trmcffkeyD.alpha<<" "<<TRMCFFKEY.alpha<<endl;
    if  (trmcffkeyD.beta  != TRMCFFKEY.beta) 
      cout<<"beta  "<<trmcffkeyD.beta <<" "<<TRMCFFKEY.beta<<endl;
    if  (trmcffkeyD.gamma != TRMCFFKEY.gamma) 
      cout<<"gamma "<<trmcffkeyD.gamma<<" "<<TRMCFFKEY.gamma<<endl;
    if  (trmcffkeyD.dedx2nprel != TRMCFFKEY.dedx2nprel) 
      cout<<"dedx2nprel "<<trmcffkeyD.dedx2nprel<<" "
          <<TRMCFFKEY.dedx2nprel<<endl;
    if  (trmcffkeyD.fastswitch != TRMCFFKEY.fastswitch) 
      cout<<"fastswitch "<<trmcffkeyD.fastswitch<<" "
          <<TRMCFFKEY.fastswitch<<endl;
   
  for (i=0; i<2; i++) {
         if (trmcffkeyD.ped[i]    != TRMCFFKEY.ped[i])   
          cout<<"ped["<<i<<"] "<<trmcffkeyD.ped[i]<<" "
              <<TRMCFFKEY.ped[i]<<endl;
         if (trmcffkeyD.sigma[i]  != TRMCFFKEY.sigma[i]) 
          cout<<"sigma[ "<<i<<"] "
              <<trmcffkeyD.sigma[i]<<" "<<TRMCFFKEY.sigma[i]<<endl;
         if (trmcffkeyD.gain[i]   != TRMCFFKEY.gain[i]) 
          cout<<"gain["<<i<<"] "
              <<trmcffkeyD.gain[i]<<" "<<TRMCFFKEY.gain[i]<<endl;
         if (trmcffkeyD.thr[i]    != TRMCFFKEY.thr[i]) 
          cout<<"thr["<<i<<"] "<<trmcffkeyD.thr[i]<<" "
              <<TRMCFFKEY.thr[i]<<endl;
         if (trmcffkeyD.neib[i]   != TRMCFFKEY.neib[i]) 
          cout<<"neib["<<i<<"] "
              <<trmcffkeyD.neib[i]<<" "<<TRMCFFKEY.neib[i]<<endl;
         if (trmcffkeyD.cmn[i]    != TRMCFFKEY.cmn[i]) 
          cout<<"cmn["<<i<<"] "
              <<trmcffkeyD.cmn[i]<<" "<<TRMCFFKEY.cmn[i]<<endl;
  }

betafit:
  cout<<"AMScommonsD::CmpConstants -I- BETAFITFFKEY"<<endl;
  for (i=0; i<npatb; i++) {
    if (betafitffkeyD.pattern[i]    != BETAFITFFKEY.pattern[i]) {
      cout<<"pattern["<<i<<"] "<<betafitffkeyD.pattern[i]<<" "
          <<BETAFITFFKEY.pattern[i]<<endl;
   }
  }
  if (betafitffkeyD.Chi2    != BETAFITFFKEY.Chi2) {
      cout<<"Chi2 "<<betafitffkeyD.Chi2<<" "<<BETAFITFFKEY.Chi2<<endl;
   }
  for (i=0; i<3; i++) {
    if (betafitffkeyD.SearchReg[i]    != BETAFITFFKEY.SearchReg[i]) {
      cout<<"pattern["<<i<<"] "<<betafitffkeyD.SearchReg[i]<<" "
          <<BETAFITFFKEY.SearchReg[i]<<endl;
   }
  }

trfit:

  cout<<"AMScommonsD::CmpConstants -I- TRFITFFKEY"<<endl;
  for (i=0; i<npat; i++) {
    if (trfitffkeyD.pattern[i]    != TRFITFFKEY.pattern[i]) {
      cout<<"pattern["<<i<<"] "<<trfitffkeyD.pattern[i]<<" "
          <<TRFITFFKEY.pattern[i]<<endl;
   }
  }

      if (trfitffkeyD.UseTOF         != TRFITFFKEY.UseTOF) 
       cout<<"trfitffkeyD.UseTOF "<<trfitffkeyD.UseTOF
           <<" "<<TRFITFFKEY.UseTOF<<endl;
      if (trfitffkeyD.Chi2FastFit    != TRFITFFKEY.Chi2FastFit) 
       cout<<"trfitffkeyD.Chi2FastFit "<<trfitffkeyD.Chi2FastFit
           <<" "<<TRFITFFKEY.Chi2FastFit<<endl;
      if (trfitffkeyD.Chi2StrLine    != TRFITFFKEY.Chi2StrLine) 
        cout<<"trfitffkeyD.Chi2StrLine "<<trfitffkeyD.Chi2StrLine
            <<" "<<TRFITFFKEY.Chi2StrLine<<endl;
      if (trfitffkeyD.Chi2Circle     != TRFITFFKEY.Chi2Circle) 
        cout<<"trfitffkeyD.Chi2Circle "<<trfitffkeyD.Chi2Circle
            <<" "<<TRFITFFKEY.Chi2Circle<<endl;
      if (trfitffkeyD.ResCutFastFit  != TRFITFFKEY.ResCutFastFit) 
        cout<<"trfitffkeyD.ResCutFastFit "<<trfitffkeyD.ResCutFastFit
            <<" "<<TRFITFFKEY.ResCutFastFit<<endl;
      if (trfitffkeyD.ResCutStrLine  != TRFITFFKEY.ResCutStrLine) 
        cout<<"trfitffkeyD.ResCutStrLine "<<trfitffkeyD.ResCutStrLine
            <<" "<<TRFITFFKEY.ResCutStrLine<<endl;
      if (trfitffkeyD.ResCutCircle   != TRFITFFKEY.ResCutCircle) 
         cout<<"trfitffkeyD.ResCutCircle "<<trfitffkeyD.ResCutCircle
             <<" "<<TRFITFFKEY.ResCutCircle<<endl;
      if (trfitffkeyD.SearchRegStrLine   != TRFITFFKEY.SearchRegStrLine)
         cout<<"trfitffkeyD.SearchRegStrLine "<<trfitffkeyD.SearchRegStrLine
             <<" "<<TRFITFFKEY.SearchRegStrLine<<endl;
      if (trfitffkeyD.SearchRegCircle   != TRFITFFKEY.SearchRegCircle)
         cout<<"trfitffkeyD.SearchRegCircle "<<trfitffkeyD.SearchRegCircle
             <<" "<<TRFITFFKEY.SearchRegCircle<<endl;
      if (trfitffkeyD.RidgidityMin   != TRFITFFKEY.RidgidityMin) 
         cout<<"trfitffkeyD.RidgidityMin "<<trfitffkeyD.RidgidityMin
             <<" "<<TRFITFFKEY.RidgidityMin<<endl;

  cout<<"AMScommonsD::CmpConstants -I- CCFFKEY"<<endl;
      if (ccffkeyD.fixp     != CCFFKEY.fixp) 
       cout<<" ccffkey.fixp "<<ccffkeyD.fixp<<" "<<CCFFKEY.fixp<<endl;
      if (ccffkeyD.albedor  != CCFFKEY.albedor) 
        cout<<" ccffkeyD.albedor "<<ccffkeyD.albedor<<" "<<CCFFKEY.albedor
            <<endl;
      if (ccffkeyD.albedocz != CCFFKEY.albedocz) 
        cout<<" ccffkeyD.albedocz  "<<ccffkeyD.albedocz<<" "<<CCFFKEY.albedocz
            <<endl;
      if (ccffkeyD.npat     != CCFFKEY.npat)  
        cout<<"ccffkeyD.npat "<<ccffkeyD.npat<<" "<<CCFFKEY.npat<<endl;

  for(i=0; i<6; i++) {
    if (ccffkeyD.coo[i]     != CCFFKEY.coo[i]) 
      cout<<"ccffkey.coo["<<i<<"] "<<ccffkeyD.coo[i]
          <<" "<<CCFFKEY.coo[i]<<endl;
    if (ccffkeyD.dir[i]     != CCFFKEY.dir[i]) 
      cout<<"ccffkey.dir["<<i<<"] "<<ccffkeyD.dir[i]
          <<" "<<CCFFKEY.dir[i]<<endl;
  }

   for(i=0; i<2; i++) {
    if( (ccffkeyD.momr[i]     != CCFFKEY.momr[i])) {
      cout<<"ccffkey.momr["<<i<<"] "<<ccffkeyD.momr[i]<<" "
          <<CCFFKEY.momr[i]<<endl;
    }
  }

trcl:  
  cout<<"AMScommonsD::CmpConstants -I- TRCLFFKEY"<<endl;
  for(i=0; i<2; i++) {
    if(trclffkeyD.ThrClA[i]     != TRCLFFKEY.ThrClA[i]) 
      cout<<"trclffkeyD.ThrClA["<<i<<"] "<<trclffkeyD.ThrClA[i]<<" "
                                        <<TRCLFFKEY.ThrClA[i]<<endl;
    if (trclffkeyD.ThrClS[i]     != TRCLFFKEY.ThrClS[i]) 
      cout<<"trclffkeyD.ThrClS["<<i<<"] "<<trclffkeyD.ThrClS[i]<<" "
                                        <<TRCLFFKEY.ThrClS[i]<<endl; 
    if (trclffkeyD.ThrClR[i]    != TRCLFFKEY.ThrClR[i]) 
      cout<<"trclffkeyD.ThrClR["<<i<<"] "<<trclffkeyD.ThrClR[i]<<" "
                                        <<TRCLFFKEY.ThrClR[i]<<endl; 
    if (trclffkeyD.Thr1A[i]     != TRCLFFKEY.Thr1A[i]) 
      cout<<"trclffkeyD.Thr1A["<<i<<"] "<<trclffkeyD.Thr1A[i]<<" "
                                        <<TRCLFFKEY.Thr1A[i]<<endl;
    if (trclffkeyD.Thr1S[i]     != TRCLFFKEY.Thr1S[i]) 
      cout<<"trclffkeyD.Thr1S["<<i<<"] "<<trclffkeyD.Thr1S[i]<<" "
                                        <<TRCLFFKEY.Thr1S[i]<<endl;
    if (trclffkeyD.Thr1R[i]     != TRCLFFKEY.Thr1R[i]) 
      cout<<"trclffkeyD.ThrC1R["<<i<<"] "<<trclffkeyD.Thr1R[i]<<" "
                                        <<TRCLFFKEY.Thr1R[i]<<endl; 
    if (trclffkeyD.Thr2A[i]     != TRCLFFKEY.Thr2A[i]) 
      cout<<"trclffkeyD.Thr2A["<<i<<"] "<<trclffkeyD.Thr2A[i]<<" "
                                        <<TRCLFFKEY.Thr2A[i]<<endl; 
    if (trclffkeyD.Thr2S[i]     != TRCLFFKEY.Thr2S[i]) 
      cout<<"trclffkeyD.Thr2S["<<i<<"] "<<trclffkeyD.Thr2S[i]<<" "
                                        <<TRCLFFKEY.Thr2S[i]<<endl; 
    if (trclffkeyD.Thr2R[i]     != TRCLFFKEY.Thr2R[i]) 
      cout<<"trclffkeyD.Thr2R["<<i<<"] "<<trclffkeyD.Thr2R[i]<<" "
                                        <<TRCLFFKEY.Thr2R[i]<<endl; 
    if (trclffkeyD.Thr3R[i]     != TRCLFFKEY.Thr3R[i]) 
      cout<<"trclffkeyD.Thr3R["<<i<<"] "<<trclffkeyD.Thr3R[i]<<" "
                                        <<TRCLFFKEY.Thr3R[i]<<endl; 
  }

  if (rstatus == oocError) 
      cout<<"AMScommonsD::CmpConstants -W- done, discrepancy found "<<endl;
  if (rstatus == oocSuccess) 
      cout<<"AMScommonsD::CmpConstants -I- done"<<endl;

return rstatus;
}
