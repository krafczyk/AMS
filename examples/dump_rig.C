
/*
12/Dec/2014:
New example for B9XX/pass6

> root -b -l -q dump_rig.C | & grep -A 6 DumpTrTrackPar-I-Dump:

You should get the following output

  // Refit without magtemp correction

AMSEventR::DumpTrTrackPar-I-Dump: 
Run/Event : 1385000999 325669
btempcor= 1.0093
iTrTrackPar(1, 3,  3, 0.938, 1.0)  Rigidity=   528.263 NormChisqY=    3.570
iTrTrackPar(1, 7, 23, 0.938, 1.0)  Rigidity=   198.455 NormChisqY=    2.506
Hit(L1): PG=  27.0902   2.2742  159.0298  MD=  27.0932   2.2765  159.0324
Hit(L9): PG= -18.9845  -4.0714 -136.0306  MD= -18.9835  -4.0701 -136.0392


  // Refit with magtemp correction

AMSEventR::DumpTrTrackPar-I-Dump: 
Run/Event : 1385000999 325669
Bcorr(already applied on refit  3)= 1.0089
Bcorr(already applied on refit 23)= 1.0089
iTrTrackPar(1, 3,  3, 0.938, 1.0)  Rigidity=   532.990 NormChisqY=    3.570
iTrTrackPar(1, 7, 23, 0.938, 1.0)  Rigidity=   200.247 NormChisqY=    2.507
Hit(L1): PG=  27.0902   2.2742  159.0298  MD=  27.0932   2.2765  159.0324
Hit(L9): PG= -18.9845  -4.0714 -136.0306  MD= -18.9835  -4.0701 -136.0392


  // Stored rigidity with magtemp correction

AMSEventR::DumpTrTrackPar-I-Dump: 
Run/Event : 1385000999 325669
Bcorr(already applied on refit  0)= 1.0089
Bcorr(already applied on refit 20)= 1.0089
iTrTrackPar(1, 3,  0, 0.938, 1.0)  Rigidity=   532.990 NormChisqY=    3.570
iTrTrackPar(1, 7, 20, 0.938, 1.0)  Rigidity=   200.247 NormChisqY=    2.507
Hit(L1): PG=  27.0902   2.2742  159.0298  MD=  27.0932   2.2765  159.0324
Hit(L9): PG= -18.9845  -4.0714 -136.0306  MD= -18.9835  -4.0701 -136.0392
*/

{
  TString arch = gSystem->GetFromPipe(
                 gSystem->ExpandPathName("$ROOTSYS/bin/root-config --arch"));
  TString alib = gSystem->ExpandPathName("$AMSWD/lib/")+arch;
  if (gROOT->GetVersionInt() >= 53400) alib += "5.34";

  TString slc
    = gSystem->GetFromPipe("awk '{print $6}' /etc/redhat-release | cut -c 1");

  if (slc == "6") alib += "/ntuple_slc6_PG.so";
  else            alib += "/ntuple_slc4_PG.so";

  cout << "Loading: " << alib.Data() << endl;
  gSystem->Load(alib);

  TkDBc::UseFinal();

  // pass6 with old alignment
  TString sp1 = "/eos/ams/Data/AMS02/2014/ISS.B930/pass6/";

  // pass6 with new alignment and MD, (PG+MD)/2 rigidities pre-calculated
  TString sp2 = "/eos/ams/Data/AMS02/2014/ISS.B933/pass6/";

  Int_t run   = 1385000999;
  Int_t event = 325669;

  // Refit without magtemp correction: Rigidity= 528.263, 198.455
  AMSEventR::DumpTrTrackPar(run, event);                       // pass4
  AMSEventR::DumpTrTrackPar(run, event, 0, 3, 23, 1, 0, sp1);  // pass6-B930

  // Refit with magtemp correction:    Rigidity= 532.990, 200.247 
  AMSEventR::DumpTrTrackPar(run, event, 0, 3, 23, 1, 1, sp1);  // pass6-B930
  AMSEventR::DumpTrTrackPar(run, event, 0, 3, 23, 1, 1, sp2);  // pass6-B933

  // Stored rigidity in B933/pass6:    Rigidity= 532.990, 200.247 
  AMSEventR::DumpTrTrackPar(run, event, 0, 0, 20, 0, 0, sp2);  // pass6-B933

  // Old checks 
  /*
  AMSEventR::DumpTrTrackPar(1356913970, 205713); // RigidityFS=  212.324
  AMSEventR::DumpTrTrackPar(1356913970, 280313); // RigidityFS=  239.682
  AMSEventR::DumpTrTrackPar(1356913970, 381090); // RigidityFS= 1027.857
  AMSEventR::DumpTrTrackPar(1356913970, 490588); // RigidityFS=  375.203
  AMSEventR::DumpTrTrackPar(1356913970, 565983); // RigidityFS=  327.810
  */
}
