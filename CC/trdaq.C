#include "trdaq.h"
#include "daqevt.h"
#include "trrec.h"
#include "event.h"
#include "TkDBc.h"
#include "tkdcards.h"
#include "event.h"
#include "TrMCCluster.h"
//============  DAQ INTERFACE =======================


int TrDAQ::MaxClusterLength = 128;

integer TrDAQ::getdaqid(int16u crate){
 for(int i=0;i<31;i++){
   if(checkdaqid(i)-1==crate)return i;
 }
 return -1;
}


integer TrDAQ::checkdaqid(int16u id){
  char sstr[128];
 for(int i=0;i<getmaxblocks();i++){
  sprintf(sstr,"JINFT%X",i);
  if(DAQEvent::ismynode(id,sstr))return i+1; 
 }
 for(int i=0;i<getmaxblocks();i++){
  sprintf(sstr,"JF-T%d",i);
  if(DAQEvent::ismynode(id,sstr))return i+1; 
 }
 return 0;
}







void TrDAQ::builddaq(integer i, integer n, int16u *p){
  int index=0;
  int crate2JinJnum[8]={3,9,0,1,16,17,22,23};

  AMSTrRawCluster *ptr=(AMSTrRawCluster*)AMSEvent::gethead()->
    getheadC("AMSTrRawCluster",0);


  map<int,vector<AMSTrRawCluster*> > mymap;
  map<int,vector<AMSTrRawCluster*> >::iterator it_mymap;
  //  for(int tdr=0;tdr<trconst::ntdr;tdr++) mymap[tdr].push_back(0); 
  while (ptr){
    
    int tdrnum=ptr->GetHwId()%100;
    int crate=ptr->GetHwId()/100;
    if(crate==i)mymap[tdrnum].push_back(ptr);
    ptr=ptr->next();
  }

  int pindex=0;
  
  for(it_mymap=mymap.begin();it_mymap!=mymap.end();it_mymap++){
    
    int ncl=it_mymap->second.size();
    if(ncl>0){
      //      printf("Crate %d JINJ#: %02d TDR: %02d numclus: %3d\n",
      // 	     i,crate2JinJnum[i],it_mymap->first,ncl);
      int tdr_length_index=pindex++;
      for(int icl=0;icl<ncl;icl++){
	ptr=it_mymap->second.at(icl);

	p[pindex++] = ptr->GetNelem()-1;
	p[pindex++] = 	ptr->GetAddress();
	for(int ii=0;ii<ptr->GetNelem();ii++)
	  
	  p[pindex++]=ptr->GetSignal(ii)*8;
      }
      p[pindex++]=1<<7|1<<15|1<<5|it_mymap->first;
      p[tdr_length_index]=pindex-tdr_length_index-1;
    }
    if(pindex>=n){
      cerr<<"TrDAQ::builddaq-E-indext too big "<<index<< " "<<n<<endl;       break; 
    }
  }
  p[pindex++]=1<<5| 1<<15| crate2JinJnum[i];
  if(pindex!=n){
    cerr<<"TrDAQ::builddaq-E-indext wrong length "<<pindex<< " "<<n<<endl;
  }  
  
}




integer TrDAQ::calcdaqlength(integer i){
  AMSTrRawCluster *ptr=(AMSTrRawCluster*)AMSEvent::gethead()->
  getheadC("AMSTrRawCluster",0);
  int tdra[trconst::ntdr]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  integer l=0;
  while (ptr){
    int tdrnum=ptr->GetHwId()%100;
    int crate=ptr->GetHwId()/100;
    if(crate==i){
      tdra[tdrnum]+=ptr->GetNelem()+2;
    }
    ptr=ptr->next();
  }
  for(int k=0;k<trconst::ntdr;k++){
     if(tdra[k]){
       l+=tdra[k]+2;
     }
   }
   if(l)l++;
  return -l;
}



