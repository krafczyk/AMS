{
  gROOT->ProcessLine(".L analyze_CINT.C");

 AMSChain ch;
 //ch.Add("/some/root/files/*.root");
 ch.Add("/Volumes/CaseSensitive/1281355854.00000001.root");
 analyze_CINT(&ch,10000);

}
