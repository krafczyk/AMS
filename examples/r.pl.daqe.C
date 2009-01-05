{
TProofLite *tl=new TProofLite("workers=7")        ;
TFileCollection *tf=new TFileCollection("AMSRoot");
tf->Add("/s0fc00/Data/AMS02/2006A/data2008/cern.cosmics.sci.4p/*root");
tf->Add("/r0fc00/Data/AMS02/2006A/data2008/cern.cosmics.sci.4p/*root");
//tf->Add("/s0fc00/Data/AMS02/2006A/data2008/cern.cosmics.sci.4p/1207*.root");
tl->Process(tf,"daqe.C+","oo.root");
}


