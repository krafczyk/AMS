//   \example ElectronIdentification.C
///
///  currently just working on electron samples of testbeam
///  needs interface to testbeam table to get Cherenkov EG definitions

#ifndef _PGTRACK_
#define _PGTRACK_
#endif
#include "TrTrackSelection.h"

#include "root.h"
#include "amschain.h"
#include "TFile.h"
#include "TH1.h"
#include <cstring>
#include <TVector3.h>
#include <map>
#include <bitset>

/// Uncomment this to use SH's track selection
///  (supported only with CVS as of 2/Feb/2011 or later)
//#define sel_Tracker_SH

using namespace std;
class ElectronIdentification : public AMSEventR {
public :
  /// This is necessary if you want to run without compilation as:
  ///     chain.Process("ElectronIdentification.C");
#ifdef __CINT__
#include <process_cint.h>
#endif

  ElectronIdentification(){};
  ~ElectronIdentification(){};

  void    UBegin();
  bool    UProcessCut();
  void    UProcessFill();
  void    UTerminate();

  AMSEventList* select_list;
  AMSEventList* badrun_list;
  TrdHReconR *trdhrecon;

  TFile* outfile;
  TH1F* h_ang;
  TH1F* h_ang2;

  bool Trigger_Flag();
  bool Ecal_Geom_Flag(int debug=0);
  bool Cherenkov_Flag(int debug=0);

  bool select_Trd(int debug=0);
  bool select_Ecal(int debug=0);
  bool select_Tracker(int debug=0);
  int nTRTrackLayer(TrTrackR* tr,int& planestype, int& nseg);
   
  bool Ecal_electron(int version,int debug=0);
  bool Ecal_mip(int debug=0);
  bool Trd_electron(int debug=0);

  //additional ecal routines
  bool Ecal_EneMomMatch(float Enedep);
  bool Ecal_StandAlone(float Enedep, float Frac);
 
  int triter;
  int trditer;
  int ecaliter;

  int eventno;

  vector<int> nelectron;
  vector<bool> nmip;

  int particle_type;
};

void ElectronIdentification::UBegin(){
  /*  if(OutputFile()) {
    // This means that you have processed the file with something like:
    //  Process("ElectronIdentification.C+","select.root");
    outfile = OutputFile();
  } else { 
    // This case means that you prefer to define the output file here
    outfile = new TFile ("select.root", "RECREATE"); 
    }*/

  eventno=0;
  triter=-1;
  trditer=-1;
  ecaliter=-1;

  select_list = new AMSEventList;
  badrun_list = new AMSEventList("my_badruns.list");

  trdhrecon=new TrdHReconR();
}

// UProcessCut() is not mandatory
bool ElectronIdentification::UProcessCut(){
  eventno++;
  if (!(eventno%999)) {
    std::printf("Processed Events: %5i\r",(int)eventno);
    std::cout << std::flush;
  }
  if(eventno>10000)return false;

  if (badrun_list->Contains(this)) return false;
  if (Trigger_Flag()==false) return false;

  // selection by MM
  if(!Cherenkov_Flag())return false;
  if(!select_Tracker()) return false;
  if(!Ecal_Geom_Flag()) return false;

  if(!select_Ecal())return false;
  if(!select_Trd())return false;

  return true;
}

void ElectronIdentification::UProcessFill() {

  nmip.push_back(Ecal_mip());

  bool trde=Trd_electron();
  bool ecale=Ecal_electron(1);
  nelectron.push_back(trde+2*ecale);
  
  // Write out selected entries "as we go"
  //if (select_list->Contains(this)) Fill();
};


