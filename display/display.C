{
  gROOT->Reset();
  gSystem->Load("libAMS.so");
  debugger.Off();		// debugger.On() to turn on printing messages
//TFile f("t66.root");
//TFile f("tof.root");
//TFile f("tof025152-025156.root");
//TFile f("vitali_old.root");
//TFile f("run26.3.5.40.cls.root");
  TFile f("prmu-old.root");

  TTree * t = f.Get("h1");
  AMSRoot amsroot("AMS", "AMS Display");
  amsroot.Init(t);
  amsroot.MakeTree("AMSTree", "AMS Display Tree");

  TFile fgeo("ams_group.root");
  TGeometry * geo = fgeo.Get("ams");
  AMSDisplay * amd= new AMSDisplay("AMSRoot Event Display", geo);
   amsroot.GetEvent(0);
   amd->ShowNextEvent(-1);




}






