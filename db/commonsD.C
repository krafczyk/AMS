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

amsffkeyD     = AMSFFKEY;
iopaD         = IOPA;
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
//
  //  if(
  //     (amsffkeyD.Simulation != AMSFFKEY.Simulation) ||
  //     (amsffkeyD.Reconstruction != AMSFFKEY.Reconstruction) ||
  //     (amsffkeyD.Jobtype != AMSFFKEY.Jobtype) ||
  //     (amsffkeyD.Read != AMSFFKEY.Read) ||
  //     (amsffkeyD.Write != AMSFFKEY.Write) ) {
  //    cout<<"AMScommonsD::CmpConstants -E- AMSFFKEY are different "<<endl;
  //  }
  //  for (i=0; i<40; i++) {
  //    if(amsffkeyD.Setupname[i] != AMSFFKEY.Setupname[i] ||
  //       amsffkeyD.Jobname[i]   != AMSFFKEY.Jobname[i] )
  //      {
  //       cout<<"AMScommonsD::CmpConstants -E- Check Setup or Jobname "<<endl;
  //      }
  //  }
  //  if(
  //      (iopaD.ntuple   != IOPA.ntuple) ||
  //      (iopaD.WriteAll != IOPA.WriteAll) ||
  //      (iopaD.TriggerI != IOPA.TriggerI) ){
  //       rstatus = oocError;
  //       cout<<"AMScommonsD::CmpConstants -E- IOPA  are different "<<endl;
  //  }