void TrDAQ::buildraw(integer n, int16u *pbeg){
  //  have to split integer n; add crate number on the upper part...
  unsigned int leng=n&0xFFFF;
  uinteger ic=(n>>16);
  int cmn=0;
  int16u st=*(pbeg-1+leng);
  int num=st&0x1F;
  char Jname[20];
  sprintf(Jname,"JINF %d",num);
  int ret=TestBoardErrors(Jname,st,0);
  if( ret<0) return;
  if(TRCALIB.Version==0)cmn=16;
  else cmn=0;
  //  integer ic1=checkdaqid(*(pbeg-1+leng))-1;
  (void)checkdaqid(num);
  //  cout <<"  crate "<<ic<<" found" <<" "<<ic1<<endl;
  int add2=0;
  if(DAQCFFKEY.DAQVersion==1)add2=2;
  for (int16u* p=pbeg;p<pbeg+leng-1-add2;p+=*p+1){
    ReadOneTDR(p,*p,ic,0);
    
  }
  


  //#ifdef __AMSDEBUG__
//   {
//     AMSContainer * ptrc =AMSEvent::gethead()->getC("AMSTrRawCluster");
//     printf("Ho prodotto %d TrRawCluster\n",ptrc->getnelem());
//     for (AMSTrRawCluster * ptr= (AMSTrRawCluster *) ptrc->gethead();ptr!=0;ptr=ptr->next())
//       cerr<<*ptr<<endl;
//     // if(ptrc && AMSEvent::debug>1)ptrc->printC(cout);
//   }
      //#endif

}





int TrDAQ::ReadOneTDR(int16u* blocks,int tsize,int cratenum,int pri){

  //FIXME this value should be 1 for some run

  int   TDROff=1;
  int newformat=0;
  int CNWords=0;

  int run=  AMSEvent::gethead()->getrun();
  if(run>=1210067687) TDROff=2;
  if(run>=1210237802) TDROff=1;
  if(run==1210262957) TDROff=2;
  if(run>=1210668275 &&
     run< 1210678767) TDROff=2; 
  
  if(run>=1257416265){ // Nov09 format
    newformat=1;
    TDROff=1;
    CNWords=0;
  }
  int clcount=0;
  if(TRCALIB.Version==0)CNWords=16;
  else CNWords=0;

  //FIXME PZ DATA FORMAT
  if(run>=1208965124) CNWords=0;
  //  if(DAQCFFKEY.DAQVersion==1)CNWords=2;
  int rwords=1+CNWords;
  
  char ttname[20];
  ushort status=blocks[tsize];
  int TDRnum=((unsigned int)status)&0x1f;



 
  sprintf(ttname,"CrateT%d_TDR%02d",cratenum,TDRnum);
  int ret=TestBoardErrors(ttname,status,pri);
  if(ret<0) return -1;

  int RawOffset=0;
  if (status>>6&1) {
    RawOffset=1024;   // RAW data present
    if(pri>4)printf("  RAW data present  ignoring it\n ");
    
  }  
    
  int cluster_size=tsize-rwords;
  
  if ((status>>7&1)&&(cluster_size>0)){    // Compressed data present
    if(pri>3)printf(" Dumping cluster  Compressed data present \n");
    //dump clusters
    int  count=RawOffset+TDROff;
    while (count<(tsize-rwords)){

      int cluslenraw=blocks[count++];
      int clusaddraw=blocks[count++];
      int cluslen=cluslenraw;
      int clusadd=clusaddraw;
      if(newformat) {
	cluslen=cluslenraw&0x7f;
	clusadd=clusaddraw&0x3ff;
      }
      cluslen++;      
 
      if(cluslen>MaxClusterLength){
	static int junkerr=0;
	if(junkerr++<100)cerr << "TrDAQ::buildraw-E-TooLongCluster (>" << MaxClusterLength << ") " << cluslen << endl;
	continue;
      }
      if(((clusadd+cluslen-1)>639)&&(clusadd<640)){
	static int junkerr=0;
	if(junkerr++<100)cerr << "TrDAQ::buildraw-E-ClusterAcrossPN add " << clusadd << " len " << cluslen << endl;
      }

      if((clusadd+cluslen-1)>1023){
	static int junkerr=0;
	if(junkerr++<100)cerr << "TrDAQ::buildraw-E-ClusterOutsideBoundary add " << clusadd << " len " << cluslen << endl;
      }
      short int  signal[1024];
      if(pri>3)printf("Cluster: add: %d  lenght: %d\n",clusadd,cluslen);
      //count+=(cluslen-1);
      if(cluslen>1024) cluslen=1024;
      int hwid=cratenum*100+TDRnum;
      TkLadder *lad= TkDBc::Head->FindHwId(hwid);
      int tkid=0;
      if(lad) tkid=lad->GetTkId();
      else printf("TrDAQ::ReadOneTDR Very bad cant find hwid %03d in TkDBc\n",hwid);
      for(int hh=0;hh<cluslen;hh++){
	if(pri>3)printf("signal: %d %d\n",hh,(short int)blocks[count]);
	signal[hh]  = (short int)blocks[count];
	//if(pri)printf("        %d %f %f %d\n",hh,signal[hh]/8.,sigma[hh],status[hh]);
	count++;
      }
      clcount++;
      int sid=0;
      if(clusadd>640) sid=1;
      // Bug in the 3e73 DSP code: delete cluster on the 1022 and 1023 strip
      if ( (run>1258105753)&&(run<1258318946) ) {
	if (clusadd>=1022) continue;	
      }
      
      AMSTrRawCluster* pp= new AMSTrRawCluster(tkid,clusaddraw,cluslenraw,signal);
      AMSContainer* con= AMSEvent::gethead()->getC(AMSID("AMSTrRawCluster"));
      if(con)	con->addnext(pp);
      
      else
	cerr<<" TrDAQ::ReadOneCluster: ERROR --  Cant find the AMSTrRawCluster container!!!!"<<endl;

    }
  }    
  int cc=1;
  //Read the  cnoise values
  
  if(pri>4&&rwords==17){
    printf("Cnoise \n" );
    for (int ii=tsize-rwords;ii<tsize-1;ii++){
      printf(" %4d:  %6.3f ",cc++,((short int)blocks[ii])/8.);
      if(cc%4==1) printf("\n");
    }
  }

  if(pri>2) printf(" On TDR %2d Found %2d Clus\n",TDRnum,clcount);
  if(pri>3)printf("End of the TDR %2d  \n",TDRnum);
  
 
  return 0;
    
}


