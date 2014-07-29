// 
// File Is Auto Generated 
// All Manual Changes Will Be Lost 
// 
   unsigned int daqevent_length(unsigned int i){return pDaqEvent(i)?pDaqEvent(i)->Length:0;} 
   unsigned int daqevent_tdr(unsigned int i){return pDaqEvent(i)?pDaqEvent(i)->Tdr:0;} 
   unsigned int daqevent_udr(unsigned int i){return pDaqEvent(i)?pDaqEvent(i)->Udr:0;} 
   unsigned int daqevent_sdr(unsigned int i){return pDaqEvent(i)?pDaqEvent(i)->Sdr:0;} 
   unsigned int daqevent_rdr(unsigned int i){return pDaqEvent(i)?pDaqEvent(i)->Rdr:0;} 
   unsigned int daqevent_edr(unsigned int i){return pDaqEvent(i)?pDaqEvent(i)->Edr:0;} 
   unsigned int daqevent_l1dr(unsigned int i){return pDaqEvent(i)?pDaqEvent(i)->L1dr:0;} 
   unsigned int daqevent_l3dr(unsigned int i){return pDaqEvent(i)?pDaqEvent(i)->L3dr:0;} 
   unsigned long long  daqevent_l3error(unsigned int i){return pDaqEvent(i)?pDaqEvent(i)->L3Error:0;} 
   unsigned int  daqevent_l3vevent(unsigned int i){return pDaqEvent(i)?pDaqEvent(i)->L3VEvent:0;} 
   unsigned int  daqevent_l3timed(unsigned int i){return pDaqEvent(i)?pDaqEvent(i)->L3TimeD:0;} 
   unsigned short int daqevent_jinjstatus(unsigned int j, unsigned int i){return (j<4 && pDaqEvent(i))?pDaqEvent(i)->JINJStatus[j]:0;} 
   unsigned char daqevent_jerror(unsigned int j, unsigned int i){return (j<24 && pDaqEvent(i))?pDaqEvent(i)->JError[j]:0;} 
   unsigned int   ecalhit_status(unsigned int i){return pEcalHit(i)?pEcalHit(i)->Status:0;} 
   short   ecalhit_idsoft(unsigned int i){return pEcalHit(i)?pEcalHit(i)->Idsoft:0;} 
   short   ecalhit_proj(unsigned int i){return pEcalHit(i)?pEcalHit(i)->Proj:0;} 
   short   ecalhit_plane(unsigned int i){return pEcalHit(i)?pEcalHit(i)->Plane:0;} 
   short   ecalhit_cell(unsigned int i){return pEcalHit(i)?pEcalHit(i)->Cell:0;} 
   float ecalhit_edep(unsigned int i){return pEcalHit(i)?pEcalHit(i)->Edep:0;} 
   float ecalhit_edcorr(unsigned int i){return pEcalHit(i)?pEcalHit(i)->EdCorr:0;} 
   float ecalhit_attcor(unsigned int i){return pEcalHit(i)?pEcalHit(i)->AttCor:0;} 
   float ecalhit_coo(unsigned int j, unsigned int i){return (j<3 && pEcalHit(i))?pEcalHit(i)->Coo[j]:0;} 
   float ecalhit_adc(unsigned int j, unsigned int i){return (j<3 && pEcalHit(i))?pEcalHit(i)->ADC[j]:0;} 
   float ecalhit_gain(unsigned int i){return pEcalHit(i)?pEcalHit(i)->Gain:0;} 
   unsigned int ecalcluster_status(unsigned int i){return pEcalCluster(i)?pEcalCluster(i)->Status:0;} 
   int ecalcluster_proj(unsigned int i){return pEcalCluster(i)?pEcalCluster(i)->Proj:0;} 
   int ecalcluster_plane(unsigned int i){return pEcalCluster(i)?pEcalCluster(i)->Plane:0;} 
   int ecalcluster_left(unsigned int i){return pEcalCluster(i)?pEcalCluster(i)->Left:0;} 
   int ecalcluster_center(unsigned int i){return pEcalCluster(i)?pEcalCluster(i)->Center:0;} 
   int ecalcluster_right(unsigned int i){return pEcalCluster(i)?pEcalCluster(i)->Right:0;} 
   float ecalcluster_edep(unsigned int i){return pEcalCluster(i)?pEcalCluster(i)->Edep:0;} 
   float ecalcluster_sideleak(unsigned int i){return pEcalCluster(i)?pEcalCluster(i)->SideLeak:0;} 
   float ecalcluster_deadleak(unsigned int i){return pEcalCluster(i)?pEcalCluster(i)->DeadLeak:0;} 
   float ecalcluster_coo(unsigned int j, unsigned int i){return (j<3 && pEcalCluster(i))?pEcalCluster(i)->Coo[j]:0;} 
