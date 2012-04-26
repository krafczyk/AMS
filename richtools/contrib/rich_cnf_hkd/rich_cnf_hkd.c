#define _LARGEFILE64_SOURCE

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <assert.h>
#include <sys/types.h>
#include <dirent.h>
#include <math.h>

#include "rich_cnf_hkd.h"

//int DEBUG=4;

// Dirs time span
const int DirsTimeSpan = 1000000;

// Association windows 
const int MaxRunDelay = 5;
const int MaxRunSpan = 1800;

// Directories
#define BLKTYPES 2
const char *blkDir[BLKTYPES] = { "CNF", "HKD" };

void Select(u32 ID, bool force);
int ReadRaw(char *blockFile, bool force);
int file_exists(const char * fileName);

main(int argc, char *argv[]) {
  int run=0, force=false;
  char dirName[1024], runsDir[1024], fileName[1024];

  if (argc<2) {
    printf("Usage: %s <runID> [force]\n       %s <fileName> [force]\n", argv[0], argv[0]);
    return 1;
  }

  if (sscanf(argv[1],"%d",&run) && (int)log10((double)run)==strlen(argv[1])-1)
    printf("%s-I-ProcessingCNF&HKDforRun %d\n", argv[0], run);
  else {
    run = 0;
    sprintf(fileName, argv[1]);
    printf("%s-I-ProcessingFile %s\n", argv[0], fileName);
  }

  if (argc>2) force=strcmp(argv[2],"!");
  if (force)
    printf("%s-I-ExistingOutputsWillBeOverwritten\n", argv[0]); 

  if (run) {
    sprintf(runsDir, ".");
    sprintf(dirName, getenv("RunsDir"));
    if (strcmp(dirName, "")) sprintf(runsDir, dirName);

    int type, ind;
    for (type=0; type<BLKTYPES; type++) {

      for (ind=0; ind<3; ind++) {

	if(((run-MaxRunDelay)/DirsTimeSpan != run/DirsTimeSpan-1+ind) &&
	   ((run+MaxRunSpan )/DirsTimeSpan != run/DirsTimeSpan-1+ind) ) continue;

	// Fill List Of Files
	char runsDirG[20];
	DIR *pDir;
	sprintf(runsDirG, "%d", (run/DirsTimeSpan-1+ind)*DirsTimeSpan);
	sprintf(dirName,"%s/%s/%s/", runsDir, blkDir[type], runsDirG);
	printf("%s-I-ListFilesInDirectory %s\n", argv[0], dirName);
	pDir=opendir(dirName);
	if (!pDir) continue;

	// Loop On Files
	int utime;
	struct dirent *file;
	while (file = readdir(pDir)) {
	  utime = atoi(file->d_name);
	  if (utime<run-MaxRunDelay || utime>run+MaxRunSpan) continue;
	  
	  sprintf(fileName,"%s%s", dirName, file->d_name);
	  printf("%s-I-ReadRaw %s\n", argv[0], fileName);
	  ReadRaw(fileName, force);
	}

      }

    }

  }

  else
    ReadRaw(fileName, force);

  //return 0;
}


ReadRaw(char *BlockFileName, bool force) {
  u32 event_ok=0;

  u32 bytes_read=0;

  u32 event_size,event_length,utime;
  u16 size,size_ext,node,status,tag,time_msb,time_lsb;
  u32 type;
  char rr,rw;

  struct stat buf;

  u32 ID=0;

  int i;

  printf(">>> Opening File %s\n",BlockFileName);
  int fd=open(BlockFileName,O_LARGEFILE);
  printf("     File Descriptor : %d\n",fd);

  // Obtain file info 
  if(!fstat(fd,&buf))
    printf("     st_uid %d, st_gid %d, st_size %d, st_mode, %d st_mtime %d\n",
	   buf.st_uid,buf.st_gid,(int)buf.st_size,buf.st_mode,(int)buf.st_mtime);
  else {
    printf("Unable to retrieve file information. Stop here!\n");
    return -2;
  }

  // Initialize Global Counters
  rdr_config_blocks=0;
  rdr_daqhk_blocks=0;
  jinr_config_blocks=0;
  jinr_daqhk_blocks=0;
  for (i=0; i<NJINR; i++) {
    memset(&JINRConfig[i],0,sizeof(JINRConfig[i]));
    memset(&JINRHKinfo[i],0,sizeof(JINRHKinfo[i]));
  }
  for (i=0; i<NRDR; i++) {
    memset(&RDRConfig[i],0,sizeof(RDRConfig[i]));
    memset(&RDRHKinfo[i],0,sizeof(RDRHKinfo[i]));
  }

  // Init FCS Table
  PrepareFCSTable();

  // Init Vars
  Init();

    // main loop
    while (read(fd,&size,sizeof(size))==sizeof(size)){
      int rc=1;

      bytes_read+=sizeof(size);

      if (event_ok)
        memcpy(&EventPrev,&EventLast,sizeof(Event));

      EventInit(&EventLast);

      crc_error=0;
      event_jok=1;
      event_eok=1;
      event_rok=1;
      event_sok=1;
      event_tok=1;
      event_uok=1;
      event_ok=0;

      event_size=size&0x7fff;
      if (size&0x8000){
        bytes_read+=read(fd,&size_ext,sizeof(size_ext));
        event_size&~0x8000;
        event_size=(event_size<<16)|size_ext;
      }
      //if (event_size%2) printf("WARNING-ODD-EVENT_SIZE %d\n",event_size);

      if (DEBUG>0) printf("\n>>> NEW EVENT <<<\nEvent Size %d (%#x) (Bytes)\n",event_size,event_size);

      event_length=(event_size+1)/2;
      u16 *event=calloc(event_length,2);

      u32 nbytes=read(fd,event,event_length*sizeof(u16));
      u32 nbytes_exp=event_length*sizeof(u16);

      if (nbytes==nbytes_exp){

        bytes_read+=nbytes;

        /* Event Primary Header */
        int pos=0;
        rr=(event[pos]>>15)&0x1;
        rw=(event[pos]>>14)&0x1;
        node=(event[pos]>>5)&0x1ff;
        type=event[pos]&0x1f;
        if (type==0x1f){
  	pos++;
  	type=(type<<16)|event[pos];
        }

        EventLast.PrimaryHeader.Length=event_length;
        EventLast.PrimaryHeader.Size=event_size;
        EventLast.PrimaryHeader.RR=rr;
        EventLast.PrimaryHeader.RW=rw;
        EventLast.PrimaryHeader.Node=node;
        EventLast.PrimaryHeader.Type=type;

        /* only replies */
        if(rw) continue;

        /* Check Node Type here */
        if (!is_AMSW[node] && !is_CAN[node]) continue;

        if (is_AMSW[node]) {

          /* Check Data Type here : Update Counters & decode non-JMDC Assemblies */
          if (is_JMDC[node]) {
              switch (type) {

                 case JMDC_CONF_DATA:
                   if (DEBUG>0) printf(">>>> SCIENCE DATA : CONFIGURATION DATA FOUND <<<<\n");
                   break;

                 case JMDC_DAQHK_DATA:
                   if (DEBUG>0) printf(">>>> SCIENCE DATA : DAQ HK DATA FOUND <<<<\n");
                   break;

              }
          }

	}

/* Event Secondary Header */
        pos++;
        status=(event[pos]>>12)&0xf;
        tag=event[pos]&0xfff;
        pos++;
        time_msb=event[pos];
        pos++;
        time_lsb=event[pos];
        utime=time_msb<<16|time_lsb;
        if (DEBUG>0) printf("Unix time %d (%#x)\n",utime,utime);

        EventLast.SecondaryHeader.Status=status;
        EventLast.SecondaryHeader.Tag=tag;
        EventLast.SecondaryHeader.Time=utime;

	if (ID<=0) ID=EventLast.SecondaryHeader.Time;

        /* Decode JMDC event */
        if (node<=JMDCaddrLast && (type<0x1f || type==JMDC_ENVELOP_DATA)) {

           if (status) {
      	    printf("Wrong status found in JMDC Secondary Header : %#x\n",status);
  	    EventPrint(&EventLast);
              break;
           }
           else {
              pos++;
              u32 block_size,block_length;
              while(pos<event_length){
                 block_size=event[pos];
                 if (block_size&0x8000){
                    pos++;
                    block_size=block_size&~0x8000;
                    block_size=(block_size<<16)|event[pos];
                 }
                 block_length=(block_size+1)/2;
                 pos++;
                 rc*=decode_block(type,&event[pos],block_size,block_length);
                 pos+=block_length;
              }
           }

        }

      }

      free(event);

    }

    close(fd);

    Select(ID,force);

}