int TrDAQ::TestBoardErrors(char *name,ushort status,int pri){
  char errmess[16][80]={"","","","","","replying node is CDP",
			" RAW events",
			" COMPRESSED events",
			" cumulative status",
			"build errors (sequencer errors for event building",
			"build conditions (FE power fault for event building)",
			"NEXT bit / timeout (if DATA=1), set by master"	      ,
			"ERROR bit / AMSW error (if DATA=1), set by master",
			"ABORT bit / assembly error (if DATA=1), set by master",
			"END bit / CRC error (if DATA=1), set by master",
			"DATA bit, set by master when assembling group reply"};

  int count;
  if(pri>4){
         printf("%s \n",name);
    for (count=5;count<16;count++)
      if(status>>count&1)
	printf("%02d: %s\n",count,errmess[count]);
  }
  
  // bit 10 is managed is sub-detector specific: ignore our (power failure)!
  if( // (status>>9&0x3f)>0)    
      (status>>9&1) || (status>>11&1) || (status>>12&1) || (status>>13&1) || (status>>14&1) )      
    {      
     if(pri>0)     printf("%s",name);
      count=5;
      for (count=5;count<16;count++)
	if(status>>count&1)
	  if(pri>0)		printf("%02d: %s\n",count,errmess[count]);
      if(pri>0)		printf("\n");
      return -1;
    }
  return 0;
}
 
//================================================================
//================================================================
//================================================================

int TrDAQMC::onesize=45;


int TrDAQMC::calcdaqlength_old(integer i){
  AMSContainer *p = AMSEvent::gethead()->getC("AMSTrMCCluster");
  int len=1; 
  if(p){
    AMSTrMCCluster *ptr=(AMSTrMCCluster*)AMSEvent::gethead()->
      getheadC("AMSTrMCCluster",0);
    while(ptr){ 
      const integer big=10000;
      if(ptr->_edep!=0 && ptr->_xgl[0]+big>0 && ptr->_xgl[1]+big>0 && ptr->_xgl[2]+big>0)len+=8; 
      ptr=ptr->next();
    }
  }
  return -len;
}


