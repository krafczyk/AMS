
/*
You should get the following dump:

AMSEventR::DumpTrTrackPar-I-Dump: 
Run/Event : 1356913970 205713
btempcor= 1.0103
iTrTrackPar(1, 3,  3)=      17 Rigidity=  -277.397 NormChisqY=    4.930
iTrTrackPar(1, 7, 23)= 8391697 Rigidity=   210.638 NormChisqY=    5.468
Hit(L1): PG=  24.5378  -3.4279  159.0276  MD=  24.5402  -3.4267  159.0322
Hit(L9): PG=  28.4353 -12.3547 -136.0149  MD=  28.4360 -12.3567 -136.0137
HeaderR-W-EventSeqSeemsToBeBroken 280313 205713 0

AMSEventR::DumpTrTrackPar-I-Dump: 
Run/Event : 1356913970 280313
btempcor= 1.0103
iTrTrackPar(1, 3,  3)=      17 Rigidity=  -171.346 NormChisqY=    0.537
iTrTrackPar(1, 7, 23)= 8391697 Rigidity=   236.841 NormChisqY=    1.470
Hit(L1): PG=  13.5556  -2.0039  159.0275  MD=  13.5577  -2.0032  159.0313
Hit(L9): PG= -26.4866 -15.1504 -136.0509  MD= -26.4859 -15.1497 -136.0496
HeaderR-W-EventSeqSeemsToBeBroken 381090 280313 0

AMSEventR::DumpTrTrackPar-I-Dump: 
Run/Event : 1356913970 381090
btempcor= 1.0103
iTrTrackPar(1, 3,  3)=      17 Rigidity=   448.964 NormChisqY=    0.947
iTrTrackPar(1, 7, 23)= 8391697 Rigidity=   832.785 NormChisqY=    0.854
Hit(L1): PG=  34.9135  47.0937  159.0437  MD=  34.9157  47.0937  159.0493
Hit(L9): PG=  20.9024 -16.3117 -136.0151  MD=  20.9034 -16.3129 -136.0195
HeaderR-W-EventSeqSeemsToBeBroken 490588 381090 0

AMSEventR::DumpTrTrackPar-I-Dump: 
Run/Event : 1356913970 490588
btempcor= 1.0103
iTrTrackPar(1, 3,  3)=      17 Rigidity=  1082.850 NormChisqY=    0.332
iTrTrackPar(1, 7, 23)= 8391697 Rigidity=   372.941 NormChisqY=    1.202
Hit(L1): PG=  16.2686  30.6020  159.0432  MD=  16.2703  30.6018  159.0470
Hit(L9): PG= -21.0044  29.2664 -136.0334  MD= -21.0016  29.2671 -136.0248
HeaderR-W-EventSeqSeemsToBeBroken 565983 490588 0

AMSEventR::DumpTrTrackPar-I-Dump: 
Run/Event : 1356913970 565983
btempcor= 1.0103
iTrTrackPar(1, 3,  3)=      17 Rigidity=   285.653 NormChisqY=    0.209
iTrTrackPar(1, 7, 23)= 8391697 Rigidity=   325.227 NormChisqY=    0.240
Hit(L1): PG=  12.8557  -8.8739  159.0282  MD=  12.8564  -8.8744  159.0325
Hit(L9): PG= -27.7805 -13.5129 -136.0524  MD= -27.7791 -13.5119 -136.0527
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
  AMSEventR::DumpTrTrackPar(1356913970, 205713); // RigidityFS=  212.324
  AMSEventR::DumpTrTrackPar(1356913970, 280313); // RigidityFS=  239.682
  AMSEventR::DumpTrTrackPar(1356913970, 381090); // RigidityFS= 1027.857
  AMSEventR::DumpTrTrackPar(1356913970, 490588); // RigidityFS=  375.203
  AMSEventR::DumpTrTrackPar(1356913970, 565983); // RigidityFS=  327.810
}