void Select(u32 ID, bool force) {

  char AMSDataDir[1024];
  char cmd[1024], dirID[20], dirDT[1024], dir[1024]=".";
  char fname[1024], findx[1024];
  FILE *pfile, *pindx;

  sprintf(AMSDataDir, getenv("AMSDataDir"));
  if (strcmp(AMSDataDir,"")) {
    if (getenv("PGTRACK"))
      sprintf(dir, "%s/v5.00/RichDefaultPMTCalib/RichPMTCalib", getenv("AMSDataDir"));
    else
      sprintf(dir, "%s/v4.00/RichDefaultPMTCalib/RichPMTCalib", getenv("AMSDataDir"));
  }
  if (DEBUG>0)
    printf("dir : %s\n", dir);

  if (ID<=0) return;
  sprintf(dirID, "/%d", ID/1000000*1000000);

  if (rdr_config_blocks>0) {

    ////////////////////////RCONF///////////////////////////
    unsigned short RDRProVers;
    int RDRFEPower_p[NRDR];
    int RDRFEPower_r[NRDR];
    char RunRDRWord[1024]="";
    int aCDP;
    for (aCDP = 0; aCDP<NRDR; aCDP++) {
      RDRProVers=RDRConfig[aCDP].RDRVer.pro_vers;
      RDRFEPower_p[aCDP]=RDRConfig[aCDP].RDRSeq.fepow_ctrl.Parameter;
      RDRFEPower_r[aCDP]=RDRConfig[aCDP].RDRSeq.fepow_ctrl.Readback;
      if(!RDRProVers || 
	 RDRFEPower_p[aCDP]!=RDRFEPower_r[aCDP]) strcat(RunRDRWord,"d");
      else if(RDRFEPower_p[aCDP]==0) strcat(RunRDRWord,"c"); 
      else if(RDRFEPower_p[aCDP]==1) strcat(RunRDRWord,"b");
      else if(RDRFEPower_p[aCDP]==2) strcat(RunRDRWord,"a");
      else  strcat(RunRDRWord,"0");
    }

    {
      int write_file=false, write_indx=false;

      sprintf(dirDT, dir);
      strcat (dirDT,"/RunClassifierR");
      strcat (dirDT,dirID);
      sprintf(cmd, "/bin/mkdir -p %s", dirDT);
      system(cmd);

      sprintf(findx, "%s.txt", dirDT);
      sprintf(fname, "%s/%d", dirDT, ID);
      if (!file_exists(fname)) { write_file = true; write_indx = true; }
      else if (force)          { write_file = true; }
      else 
	printf(">>>> File %s already exists : skip it!\n", fname);

      if (write_file) {
	printf(">>>> Create File: %s\n", fname);
	pfile = fopen(fname, "w");
	fprintf(pfile, "%d\t%s\n", ID, RunRDRWord);
	fclose(pfile);
      }

      if (write_indx) {
	printf(">>>> Append File: %s\n", findx);
	pindx = fopen(findx, "a");
	fprintf(pindx, "%d\n", ID);
	fclose(pindx);
      }
    }

  }


  if (jinr_config_blocks>0) {

    ////////////////////////JCONF/////////////////////////// 
#define NHVB 4
#define NLR 40
#define NJINFR 4
#define NDCDC 2

    int HV[NHVB][NJINFR][NLR], HVn[NHVB][NLR], DCDC[NHVB][NJINFR][NDCDC];
    int iside[NHVB], Vmax[NHVB];
    int HVREF[NHVB][NLR]={
      {750,762,761,768,768,767,766,787, 786,789,789,759,764,757,763,773,
       895,831,805,865,782,792,795,799, 806,848,834,804,800,834,835,837,
       840,848,849,852,851,801,805,802},
      {805,822,821,824,824,827,826,829, 825,792,791,795,798,790,792,791,
       800,756,832,806,804,809,813,827, 786,788,787,787,785,781,784,776,
       780,775,777,757,760,754,760,754},
      {757,772,770,782,775,776,773,784, 782,789,786,781,783,729,747,778,
       800,784,809,812,743,743,758,758, 821,825,818,812,808,798,801,809,
       810,818,818,815,828,811,811,805},
      {809,793,791,798,797,794,794,793, 784,797,793,853,870,814,828,866,
       749,803,770,799,782,798,815,825, 758,773,768,776,772,775,787,779,
       782,783,784,770,773,743,754,734}
    };
    int hvbrick[NHVB][NJINFR]={ 
      {0,1,2,3}, {0,1,2,3}, {1,0,3,2}, {1,0,3,2}
    };
    int side[NHVB][NJINFR]={
      {0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {1,1,1,1}
    };
    
    int i, j, k, ii;
    for (i=0; i<NHVB; i++) {
      for (j=0; j<NJINFR; j++) {   
	DCDC[i][j][0]=JINRConfig[j].JINRx2.JINRhv[i].Value[0][0];
	DCDC[i][j][1]=(int)(Q_DCDC + K_DCDC + 
			    K_DCDC * JINRConfig[j].JINRx2.JINRhv[i].Value[1][0] +
			    0.5);  
	for (k=0; k<NLR; k++) {
	  HV[i][j][k]=(int)(K_LR + Q_LR + 
			    K_LR * JINRConfig[j].JINRx2.JINRhv[i].Value[2+k][0] +
			    0.5);
	}      
      }  
    }
    
    for (ii=0; ii<NHVB; ii++){ 
      Vmax[ii]=0;	
      for(k=0; k<NLR; k++){
	HVn[ii][k]=0;
      }
    }
    
    char RunHVConfWord[4][1024]={ "", "", "", ""};
    for(i=0; i<NHVB; i++){
      for(j=0; j<NJINFR; j++){ 
	ii=hvbrick[i][j]; 
	if(DCDC[i][j][0]==1){	 
	  if(side[i][j]==0) strcat(RunHVConfWord[ii], "A");
	  else              strcat(RunHVConfWord[ii], "B");
	  Vmax[ii] = MAXV(DCDC[i][j][1],Vmax[ii]);
	  if(DCDC[i][j][1]==950){
	    for(k=0; k<NLR; k++){ 
	      HVn[ii][k] = MAXV(HV[i][j][k],HVn[ii][k]);
	    }
	  }
	}
      }
    }

    for(ii=0; ii<NHVB; ii++){
      if     (!strcmp(RunHVConfWord[ii], "AB")) sprintf(RunHVConfWord[ii], "C");
      else if(!strcmp(RunHVConfWord[ii],   "")) sprintf(RunHVConfWord[ii], "0");
      if (Vmax[ii]==950) strcat(RunHVConfWord[ii], "1");
      else               strcat(RunHVConfWord[ii], "0");
      for(k=0; k<NLR; k++){ 
	if      (HVREF[ii][k]-HVn[ii][k]<= 1) strcat(RunHVConfWord[ii], "."); 
	else if (HVREF[ii][k]-HVn[ii][k]<=11) strcat(RunHVConfWord[ii], "1");
	else if (HVREF[ii][k]-HVn[ii][k]<=21) strcat(RunHVConfWord[ii], "2");
	else if (HVREF[ii][k]-HVn[ii][k]<=31) strcat(RunHVConfWord[ii], "3");
	else if (HVREF[ii][k]-HVn[ii][k]<=41) strcat(RunHVConfWord[ii], "4");
	else if (HVREF[ii][k]-HVn[ii][k]<=51) strcat(RunHVConfWord[ii], "5");
	else if (HVREF[ii][k]-HVn[ii][k]> 51) strcat(RunHVConfWord[ii], "0");
      }      
    }

    {
      int write_file=false, write_indx=false;

      sprintf(dirDT, dir);
      strcat (dirDT,"/RunClassifierJ");
      strcat (dirDT,dirID);
      sprintf(cmd, "/bin/mkdir -p %s", dirDT);
      system(cmd);

      sprintf(findx, "%s.txt", dirDT);
      sprintf(fname, "%s/%d", dirDT, ID);
      if (!file_exists(fname)) { write_file = true; write_indx = true; }
      else if (force)          { write_file = true; }
      else 
	printf(">>>> File %s already exists : skip it!\n", fname);

      if (write_file) {
	printf(">>>> Create File: %s\n", fname); 
	pfile = fopen(fname, "w");
	fprintf(pfile, "%d\t%s\t%s\t%s\t%s\t\n", ID, 
		RunHVConfWord[0], RunHVConfWord[1], RunHVConfWord[2], RunHVConfWord[3]);
	fclose(pfile);
      }

      if (write_indx) {
	printf(">>>> Append File: %s\n", findx);
	pindx = fopen(findx, "a");
	fprintf(pindx, "%d\n", ID);
	fclose(pindx);
      }
    }

  }


  if (rdr_daqhk_blocks>0) {

    ///////////RHK///////////////////////////
    enum { kConfInteg = 12, kCaliInteg = 13};
    unsigned short status;
    char RunRDRHK[1024]="";
    int aCDP;
    for (aCDP = 0; aCDP<NRDR; aCDP++) {
      status = RDRHKinfo[aCDP].HKRdr.rdr.status;
      strcat(RunRDRHK, status&(1<<kConfInteg) ? "1" : "0");
      strcat(RunRDRHK, status&(1<<kCaliInteg) ? "1" : "0");
      strcat(RunRDRHK, " ");
    }

    {
      int write_file=false, write_indx=false;

      sprintf(dirDT, dir);
      strcat (dirDT,"/RunClassifierRHK");
      strcat (dirDT,dirID);
      sprintf(cmd, "/bin/mkdir -p %s", dirDT);
      system(cmd);

      sprintf(findx, "%s.txt", dirDT);
      sprintf(fname, "%s/%d", dirDT, ID);
      if (!file_exists(fname)) { write_file = true; write_indx = true; }
      else if (force)          { write_file = true; }
      else 
	printf(">>>> File %s already exists : skip it!\n", fname);

      if (write_file) {
	printf(">>>> Create File: %s\n", fname); 
	pfile = fopen(fname, "w");
	fprintf(pfile, "%d\t%s\n", ID, RunRDRHK);
	fclose(pfile);
      }
      
      if (write_indx) {
	printf(">>>> Append File: %s\n", findx);
	pindx = fopen(findx, "a");
	fprintf(pindx, "%d\n", ID);
	fclose(pindx);
      }

    }

  }

  if (jinr_daqhk_blocks>0) {

    ////////////////////////JHK///////////////////////////
    enum { kSlaveMasks = 15, kIRegisters = 14,
	   kRHV0ok = 8, kRHV1ok = 9, kRHV2ok = 10, kRHV3ok = 11,
	   kRHV0on = 0, kRHV1on = 1, kRHV2on =  2, kRHV3on =  4 }; 
    unsigned short JHK[4];
    char JHKIntegrity[NJINFR][16];
    int aJINR, k;
    for (aJINR=0; aJINR<NJINFR; aJINR++) { 
      JHK[aJINR]=JINRHKinfo[aJINR].HKJinr.jinr.status;
      sprintf(JHKIntegrity[aJINR],"%d%d%d%d%d%d%d%d%d%d",
	      JHK[aJINR]&(1<<kSlaveMasks) ? 1 : 0,
	      JHK[aJINR]&(1<<kIRegisters) ? 1 : 0,
	      JHK[aJINR]&(1<<kRHV0ok) ? 1 : 0,
	      JHK[aJINR]&(1<<kRHV1ok) ? 1 : 0,
	      JHK[aJINR]&(1<<kRHV2ok) ? 1 : 0,
	      JHK[aJINR]&(1<<kRHV3ok) ? 1 : 0,
	      JHK[aJINR]&(1<<kRHV0on) ? 1 : 0,
	      JHK[aJINR]&(1<<kRHV1on) ? 1 : 0,
	      JHK[aJINR]&(1<<kRHV2on) ? 1 : 0,
	      JHK[aJINR]&(1<<kRHV3on) ? 1 : 0
	      );
    }

    {
      int write_file=false, write_indx=false;

      sprintf(dirDT, dir);
      strcat (dirDT,"/RunClassifierJHK");
      strcat (dirDT,dirID);
      sprintf(cmd, "/bin/mkdir -p %s", dirDT);
      system(cmd);

      sprintf(findx, "%s.txt", dirDT);
      sprintf(fname, "%s/%d", dirDT, ID);
      if (!file_exists(fname)) { write_file = true; write_indx = true; }
      else if (force)          { write_file = true; }
      else 
	printf(">>>> File %s already exists : skip it!\n", fname);

      if (write_file) {
	printf(">>>> Create File: %s\n", fname); 
	pfile = fopen(fname, "w");
	fprintf(pfile, "%d\t%s %s %s %s\n", ID, 
		JHKIntegrity[0], JHKIntegrity[1], JHKIntegrity[2], JHKIntegrity[3]);
	fclose(pfile);
      }

      if (write_indx) {
	printf(">>>> Append File: %s\n", findx);
	pindx = fopen(findx, "a");
	fprintf(pindx, "%d\n", ID);
	fclose(pindx);
      }
    }
  }

}


int file_exists(const char * fileName) {
  struct stat buf;
  //printf("file_exists %s\n", fileName);
  if (!stat(fileName,&buf)) return buf.st_mode;
  return 0;
}


////////////////////// AUX ////////////////////////////

int decode_block(u32 header_type, u16 *event, u32 size, u32 length){

  static int init=TRUE;
  int pos=0,sb=1;
  u16 *dat;
  u16 rr,rw,node;
  u32 type;
  rr=(event[pos]>>15)&0x1;
  rw=(event[pos]>>14)&0x1;
  node=(event[pos]>>5)&0x1ff;
  type=event[pos]&0x1f;
  if (type==0x1f){
    pos++;
    type=(type<<16)|event[pos];
  }

  if (DEBUG>0)
    printf(">>> decode_block: rr %d , rw %d, node %#x, type %#x, size %d, length %d\n",rr,rw,node,type,size,length);

  pos++;

  if (is_AMSW[node]) {

    switch (header_type) {

      case JMDC_CONF_DATA:
      case JMDC_DAQHK_DATA:

        switch (type) {

          case CONF_DATA:
            pos++; // skip event_number
            sb*=decode_config(header_type,rr,rw,node,&event[pos],size,length-1);
            break;
 
          case DAQHK_DATA:
            pos++; // skip event_number
            sb*=decode_hkinfo(header_type,rr,rw,node,&event[pos],size,length-1);
            break;
        }
        break;

    }

  }

  return sb;

}

int decode_config(u32 header_type, char rr, char rw, u16 node, u16 *event, u32 size, u32 length){

  u16 *pevent = event;

  u32 block_size;
  u16 status, crc16, node_id;

  int jind,rind;

  int sb=1;

  if (length<3) return sb;

  status=event[length-3];
  crc16=event[length-2];

  if (node>=JINRaddrFirst && node<=JINRaddrLast) {
    jind=node-JINRaddrFirst;
    if (DEBUG>0)
      printf("\tNode %s, node_status %#x, crc16 %#x\n",JINR[jind],status,crc16);
  }
  else if (node>=RDRaddrFirst && node<=RDRaddrLast) {
    jind=(node-RDRaddrFirst)/12;
    rind=(node-RDRaddrFirst)%12;
    if (DEBUG>0)
      printf("\tNode %s, node_status %#x, crc16 %#x\n",RDR[jind][rind],status,crc16);
  }

  if (header_type != JMDC_ENVELOP_DATA) {
    if (DoFCS(event,length-1)){
      if (DEBUG>=0) printf("CRC-ERROR-FOUND\n");
      crc_error=1;
      return -1;
    }
  }

  if (decode_status(status)) sb=0;

  block_size=length-2;

  if (header_type == JMDC_ENVELOP_DATA) pevent+=2;

  if  (node>=JINRaddrFirst && node<=JINRaddrLast) sb*=decode_jinr_config(0,node,pevent,block_size);
  else if (node>=RDRaddrFirst && node<=RDRaddrLast) sb*=decode_rdr_config(0,0,node,pevent,block_size);

  return sb;
}



int decode_hkinfo(u32 header_type, char rr, char rw, u16 node, u16 *event, u32 size, u32 length){

  u16 *pevent = event;

  u32 block_size;
  u16 status, crc16, node_id;

  int jind,rind;

  int sb=1;

  if (length<3) return sb;

  status=event[length-3];
  crc16=event[length-2];

  if (node>=JINRaddrFirst && node<=JINRaddrLast) {
    jind=node-JINRaddrFirst;
    if (DEBUG>0)
      printf("\tNode %s, node_status %#x, crc16 %#x\n",JINR[jind],status,crc16);
  }
  else if (node>=RDRaddrFirst && node<=RDRaddrLast) {
    jind=(node-RDRaddrFirst)/12;
    rind=(node-RDRaddrFirst)%12;
    if (DEBUG>0)
      printf("\tNode %s, node_status %#x, crc16 %#x\n",RDR[jind][rind],status,crc16);
  }

  if (header_type != JMDC_ENVELOP_DATA) {
    if (DoFCS(event,length-1)){
      if (DEBUG>=0) printf("CRC-ERROR-FOUND\n");
      crc_error=1;
      return -1;
    }
  }

  if (decode_status(status)) sb=0;

  block_size=length-2;

  if (header_type == JMDC_ENVELOP_DATA) pevent+=2;

  if  (node>=JINRaddrFirst && node<=JINRaddrLast) sb*=decode_jinr_hkinfo(0,node,pevent,block_size);
  else if (node>=RDRaddrFirst && node<=RDRaddrLast) sb*=decode_rdr_hkinfo(0,0,node,pevent,block_size);

  return sb;
}



int decode_jinr_config(u16 jnode, u16 node, u16 *event, u32 length){
  int sb=1;

  int side;
  if (jnode) side = JLINK_SMAP[jnode-JINJaddrFirst];
  else       side = NADDR_SMAP[ node-JINRaddrFirst];

  int half;
  if (jnode) half = node==JINFR0?0:1;
  else       half = (node-JINRaddrFirst)/4;

  int aJINR;
  if (jnode) aJINR = JLINK_JINR[half][side];
  else       aJINR = NADDR_JINR[node-JINRaddrFirst];

  if (FakeFS && !FSJINR[aJINR]) return sb;

  jinr_config_blocks++;

  if (DEBUG>1) {
    int i=0;
    printf("\t\tdecode_jinr_config: jnode %#x, node %#x, side %d\n",jnode,node,side);
    printf("\t\t");
    while(i<length) {
       printf("%04x ",event[i]);
       if (++i%16==0)printf("\n\t\t");
    }
    printf("\n");
  }

  u16 sdproc = 0;
  int offgen = 0;
  if (event[0] == 0x3002) { /* SD Procedure included as first word in reply */
     sdproc = event[0];
     offgen = 1; 
  }

  memcpy(&JINRConfig[aJINR].PrimaryHeader, &EventLast.PrimaryHeader, sizeof(PrimaryHeaderBlock));
  memcpy(&JINRConfig[aJINR].SecondaryHeader, &EventLast.SecondaryHeader, sizeof(SecondaryHeaderBlock));
  memcpy(&JINRConfig[aJINR].Event, &AMSblockLast, sizeof(AMSblock));
  memcpy(&JINRConfig[aJINR].JINRx2, &event[offgen], sizeof(JINRx2Block));
  memcpy(&JINRConfig[aJINR].Status, &event[length-1], 2*sizeof(event[0]));

  JINRConfigPrint(aJINR, &JINRConfig[aJINR]);

  return sb;
}

int decode_rdr_config(u16 jnode, u16 rnode, u16 node, u16 *event, u32 length){
  int sb=1;

  int side;
  if (jnode) side = JLINK_SMAP[jnode-JINJaddrFirst];
  else       side = 0;

  int half;
  if (rnode) half = rnode==JINFR0?0:1;
  else       half = (node-RDRaddrFirst)/12;

  int aCDP;
  if (jnode) aCDP = side?12*half+RLINKA[half][node]:12*half+RLINKB[half][node];
  else       aCDP = node-RDRaddrFirst;

  if (FakeFS && !FSRDR[aCDP]) return sb;

  if (event[0] == 0x011) return sb; /* get rid of SD 0x11 (used by RICH Task */

  rdr_config_blocks++;

  if (DEBUG>1) {
    int i=0;
    printf("\t\tdecode_rdr_config: jnode %#x, node %#x, side %d\n",jnode,node,side);
    printf("\t\t");
    while (i<length) {
       printf("%04x ",event[i]);
       if (++i%16==0)printf("\n\t\t");
    }
    printf("\n");
  }

  u16 sdproc = 0;
  int offgen = 0;
  if (event[0] == 0x000F) { /* SD Procedure included as first word in reply */
     sdproc = event[0];
     offgen = 1; 
  }

  int RDRVerOff, RDRSeqOff, RDRProOff, RDRUsrOff, RDRCalOff, RDRMisOff;
  u16 ProgramVersion, SubdProgramVersion;
  int offset;

  ProgramVersion     = event[1];
  SubdProgramVersion = event[2];

  offset=0;
  RDRVerOff=0;
  if (SubdProgramVersion>=0xB210) {
    RDRSeqOff=RDRVerOff+sizeof(RDRVerBlock)/sizeof(event[0]);
    RDRProOff=RDRSeqOff+sizeof(RDRSeqBlock)/sizeof(event[0]);
    RDRUsrOff=RDRProOff+sizeof(RDRProBlock)/sizeof(event[0]);
    if (SubdProgramVersion<0xB715) RDRUsrOff-=2; /* No Parameter 101C */
    if (SubdProgramVersion<0xB60D) RDRUsrOff-=2; /* No Parameter 101B */
    RDRCalOff=RDRUsrOff+sizeof(RDRUsrBlock)/sizeof(event[0]);
    RDRMisOff=RDRCalOff+sizeof(RDRCalBlock)/sizeof(event[0]);
  }
  else {
    SubdProgramVersion=0;
    RDRSeqOff=0;
    RDRProOff=RDRSeqOff+sizeof(RDRSeqBlock)/sizeof(event[0]);
    RDRUsrOff=RDRProOff+sizeof(RDRProBlock)/sizeof(event[0]);
    RDRCalOff=0;
    RDRMisOff=0;
  }

  if (SubdProgramVersion>=0xB210) {

    memcpy(&RDRConfig[aCDP].PrimaryHeader, &EventLast.PrimaryHeader, sizeof(PrimaryHeaderBlock));
    memcpy(&RDRConfig[aCDP].SecondaryHeader, &EventLast.SecondaryHeader, sizeof(SecondaryHeaderBlock));
    memcpy(&RDRConfig[aCDP].Event, &AMSblockLast, sizeof(AMSblock));
    memcpy(&RDRConfig[aCDP].RDRVer, &event[RDRVerOff+offgen], sizeof(RDRVerBlock));
    memcpy(&RDRConfig[aCDP].RDRSeq, &event[RDRSeqOff+offgen], sizeof(RDRSeqBlock));
    if      (SubdProgramVersion<0xB60D)
      memcpy(&RDRConfig[aCDP].RDRPro, &event[RDRProOff+offgen], sizeof(RDRProBlock) - 4*sizeof(event[0]));
    else if (SubdProgramVersion<0xB715)
      memcpy(&RDRConfig[aCDP].RDRPro, &event[RDRProOff+offgen], sizeof(RDRProBlock) - 2*sizeof(event[0]));
    else
      memcpy(&RDRConfig[aCDP].RDRPro, &event[RDRProOff+offgen], sizeof(RDRProBlock));
    memcpy(&RDRConfig[aCDP].RDRUsr, &event[RDRUsrOff+offgen], sizeof(RDRUsrBlock));
    memcpy(&RDRConfig[aCDP].RDRCal, &event[RDRCalOff+offgen], sizeof(RDRCalBlock));
    memcpy(&RDRConfig[aCDP].RDRMis, &event[RDRMisOff+offgen], sizeof(RDRMisBlock));
    memcpy(&RDRConfig[aCDP].Status, &event[length-1], 2*sizeof(event[0]));

  }

  else { /* OLD Format */

    if (event[RDRUsrOff+offgen]>0x2000) offset = 2*(event[RDRUsrOff+offgen]-0x2000); /* buggy vers. missing words */

    memcpy(&RDRConfig[aCDP].PrimaryHeader, &EventLast.PrimaryHeader, sizeof(PrimaryHeaderBlock));
    memcpy(&RDRConfig[aCDP].SecondaryHeader, &EventLast.SecondaryHeader, sizeof(SecondaryHeaderBlock));
    memcpy(&RDRConfig[aCDP].Event, &AMSblockLast, sizeof(AMSblock));
    memcpy(&RDRConfig[aCDP].RDRSeq, &event[RDRSeqOff+offgen], sizeof(RDRSeqBlock));
    memcpy(&RDRConfig[aCDP].RDRPro, &event[RDRProOff+offgen], sizeof(RDRProBlock) - offset*sizeof(event[0]));
    memcpy(&RDRConfig[aCDP].RDRUsr, &event[RDRUsrOff+offgen-offset], sizeof(RDRUsrBlock));
    memcpy(&RDRConfig[aCDP].Status, &event[length-1], 2*sizeof(event[0]));

  }

  RDRConfigPrint(aCDP, &RDRConfig[aCDP], SubdProgramVersion, offset);

  return sb;
}

int decode_jinr_hkinfo(u16 jnode, u16 node, u16 *event, u32 length){
  int sb=1;

  int side;
  if (jnode) side = JLINK_SMAP[jnode-JINJaddrFirst];
  else       side = NADDR_SMAP[ node-JINRaddrFirst];

  int half;
  if (jnode) half = node==JINFR0?0:1;
  else       half = (node-JINRaddrFirst)/12;

  int aJINR;
  if (jnode) aJINR = JLINK_JINR[half][side];
  else       aJINR = NADDR_JINR[node-JINRaddrFirst];

  if (FakeFS && !FSJINR[aJINR]) return sb;

  jinr_daqhk_blocks++;

  if (DEBUG>1) {
    int i=0;
    printf("\t\tdecode_jinr_hkinfo: jnode %#x, node %#x, side %d\n",jnode,node,side);
    printf("\t\t");
    while(i<length) {
       printf("%04x ",event[i]);
       if (++i%16==0)printf("\n\t\t");
    }
    printf("\n");
  }

  static int HKComOff, HKJinrOff, first=1;
  if (first) {
    first=0;
    HKComOff=0;
    HKJinrOff=HKComOff+sizeof(HKComBlock)/sizeof(event[0]);
  }

  memcpy(&JINRHKinfo[aJINR].PrimaryHeader, &EventLast.PrimaryHeader, sizeof(PrimaryHeaderBlock));
  memcpy(&JINRHKinfo[aJINR].SecondaryHeader, &EventLast.SecondaryHeader, sizeof(SecondaryHeaderBlock));
  memcpy(&JINRHKinfo[aJINR].Event, &AMSblockLast, sizeof(AMSblock));
  memcpy(&JINRHKinfo[aJINR].HKCom, &event[HKComOff], sizeof(HKComBlock));

  int sw_vers_id;
  sw_vers_id=JINRHKinfo[aJINR].HKCom.sw_vers_id;
  if (sw_vers_id < 0xB208) { 
    int sizeofhkjinr;
    sizeofhkjinr=sizeof(hkjinr)-4*sizeof(u16);
    memcpy(&JINRHKinfo[aJINR].HKJinr       , &event[HKJinrOff  ]               , 2*sizeof(u16));
    memcpy(&JINRHKinfo[aJINR].HKJinr.jinr  , &event[HKJinrOff+2]               , sizeofhkjinr );
    memcpy(&JINRHKinfo[aJINR].HKJinr.status, &event[HKJinrOff+2+sizeofhkjinr/2], 2*sizeof(u16));
  }
  else 
    memcpy(&JINRHKinfo[aJINR].HKJinr, &event[HKJinrOff], sizeof(HKJinrBlock));

  JINRHKinfoPrint(aJINR, &JINRHKinfo[aJINR]);

  return sb;
}

int decode_rdr_hkinfo(u16 jnode, u16 rnode, u16 node, u16 *event, u32 length){
  int sb=1;

  int side;
  if (jnode) side = JLINK_SMAP[jnode-JINJaddrFirst];
  else       side = 0;

  int half;
  if (rnode) half = rnode==JINFR0?0:1;
  else       half = (node-RDRaddrFirst)/12;

  int aCDP;
  if (jnode) aCDP = side?12*half+RLINKA[half][node]:12*half+RLINKB[half][node];
  else       aCDP = node-RDRaddrFirst;

  if (FakeFS && !FSRDR[aCDP]) return sb;

  rdr_daqhk_blocks++;

  if (DEBUG>1) {
    int i=0;
    printf("\t\tdecode_rdr_hkinfo: jnode %#x, node %#x, side %d\n",jnode,node,side);
    printf("\t\t");
    while (i<length) {
       printf("%04x ",event[i]);
       if (++i%16==0)printf("\n\t\t");
    }
    printf("\n");
  }

  static int HKComOff, HKRdrOff, first=1;
  if (first) {
    first=0;
    HKComOff=0;
    HKRdrOff=HKComOff+sizeof(HKComBlock)/sizeof(event[0]);
  }

  memcpy(&RDRHKinfo[aCDP].PrimaryHeader, &EventLast.PrimaryHeader, sizeof(PrimaryHeaderBlock));
  memcpy(&RDRHKinfo[aCDP].SecondaryHeader, &EventLast.SecondaryHeader, sizeof(SecondaryHeaderBlock));
  memcpy(&RDRHKinfo[aCDP].Event, &AMSblockLast, sizeof(AMSblock));
  memcpy(&RDRHKinfo[aCDP].HKCom, &event[HKComOff], sizeof(HKComBlock));
  memcpy(&RDRHKinfo[aCDP].HKRdr, &event[HKRdrOff], sizeof(HKRdrBlock));

  RDRHKinfoPrint(aCDP, &RDRHKinfo[aCDP]);

  return sb;
}


unsigned short FCS_Table[256];

void PrepareFCSTable() {

  // from Kounine + Koutsenko documentation, p30

  unsigned short const g=0x1021;
  unsigned short d,i,k,fcs;

  // prepare lookup table

  for(i=0; i<256; i++) {
    fcs=0;
    d=i<<8;
    for(k=0; k<8; k++) {
      if ((fcs^d) & 0x8000) fcs=(fcs<<1)^g;
      else fcs<<=1;
      d<<=1;
    }
    FCS_Table[i]=fcs;
  }
}

unsigned short DoFCS(unsigned short *data, int size) {

  // from Kounine + Koutsenko documentation, p30

  unsigned short fcs=0xffff;
  unsigned char val;

  int i;

  for(i=0; i<size; i++) { 
    val=data[i]>>8;
    fcs=FCS_Table[(fcs>>8)^val]^(fcs<<8);
    val=data[i] & 0xff;
    fcs=FCS_Table[(fcs>>8)^val]^(fcs<<8);
  }

  //  printf("DoFCS = %04x\n",fcs);

  return fcs;
}


int decode_status(u16 status){

  static int last_event_read=-1;
  static u16 StatusError[2]={0,0};

  int error=status&0x7e00;

  if (error&&DEBUG>=0) {
    printf("ERROR-FOUND-IN-STATUS-WORD\n");
    int DataBit,bit;
    DataBit=status>>15&1;
    StatusError[DataBit]|=status;
    if (last_event_read !=events_read) {
      last_event_read=events_read;
      for (DataBit=0; DataBit<2; DataBit++) {
        for (bit=9; bit<15; bit++) 
          StatusErrorCounter[DataBit][bit]+=(StatusError[DataBit]>>bit&1);
        StatusError[DataBit]=0;
      }
    }
  }


  if (error&&DEBUG>=0 || DEBUG>2) {

    u16 DAT, ReplyCode, SlaveStatus, SlaveID;
    char cReply[100];

    DAT=status>>15&1;
    ReplyCode=status>>11&0xf;
    SlaveStatus=status>>5&0x3f;
    SlaveID=status&0x1f;

    printf("Reply Status %#x\n",status);
    printf("  DAT %#x, ReplyCode %#x, SlaveStatus %#x, SlaveID %#x\n",DAT,ReplyCode,SlaveStatus,SlaveID);
    if (status>>5&1) printf("  no internal structure\n");
    if (status>>6&1) printf("  building RAW Events\n");
    if (status>>7&1) printf("  building COMPRESSED Events\n");
    if (status>>8&1) printf("  cumulative node status (OR of bits 11-8 in the Node Status word)\n");
    if (status>>9&1) printf("  build errors (sequencer errors (CDP) or event assembly (CDDC))\n");
    if (status>>10&1)printf("  build conditions error (Sub-Detector specific)\n");

    fill_reply(cReply, ReplyCode, DAT);
    if(strcmp(cReply,"")) printf("%s\n",cReply);

    /* Do not increase of error counters if "No room to append packet" is set */
    if (ReplyCode==0x2  || ReplyCode==0xa) error=0;

  }

  return error;

}

void fill_reply(char *cReply, u16 ReplyCode, u16 DAT) {

    if (DAT) {
      switch (ReplyCode)
	{
	case 0x0: sprintf(cReply,"  No errors during assembly"); break;
	case 0x1: sprintf(cReply,"  First received packet have no BC1 bit set"); break;
	case 0x2: sprintf(cReply,"  No room to append BC=11 packet"); break;
	case 0x3: sprintf(cReply,"  No valid data"); break;
	case 0x4: sprintf(cReply,"  Reserved for EVBLD (event No mismatch)"); break;
	case 0x5: sprintf(cReply,"  BC=11 packet with CRC error"); break;
	case 0x6: sprintf(cReply,"  Timeout on the reply to NEXT command"); break;
	case 0x7: sprintf(cReply,"  0-length reply to NEXT command"); break;
	case 0x8: sprintf(cReply,"  Intermediate/last packet has wrong BC bit settings"); break;
	case 0x9: sprintf(cReply,"  No buffers available for multi-packet transaction"); break;
	case 0xa: sprintf(cReply,"  No room to append BC=00/10 packet"); break;
	case 0xb: sprintf(cReply,"  Reserved for EVBLD (event No mismatch, multi-packet)"); break;
	case 0xc: sprintf(cReply,"  Multi-packet reply with CRC error"); break;
        default:  sprintf(cReply,"  UNEXPECTED REPLY : DAT %d ReplyCode %d\n",DAT,ReplyCode);
	}
    }
    else {
      switch (ReplyCode)
	{
        case 0x0: sprintf(cReply,""); break;
	case 0x1: sprintf(cReply,"  0-length reply NEXT"); break;
	case 0x2: sprintf(cReply,"  0-length reply ABORT"); break;
	case 0x3: sprintf(cReply,"  0-length reply ERROR"); break;
	case 0x4: sprintf(cReply,"  0-length reply END"); break;
	case 0x5: sprintf(cReply,"  Timeout"); break;
	case 0x6: sprintf(cReply,"  AMSWire errors"); break;
        default:  sprintf(cReply,"  UNEXPECTED REPLY : DAT %d ReplyCode %d\n",DAT,ReplyCode);
	}
		    
    }

}



void EventInit(Event *Event){

  Event->JINJSlaves=0;
  Event->DataBlocks=0;
  Event->JMDCBlocks=0;

  Event->JMDCDeltaTime = 0;
  Event->JLV1DeltaTime = 0;

  memset(&Event->PrimaryHeader,0,sizeof(PrimaryHeaderBlock));
  memset(&Event->SecondaryHeader,0,sizeof(SecondaryHeaderBlock));

  memset(&Event->JMDC,0,sizeof(JMDCblock));
  memset(&Event->JLV1,0,sizeof(JLV1block));

  memset(&Event->JINJ,0,NJINJ*sizeof(BlockStat));
  memset(&Event->JINF,0,NJINJ*NJINJSlaves*sizeof(BlockStat));
  memset(&Event->RDR,0,2*12*sizeof(BlockStat));

  memset(&Event->RICH,0,sizeof(RICHblock));

}


void EventPrint(Event *Event){
  int i,j;

  printf(">>>>>> EVENT PRINT <<<<<<\n");

  printf("Data Blocks: %d\n",Event->DataBlocks);

  printf("JINJ Slaves: ");
  for (i=0; i<6; i++) {
    for (j=0; j<4; j++) printf("%1d",(Event->JINJSlaves>>(23-4*i-j))&1);
    printf(" ");
  }
  printf("\n");

  if (Event->PrimaryHeader.Size) {
    printf("Event Header:\n Event.\tSize\t%d\n \tRR\t%d\n \tRW\t%d\n \tNode\t%#x\n \tType\t%#x\n \tStatus\t%#x\n \tTag\t%#x\n \tTime\t%d\n",
	 Event->PrimaryHeader.Size,
	 Event->PrimaryHeader.RR,
	 Event->PrimaryHeader.RW,
	 Event->PrimaryHeader.Node,
	 Event->PrimaryHeader.Type,
	 Event->SecondaryHeader.Status,
	 Event->SecondaryHeader.Tag,
	 Event->SecondaryHeader.Time);
    printf("\n");
  }

  if (Event->JMDC.Size) {
    printf("JMDC Block:\n \tSize\t%d\n \tRR\t%d\n \tRW\t%d\n \tNode\t%#x\n \tType\t%#x\n \tTime\t%d\n \tRunTag\t%#x\n \tRunID\t%d\n \tEventNo\t%d\n \tCmdNb\t%d\n \tNbCmds\t%d", 
	 Event->JMDC.Size,
	 Event->JMDC.RR,
	 Event->JMDC.RW,
	 Event->JMDC.Node,
	 Event->JMDC.Type,
	 Event->JMDC.Time,
	 Event->JMDC.RunTag,
	 Event->JMDC.RunID,
	 Event->JMDC.EventNo,
	 Event->JMDC.CmdNb,
	 Event->JMDC.NbCmds);
    printf("\n \tLV3\t%#x\t%#x\t%#x\t%#x",
	 Event->JMDC.LV3[0],
	 Event->JMDC.LV3[1],
	 Event->JMDC.LV3[2],
	 Event->JMDC.LV3[3]);
    printf("\n  \tGPS\t%#x\t%#x\t%#x\t%#x\t%#x",
	 Event->JMDC.GPS[0],
	 Event->JMDC.GPS[1],
	 Event->JMDC.GPS[2],
	 Event->JMDC.GPS[3],
	 Event->JMDC.GPS[4]);
    printf("\n");
  }

  /*printf("%s\n",Event->cstat);*/

  int jinj, slave, rdr;
  for (jinj=0; jinj<NJINJ; jinj++) {
    BlockStat *jinj_stat = &Event->JINJ[jinj];
    if (jinj_stat->Length>0) {
      printf("\n%s: Status %#04x, Length %#04x, event %#04x\n", JINJ[jinj], jinj_stat->Status, jinj_stat->Length, jinj_stat->event);
       for (slave=0; slave<NJINJSlaves; slave++) {
          BlockStat *jinf_stat = &Event->JINF[jinj][slave];
          if (jinf_stat->Length>0) {
            printf("\t%s:  Status %#04x, Length %#04x", JINJLNK[slave], jinf_stat->Status, jinf_stat->Length);
            if (jinf_stat->rx_sta)
              printf(", rx_sta %#04x, rx_len %#04x, event %#04x\n",
                jinf_stat->rx_sta, jinf_stat->rx_len, jinf_stat->event);
            else printf("\n");
          }
          switch (slave) {
            case JINFR0:
            case JINFR1:
              for (rdr=0; rdr<12; rdr++) {
                 BlockStat *rdr_stat = &Event->RDR[slave-JINFR0][rdr];
                 if (rdr_stat->Length>0) {
                   printf("\t\t%s:  Status %#04x, Length %#04x", RDR[slave-JINFR0][rdr], rdr_stat->Status, rdr_stat->Length);
                   if (rdr_stat->rx_sta) 
                     printf(", rx_sta %#04x, rx_len %#04x, event %#04x\n", 
                       rdr_stat->rx_sta, rdr_stat->rx_len, rdr_stat->event);
                   else printf("\n");
                 }
              }
              break;
            default:
              break;
          }
       }
    }
  }
}


int Init(){
  int i,j;

  int NodeAddress;
  for (NodeAddress=AMSaddrFirst; NodeAddress<=AMSaddrLast; NodeAddress++) {
    is_JMDC[NodeAddress] = is_jmdc(NodeAddress);
    is_AMSW[NodeAddress] = is_amsw(NodeAddress);
    is_CAN[NodeAddress]  = is_can(NodeAddress);
  }

 /* JMDC names */
  for (i=0; i<sizeof(JMDC)/sizeof(JMDC[0]); i++) sprintf(JMDC[i],_JMDC[i]);

  /* JINJ names */
  for (i=0; i<sizeof(JINJ)/sizeof(JINJ[0]); i++) sprintf(JINJ[i],_JINJ[i]);

  /* JINF names */
  for (i=0; i<sizeof(JINF)/sizeof(JINF[0]); i++) sprintf(JINF[i],_JINF[i]);

  /* JPD names */
  for (i=0; i<sizeof(JPD)/sizeof(JPD[0]); i++) sprintf(JPD[i],_JPD[i]);

  /* JINJ Links */
  for (i=0; i<sizeof(JINJLNK)/sizeof(JINJLNK[0]); i++) sprintf(JINJLNK[i],_JINJLNK[i]);

  /* JINJ Reduced Node Addr. -> A/B side (A=1, B=0) and aJINR (0-3) maps (default AMSnode Table assumed) */
  memcpy(JLINK_SMAP, _JLINK_SMAP, sizeof(JLINK_SMAP));
  memcpy(JLINK_JINR, _JLINK_JINR, sizeof(JLINK_JINR));

  /* JINR Reduced Node Addr. -> A/B side map (A=1, B=0) (dflt. AMSnode Table assumed for logical addr.) */
  memcpy(NADDR_SMAP, _NADDR_SMAP, sizeof(NADDR_SMAP));

  /* JINR Reduced Node Addr. -> aJINR map (dflt. AMSnode Table assumed for logical addr.) */
  memcpy(NADDR_JINR, _NADDR_JINR, sizeof(NADDR_JINR));

  /* aJINR -> JINR Reduced Node Addr. map (physical node Addresses)*/
  memcpy(JINR_INDEX, _JINR_INDEX, sizeof(JINR_INDEX));

  /* RDR Links in JINR-A/B sides */
  memcpy(RLINKA, _RLINKA, sizeof(RLINKA));
  memcpy(RLINKB, _RLINKB, sizeof(RLINKB));

  /* JINR Node Names */
  for (i=0; i<sizeof(JINR)/sizeof(JINR[0]); i++) sprintf(JINR[i],_JINR[i]);

  /* RDR Node Names  */
  for (i=0; i<2; i++)
    for (j=0; j<12; j++) sprintf(RDR[i][j],_RDR[i][j]);

  /* RDR PMTs */
  memcpy(PMTS, _PMTS, sizeof(PMTS));

  for (i=0; i<sizeof(DATA)/sizeof(DATA[0]); i++) sprintf(DATA[i],_DATA[i]);

  for (i=0; i<sizeof(RICH_CALIB_TABLE)/sizeof(RICH_CALIB_TABLE[0]); i++) sprintf(RICH_CALIB_TABLE[i],_RICH_CALIB_TABLE[i]);

  for (i=0; i<sizeof(GAIN)/sizeof(GAIN[0]); i++) sprintf(GAIN[i],_GAIN[i]);

  /* RDR TYPE FOR FE LV MONITOR */
  memcpy(RDR_TYPE, _RDR_TYPE, sizeof(RDR_TYPE));
  memcpy(RDR_FE_LV, _RDR_FE_LV, sizeof(RDR_FE_LV));
 
  /* PMT Base Addr */
  int rdr, pmtb=0;
  for (rdr=0; rdr<NRDR; rdr++) {
     PMTB[rdr/12][rdr%12] = pmtb;
     pmtb += PMTS[rdr/12][rdr%12];
  }

  return 1;
}


int is_jmdc(u16 NodeAddress) {

   switch(NodeAddress) {

     case JMDCaddrFirst ... JMDCaddrLast:
          return 1;

   }

   return 0;
}

int is_amsw(u16 NodeAddress) {

   switch(NodeAddress) {

     case JMDCaddrFirst ... JMDCaddrLast:
     case JINJaddrFirst ... JINJaddrLast:
     case JLV1addrFirst ... JLV1addrLast:
     case ETRGaddrFirst ... ETRGaddrLast:
     case JINFaddrFirst ... JINFaddrLast:
     case xDRaddrFirst  ... xDRaddrLast:
          return 1;

   }

   return 0;
}

int is_can(u16 NodeAddress) {

   switch(NodeAddress) {

     case JPDaddrFirst ... JPDaddrLast:
          return 1;

   }

   return 0;
}



/////////////////// DEBUG //////////////////////


void RDRHKinfoPrint(int aCDP, RDRHouseKeeping *RDRHKinfo) {
  int i,nl=0;

  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\t>>>>>> RDR HKINFO PRINT <<<<<<\n");

//  int node = RDRHouseKeeping->PrimaryHeader.Node;
  int node = RDRaddrFirst+aCDP;
  int jind = (node-RDRaddrFirst)/12;
  int rind = (node-RDRaddrFirst)%12;

  static int HKComOffset, HKRdrOffset;
  static int HKComLength, HKRdrLength;
  static int first=1;
  if (first) {
    first=0;
    HKComOffset=(sizeof(PrimaryHeaderBlock)+sizeof(SecondaryHeaderBlock)+sizeof(AMSblock))/sizeof(u16);
    HKComLength=sizeof(HKComBlock)/sizeof(u16);
    HKRdrOffset=HKComOffset+HKComLength;
    HKRdrLength=sizeof(HKRdrBlock)/sizeof(u16);
  }

  char fdate[40],BlockDate[40];
  time_t BlockTime = (time_t)RDRHKinfo->SecondaryHeader.Time;
  sprintf(fdate, (char *)ctime(&BlockTime));
  sprintf(BlockDate,"");
  strncat(BlockDate, fdate, strlen(fdate)-1);

  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\tLength %d, Node %#x, Type %#x\n",
    RDRHKinfo->PrimaryHeader.Length, 
    RDRHKinfo->PrimaryHeader.Node, 
    RDRHKinfo->PrimaryHeader.Type);
  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\tStatus %#x, Tag %#x, Time %d (%s)\n",
    RDRHKinfo->SecondaryHeader.Status, 
    RDRHKinfo->SecondaryHeader.Tag, 
    RDRHKinfo->SecondaryHeader.Time,
    BlockDate);
  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\tEvent %d, Time %d\n",
    RDRHKinfo->Event,
    RDRHKinfo->Time);
  if (nl<MAXLP)
   sprintf(lout[nl++],"\t\tNode %#x (%s)\n", node, RDR[jind][rind]);

  if (DEBUG>0)
    for (i=0; i<nl; i++) printf(lout[i]);

  u16 *p;
  u16 sd_vers_id;

  p=(u16 *)RDRHKinfo+HKComOffset;
  HKComPrint(1,(HKComBlock *)p);
 
  sd_vers_id = ((HKComBlock *)p)->sd_vers_id;
  p=(u16 *)RDRHKinfo+HKRdrOffset;
  HKRdrPrint((HKRdrBlock *)p, sd_vers_id);
}