void ElectronIdentification::UTerminate()
{
  cout << "nmip size " << nmip.size() << " nelectron size " << nelectron.size()<< endl;;

  float nsum=0;
  for(vector<bool>::const_iterator it=nmip.begin();it!=nmip.end();it++)nsum+=(int)*it;
  printf("%i of %i events identified as MIPs\n",(int)nsum,(int)nmip.size());
  
  nsum=0;
  for(vector<int>::const_iterator it=nelectron.begin();it!=nelectron.end();it++)if(*it==1||*it==3)nsum++;
  printf("%i of %i events identified as leptons by Trd\n",(int)nsum,(int)nelectron.size());

  nsum=0;
  for(vector<int>::const_iterator it=nelectron.begin();it!=nelectron.end();it++)if(*it==2||*it==3)nsum++;
  printf("%i of %i events identified as leptons by Ecal\n",(int)nsum,(int)nelectron.size());

  nsum=0;
  for(vector<int>::const_iterator it=nelectron.begin();it!=nelectron.end();it++)if(*it==3)nsum++;
  printf("%i of %i events identified as leptons by both\n",(int)nsum,(int)nelectron.size());
  
  // Write histograms into a ROOT file
  if(!OutputFile()) 
    outfile->Write();

  // Write the list of selected events
  if(select_list)select_list->Write("select.list");

  // Write all selected events into a new root file 
  // TFile* another_file = new TFile("anotherfile.root","RECREATE");
  // select_list->Write(Tree(), another_file);
      
  // Write only Header and Electron branches of selected events
  // (added to the default output file in this example)
  //Tree()->SetBranchStatus("*",0);
  //Tree()->SetBranchStatus("ev.fHeader",1);
  //Tree()->SetBranchStatus("ev.fElectron",1);
  //select_list->Write(Tree(), outfile);

  //  if(Tree())printf("\n>>> We have processed %d events\n\n", (int)Tree()->GetEntries());
  //  if(outfile)printf("\n>>> Histograms saved in '%s'\n\n", outfile->GetName());

}

bool ElectronIdentification::Trigger_Flag() {
  if (nLevel1()<=0) return 0;
  Level1R lvl1 = Level1(0);
        
  // TOF
  int tofok = 0;
  if (lvl1.TofFlag1>=0) tofok = 1; 

  // ECAL
  int ecalok = 0;
  if (lvl1.EcalFlag>1) ecalok = 1; 

  // Anticounters
  const int ANTICUT = 1;
  const float MAGCUT = 0.7;
  const int MAXANTI = 8;
  int antiok = 0;
  unsigned int antipatt = lvl1.AntiPatt>0;
  int nanti = 0;
  for(int i=0;i<MAXANTI;i++){ 
    unsigned int my_bit = 1<<i;
    if( (antipatt & my_bit)>0 || (antipatt & (my_bit<<8))>0 ) nanti += 1;
  }
  float maglat = fabs(fHeader.ThetaM);
  if (maglat>MAGCUT && nanti==0) antiok=1;
  if (maglat<MAGCUT && nanti<=ANTICUT) antiok=1;

  // Ready for trigger flag
  //    return tofok || ecalok ;
  return (tofok && antiok) || ecalok ;

};