void TrDAQMC::builddaq_old(integer i, integer length, int16u *p){
  AMSTrMCCluster *ptr=(AMSTrMCCluster*)AMSEvent::gethead()->getheadC("AMSTrMCCluster",0);
  p--;
  geant sum=0;
  while(ptr){ 
    const uinteger c=65535;
    *(p+1)=ptr->_itra;
    integer big=10000;
    if(ptr->_edep==0)goto metka;
    for(int k=0;k<3;k++){
      if(ptr->_xgl[k]+big<=0){
	goto metka;
      }
      uinteger cd=(ptr->_xgl[k]+big)*10000;
      *(p+3+2*k)=int16u(cd&c);
      *(p+2+2*k)=int16u((cd>>16)&c);
    }
    sum=ptr->_edep*1000000;
    if(sum>c)sum=c;
    *(p+8)=int16u(sum);
    p+=8;
  metka:
    ptr=ptr->next();
  }
  *(p+1)=getdaqid();
}


void TrDAQMC::buildraw_old(integer n, int16u *p){
  integer ip;
  int len=n&65535;
  for(int16u *ptr=p;ptr<p+len-1;ptr+=8){ 
    ip=*(ptr);
    uinteger cdx=  (*(ptr+2)) | (*(ptr+1))<<16;  
    uinteger cdy=  (*(ptr+4)) | (*(ptr+3))<<16;  
    uinteger cdz=  (*(ptr+6)) | (*(ptr+5))<<16;  
    geant sum=geant(*(ptr+7))/1000000;
    AMSPoint coo(cdx/10000.-10000.,cdy/10000.-10000.,cdz/10000.-10000.);
    AMSTrMCCluster *ptrhit = (AMSTrMCCluster*) AMSEvent::gethead()->addnext(AMSID("AMSTrMCCluster",0), new AMSTrMCCluster(coo,ip,sum));
    if(ptrhit->IsNoise())ptrhit->setstatus(AMSDBc::AwayTOF);
  }
} 


int  TrDAQMC::onesize_new = 23;
int  TrDAQMC::maxword_new = 65535; 
 
void TrDAQMC::builddaq_new(integer i, integer length, int16u *p){

  // Create a "fake" DAQ block for TrMCCluster
  // Description of the format: 
  // - GlobalSize [ SizeCluster | ClusterData, SizeCluster | ClusterData, ... ] Status
  // - Size = 2 + Sum_i (ClusterSize_i + 1)
  // - ClusterSize is fixed (onesize_new)!
 
  AMSContainer* container = AMSEvent::gethead()->getC("AMSTrMCCluster",0);
  if (container==0) { 
    cerr << " TrDAQMC::builddaq_new-E Cannot find the TrMCCluster container "<< endl;
    return;
  }
  int ncluster = container->getnelem();
  // max number of cluster that can encoded in the global size a word 
  int max_ncluster = floor(1.*(maxword_new-2)/(onesize_new+1));  
  if (ncluster>max_ncluster) ncluster = max_ncluster;

  // first word is the global size (with size word)  
  int pindex = 0;
  p[pindex++] = int16u(ncluster*(onesize_new + 1) + 2);  

  // loop on MC clusters
  for (int icluster=0; icluster<ncluster; icluster++) {
    AMSTrMCCluster* mccluster = (AMSTrMCCluster*) container->at(icluster);
    // mccluster->Print(); // debug 
  
    // size of the cluster   
    p[pindex++] = int16u(onesize_new + 1); 

    // pack first high bits then low bits (why? ask Vitaly) 
    p[pindex++] = TrDAQMC::Pack((int)mccluster->_idsoft,true);
    p[pindex++] = TrDAQMC::Pack((int)mccluster->_idsoft,false);
    p[pindex++] = *((int16u*)(&(mccluster->_itra)));
    p[pindex++] = TrDAQMC::Pack((float)mccluster->_step,true);
    p[pindex++] = TrDAQMC::Pack((float)mccluster->_step,false);
    p[pindex++] = TrDAQMC::Pack((float)mccluster->_xgl.x(),true);
    p[pindex++] = TrDAQMC::Pack((float)mccluster->_xgl.x(),false);
    p[pindex++] = TrDAQMC::Pack((float)mccluster->_xgl.y(),true);
    p[pindex++] = TrDAQMC::Pack((float)mccluster->_xgl.y(),false);
    p[pindex++] = TrDAQMC::Pack((float)mccluster->_xgl.z(),true);
    p[pindex++] = TrDAQMC::Pack((float)mccluster->_xgl.z(),false);
    p[pindex++] = TrDAQMC::Pack((float)mccluster->_dir.x(),true);
    p[pindex++] = TrDAQMC::Pack((float)mccluster->_dir.x(),false);
    p[pindex++] = TrDAQMC::Pack((float)mccluster->_dir.y(),true);
    p[pindex++] = TrDAQMC::Pack((float)mccluster->_dir.y(),false);
    p[pindex++] = TrDAQMC::Pack((float)mccluster->_dir.z(),true);
    p[pindex++] = TrDAQMC::Pack((float)mccluster->_dir.z(),false);
    p[pindex++] = TrDAQMC::Pack((float)mccluster->_mom,true);
    p[pindex++] = TrDAQMC::Pack((float)mccluster->_mom,false);
    p[pindex++] = TrDAQMC::Pack((float)mccluster->_edep,true);
    p[pindex++] = TrDAQMC::Pack((float)mccluster->_edep,false);
    p[pindex++] = TrDAQMC::Pack((int)mccluster->Status,true);
    p[pindex++] = TrDAQMC::Pack((int)mccluster->Status,false);
  }

  // simulating status
  p[pindex++] = int16u(getdaqid());

  if (pindex!=((onesize_new+1)*ncluster)+2) {
    cerr << " TrDAQMC::builddaq_new-E wrong size. Length in words is " << pindex << " instead of " << ((onesize+1)*ncluster+2) << endl;
  }
  if(pindex >= maxword_new){
    cerr<<"TrDAQMC::builddaq_new-E TrMC size too large. Actual length in words is " << pindex << ", max is " << maxword_new << ". This should not happen." << endl;
  }
 
  // debug 
  // for (int i=0; i<pindex; i++) {
  //   printf("%4d %6d %4hX\n",i,p[i],p[i]);
  // }
}