void HKComPrint(int isCDP, HKComBlock *HKCom) {

  int i=0,nl=0;
  int length;

  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\t>>>>>>>>>>>>> COMMON PART <<<<<<<<<<<<\n");

  u16 *p=(u16 *)HKCom;
  length=sizeof(*HKCom)/sizeof(*p);

  if (nl<MAXLP) {
    sprintf(lout[nl],"\t\t");
    while (i<length) {
      char ch[10];
      sprintf(ch,"%04x ",*p++);
      strcat(lout[nl],ch);
      if (++i%16==0 && i<length) {
        if (nl<MAXLP) {
           nl++;
           sprintf(lout[nl],"\n\t\t");
        }
      }
    }
    strcat(lout[nl++],"\n");
  }

  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\tProgram Version ID : %#x\n\t\tSubdetector Version ID : %#x\n\t\tNode status word : %#x\n\t\tLast Event Number : %#x\n\t\tAverage Event processing time : %#x\n",
      HKCom->sw_vers_id,
      HKCom->sd_vers_id,
      HKCom->node_status,
      HKCom->last_evt_nb,
      HKCom->ave_proc_time);

  if (DEBUG>0)
    for (i=0; i<nl; i++) printf(lout[i]);


}

void HKJinrPrint(HKJinrBlock *HKJinr) {

  int i=0,nl=0;
  int length;

  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\t>>>>>>>>> JINFR SPECIFIC WORDS <<<<<<<<<\n");

  u16 *p=(u16 *)HKJinr;
  length=sizeof(*HKJinr)/sizeof(*p);

  if (nl<MAXLP) {
    sprintf(lout[nl],"\t\t");
    while (i<length) {
      char ch[10];
      sprintf(ch,"%04x ",*p++);
      strcat(lout[nl],ch);
      if (++i%16==0 && i<length) {
        if (nl<MAXLP) {
           nl++;
           sprintf(lout[nl],"\n\t\t");
        }
      }
    }
    strcat(lout[nl++],"\n");
  }

  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\tBUSY Error_H : %#x\n\t\tBUSY Error_L : %#x\n",
      HKJinr->busy_error_h,
      HKJinr->busy_error_l);

  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\tJINF-R Status Word : %#x\n\t\tSlave Mask Error Code : %#x\n\t\tInternal JINF-R registers Error Code : %#x\n\t\tRHV0 Error Code : %#x\n\t\tRHV1 Error Code : %#x\n\t\tRHV2 Error Code : %#x\n\t\tRHV3 Error Code : %#x\n\t\tRHV0 Status || Error5 : %#x\n\t\tRHV1 Status || Error5 : %#x\n\t\tRHV2 Status || Error5 : %#x\n\t\tRHV3 Status || Error5 : %#x\n",
      HKJinr->jinr.status,
      HKJinr->jinr.slave_mask_error_code,
      HKJinr->jinr.internal_reg_error_code,
      HKJinr->jinr.rhv0_error_code,
      HKJinr->jinr.rhv1_error_code,
      HKJinr->jinr.rhv2_error_code,
      HKJinr->jinr.rhv3_error_code,
      HKJinr->jinr.rhv0_status_error5,
      HKJinr->jinr.rhv1_status_error5,
      HKJinr->jinr.rhv2_status_error5,
      HKJinr->jinr.rhv3_status_error5);

  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\tReply Status : %#x\n\t\tFrame Check Sequence : %#x\n",
      HKJinr->status,
      HKJinr->crc16);

  if (DEBUG>0)
    for (i=0; i<nl; i++) printf(lout[i]);

}