int ecalcluster_fecalhit(unsigned int j, unsigned int i){return ( pEcalCluster(i) && j<pEcalCluster(i)->fEcalHit.size() )?pEcalCluster(i)->fEcalHit[j]:0;} 
int ecalcluster_fecalhit_size(unsigned int i){return ( pEcalCluster(i)  )?pEcalCluster(i)->fEcalHit.size():0;} 
   unsigned int ecal2dcluster_status(unsigned int i){return pEcal2DCluster(i)?pEcal2DCluster(i)->Status:0;} 
   int ecal2dcluster_proj(unsigned int i){return pEcal2DCluster(i)?pEcal2DCluster(i)->Proj:0;} 
   float ecal2dcluster_edep(unsigned int i){return pEcal2DCluster(i)?pEcal2DCluster(i)->Edep:0;} 
   float ecal2dcluster_coo(unsigned int i){return pEcal2DCluster(i)?pEcal2DCluster(i)->Coo:0;} 
   float ecal2dcluster_tan(unsigned int i){return pEcal2DCluster(i)?pEcal2DCluster(i)->Tan:0;} 
   float ecal2dcluster_chi2(unsigned int i){return pEcal2DCluster(i)?pEcal2DCluster(i)->Chi2:0;} 
int ecal2dcluster_fecalcluster(unsigned int j, unsigned int i){return ( pEcal2DCluster(i) && j<pEcal2DCluster(i)->fEcalCluster.size() )?pEcal2DCluster(i)->fEcalCluster[j]:0;} 
int ecal2dcluster_fecalcluster_size(unsigned int i){return ( pEcal2DCluster(i)  )?pEcal2DCluster(i)->fEcalCluster.size():0;} 
   unsigned int   ecalshower_status(unsigned int i){return pEcalShower(i)?pEcalShower(i)->Status:0;} 
   float ecalshower_dir(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->Dir[j]:0;} 
   float ecalshower_dircr(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->DirCR[j]:0;} 
   float ecalshower_emdir(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->EMDir[j]:0;} 
   float ecalshower_entry(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->Entry[j]:0;} 
   float ecalshower_exit(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->Exit[j]:0;} 
   float ecalshower_cofg(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->CofG[j]:0;} 
   float ecalshower_cofgcr(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->CofGCR[j]:0;} 
   float ecalshower_erdir(unsigned int i){return pEcalShower(i)?pEcalShower(i)->ErDir:0;} 
   float ecalshower_erdire(unsigned int i){return pEcalShower(i)?pEcalShower(i)->ErDirE:0;} 
   float ecalshower_chi2dir(unsigned int i){return pEcalShower(i)?pEcalShower(i)->Chi2Dir:0;} 
   float ecalshower_firstlayeredep(unsigned int i){return pEcalShower(i)?pEcalShower(i)->FirstLayerEdep:0;} 
   float ecalshower_energyd(unsigned int i){return pEcalShower(i)?pEcalShower(i)->EnergyD:0;} 
   float ecalshower_energyc(unsigned int i){return pEcalShower(i)?pEcalShower(i)->EnergyC:0;} 
   float ecalshower_energye(unsigned int i){return pEcalShower(i)?pEcalShower(i)->EnergyE:0;} 
   float ecalshower_energye2014(unsigned int i){return pEcalShower(i)?pEcalShower(i)->EnergyE2014:0;} 
   float ecalshower_elayer_corr(unsigned int j, unsigned int i){return (j<18 && pEcalShower(i))?pEcalShower(i)->Elayer_corr[j]:0;} 
   float ecalshower_energy3c(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->Energy3C[j]:0;} 
   float  ecalshower_s13r(unsigned int i){return pEcalShower(i)?pEcalShower(i)->S13R:0;} 
   float  ecalshower_s13rpi(unsigned int j, unsigned int i){return (j<2 && pEcalShower(i))?pEcalShower(i)->S13Rpi[j]:0;} 
   float ecalshower_erenergyc(unsigned int i){return pEcalShower(i)?pEcalShower(i)->ErEnergyC:0;} 
   float ecalshower_erenergye(unsigned int i){return pEcalShower(i)?pEcalShower(i)->ErEnergyE:0;} 
   float ecalshower_erenergye2014(unsigned int i){return pEcalShower(i)?pEcalShower(i)->ErEnergyE2014:0;} 
   float ecalshower_difosum(unsigned int i){return pEcalShower(i)?pEcalShower(i)->DifoSum:0;} 
   float ecalshower_sideleak(unsigned int i){return pEcalShower(i)?pEcalShower(i)->SideLeak:0;} 
   float ecalshower_rearleak(unsigned int i){return pEcalShower(i)?pEcalShower(i)->RearLeak:0;} 
   float ecalshower_deadleak(unsigned int i){return pEcalShower(i)?pEcalShower(i)->DeadLeak:0;} 
   float ecalshower_attleak(unsigned int i){return pEcalShower(i)?pEcalShower(i)->AttLeak:0;} 
   float ecalshower_sideleakpi(unsigned int i){return pEcalShower(i)?pEcalShower(i)->SideLeakPI:0;} 
   float ecalshower_rearleakpi(unsigned int i){return pEcalShower(i)?pEcalShower(i)->RearLeakPI:0;} 
   float ecalshower_deadleakpi(unsigned int i){return pEcalShower(i)?pEcalShower(i)->DeadLeakPI:0;} 
   float ecalshower_attleakpi(unsigned int i){return pEcalShower(i)?pEcalShower(i)->AttLeakPI:0;} 
   float ecalshower_nlinleak(unsigned int i){return pEcalShower(i)?pEcalShower(i)->NLinLeak:0;} 
   float ecalshower_nlinleakpi(unsigned int i){return pEcalShower(i)?pEcalShower(i)->NLinLeakPI:0;} 
   float ecalshower_s13leak(unsigned int i){return pEcalShower(i)?pEcalShower(i)->S13Leak:0;} 
   float ecalshower_s13leakxpi(unsigned int i){return pEcalShower(i)?pEcalShower(i)->S13LeakXPI:0;} 
   float ecalshower_s13leakypi(unsigned int i){return pEcalShower(i)?pEcalShower(i)->S13LeakYPI:0;} 
   float ecalshower_orpleak(unsigned int i){return pEcalShower(i)?pEcalShower(i)->OrpLeak:0;} 
   float ecalshower_orpleakpi(unsigned int i){return pEcalShower(i)?pEcalShower(i)->OrpLeakPI:0;} 
   float ecalshower_energy0a(unsigned int j, unsigned int i){return (j<2 && pEcalShower(i))?pEcalShower(i)->Energy0A[j]:0;} 
   float ecalshower_energya(unsigned int i){return pEcalShower(i)?pEcalShower(i)->EnergyA:0;} 
   float ecalshower_erenergya(unsigned int i){return pEcalShower(i)?pEcalShower(i)->ErEnergyA:0;} 
   float ecalshower_sideleaka(unsigned int i){return pEcalShower(i)?pEcalShower(i)->SideLeakA:0;} 
   float ecalshower_rearleaka(unsigned int i){return pEcalShower(i)?pEcalShower(i)->RearLeakA:0;} 
   float ecalshower_deadleaka(unsigned int i){return pEcalShower(i)?pEcalShower(i)->DeadLeakA:0;} 
   float ecalshower_attleaka(unsigned int i){return pEcalShower(i)?pEcalShower(i)->AttLeakA:0;} 
   float ecalshower_nlinleaka(unsigned int i){return pEcalShower(i)?pEcalShower(i)->NLinLeakA:0;} 
   float ecalshower_s13leakxa0(unsigned int i){return pEcalShower(i)?pEcalShower(i)->S13LeakXA0:0;} 
   float ecalshower_s13leakya0(unsigned int i){return pEcalShower(i)?pEcalShower(i)->S13LeakYA0:0;} 
   float ecalshower_s13leakxa(unsigned int i){return pEcalShower(i)?pEcalShower(i)->S13LeakXA:0;} 
   float ecalshower_s13leakya(unsigned int i){return pEcalShower(i)?pEcalShower(i)->S13LeakYA:0;} 
   float ecalshower_orpleaka(unsigned int i){return pEcalShower(i)?pEcalShower(i)->OrpLeakA:0;} 
   float ecalshower_s35leakxa(unsigned int i){return pEcalShower(i)?pEcalShower(i)->S35LeakXA:0;} 
   float ecalshower_s35leakya(unsigned int i){return pEcalShower(i)?pEcalShower(i)->S35LeakYA:0;} 
   float ecalshower_varleakxa(unsigned int i){return pEcalShower(i)?pEcalShower(i)->VarLeakXA:0;} 
   float ecalshower_varleakya(unsigned int i){return pEcalShower(i)?pEcalShower(i)->VarLeakYA:0;} 
   float ecalshower_s13ra(unsigned int j, unsigned int i){return (j<2 && pEcalShower(i))?pEcalShower(i)->S13Ra[j]:0;} 
   float ecalshower_s35ra(unsigned int j, unsigned int i){return (j<2 && pEcalShower(i))?pEcalShower(i)->S35Ra[j]:0;} 
   float ecalshower_s1tot(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->S1tot[j]:0;} 
   float ecalshower_s3tot(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->S3tot[j]:0;} 
   float ecalshower_s5tot(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->S5tot[j]:0;} 
   float ecalshower_orp2denergy(unsigned int i){return pEcalShower(i)?pEcalShower(i)->Orp2DEnergy:0;} 
   float ecalshower_chi2profile(unsigned int i){return pEcalShower(i)?pEcalShower(i)->Chi2Profile:0;} 
   float ecalshower_chi2profileback(unsigned int i){return pEcalShower(i)?pEcalShower(i)->Chi2ProfileBack:0;} 
   float ecalshower_parprofile(unsigned int j, unsigned int i){return (j<4 && pEcalShower(i))?pEcalShower(i)->ParProfile[j]:0;} 
   float ecalshower_chi2trans(unsigned int i){return pEcalShower(i)?pEcalShower(i)->Chi2Trans:0;} 
   float ecalshower_sphericityev(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->SphericityEV[j]:0;} 
   int ecalshower_nhits(unsigned int i){return pEcalShower(i)?pEcalShower(i)->Nhits:0;} 