void TrDAQMC::buildraw_new(integer n, int16u *p) {

  // Decode the "fake" DAQ block and create the TrMCCluster
  // - the length is passed as well as the first word pointer 

  unsigned int leng = n&0xFFFF; 
  int16u length = p[0]; 
  if(leng!=length) {
    cerr << "TrDAQMC::buildraw_new-E passed length is different from written size. Passed is " << leng << " instead written is " << length << ". Quit." << endl;
    return;
  }
  int16u status = *(p-1+length);
  int num = status&0x1F;
  if(num!=getdaqid()) {
    cerr << "TrDAQMC::buildraw_new-E called on a wrong segment. Node ID is " << num << " instead of " << getdaqid() << ". Quit." << endl;
    return;
  }
  int nclusters = (length-2)/(onesize_new+1);
  int rest = (length-2)%(onesize_new+1);
  if(rest!=0) {
    cerr << "TrDAQMC::buildraw_new-E the format is corrupted. Rest is = " << rest << ". Quit." << endl;
    return;
  }
  AMSContainer* container = AMSEvent::gethead()->getC(AMSID("AMSTrMCCluster"));
  if (!container) {
    cerr << "TrDAQMC::buildraw_new-E cannot find the AMSTrMCCluster container. Quit."<<endl;
    return;
  }

  int pindex=1;
  for (int icluster=0; icluster<nclusters; icluster++){
    int16u cluster_size = p[pindex++];
    if(cluster_size!=(onesize_new+1)) {
      cerr << "TrDAQMC::buildraw-E the size of cluster is not the expexted one. Should be " << (onesize_new+1) << " instead is "<< cluster_size << ". Quit." << endl;
      return;
    }

    AMSTrMCCluster* mccluster = new AMSTrMCCluster();

    mccluster->_idsoft = TrDAQMC::UnPackInt  (p[pindex+1],p[pindex]); pindex += 2;
    mccluster->_itra   = p[pindex++];
    mccluster->_step   = TrDAQMC::UnPackFloat(p[pindex+1],p[pindex]); pindex += 2; 
    mccluster->_xgl[0] = TrDAQMC::UnPackFloat(p[pindex+1],p[pindex]); pindex += 2;
    mccluster->_xgl[1] = TrDAQMC::UnPackFloat(p[pindex+1],p[pindex]); pindex += 2;
    mccluster->_xgl[2] = TrDAQMC::UnPackFloat(p[pindex+1],p[pindex]); pindex += 2;
    mccluster->_dir[0] = TrDAQMC::UnPackFloat(p[pindex+1],p[pindex]); pindex += 2;
    mccluster->_dir[1] = TrDAQMC::UnPackFloat(p[pindex+1],p[pindex]); pindex += 2;
    mccluster->_dir[2] = TrDAQMC::UnPackFloat(p[pindex+1],p[pindex]); pindex += 2;
    mccluster->_mom    = TrDAQMC::UnPackFloat(p[pindex+1],p[pindex]); pindex += 2; 
    mccluster->_edep   = TrDAQMC::UnPackFloat(p[pindex+1],p[pindex]); pindex += 2;
    mccluster->Status  = TrDAQMC::UnPackInt  (p[pindex+1],p[pindex]); pindex += 2;

    // mccluster->Print(); // debug 

    if (mccluster->IsNoise()) mccluster->setstatus(AMSDBc::AwayTOF);
    if (container) container->addnext(mccluster);
    else{
      cerr << "TrDAQMC::buildraw_new-E cannot find the AMSTrMCCluster container, in a point of the code that should not happen. Quit." << endl;
      if (mccluster) delete mccluster;
    }
  }
}