void HKRdrPrint(HKRdrBlock *HKRdr, u16 sd_vers_id) {
  int i=0,nl=0;
  int length;
  int offset;

  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\t>>>>>>>>> CDP SPECIFIC WORDS <<<<<<<<<\n");

  u16 *p=(u16 *)HKRdr;
  length=sizeof(*HKRdr)/sizeof(*p);

  offset = 3;
  if (sd_vers_id>= 0xa40d) offset = 1;
  if (sd_vers_id>= 0xa716) offset = 0;

  if (nl<MAXLP) {
    sprintf(lout[nl],"\t\t");
    while (i<length-offset) {
      char ch[10];
      sprintf(ch,"%04x ",*p++);
      strcat(lout[nl],ch);
      if (++i%16==0 && i<length) {
        if (nl<MAXLP) {
           nl++;
           sprintf(lout[nl],"\n\t\t");
        }
      }
    }
    strcat(lout[nl++],"\n");
  }

  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\tCalibration Type : %#x\n\t\tCalibration Status : %#x\n",
      HKRdr->calib_type,
      HKRdr->calib_status);

  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\tStatus Word : %#x\n\t\tNb. chn. masked by user : %#x\n\t\tNb. chn. failed iterations : %#x\n\t\tNb. chn. failed overfow : %#x\n\t\tNb. chn. masked : %#x\n\t\tMin. FE LV value 1  : %#x\n\t\tMax. FE LV value 1 : %#x\n\t\tMin. FE LV value 2  : %#x\n\t\tMax. FE LV value 2 : %#x\n\t\tNumber Processed Events : %#x\n\t\tMean Nb. of hits (x64) : %#x\n\t\tMax. Nb. of hits : %#x\n\t\tNb. Evts. above Low Thrs. : %#x\n\t\tNb. Evts. above High Thrs. : %#x\n",
      HKRdr->rdr.status,
      HKRdr->rdr.chn_masked_user,
      HKRdr->rdr.chn_masked_iter,
      HKRdr->rdr.chn_masked_over,
      HKRdr->rdr.chn_masked,
      HKRdr->rdr.min_lv_1,
      HKRdr->rdr.max_lv_1,
      HKRdr->rdr.min_lv_2,
      HKRdr->rdr.max_lv_2,
      HKRdr->rdr.proc_evts,
      HKRdr->rdr.mean_len,
      HKRdr->rdr.max_len,
      HKRdr->rdr.evts_above_low_thr_evt_len,
      HKRdr->rdr.evts_above_hgh_thr_evt_len);

  if (sd_vers_id>= 0xa40d) {
    if (nl<MAXLP)
      sprintf(lout[nl++],"\t\tPMT Status W1 : %#x\n\t\tPMT Status W2 : %#x\n",
      HKRdr->rdr.pmt_status_w1,
      HKRdr->rdr.pmt_status_w2);
  }

  if (sd_vers_id>= 0xa716) {
    if (nl<MAXLP)
      sprintf(lout[nl++],"\t\tNb. Bad Hits : %#x\n",
      HKRdr->rdr.bad_hits);
  }

  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\tReply Status : %#x\n\t\tFrame Check Sequence : %#x\n",
      *(&HKRdr->status-offset),
      *(&HKRdr->crc16-offset));

  if (DEBUG>0)
    for (i=0; i<nl; i++) printf(lout[i]);

}


