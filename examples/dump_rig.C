
/*
You should get the following dump:

AMSEventR::DumpTrTrackPar-I-Dump: 
Run/Event : 1356913970 205713
btempcor= 1.0103
iTrTrackPar(1, 3,  3)=      17 Rigidity=  -277.397 NormChisqY=    4.930
iTrTrackPar(1, 3, 23)= 8391697 Rigidity=   212.324 NormChisqY=    5.439
HeaderR-W-EventSeqSeemsToBeBroken 280313 205713 0

AMSEventR::DumpTrTrackPar-I-Dump: 
Run/Event : 1356913970 280313
btempcor= 1.0103
iTrTrackPar(1, 3,  3)=      17 Rigidity=  -171.346 NormChisqY=    0.537
iTrTrackPar(1, 3, 23)= 8391697 Rigidity=   239.682 NormChisqY=    1.454
HeaderR-W-EventSeqSeemsToBeBroken 381090 280313 0

AMSEventR::DumpTrTrackPar-I-Dump: 
Run/Event : 1356913970 381090
btempcor= 1.0103
iTrTrackPar(1, 3,  3)=      17 Rigidity=   448.964 NormChisqY=    0.947
iTrTrackPar(1, 3, 23)= 8391697 Rigidity=  1027.857 NormChisqY=    0.897
HeaderR-W-EventSeqSeemsToBeBroken 490588 381090 0

AMSEventR::DumpTrTrackPar-I-Dump: 
Run/Event : 1356913970 490588
btempcor= 1.0103
iTrTrackPar(1, 3,  3)=      17 Rigidity=  1082.850 NormChisqY=    0.332
iTrTrackPar(1, 3, 23)= 8391697 Rigidity=   375.203 NormChisqY=    1.212
HeaderR-W-EventSeqSeemsToBeBroken 565983 490588 0

AMSEventR::DumpTrTrackPar-I-Dump: 
Run/Event : 1356913970 565983
btempcor= 1.0103
iTrTrackPar(1, 3,  3)=      17 Rigidity=   285.653 NormChisqY=    0.209
iTrTrackPar(1, 3, 23)= 8391697 Rigidity=   327.810 NormChisqY=    0.243
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