int TrDAQMC::calcdaqlength_new(integer i){
  AMSContainer* container = AMSEvent::gethead()->getC("AMSTrMCCluster",0);
  if (container==0) {
    cerr << " TrDAQMC::calcdaqlength_new-E Cannot find the TrMCCluster container "<< endl;
    return -1;
  }
  int ncluster = container->getnelem();
  // max number of cluster that can encoded in the global size a word 
  int max_ncluster = floor(1.*(maxword_new-2)/(onesize_new+1));
  if (ncluster>max_ncluster) ncluster = max_ncluster;
  int length = ncluster*(onesize_new + 1) + 2;
  return -length;
}


void TrDAQMC::builddaq(integer i, integer length, int16u *p){
  // new writing format from build > 592
  // if (AMSCommonsI::getbuildno()<=592) TrDAQMC::builddaq_old(i,length,p);
  TrDAQMC::builddaq_new(i,length,p);
}


static int check_that_nothing_happens = 0;
void TrDAQMC::buildraw(integer n, int16u *p) {
  if (n<1) return;
  // test new reading format (this is not 100 percent sure, but I do not have the build number of the production)
  // however a message is delivered in case of change of different kinds of decoding
  if ( (int16u(p[0])==(n&0xFFFF))&&(int16u(p[n-1]==getdaqid()))&&((n<3)||(int16u(p[1])==TrDAQMC::onesize_new+1)) ) { 
    if (check_that_nothing_happens==0) check_that_nothing_happens = 2;
    if (check_that_nothing_happens==1) 
      cerr << "TrDAQMC::buildraw-E unexpected format switch: old format (prod<592) was used at the beginning, new format (prod>=592) requested for this event." << endl;  
    TrDAQMC::buildraw_new(n,p);
  }
  else {
    if (check_that_nothing_happens==0) check_that_nothing_happens = 1;
    if (check_that_nothing_happens==2)  
      cerr << "TrDAQMC::buildraw-E unexpected format switch: new format (prod>=592) was used at the beginning, old format (prod<592) requested for this event." << endl;
    TrDAQMC::buildraw_old(n,p);
  }
}


int TrDAQMC::calcdaqlength(integer i) {
  // new writing format from build > 592
  // if (AMSCommonsI::getbuildno()<=592) return TrDAQMC::calcdaqlength_old(i);
  return TrDAQMC::calcdaqlength_new(i);
}


int16u TrDAQMC::Pack(float xx, bool high_bits) {
  int* pp=(int*) &xx;
  if(high_bits) return ((*pp)>>16)&0xffff;
  else          return (*pp)&0xffff;
}


int16u TrDAQMC::Pack(int xx, bool high_bits) {
  if (high_bits) return (xx>>16)&0xffff;
  else           return xx&0xffff;
}


float TrDAQMC::UnPackFloat(int16u lower, int16u upper) {
  int a = 0;      
  a &= 0;
  a |= (lower);
  a |= (upper<<16);
  return *((float*)&a);
}