void JINRHKinfoPrint(int aJINR, JINRHouseKeeping *JINRHKinfo) {
  int i,nl=0;


  if (nl<MAXLP) 
    sprintf(lout[nl++],"\t\t>>>>>> JINFR HKINFO PRINT <<<<<<\n");

  int node = JINRaddrFirst+JINR_INDEX[aJINR];
  int jind = node-JINRaddrFirst;
 
  static int HKComOffset, HKJinrOffset;
  static int HKComLength, HKJinrLength;
  static int first=1;
  if (first) {
    first=0;
    HKComOffset=(sizeof(PrimaryHeaderBlock)+sizeof(SecondaryHeaderBlock)+sizeof(AMSblock))/sizeof(u16);
    HKComLength=sizeof(HKComBlock)/sizeof(u16);
    HKJinrOffset=HKComOffset+HKComLength;
    HKJinrLength=sizeof(HKJinrBlock)/sizeof(u16);
  }

  char fdate[40],BlockDate[40];
  time_t BlockTime = (time_t)JINRHKinfo->SecondaryHeader.Time;
  sprintf(fdate, (char *)ctime(&BlockTime));
  sprintf(BlockDate,"");
  strncat(BlockDate, fdate, strlen(fdate)-1);

  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\tLength %d, Node %#x, Type %#x\n",
    JINRHKinfo->PrimaryHeader.Length, 
    JINRHKinfo->PrimaryHeader.Node, 
    JINRHKinfo->PrimaryHeader.Type);
  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\tStatus %#x, Tag %#x, Time %d (%s)\n",
    JINRHKinfo->SecondaryHeader.Status, 
    JINRHKinfo->SecondaryHeader.Tag, 
    JINRHKinfo->SecondaryHeader.Time,
    BlockDate);
  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\tEvent %d, Time %d\n",
    JINRHKinfo->Event,
    JINRHKinfo->Time);
  if (nl<MAXLP)
   sprintf(lout[nl++],"\t\tNode %#x (%s)\n", node, JINR[jind]);

  if (DEBUG>0)
    for (i=0; i<nl; i++) printf(lout[i]);

  u16 *p;

  p=(u16 *)JINRHKinfo+HKComOffset;
  HKComPrint(0,(HKComBlock *)p);
 
  p=(u16 *)JINRHKinfo+HKJinrOffset;
  HKJinrPrint((HKJinrBlock *)p);
}