int ecalshower_fecal2dcluster(unsigned int j, unsigned int i){return ( pEcalShower(i) && j<pEcalShower(i)->fEcal2DCluster.size() )?pEcalShower(i)->fEcal2DCluster[j]:0;} 
int ecalshower_fecal2dcluster_size(unsigned int i){return ( pEcalShower(i)  )?pEcalShower(i)->fEcal2DCluster.size():0;} 
   int ecalshower_nblayerx(unsigned int i){return pEcalShower(i)?pEcalShower(i)->NbLayerX:0;} 
   int ecalshower_nblayery(unsigned int i){return pEcalShower(i)?pEcalShower(i)->NbLayerY:0;} 
   float ecalshower_showerlatdisp(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->ShowerLatDisp[j]:0;} 
   float ecalshower_showerlongdisp(unsigned int i){return pEcalShower(i)?pEcalShower(i)->ShowerLongDisp:0;} 
   float ecalshower_showerdepth(unsigned int i){return pEcalShower(i)?pEcalShower(i)->ShowerDepth:0;} 
   float ecalshower_showerfootprint(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->ShowerFootprint[j]:0;} 
   float ecalshower_zprofilechi2(unsigned int i){return pEcalShower(i)?pEcalShower(i)->ZprofileChi2:0;} 
   float ecalshower_zprofile(unsigned int j, unsigned int i){return (j<4 && pEcalShower(i))?pEcalShower(i)->Zprofile[j]:0;} 
   float ecalshower_zprofilechi2v2(unsigned int i){return pEcalShower(i)?pEcalShower(i)->ZprofileChi2v2:0;} 
   float ecalshower_zprofilev2(unsigned int j, unsigned int i){return (j<4 && pEcalShower(i))?pEcalShower(i)->Zprofilev2[j]:0;} 
   float ecalshower_energyf(unsigned int i){return pEcalShower(i)?pEcalShower(i)->EnergyF:0;} 
   float ecalshower_energyfractionlayer(unsigned int j, unsigned int i){return (j<18 && pEcalShower(i))?pEcalShower(i)->EnergyFractionLayer[j]:0;} 
   float ecalshower_ns1tot(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->NS1tot[j]:0;} 
   float ecalshower_ns3tot(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->NS3tot[j]:0;} 
   float ecalshower_ns5tot(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->NS5tot[j]:0;} 
   float ecalshower_nshowerlatdisp(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->NShowerLatDisp[j]:0;} 
   float ecalshower_nshowerlongdisp(unsigned int i){return pEcalShower(i)?pEcalShower(i)->NShowerLongDisp:0;} 
   float ecalshower_nshowerfootprint(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->NShowerFootprint[j]:0;} 
   float ecalshower_nzprofile(unsigned int j, unsigned int i){return (j<4 && pEcalShower(i))?pEcalShower(i)->NZprofile[j]:0;} 
   float ecalshower_nzprofilechi2(unsigned int i){return pEcalShower(i)?pEcalShower(i)->NZprofileChi2:0;} 
   float ecalshower_nzprofilemaxratio(unsigned int i){return pEcalShower(i)?pEcalShower(i)->NZProfileMaxRatio:0;} 
   float ecalshower_nenergyfractionlayer(unsigned int j, unsigned int i){return (j<18 && pEcalShower(i))?pEcalShower(i)->NEnergyFractionLayer[j]:0;} 
   float ecalshower_ns1s3(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->NS1S3[j]:0;} 
   float ecalshower_ns3s5(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->NS3S5[j]:0;} 
   float ecalshower_ns13r(unsigned int i){return pEcalShower(i)?pEcalShower(i)->NS13R:0;} 
   float ecalshower_nenergy3c2(unsigned int i){return pEcalShower(i)?pEcalShower(i)->NEnergy3C2:0;} 
   float ecalshower_nenergy3c3(unsigned int i){return pEcalShower(i)?pEcalShower(i)->NEnergy3C3:0;} 
   int tofrawside_swid(unsigned int i){return pTofRawSide(i)?pTofRawSide(i)->swid:0;} 
   int tofrawside_hwidt(unsigned int i){return pTofRawSide(i)?pTofRawSide(i)->hwidt:0;} 
   int tofrawside_hwidq(unsigned int j, unsigned int i){return (j<4 && pTofRawSide(i))?pTofRawSide(i)->hwidq[j]:0;} 
   int tofrawside_stat(unsigned int i){return pTofRawSide(i)?pTofRawSide(i)->stat:0;} 
   int tofrawside_nftdc(unsigned int i){return pTofRawSide(i)?pTofRawSide(i)->nftdc:0;} 