int TrDAQMC::UnPackInt(int16u lower, int16u upper) {
  int a = 0;
  a &= 0;
  a |= (lower);            
  a |= (upper<<16);
  return a;
}



// void TrDAQMC::buildraw(integer n, int16u *pbeg){
//   //  have to split integer n; add crate number on the upper part...
//   unsigned int leng=n&0xFFFF;
//   uinteger ic=(n>>16);
//   int cmn=0;
//   int16u st=*(pbeg-1+leng);
//   int num=st&0x1F;
//   if(num!=getdaqid()) {
//     cerr << "TrDAQMC::buildraw -E-  Called on a wrong segment!! Node id num="<<num<<endl;
//     return;
//   }
//   int ncl=(leng-2)/(onesize+1);
//   int nclR=(leng-2)%(onesize+1);
//   if(nclR!=0) {
//     cerr << "TrDAQMC::buildraw -E-  The number of cluster is not an integer! Resto= "<<nclR<<endl;
//     return;
//   }
//   AMSContainer* con= AMSEvent::gethead()->getC(AMSID("AMSTrMCCluster"));
//   if(!con){
//     cerr<<"TrDAQMC::buildraw -E- Cannot find the AMSTrMCCluster container "<<endl;
//     return;
//   }
//   int pindex=1;
//   for (int cl=0;cl<ncl;cl++){
//     int csize=pbeg[pindex++];
//     if(csize!=onesize) {
//       cerr << "TrDAQMC::buildraw -E-  The size of cluster is notthe expexted one! Should be "<<onesize<<" and is "<<csize <<endl;
//       return;
//     }
//     AMSTrMCCluster* mccl=new AMSTrMCCluster();
//     mccl->_idsoft  = pbeg[pindex++];
//     mccl->_idsoft  = mccl->_idsoft<<16;
//     mccl->_idsoft |= pbeg[pindex++];
//     mccl->_itra    = pbeg[pindex++];
//     mccl->_left[0] = pbeg[pindex++];
//     mccl->_left[1] = pbeg[pindex++];
//     mccl->_center[0] = pbeg[pindex++];
//     mccl->_center[1] = pbeg[pindex++];
//     mccl->_right[0] = pbeg[pindex++];
//     mccl->_right[1] = pbeg[pindex++];
//     for (int ss=0;ss<5;ss++)
//       for(int ii=0;ii<2;ii++){
// 	int aa= pbeg[pindex++];	
// 	aa  = aa <<16;
// 	aa |= pbeg[pindex++];
// 	mccl->_ss[ii][ss]=*((float*)&aa);
//       }
//     float x[3];
//     for(int ii=0;ii<3;ii++){
//       int aa= pbeg[pindex++];	
//       aa  = aa <<16;
//       aa |= pbeg[pindex++];
//       x[ii]=*((float*)&aa);
//     }
//     mccl->_xgl.setp(x);
//     for(int ii=0;ii<3;ii++){
//       int aa= pbeg[pindex++];	
//       aa  = aa <<16;
//       aa |= pbeg[pindex++];
//       x[ii]=*((float*)&aa);
//     }
//     mccl->_Momentum.setp(x);
//     int aa= pbeg[pindex++];	
//     aa  = aa <<16;
//     aa |= pbeg[pindex++];
//     mccl->_sum=*((float*)&aa);
//     mccl->Status= pbeg[pindex++];	
//     mccl->Status  = mccl->Status <<16;
//     mccl->Status |= pbeg[pindex++];
//     mccl->simcl[0]=0;
//     mccl->simcl[1]=0;
//     if(con)	con->addnext(mccl);
//     else{
//       cerr<<" TrDAQMC::buildraw: ERROR --  Cant find the AMSTrMCCluster container!!!!"<<endl;
//       if(mccl) delete mccl;
//     }
//   }
//   return;	
// }


// int TrDAQMC::calcdaqlength(int i){
// 	AMSContainer *p = AMSEvent::gethead()->getC("AMSTrMCCluster");
// 	int len=1; 
// 	int maxclnum= ((32*1024)-2)/(onesize+1);
// 	if(p){
// 		int ncl=p->getnelem();
// 		if (ncl>maxclnum) {
// 		  cerr<<"TrDAQMC::calcdaqlength-E- to many TrMCClus "<<ncl<<" truncate to max"<<maxclnum<<endl;	  
// 		  ncl=maxclnum;
// 		}
// 		len+=(onesize+1)*ncl;
// 	        len++;
// 	}
// 	return -len;
// }