void JINRConfigPrint(int aJINR, JINRConfiguration *JINRConfig) {

  int i,nl=0;

  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\t>>>>>> JINFR CONFIG PRINT <<<<<<\n");

  int node = JINRaddrFirst+JINR_INDEX[aJINR];
  int jind = node-JINRaddrFirst;

  static int JINRx2Offset;
  static int JINRx2Length;
  static int first=1;
  if (first) {
    first=0;
    JINRx2Offset=(sizeof(PrimaryHeaderBlock)+sizeof(SecondaryHeaderBlock)+sizeof(AMSblock))/sizeof(u16);
    JINRx2Length=sizeof(JINRx2Block)/sizeof(u16);
  }

  char fdate[40],BlockDate[40];
  time_t BlockTime = (time_t)JINRConfig->SecondaryHeader.Time;
  sprintf(fdate, (char *)ctime(&BlockTime));
  sprintf(BlockDate,"");
  strncat(BlockDate, fdate, strlen(fdate)-1);

  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\tLength %d, Node %#x, Type %#x\n",
      JINRConfig->PrimaryHeader.Length,
      JINRConfig->PrimaryHeader.Node, 
      JINRConfig->PrimaryHeader.Type);
  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\tStatus %#x, Tag %#x, Time %d (%s)\n",
      JINRConfig->SecondaryHeader.Status, 
      JINRConfig->SecondaryHeader.Tag, 
      JINRConfig->SecondaryHeader.Time,
      BlockDate);
  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\tEvent %d, Time %d\n",
      JINRConfig->Event,
      JINRConfig->Time);
  if (nl<MAXLP)
   sprintf(lout[nl++],"\t\tNode %#x (%s)\n", node, JINR[jind]);

  if (DEBUG>0)
    for (i=0; i<nl; i++) printf(lout[i]);

  u16 *p;
  p=(u16 *)JINRConfig+JINRx2Offset;
  JINRx2Print((JINRx2Block *)p);
 
}