//
  if(
     (tofmcffkeyD.TimeSigma != TOFMCFFKEY.TimeSigma) ||
     (tofmcffkeyD.padl != TOFMCFFKEY.padl) ||
     (tofmcffkeyD.Thr  != TOFMCFFKEY.Thr) ||
     (tofmcffkeyD.edep2ph != TOFMCFFKEY.edep2ph) ||
     (tofmcffkeyD.pmqeff  != TOFMCFFKEY.pmqeff)  ||
     (tofmcffkeyD.trtspr  != TOFMCFFKEY.trtspr)  ||
     (tofmcffkeyD.fladctb != TOFMCFFKEY.fladctb) ||
     (tofmcffkeyD.shaptb  != TOFMCFFKEY.shaptb)  ||
     (tofmcffkeyD.di2anr  != TOFMCFFKEY.di2anr)  ||
     (tofmcffkeyD.shrtim  != TOFMCFFKEY.shrtim)  ||
     (tofmcffkeyD.shftim  != TOFMCFFKEY.shftim)  ||
     (tofmcffkeyD.strrat  != TOFMCFFKEY.strrat)  ||
     (tofmcffkeyD.trigtb  != TOFMCFFKEY.trigtb)  ||
     (tofmcffkeyD.mcprtf  != TOFMCFFKEY.mcprtf)  ) {
      rstatus = oocError;
      cout<<"AMScommonsD::CmpConstants -E- TOFMCFFKEY  are different "
           <<" and will be set to DBASE values"<<endl;
      cout<<"tofmcffkeyD.TimeSigma "<<tofmcffkeyD.TimeSigma<<" "
          <<TOFMCFFKEY.TimeSigma<<endl;
      cout<<"tofmcffkeyD.padl "<<tofmcffkeyD.padl<<" "
          <<TOFMCFFKEY.padl<<endl;
      cout<<"tofmcffkeyD.Thr  "<<tofmcffkeyD.Thr<<" "
          <<TOFMCFFKEY.Thr<<endl;
      cout<<"tofmcffkeyD.edep2ph "<<tofmcffkeyD.edep2ph<<" "
          <<TOFMCFFKEY.edep2ph<<endl;
      cout<<"tofmcffkeyD.pmqeff  "<<tofmcffkeyD.pmqeff<<" "
          <<TOFMCFFKEY.pmqeff<<endl;
      cout<<"tofmcffkeyD.trtspr  "<<tofmcffkeyD.trtspr<<" "
          <<TOFMCFFKEY.trtspr<<endl;
      cout<<"tofmcffkeyD.fladctb "<<tofmcffkeyD.fladctb<<" "
          <<TOFMCFFKEY.fladctb<<endl;
      cout<<"tofmcffkeyD.shaptb  "<<tofmcffkeyD.shaptb<<" "
          <<TOFMCFFKEY.shaptb<<endl;
      cout<<"tofmcffkeyD.di2anr  "<<tofmcffkeyD.di2anr<<" "
          <<TOFMCFFKEY.di2anr<<endl;
      cout<<"tofmcffkeyD.shrtim  "<<tofmcffkeyD.shrtim<<" "
          <<TOFMCFFKEY.shrtim<<endl;
      cout<<"tofmcffkeyD.shftim  "<<tofmcffkeyD.shftim<<" "
          <<TOFMCFFKEY.shftim<<endl;
      cout<<"tofmcffkeyD.strrat  "<<tofmcffkeyD.strrat<<" "
          <<TOFMCFFKEY.strrat<<endl;
      cout<<"tofmcffkeyD.trigtb  "<<tofmcffkeyD.trigtb<<" "
          <<TOFMCFFKEY.trigtb<<endl;
      cout<<"tofmcffkeyD.mcprtf  "<<tofmcffkeyD.mcprtf<<" "
          <<TOFMCFFKEY.mcprtf<<endl;
      TOFMCFFKEY = tofmcffkeyD;
      goto tofrec;
   }
  for (i=0; i<2; i++) {
    if ( (tofmcffkeyD.accdel[i]    != TOFMCFFKEY.accdel[i])  ||
         (tofmcffkeyD.trlogic[i]   != TOFMCFFKEY.trlogic[i]) ||
         (tofmcffkeyD.accdelmx[i]  != TOFMCFFKEY.accdelmx[i]) )
     {
      rstatus = oocError;
      cout<<"AMScommonsD::CmpConstants -E- TOFMCFFKEY  are different "
           <<" and will be set to DBASE values"<<endl;
      cout<<"tofmcffkeyD.accdel "<<tofmcffkeyD.accdel<<" "<<TOFMCFFKEY.accdel
          <<endl;
      cout<<"tofmcffkeyD.trlogic "<<tofmcffkeyD.accdel<<" "<<TOFMCFFKEY.trlogic
          <<endl;
      cout<<"tofmcffkeyD.accdelmx "<<tofmcffkeyD.accdelmx<<" "
          <<TOFMCFFKEY.accdelmx<<endl;
      TOFMCFFKEY = tofmcffkeyD;
      goto tofrec;
   }
  }     

  for (i=0; i<4; i++) {
    if (tofmcffkeyD.tdcbin[i]    != TOFMCFFKEY.tdcbin[i]) {
      rstatus = oocError;
      cout<<"AMScommonsD::CmpConstants -E- TOFMCFFKEY  are different "
           <<" and will be set to DBASE values"<<endl;
      cout<<"tofmcffkeyD.tdcbin "<<tofmcffkeyD.tdcbin<<" "<<TOFMCFFKEY.tdcbin
          <<endl;
      TOFMCFFKEY = tofmcffkeyD;
      goto tofrec;
   }
  }

  for (i=0; i<5; i++) {
    if (tofmcffkeyD.daqthr[i]    != TOFMCFFKEY.daqthr[i]) {
      rstatus = oocError;
      cout<<"AMScommonsD::CmpConstants -E- TOFMCFFKEY  are different "
           <<" and will be set to DBASE values"<<endl;
      cout<<"tofmcffkeyD.daqthr "<<endl;
      cout<<tofmcffkeyD.daqthr<<endl;
      cout<<TOFMCFFKEY.daqpwd<<endl;
      TOFMCFFKEY = tofmcffkeyD;
      goto tofrec;
   }
  }

  for (i=0; i<10; i++) {
    if (tofmcffkeyD.daqpwd[i]    != TOFMCFFKEY.daqpwd[i]) {
      rstatus = oocError;
      cout<<"AMScommonsD::CmpConstants -E- TOFMCFFKEY  are different "
           <<" and will be set to DBASE values"<<endl;
      cout<<"tofmcffkeyD.daqpwd "<<endl;
      cout<<tofmcffkeyD.daqpwd<<endl;
      cout<<TOFMCFFKEY.daqpwd<<endl;
      TOFMCFFKEY = tofmcffkeyD;
      goto tofrec;
   }
  }

tofrec:
  if(
     (tofrecffkeyD.Thr1 != TOFRECFFKEY.Thr1) ||
     (tofrecffkeyD.ThrS != TOFRECFFKEY.ThrS) ) {
      rstatus = oocError;
      cout<<"AMScommonsD::CmpConstants -E- TOFRECFFKEY  are different "
           <<" and will be set to DBASE values"<<endl;
      TOFRECFFKEY = tofrecffkeyD;
   }

ctcgeom:
  if(
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
      goto ctcmc;
   }
  for (i=0; i<3; i++) {
    if (
         (ctcgeomffkeyD.agsize[i]    != CTCGEOMFFKEY.agsize[i]) ||
         (ctcgeomffkeyD.hcsize[i]    != CTCGEOMFFKEY.hcsize[i]) ) {
        rstatus = oocError;
        cout<<"AMScommonsD::CmpConstants -E- CTCGEOMFFKEY  are different "
            <<" and will be set to DBASE values"<<endl;
        CTCGEOMFFKEY = ctcgeomffkeyD;
        goto ctcmc;
    }
  }