// void TrDAQMC::builddaq(integer i, integer n, int16u *p){
//   int index=0;
//   AMSTrMCCluster *ptr=(AMSTrMCCluster*)AMSEvent::gethead()->
//     getheadC("AMSTrMCCluster",0);
//   int ncl=0;
//   int nn=0;
//   AMSContainer * ptr2= AMSEvent::gethead()->getC("AMSTrMCCluster",0);
//   if(ptr2 && ptr)ncl= ptr2->getnelem();
//   else {
//     // cerr<< " TrDAQMC::builddaq -E- Cannot find the TrMCCluster container "<<endl;
//     return;
//   }
//   int pindex=0;
//   int maxclnum= ((32*1024)-2)/(onesize+1);
//   if (ncl>maxclnum) ncl=maxclnum;
//   p[pindex++]=((onesize+1) * ncl)+1; //size
//   for (; ptr!=0;ptr=ptr->next()){
//     p[pindex++]=onesize; //size
//     int index0=pindex;
//     //_idsoft
//     p[pindex++]=(ptr->_idsoft>>16)&0xffff;  //1
//     p[pindex++]=ptr->_idsoft&0xffff;   
//     //_itra
//     p[pindex++]=(int16u)ptr->_itra;
//     p[pindex++]=ptr->_left[0];
//     p[pindex++]=ptr->_left[1];
//     p[pindex++]=ptr->_center[0];
//     p[pindex++]=ptr->_center[1];
//     p[pindex++]=ptr->_right[0];
//     p[pindex++]=ptr->_right[1];  //9
//     for(int kk=0;kk<5;kk++)
//       for(int ll=0;ll<2;ll++){
// 	p[pindex++]=TrDAQMC::Pack(ptr->_ss[ll][kk],true);
// 	p[pindex++]=TrDAQMC::Pack(ptr->_ss[ll][kk]);
//       }
//     p[pindex++]=TrDAQMC::Pack(ptr->_xgl.x(),true);         //30
//     p[pindex++]=TrDAQMC::Pack(ptr->_xgl.x());
//     p[pindex++]=TrDAQMC::Pack(ptr->_xgl.y(),true);         //30
//     p[pindex++]=TrDAQMC::Pack(ptr->_xgl.y());
//     p[pindex++]=TrDAQMC::Pack(ptr->_xgl.z(),true);         //30
//     p[pindex++]=TrDAQMC::Pack(ptr->_xgl.z());
//     p[pindex++]=TrDAQMC::Pack(ptr->_Momentum.x(),true);         //30
//     p[pindex++]=TrDAQMC::Pack(ptr->_Momentum.x());
//     p[pindex++]=TrDAQMC::Pack(ptr->_Momentum.y(),true);         //30
//     p[pindex++]=TrDAQMC::Pack(ptr->_Momentum.y());
//     p[pindex++]=TrDAQMC::Pack(ptr->_Momentum.z(),true);         //30
//     p[pindex++]=TrDAQMC::Pack(ptr->_Momentum.z());
//     p[pindex++]=TrDAQMC::Pack(ptr->_sum,true);
//     p[pindex++]=TrDAQMC::Pack(ptr->_sum);   //55
//     p[pindex++]=ptr->Status&0xffff;
//     p[pindex++]=(ptr->Status>>16)&0xffff;   //57
//     if((pindex-index0)>onesize){
//       cerr<<"TrDAQMC::builddaq-E-indext too big "<<pindex-index0<< " "<<onesize<<endl;       break; 
//     }
//     //cout << " aggiunto uno"<<endl;
//     nn++;
//     if(nn >=ncl) break;
//   }
//   p[pindex++]=getdaqid() ;
//   if(pindex!=((onesize+1) * ncl)+2){
//     cerr<<"TrDAQMC::builddaq-E-indext wrong length "<<pindex<< " "<<((onesize+1) * ncl)+1<<endl;
//   } 
//   if(pindex >= 32*1024){
//     cerr<<"TrDAQMC::builddaq-E-indext TOO LARGE length "<<pindex<< " "<<((onesize+1) * ncl)+1<<endl;
//   }
// }