void JINRx2Print(JINRx2Block *JINRx2) {

  int i,nl=0;

  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\t>>>>>>>> SLAVE MASKS <<<<<<<<<<");
  int msk;
  for (msk=0; msk<16; msk++) {
    if (!(msk%4))
      if (nl<MAXLP) sprintf(lout[nl++],"\n\t\t%02d-%02d :",msk,msk+3);
    if (nl<MAXLP)
      sprintf(lout[nl++]," %#04x%04x (%#04x%04x)",JINRx2->SlaveMask[2*msk][0],JINRx2->SlaveMask[2*msk+1][0],
                                      JINRx2->SlaveMask[2*msk][1],JINRx2->SlaveMask[2*msk+1][1]);
  }

  if (nl<MAXLP)
    sprintf(lout[nl++],"\n\t\tSlaveMaskErrorCode : %#04x %#04x\n",JINRx2->SlaveMaskErrorCode[0],JINRx2->SlaveMaskErrorCode[1]);

  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\t>>>>> INTERNAL REGISTERS <<<<<<");
  int reg;
  for (reg=0; reg<4; reg++) {
    if (!(reg%4))
      if (nl<MAXLP) sprintf(lout[nl++],"\n\t\t%02d-%02d :",reg,reg+3);
    if (nl<MAXLP)
      sprintf(lout[nl++]," %#04x (%#04x)",JINRx2->Register[reg][0],JINRx2->Register[reg][1]);
  }
  if (nl<MAXLP)
    sprintf(lout[nl++],"\n\t\tRegisterErrorCode  : %#04x %#04x\n",JINRx2->RegisterErrorCode[0],JINRx2->RegisterErrorCode[1]);

  int rhv;
  for (rhv=0; rhv<4; rhv++) {
     if (nl<MAXLP) sprintf(lout[nl++],"\t\t>>>>>>>>>> RHV%d <<<<<<<<<<<<<\n",rhv);

     if (nl<MAXLP) sprintf(lout[nl++],"\t\tValues:");
     int val;
     for (val=0; val<42; val++) {
       if (nl<MAXLP) {
         if (!val)               sprintf(lout[nl++],"\n\t\tDC/DC - %02d-%02d :",val,val+1);
         else if (!((val-2)%10)) sprintf(lout[nl++],"\n\t\tLR-%d  - %02d-%02d :",val/10,val,val+4);
         else if (!((val-2)%5 )) sprintf(lout[nl++],"\n\t\t        %02d-%02d :",val,val+4);
       }
       if (nl<MAXLP)
         sprintf(lout[nl++]," %#04x (%#04x)",JINRx2->JINRhv[rhv].Value[val][0],JINRx2->JINRhv[rhv].Value[val][1]);
     }
     if (nl<MAXLP)
       sprintf(lout[nl++],"\n\t\t->  Status: %#04x %#04x\n",JINRx2->JINRhv[rhv].Status[0],JINRx2->JINRhv[rhv].Status[1]);

     if (nl<MAXLP) sprintf(lout[nl++],"\t\tErrors:");
     int err;
     for (err=0; err<12; err++) {
       if (!(err%6)) 
         if (nl<MAXLP) sprintf(lout[nl++],"\n\t\t%02d-%02d :",err,err+5);
       if (nl<MAXLP)
         sprintf(lout[nl++],"%#04x ",JINRx2->JINRhv[rhv].Error[err]);
     }
     if (nl<MAXLP)
       sprintf(lout[nl++],"\n\t\t-> ErrCode: %#04x %#04x\n",JINRx2->JINRhv[rhv].ErrorCode[0],JINRx2->JINRhv[rhv].ErrorCode[1]);
  }

  if (nl<MAXLP) sprintf(lout[nl++],"\t\t>>>>>>> JINFR STATUS <<<<<<<<<<<<\n");
  if (nl<MAXLP) sprintf(lout[nl++],"\t\tStatus: %#04x\n",JINRx2->JINRstatus);

  if (DEBUG>0)
    for (i=0; i<nl; i++) printf(lout[i]);

}