ctcmc:
  for (i=0; i<2; i++) {
    if (
         (ctcmcffkeyD.Refraction[i]    != CTCMCFFKEY.Refraction[i]) ||
         (ctcmcffkeyD.AbsLength[i]     != CTCMCFFKEY.AbsLength[i])  ||
         (ctcmcffkeyD.Path2PhEl[i]     != CTCMCFFKEY.Path2PhEl[i]) ) {
        rstatus = oocError;
        cout<<"AMScommonsD::CmpConstants -E- CTCMCFFKEY are different"
           <<" and will be set to DBASE values"<<endl;
        CTCMCFFKEY = ctcmcffkeyD;
    }
  }

  if(
     (ctcrecffkeyD.Thr1 != CTCRECFFKEY.Thr1) ||
     (ctcrecffkeyD.ThrS != CTCRECFFKEY.ThrS) ) {
      rstatus = oocError;
      cout<<"AMScommonsD::CmpConstants -E- CTCRECFFKEY  are different "
           <<" and will be set to DBASE values"<<endl;
      CTCRECFFKEY = ctcrecffkeyD;
   }

  if(
     (trmcffkeyD.alpha != TRMCFFKEY.alpha) ||
     (trmcffkeyD.beta  != TRMCFFKEY.beta) ||
     (trmcffkeyD.gamma != TRMCFFKEY.gamma) ||
     (trmcffkeyD.dedx2nprel != TRMCFFKEY.dedx2nprel) ||
     (trmcffkeyD.fastswitch != TRMCFFKEY.fastswitch) ) {
      rstatus = oocError;
      cout<<"AMScommonsD::CmpConstants -E- TRMCFFKEY  are different "
           <<" and will be set to DBASE values"<<endl;
      TRMCFFKEY = trmcffkeyD;
      goto betafit;
   }
  for (i=0; i<2; i++) {
    if (
         (trmcffkeyD.ped[i]    != TRMCFFKEY.ped[i])   ||
         (trmcffkeyD.sigma[i]  != TRMCFFKEY.sigma[i]) ||
         (trmcffkeyD.gain[i]   != TRMCFFKEY.gain[i]) ||
         (trmcffkeyD.thr[i]    != TRMCFFKEY.thr[i]) ||
         (trmcffkeyD.neib[i]   != TRMCFFKEY.neib[i]) ||
         (trmcffkeyD.cmn[i]    != TRMCFFKEY.cmn[i]) ) {
        rstatus = oocError;
        cout<<"AMScommonsD::CmpConstants -E- TRMCFFKEY  are different "
            <<" and will be set to DBASE values"<<endl;
        TRMCFFKEY = trmcffkeyD;
        break;
    }
  }

betafit:
  for (i=0; i<npatb; i++) {
    if (betafitffkeyD.pattern[i]    != BETAFITFFKEY.pattern[i]) {
      rstatus = oocError;
      cout<<"AMScommonsD::CmpConstants -E- BETAFITFFKEY  are different "
           <<" and will be set to DBASE values"<<endl;
      BETAFITFFKEY = betafitffkeyD;
      goto trfit;
   }
  }
  if (betafitffkeyD.Chi2    != BETAFITFFKEY.Chi2) {
      rstatus = oocError;
      cout<<"AMScommonsD::CmpConstants -E- BETAFITFFKEY  are different "
           <<" and will be set to DBASE values"<<endl;
      BETAFITFFKEY = betafitffkeyD;
      goto trfit;
   }
  for (i=0; i<3; i++) {
    if (betafitffkeyD.SearchReg[i]    != BETAFITFFKEY.SearchReg[i]) {
      rstatus = oocError;
      cout<<"AMScommonsD::CmpConstants -E- BETAFITFFKEY  are different "
           <<" and will be set to DBASE values"<<endl;
      BETAFITFFKEY = betafitffkeyD;
      goto trfit;
   }
  }

  for (i=0; i<npat; i++) {
    if (trfitffkeyD.pattern[i]    != TRFITFFKEY.pattern[i]) {
      rstatus = oocError;
      cout<<"AMScommonsD::CmpConstants -E- BETAFITFFKEY  are different "
           <<" and will be set to DBASE values"<<endl;
      BETAFITFFKEY = betafitffkeyD;
      goto trfit;
   }
  }