int tofrawside_fftdc(unsigned int j, unsigned int i){return ( pTofRawSide(i) && j<pTofRawSide(i)->fftdc.size() )?pTofRawSide(i)->fftdc[j]:0;} 
int tofrawside_fftdc_size(unsigned int i){return ( pTofRawSide(i)  )?pTofRawSide(i)->fftdc.size():0;} 
   int tofrawside_nstdc(unsigned int i){return pTofRawSide(i)?pTofRawSide(i)->nstdc:0;} 
int tofrawside_fstdc(unsigned int j, unsigned int i){return ( pTofRawSide(i) && j<pTofRawSide(i)->fstdc.size() )?pTofRawSide(i)->fstdc[j]:0;} 
int tofrawside_fstdc_size(unsigned int i){return ( pTofRawSide(i)  )?pTofRawSide(i)->fstdc.size():0;} 
   int tofrawside_nsumh(unsigned int i){return pTofRawSide(i)?pTofRawSide(i)->nsumh:0;} 
int tofrawside_fsumht(unsigned int j, unsigned int i){return ( pTofRawSide(i) && j<pTofRawSide(i)->fsumht.size() )?pTofRawSide(i)->fsumht[j]:0;} 
int tofrawside_fsumht_size(unsigned int i){return ( pTofRawSide(i)  )?pTofRawSide(i)->fsumht.size():0;} 
   int tofrawside_nsumsh(unsigned int i){return pTofRawSide(i)?pTofRawSide(i)->nsumsh:0;} 