void RDRConfigPrint(int aCDP, RDRConfiguration *RDRConfig, u16 SubdProgramVersion, int offset) {

  int i,nl=0;

  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\t>>>>>> RDR CONFIG PRINT <<<<<<\n");

  int node = RDRaddrFirst+aCDP;
  int jind = (node-RDRaddrFirst)/12;
  int rind = (node-RDRaddrFirst)%12;

  char fdate[40],BlockDate[40];
  time_t BlockTime = (time_t)RDRConfig->SecondaryHeader.Time;
  sprintf(fdate, (char *)ctime(&BlockTime));
  sprintf(BlockDate,"");
  strncat(BlockDate, fdate, strlen(fdate)-1);

  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\tLength %d, Node %#x, Type %#x\n",
      RDRConfig->PrimaryHeader.Length, 
      RDRConfig->PrimaryHeader.Node, 
      RDRConfig->PrimaryHeader.Type);
  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\tStatus %#x, Tag %#x, Time %d (%s)\n",
      RDRConfig->SecondaryHeader.Status, 
      RDRConfig->SecondaryHeader.Tag, 
      RDRConfig->SecondaryHeader.Time,
      BlockDate);
  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\tEvent %d, Time %d\n",
      RDRConfig->Event,
      RDRConfig->Time);
  if (nl<MAXLP)
   sprintf(lout[nl++],"\t\tNode %#x (%s)\n", node, RDR[jind][rind]);

  if (DEBUG>0)
    for (i=0; i<nl; i++) printf(lout[i]);

  RDRVerPrint((RDRVerBlock *)&RDRConfig->RDRVer, SubdProgramVersion, offset);
  RDRSeqPrint((RDRSeqBlock *)&RDRConfig->RDRSeq, SubdProgramVersion, offset);
  RDRProPrint((RDRProBlock *)&RDRConfig->RDRPro, SubdProgramVersion, offset);
  RDRUsrPrint((RDRUsrBlock *)&RDRConfig->RDRUsr, SubdProgramVersion, offset);
  RDRCalPrint((RDRCalBlock *)&RDRConfig->RDRCal, SubdProgramVersion, offset);
  RDRMisPrint((RDRMisBlock *)&RDRConfig->RDRMis, SubdProgramVersion, offset);

}

void RDRVerPrint(RDRVerBlock *RDRVer, u16 SubdProgramVersion, int offset) {

  int i=0,nl=0;
  int length;

  if (SubdProgramVersion<0xB210) return;

  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\t>>>>>> PROGRAM VERSION <<<<<<\n");

  u16 *p=(u16 *)RDRVer;
  length=sizeof(*RDRVer)/sizeof(*p);
 
  if (nl<MAXLP) {
    sprintf(lout[nl],"\t\t");
    while (i<length) {
      char ch[10];
      sprintf(ch,"%04x ",*p++);
      strcat(lout[nl],ch);
      if (++i%16==0 && i<length) {
        if (nl<MAXLP) {
           nl++;
           sprintf(lout[nl],"\n\t\t");
        }
      }
    }
    strcat(lout[nl++],"\n");
  }

  if (DEBUG>0)
    for (i=0; i<nl; i++) printf(lout[i]);

}

void RDRSeqPrint(RDRSeqBlock *RDRSeq, u16 SubdProgramVersion, int offset) {

  int i=0,nl=0;
  int length;

  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\t>>>>>> RDR SEQUENCER PARAMETERS <<<<<<\n");

  u16 *p=(u16 *)RDRSeq;
  length=sizeof(*RDRSeq)/sizeof(*p);
 
  if (nl<MAXLP) {
    sprintf(lout[nl],"\t\t");
    while (i<length) {
      char ch[10];
      sprintf(ch,"%04x ",*p++);
      strcat(lout[nl],ch);
      if (++i%16==0 && i<length) {
        if (nl<MAXLP) {
           nl++;
           sprintf(lout[nl],"\n\t\t");
        }
      }
    }
    strcat(lout[nl++],"\n");
  }

  if (DEBUG>0)
    for (i=0; i<nl; i++) printf(lout[i]);

}

void RDRProPrint(RDRProBlock *RDRPro, u16 SubdProgramVersion, int offset) {

  int i=0,nl=0;
  int length;

  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\t>>>>>> RDR PROCESSING PARAMETERS <<<<<<\n");

  u16 *p=(u16 *)RDRPro;
  length=sizeof(*RDRPro)/sizeof(*p) - offset;

  if (nl<MAXLP) {
    sprintf(lout[nl],"\t\t");
    while (i<length) {
      char ch[10];
      sprintf(ch,"%04x ",*p++);
      strcat(lout[nl],ch);
      if (++i%16==0 && i<length) {
        if (nl<MAXLP) {
           nl++;
           sprintf(lout[nl],"\n\t\t");
        }
      }
    }
    strcat(lout[nl++],"\n");
  }

  if (DEBUG>0)
    for (i=0; i<nl; i++) printf(lout[i]);

}

void RDRUsrPrint(RDRUsrBlock *RDRUsr, u16 SubdProgramVersion, int offset) {

  int i=0,nl=0;
  int length;

  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\t>>>>>> RDR PMT USER MASK PARAMETERS <<<<<<\n");

  u16 *p=(u16 *)RDRUsr;
  length=sizeof(*RDRUsr)/sizeof(*p);

  if (nl<MAXLP) {
    sprintf(lout[nl],"\t\t");
    while (i<length) {
      char ch[10];
      sprintf(ch,"%04x ",*p++);
      strcat(lout[nl],ch);
      if (++i%16==0 && i<length) {
        if (nl<MAXLP) {
           nl++;
           sprintf(lout[nl],"\n\t\t");
        }
      }
    }
    strcat(lout[nl++],"\n");
  }

  if (DEBUG>0)
    for (i=0; i<nl; i++) printf(lout[i]);

}

void RDRMisPrint(RDRMisBlock *RDRMis, u16 SubdProgramVersion, int offset) {

  int i=0,nl=0;
  int length;

  if (SubdProgramVersion<0xB210) return;

  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\t>>>>>> MISMATCH COUNTER <<<<<<\n");

  u16 *p=(u16 *)RDRMis;
  length=sizeof(*RDRMis)/sizeof(*p);
 
  if (nl<MAXLP) {
    sprintf(lout[nl],"\t\t");
    while (i<length) {
      char ch[10];
      sprintf(ch,"%04x ",*p++);
      strcat(lout[nl],ch);
      if (++i%16==0 && i<length) {
        if (nl<MAXLP) {
           nl++;
           sprintf(lout[nl],"\n\t\t");
        }
      }
    }
    strcat(lout[nl++],"\n");
  }

  if (DEBUG>0)
    for (i=0; i<nl; i++) printf(lout[i]);

}

void RDRCalPrint(RDRCalBlock *RDRCal, u16 SubdProgramVersion, int offset) {

  int i=0,nl=0;
  int length;

  if (SubdProgramVersion<0xB210) return;

  if (nl<MAXLP)
    sprintf(lout[nl++],"\t\t>>>>>> RDR PMT CALIBRATION MASK PARAMETERS <<<<<<\n");

  u16 *p=(u16 *)RDRCal;
  length=sizeof(*RDRCal)/sizeof(*p);

  if (nl<MAXLP) {
    sprintf(lout[nl],"\t\t");
    while (i<length) {
      char ch[10];
      sprintf(ch,"%04x ",*p++);
      strcat(lout[nl],ch);
      if (++i%16==0 && i<length) {
        if (nl<MAXLP) {
           nl++;
           sprintf(lout[nl],"\n\t\t");
        }
      }
    }
    strcat(lout[nl++],"\n");
  }

  if (DEBUG>0)
    for (i=0; i<nl; i++) printf(lout[i]);

}