trfit:
  if(
      (trfitffkeyD.UseTOF    != TRFITFFKEY.UseTOF) ||
      (trfitffkeyD.Chi2FastFit    != TRFITFFKEY.Chi2FastFit) ||
      (trfitffkeyD.Chi2StrLine    != TRFITFFKEY.Chi2StrLine) ||
      (trfitffkeyD.Chi2Circle     != TRFITFFKEY.Chi2Circle) ||
      (trfitffkeyD.ResCutFastFit  != TRFITFFKEY.ResCutFastFit) ||
      (trfitffkeyD.ResCutStrLine  != TRFITFFKEY.ResCutStrLine) ||
      (trfitffkeyD.ResCutCircle   != TRFITFFKEY.ResCutCircle) ||
      (trfitffkeyD.RidgidityMin   != TRFITFFKEY.RidgidityMin) )
  {
      rstatus = oocError;
      cout<<"AMScommonsD::CmpConstants -E- TRFITFFKEY  are different "
           <<" and will be set to DBASE values"<<endl;
      TRFITFFKEY = trfitffkeyD;
   }

  if(
      (ccffkeyD.fixp     != CCFFKEY.fixp) ||
      (ccffkeyD.albedor  != CCFFKEY.albedor) ||
      (ccffkeyD.albedocz != CCFFKEY.albedocz) ||
      (ccffkeyD.npat     != CCFFKEY.npat) ) {
    //(ccffkeyD.run      != CCFFKEY.run) ) {
      rstatus = oocError;
      cout<<"AMScommonsD::CmpConstants -E- CCFFKEY  are different "
           <<" and will be set to DBASE values"<<endl;
      cout<<" ccffkey.fixp "<<ccffkeyD.fixp<<" "<<CCFFKEY.fixp<<endl;
      cout<<" ccffkeyD.albedor "<<ccffkeyD.albedor<<" "<<CCFFKEY.albedor
          <<endl;
      cout<<" ccffkeyD.albedocz  "<<ccffkeyD.albedocz<<" "<<CCFFKEY.albedocz
          <<endl;
      cout<<"ccffkeyD.npat "<<ccffkeyD.npat<<" "<<CCFFKEY.npat<<endl;
      cout<<"ccffkeyD.run  "<<ccffkeyD.run<<" "<<CCFFKEY.run<<endl;
      CCFFKEY = ccffkeyD;
      goto trcl;
   }
  for(i=0; i<6; i++) {
    if( (ccffkeyD.coo[i]     != CCFFKEY.coo[i]) ||
        (ccffkeyD.dir[i]     != CCFFKEY.dir[i]) ) {
      rstatus = oocError;
      cout<<"AMScommonsD::CmpConstants -E- CCFFKEY  are different "
           <<" and will be set to DBASE values"<<endl;
      cout<<"ccffkey.coo "<<ccffkeyD.coo<<" "<<CCFFKEY.coo<<endl;
      cout<<"ccffkey.dir "<<ccffkeyD.dir<<" "<<CCFFKEY.dir<<endl;
      CCFFKEY = ccffkeyD;
      goto trcl;
    }
  }
  for(i=0; i<2; i++) {
    if( (ccffkeyD.momr[i]     != CCFFKEY.momr[i])) {
      rstatus = oocError;
      cout<<"AMScommonsD::CmpConstants -E- CCFFKEY  are different "
           <<" and will be set to DBASE values"<<endl;
      cout<<"ccffkey.momr "<<ccffkeyD.momr<<" "<<CCFFKEY.momr<<endl;
      CCFFKEY = ccffkeyD;
      goto trcl;
    }
  }

trcl:  
  for(i=0; i<2; i++) {
    if( (trclffkeyD.ThrClA[i]     != TRCLFFKEY.ThrClA[i]) ||
        (trclffkeyD.ThrClS[i]     != TRCLFFKEY.ThrClS[i]) ||
        (trclffkeyD.ThrClR[i]     != TRCLFFKEY.ThrClR[i]) ||
        (trclffkeyD.Thr1A[i]     != TRCLFFKEY.Thr1A[i]) ||
        (trclffkeyD.Thr1S[i]     != TRCLFFKEY.Thr1S[i]) ||
        (trclffkeyD.Thr1R[i]     != TRCLFFKEY.Thr1R[i]) ||
        (trclffkeyD.Thr2A[i]     != TRCLFFKEY.Thr2A[i]) ||
        (trclffkeyD.Thr2S[i]     != TRCLFFKEY.Thr2S[i]) ||
        (trclffkeyD.Thr2R[i]     != TRCLFFKEY.Thr2R[i]) ||
        (trclffkeyD.Thr3R[i]     != TRCLFFKEY.Thr3R[i])) {
      rstatus = oocError;
      cout<<"AMScommonsD::CmpConstants -E- TRCLFFKEY  are different "
           <<" and will be set to DBASE values"<<endl;
      TRCLFFKEY = trclffkeyD;
      break;
    }
  }

  if (rstatus == oocError) 
      cout<<"AMScommonsD::CmpConstants -E- done, discrepancy found "<<endl;
  if (rstatus == oocSuccess) 
      cout<<"AMScommonsD::CmpConstants -I- done"<<endl;

return rstatus;
}