int tofrawside_fsumsht(unsigned int j, unsigned int i){return ( pTofRawSide(i) && j<pTofRawSide(i)->fsumsht.size() )?pTofRawSide(i)->fsumsht[j]:0;} 
int tofrawside_fsumsht_size(unsigned int i){return ( pTofRawSide(i)  )?pTofRawSide(i)->fsumsht.size():0;} 
   float tofrawside_adca(unsigned int i){return pTofRawSide(i)?pTofRawSide(i)->adca:0;} 
   int tofrawside_nadcd(unsigned int i){return pTofRawSide(i)?pTofRawSide(i)->nadcd:0;} 
   float tofrawside_adcd(unsigned int j, unsigned int i){return (j<3 && pTofRawSide(i))?pTofRawSide(i)->adcd[j]:0;} 
   float tofrawside_temp(unsigned int i){return pTofRawSide(i)?pTofRawSide(i)->temp:0;} 
   float tofrawside_tempc(unsigned int i){return pTofRawSide(i)?pTofRawSide(i)->tempC:0;} 
   float tofrawside_tempp(unsigned int i){return pTofRawSide(i)?pTofRawSide(i)->tempP:0;} 
   unsigned int   tofrawcluster_status(unsigned int i){return pTofRawCluster(i)?pTofRawCluster(i)->Status:0;} 
   int   tofrawcluster_layer(unsigned int i){return pTofRawCluster(i)?pTofRawCluster(i)->Layer:0;} 
   int   tofrawcluster_bar(unsigned int i){return pTofRawCluster(i)?pTofRawCluster(i)->Bar:0;} 
   float tofrawcluster_adca(unsigned int j, unsigned int i){return (j<2 && pTofRawCluster(i))?pTofRawCluster(i)->adca[j]:0;} 
   float tofrawcluster_adcd(unsigned int j, unsigned int i){return (j<2 && pTofRawCluster(i))?pTofRawCluster(i)->adcd[j]:0;} 
   float tofrawcluster_adcdr(unsigned int j, unsigned int k, unsigned int i){return (j<2 && k<3 && pTofRawCluster(i))?pTofRawCluster(i)->adcdr[j][k]:0;} 
   float tofrawcluster_sdtm(unsigned int j, unsigned int i){return (j<2 && pTofRawCluster(i))?pTofRawCluster(i)->sdtm[j]:0;} 
   float tofrawcluster_edepa(unsigned int i){return pTofRawCluster(i)?pTofRawCluster(i)->edepa:0;} 
   float tofrawcluster_edepd(unsigned int i){return pTofRawCluster(i)?pTofRawCluster(i)->edepd:0;} 
   float tofrawcluster_time(unsigned int i){return pTofRawCluster(i)?pTofRawCluster(i)->time:0;} 
   float tofrawcluster_cool(unsigned int i){return pTofRawCluster(i)?pTofRawCluster(i)->cool:0;} 
