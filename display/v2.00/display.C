
//void display(char *rootFile=0)

{
  gROOT->Reset();
//enum EAMSR_View {kFrontView=0, kSideView=1, kTopView=2, kAllView=3, kTwoView=4};
  gSystem->Load("bin/libAMS.so");
//  debugger.On();                // turn on printing messages
  debugger.Off();		// turn off printing messages


  AMSR_Root amsroot("AMS", "AMS Display");

  TFile fgeo("ams_group.root");
  TGeometry * geo = fgeo.Get("ams");
  AMSR_Display * amd= new AMSR_Display("AMSR_Root Event Display", geo, 1024,768);

  //  amd->SetView(kTwoView);
  amd->SetNextView(4);

  // char filename[200];
  // amsroot.OpenDataFile(filename);
  // amsroot.OpenDataFile("Reco-3.root");
  // amsroot.OpenDataFile("root://ihp-power8.ethz.ch:8195//u5/yesw/software/tools/display/newclass/Reco-3.root");

  //
  // Set IdleTimer
  //
  amd->SetIdleTimer(5,"IdleHandle(0)");

  amd->GetCanvas()->OpenFileCB();


//  amsroot.GetEvent(0);
//  amd->ShowNextEvent(-1);

}

// ===============================================================
//  !!! Must execute the following macro under ROOT prompt !!!
//      before activate the IdleTimer
// ===============================================================

//
// Load "IdleHandle()" as handler of IdleTimer
//
  .L IdleHandle.C

//
// Initialize IdleHandle()
//
  IdleHandle(-1);