bool ElectronIdentification::select_Trd(int debug){
  trditer=-1;
  trdhrecon->BuildTRDEvent(TrdRawHit());
  //  trdhrecon.ReadTRDEvent(TrdRawHit(),TrdHSegment(),TrdHTrack());
  
  int ngood=0;
  float fraction_02=0.;
  for(int n=0;n<(int)trdhrecon->htrvec.size();n++){ 
    int trd_ntracklay[20];
    for(int i=0;i!=20;i++){trd_ntracklay[i]=0;}
    TrdHTrackR *trd_track=&trdhrecon->htrvec.at(n);
    if(!trd_track){
      cerr<<"TrdHTrack "<<n<<" not found"<<endl;
      continue;
    }
    
    for(int seg=0;seg<trd_track->nTrdHSegment();seg++){
      if(!trd_track->pTrdHSegment(seg)){
	cerr<<"TrdHSegment "<<seg<<" not found"<<endl;
	cerr<<"nTrdHSegment in event "<<nTrdHSegment()<<endl;
	cerr<<"nTrdHSegment in track "<<trd_track->NTrdHSegment()<<
	  "iTrdHSegment "<<trd_track->iTrdHSegment(0)<<" "<<trd_track->iTrdHSegment(1)<<endl;
	continue;
      }
      
      for(int i=0;i<(int)trd_track->pTrdHSegment(seg)->fTrdRawHit.size();i++){
	TrdRawHitR* hit=trd_track->pTrdHSegment(seg)->pTrdRawHit(i);
	if(!hit){
	  cerr<<"TrdRawHit "<<trd_track->pTrdHSegment(seg)->iTrdRawHit(i)<<" not found"<<endl;
	  continue;
	}

	float amp=hit->Amp/33.;//rough adc to kev conversion
	if(debug)printf("LLTA %i %i %i %.2f -> %.2f keV\n",hit->Layer,hit->Ladder,hit->Tube,hit->Amp,amp);
	if(amp<0.2)continue;
	trd_ntracklay[hit->Layer]++;
      }
    }
    
    int trd_layers=0;
    int ok[3];
    for(int i=0;i!=3;i++)ok[i]=0;
    for(int i=0;i!=20;i++){
      if(trd_ntracklay[i]>0){
	trd_layers++;
	if(i<4)ok[0]++;
	else if(i<16)ok[1]++;
	else ok[2]++;
      }
    }
 
    int n02=0;
    for(int i=0;i!=nTrdRawHit();i++){
      TrdRawHitR* rhit=pTrdRawHit(i);
      if(!rhit)continue;
      double amp_=rhit->Amp/33.;//rough conversion adc to kev
      if(amp_>0.2)n02++;
    }
    
    fraction_02=double(ok[0]+ok[1]+ok[2])/double(n02);
    
    if(ok[0]<2||ok[1]<8||ok[2]<2||fraction_02<0.8)continue;
    
    ngood++;trditer=n;
  }    
  if(debug)printf("ngood %i\n",ngood);

  return  (ngood==1);

}

bool ElectronIdentification::Ecal_Geom_Flag(int debug){
  if(triter<0)return 0;

  AMSPoint pos;
  AMSDir dir;
  pTrTrack(triter)->Interpolate(-137.,pos,dir);

  float propdist=0.;
  if(fabs(pos.x())<32.9&&fabs(pos.y())<32.9){
    
    float fx=0.,fy=0.,fz;
    if(dir.x()>0)fx=(32.9-pos.x())/dir.x();
    else if(dir.x()<0)fx=(-32.9+pos.x())/dir.x();
    
    if(dir.y()>0)fy=(32.9-pos.y())/dir.y();
    else if(dir.y()<0)fy=(-32.9+pos.y())/dir.y();
    fz=(pos.z()+154.5)/fabs(dir.z());
    
    if(debug)printf("fx %.2f fy %.2f fz %.2f\n",fx,fy,fz);
    float factor=0.;
    if(fx<fy){
      if(fx<fz)factor=fx;
      else factor=fz;}
    else{
      if(fy<fz)factor=fy;
      else factor=fz;}
    
    propdist= dir.norm() * factor;
    if(debug)printf("propdist %.2f = %.2f * %.2f\n",propdist,dir.norm(),factor);
  }

  return (propdist>17.);
}