int tofrawcluster_ftofrawside(unsigned int j, unsigned int i){return ( pTofRawCluster(i) && j<pTofRawCluster(i)->fTofRawSide.size() )?pTofRawCluster(i)->fTofRawSide[j]:0;} 
int tofrawcluster_ftofrawside_size(unsigned int i){return ( pTofRawCluster(i)  )?pTofRawCluster(i)->fTofRawSide.size():0;} 
   int antirawside_swid(unsigned int i){return pAntiRawSide(i)?pAntiRawSide(i)->swid:0;} 
   int antirawside_stat(unsigned int i){return pAntiRawSide(i)?pAntiRawSide(i)->stat:0;} 
   float antirawside_temp(unsigned int i){return pAntiRawSide(i)?pAntiRawSide(i)->temp:0;} 
   float antirawside_adca(unsigned int i){return pAntiRawSide(i)?pAntiRawSide(i)->adca:0;} 
   int antirawside_nftdc(unsigned int i){return pAntiRawSide(i)?pAntiRawSide(i)->nftdc:0;} 
   int antirawside_ftdc(unsigned int j, unsigned int i){return (j<8 && pAntiRawSide(i))?pAntiRawSide(i)->ftdc[j]:0;} 
   int antirawside_ntdct(unsigned int i){return pAntiRawSide(i)?pAntiRawSide(i)->ntdct:0;} 
   int antirawside_tdct(unsigned int j, unsigned int i){return (j<16 && pAntiRawSide(i))?pAntiRawSide(i)->tdct[j]:0;} 
   unsigned int tofcluster_status(unsigned int i){return pTofCluster(i)?pTofCluster(i)->Status:0;} 
   int tofcluster_layer(unsigned int i){return pTofCluster(i)?pTofCluster(i)->Layer:0;} 
   int tofcluster_bar(unsigned int i){return pTofCluster(i)?pTofCluster(i)->Bar:0;} 
   float tofcluster_edep(unsigned int i){return pTofCluster(i)?pTofCluster(i)->Edep:0;} 
   float tofcluster_edepd(unsigned int i){return pTofCluster(i)?pTofCluster(i)->Edepd:0;} 
   float tofcluster_time(unsigned int i){return pTofCluster(i)?pTofCluster(i)->Time:0;} 
   float tofcluster_errtime(unsigned int i){return pTofCluster(i)?pTofCluster(i)->ErrTime:0;} 
   float tofcluster_coo(unsigned int j, unsigned int i){return (j<3 && pTofCluster(i))?pTofCluster(i)->Coo[j]:0;} 
   float tofcluster_errorcoo(unsigned int j, unsigned int i){return (j<3 && pTofCluster(i))?pTofCluster(i)->ErrorCoo[j]:0;} 
int tofcluster_ftofrawcluster(unsigned int j, unsigned int i){return ( pTofCluster(i) && j<pTofCluster(i)->fTofRawCluster.size() )?pTofCluster(i)->fTofRawCluster[j]:0;} 
int tofcluster_ftofrawcluster_size(unsigned int i){return ( pTofCluster(i)  )?pTofCluster(i)->fTofRawCluster.size():0;} 
    unsigned int richhit_status(unsigned int i){return pRichHit(i)?pRichHit(i)->Status:0;} 
   int   richhit_channel(unsigned int i){return pRichHit(i)?pRichHit(i)->Channel:0;} 
   int   richhit_counts(unsigned int i){return pRichHit(i)?pRichHit(i)->Counts:0;} 
   float richhit_npe(unsigned int i){return pRichHit(i)?pRichHit(i)->Npe:0;} 
   float richhit_coo(unsigned int j, unsigned int i){return (j<3 && pRichHit(i))?pRichHit(i)->Coo[j]:0;} 
   int   richhit_hwaddress(unsigned int i){return pRichHit(i)?pRichHit(i)->HwAddress:0;} 
   int richhit_softid(unsigned int i){return pRichHit(i)?pRichHit(i)->SoftId:0;} 
