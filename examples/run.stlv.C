{
gSystem->Load("/offline/vdev/lib/linux/icc/ntuple.so");
TFile f("dst/g3v1g3.root");
TTree *t=f.Get("AMSRoot");
t->Process("stlv.C++");
}