bool ElectronIdentification::select_Ecal(int debug){
  ecaliter=-1;
  if(nEcalShower()==0||triter<0)return 0;

  int ngoodecal=0;
  for(int i=0;i!=nEcalShower();i++){
    TVector3 showdir;
    showdir.SetXYZ(pEcalShower(i)->EMDir[0],pEcalShower(i)->EMDir[1],pEcalShower(i)->EMDir[2]);
    if(showdir[2]<0.)showdir=-showdir;
    
    TVector3 ec_enter;ec_enter.SetXYZ(pEcalShower(i)->Entry[0],pEcalShower(i)->Entry[1],pEcalShower(i)->Entry[2]);

    float dist=0.;
    AMSPoint tk_pnt;
    AMSDir tk_dir;
    pTrTrack(triter)->Interpolate(ec_enter[2],tk_pnt,tk_dir);
    for(int d=0;d!=3;d++)dist+=pow(tk_pnt[d]-ec_enter[d],2);
    TVector3 lowdir;lowdir.SetXYZ(tk_dir.x(),tk_dir.y(),tk_dir.z());

    if(debug)printf("ECAL to TK angle %.2f dist %.2f\n",showdir.Angle(lowdir),sqrt(dist));
    if(showdir.Angle(lowdir)>0.3||sqrt(dist)>5.)continue;
    ecaliter=i;ngoodecal++;
  }
  if(debug)printf("set ecaliter %i\n",ecaliter);
  return (ngoodecal==1);
}

bool ElectronIdentification::Trd_electron(int debug){
  if(trditer<0)return 0;
  float lik=trdhrecon->htrvec.at(trditer).elikelihood;

  // this cut value is approx 90% electron efficiency - for cleaner sample reduce likelihood cut
  if(debug)printf("trd iter %i lik %.2f e? %i\n",trditer,lik, lik>0.&&lik<0.6);
  return (lik>0.&&lik<0.6);
}

bool ElectronIdentification::Ecal_mip(int debug){
  if(ecaliter<0)return 0;
  
  float energy_uncorr=0.;
  for(int n=0;n<pEcalShower(ecaliter)->NEcal2DCluster();n++){
    Ecal2DClusterR* cl=pEcalShower(ecaliter)->pEcal2DCluster(n);
    if(cl)energy_uncorr+=cl->Edep/1000.;//energy to GeV
    else printf("ecal cluster %i not found",n);
  }
  
  return (energy_uncorr<1.);
}

bool ElectronIdentification::Ecal_electron(int version,int debug){
  int ecand=0;
  if(debug)printf("ecaliter %i\n",ecaliter);
  if(ecaliter<0)return 0;

  if(version==0){
    float energy=pEcalShower(ecaliter)->EnergyC;
    float ecal_showermax=pEcalShower(ecaliter)->ParProfile[1];
    float ecal_chi2=pEcalShower(ecaliter)->Chi2Profile;
    float ecal_enratio_2cm=pEcalShower(ecaliter)->Energy3C[0];
    float ecal_rearleak=pEcalShower(ecaliter)->RearLeak;       
    
    float energy_uncorr=0.;
    for(int n=0;n<pEcalShower(ecaliter)->NEcal2DCluster();n++){
      Ecal2DClusterR* cl=pEcalShower(ecaliter)->pEcal2DCluster(n);
      if(cl)energy_uncorr+=cl->Edep/1000.;//energy to GeV
      else printf("Ecal2DClusterR %i not found\n",n);
    }
    
    float rigidity=pTrTrack(triter)->GetRigidity();
    
    if(ecal_chi2>0.1 && ecal_chi2<2. &&
       ecal_showermax>6. && ecal_showermax<11. &&
       ecal_enratio_2cm>0.945  && ecal_enratio_2cm<0.975 &&
       ecal_rearleak>0.05 && ecal_rearleak <0.2 &&
       energy_uncorr / fabs(rigidity) > 0.9 ) ecand=1;
    
    if(debug){
      printf("ECAND chi2 %.2f shmax %.2f E2cm %.2f rearleak %.2f E %.2f P %.2f EtoP %.2f - ok %i \n",ecal_chi2,ecal_showermax,ecal_enratio_2cm,ecal_rearleak,energy,rigidity,energy/fabs(rigidity),ecand);
    }
  }
  else if(version==1){
    EcalShowerR *shower=pEcalShower(ecaliter);
    
    float Frac=0;
    float Enedep=0;
    
    for(int icl2D=0; icl2D<shower->NEcal2DCluster(); icl2D++)
      for(int icl=0; icl<shower->pEcal2DCluster(icl2D)->NEcalCluster(); icl++)
	for(int ihit=0; ihit<shower->pEcal2DCluster(icl2D)->pEcalCluster(icl)->NEcalHit(); ihit++)
	  {
	    EcalHitR *hit = shower->pEcal2DCluster(icl2D)->pEcalCluster(icl)->pEcalHit(ihit);
	    if(hit->ADC[0]>4)
	      {
		Enedep+=hit->Edep;
		if(hit->Plane==16 || hit->Plane==17) Frac+=hit->Edep;
	      }
	  }
    Frac=Frac/Enedep;
    Enedep/=1000.;
    if(Ecal_StandAlone(Enedep,Frac) && Ecal_EneMomMatch(Enedep)) ecand=1;
     }

  return ecand;
}

