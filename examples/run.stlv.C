{
/** \example run.stlv.C
 * This is an example of how to load stlv.C
 (to run: root;  .x run.stlv.C)   
 */
gSystem->Load("/offline/vdev/lib/linux/icc/ntuple.so");
TFile f("/f2users/choutko/root/dst/g3v1g3.root");
TTree *t=f.Get("AMSRoot");
t->Process("stlv.C++");
}
