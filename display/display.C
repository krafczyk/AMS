{
  gROOT->Reset();
  gSystem->Load("libAMS.so");
  debugger.Off();		// debugger.On() to turn on printing messages
  TFile f("mc.root");
//TFile f("vitali_old.root");
//TFile f("run26.3.5.40.cls.root");
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