bool ElectronIdentification::Ecal_StandAlone(float Enedep, float Frac){
  bool LongCut = false;
  bool LatCut = false;
  
  if((Enedep<5 && Frac<0.056419) || (Enedep>=5 && Frac<0.03544*TMath::Power(Enedep,0.2889))) LongCut=true;
  if( (Enedep<10 && (pEcalShower(ecaliter)->Energy3C[0]>0.907862 && 
		     pEcalShower(ecaliter)->Energy3C[0]<0.988989 ))    
      ||    (Enedep>150 && (pEcalShower(ecaliter)->Energy3C[0]<0.969656 && 
			    pEcalShower(ecaliter)->Energy3C[0]>0.955536 ))
      ||    (Enedep<=150 && Enedep>=10 && 
	     pEcalShower(ecaliter)->Energy3C[0]>0.8692*TMath::Power(Enedep,0.0189) 
	     && pEcalShower(ecaliter)->Energy3C[0]<1.00573*TMath::Power(Enedep,-0.00729))
      ) LatCut=true;
  
//  if(LatCut) hLong->Fill(Enedep,Frac); // !! hLong is not defined !!

  return(LongCut && LatCut);
}

bool ElectronIdentification::Ecal_EneMomMatch(float Enedep){
  
  float Rigidity=pTrTrack(triter)->GetRigidity();
  
  return((1./Rigidity > 0.5138*TMath::Power(Enedep, -0.8875)) || 
	 (1./Rigidity < (-0.5138*TMath::Power(Enedep, -0.8875))) ) ;

}

bool ElectronIdentification::select_Tracker(int debug){

#ifdef sel_Tracker_SH
  // SH: TrTrackSelection
  // supported only with CVS as of 2/Feb/2011 or later
  {
    int algo  = 1; // 1:Choutko  2:Alcaraz  (+10 no multiple scattering)
    int patt  = 7; // 3:Inner only 5:w/ L1N 6:w/ L9 7:Full span
    int refit = 0; // Set 2 if you want to refit

    int span  = TrTrackSelection::kMaxSpan;  // Max span
              //TrTrackSelection::kHalfL1N;  // Half span with L1N
              //TrTrackSelection::kHalfL9;   // Half span with L9
              //TrTrackSelection::kMaxInt;   // Inner only

    // Select the first track for the moment
    int itrk = 0;
    TrTrackR *track = pTrTrack(itrk);

    // Check if the requested fitting exists
    int mfit = track->iTrTrackPar(algo, patt, refit);
    if (mfit < 0) return false;

    // Check span flag
    int sflag = TrTrackSelection::GetSpanFlags(track);
    if (debug) {
      cout << Form("track %d span: %04x", itrk, sflag);
      if      (sflag & TrTrackSelection::kMaxSpan ) cout << " MaxSpan";
      else if (sflag & TrTrackSelection::kHalfL1N ) cout << " Half(L1N)";
      else if (sflag & TrTrackSelection::kHalfL9  ) cout << " Half(L9)";
      else if (sflag & TrTrackSelection::kMaxInt  ) cout << " MaxInt";
      else if (sflag & TrTrackSelection::kAllPlane) cout << " AllPlane";
      cout << endl;
    }

    // Request at least one hit in one plane
    if (!(span & TrTrackSelection::kAllPlane)) return false;

    // Span request
    if (sflag & span) return false;

    // Chisquare selection
    // (ChisqY is more sensitive to wrong momentum events,
    //  so to be applied more tightly)
    if (track->GetNormChisqX(mfit) > 100) return false;
    if (track->GetNormChisqY(mfit) >  20) return false;

    // External residual (only for full span tracks)
    if (span == 7) {
      double rsy = TrTrackSelection::GetHalfRessq(track, span, algo, refit);
      if (rsy > 20) return false;
    }

    // Half rigidity difference
    else {
      double dhr = TrTrackSelection::GetHalfRdiff(track, span, algo, refit);
      if (dhr > 20) return false;
    }

    // Set track index
    triter = itrk;
    return true;
  }
#endif

  // MM: Simple track check
  int ngood=0;
  triter=-1;
  for(int i=0;i!=nTrTrack();i++){
    TrTrackR *tr_track=pTrTrack(i);
    if(!tr_track)continue;
    double recoP=0;
    if(tr_track->GetNhits()<=0)continue;
    recoP=tr_track->GetRigidity();

    int planestype=0;
    int ntrseg=0;
    int ntrtracklay=nTRTrackLayer(tr_track,planestype,ntrseg);
    if(debug)printf("track %i planes %i type %i\n",i,ntrtracklay,planestype);
    if(ntrtracklay<5||planestype==0)continue;
    
    ngood++;
    triter=i;
  }

  if(debug)printf("found %i good tracker tracks - set triter to %i\n",ngood,triter);
  return (ngood==1);
}

int ElectronIdentification::nTRTrackLayer(TrTrackR* tr,int& planestype, int& nseg){
  if(!tr)return 0;

  const int NL = 9;  // For AMS-PM 9 layers

  int lay[NL];
  for(int i=0;i!=NL;i++)lay[i]=0;
/*
  int bits=tr->GetHitBits();
  int tmp=bits;
  for(int i=7;i>=0;i--){
    if (tr->TestHitBits(i+1)
    while((double)tmp/pow((double)2.,(double)i)>=1.){
      lay[i]++;
      if(lay[i]>0)tmp-=(int)pow((double)2,(double)i);
    }
  }
  if(tmp!=0){
    printf("hitbits error\n");
    printf("   initial %i -> %i %i %i %i %i %i %i %i  remain %i\n",bits,
	   lay[0],lay[1],lay[2],lay[3],lay[4],lay[5],lay[6],lay[7],tmp);
    exit(0);
  }
*/
  // SH: Use TrTrackR::TestHitBits instead of pow(2, i)
  for(int i=0;i<NL;i++){
    if (tr->TestHitBits(i+1)) lay[i]++;
  }

  // SH: In case you want FULL span (L1N and L9)
  if(lay[7]>0 && lay[8]>0 )planestype=3;

  // SH: In case you want Half span (L1N or L9)
  else if(lay[7]>0 || lay[8]>0 )planestype=2;

  // SH: In case you want Inner full (L1 and (L6 || L7))
  else if(lay[0]>0 && (lay[5]>0 || lay[6]>0) )planestype=1;

  int n=0;
  for(int i=0;i!=NL;i++)if(lay[i]>0)n++;
  
  return n;
}

bool ElectronIdentification::Cherenkov_Flag(int debug){
  if(nLevel1()!=1)return 0;
  if(!pLevel1(0))return 0;

  bitset<16> mybits (pLevel1(0)->JMembPatt);

  bool eg0=mybits.test(15);
  bool eg1=mybits.test(14);
  
  if(debug)printf("EG0 %i EG1 %i (JMembPattern %i)\n",eg0,eg1,pLevel1(0)->JMembPatt);
  return (eg0&&eg1);
}
