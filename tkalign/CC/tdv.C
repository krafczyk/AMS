// $Id: tdv.C,v 1.1 2011/06/17 00:36:36 shaino Exp $
#include "TkDBc.h"
#include "timeid.h"
#include "commonsi.h"

int SaveAlign2DB(time_t brun, time_t erun, char * filename, int setup =3)
{
  TkDBc::CreateTkDBc();
  TkDBc::Head->init(setup,filename,1);
  TkDBc::CreateLinear();
  TkDBc::Head->Align2Lin();

  brun += 3600;
  erun += 3600;

  time_t bb= brun; 
  tm begin; gmtime_r(&brun, &begin);
  tm end;   gmtime_r(&erun, &end);
  cout << "Begin: " <<(int)brun<<"  " <<asctime(&begin)<<endl;
  cout << "End  : " <<(int)erun<<"  " <<asctime(&end  )<<endl;
  
  AMSTimeID *tt
    = new AMSTimeID(AMSID("TrackerAlignPM2",1),begin,end,
		    TkDBc::GetLinearSize(),
		    TkDBc::linear,AMSTimeID::Standalone,1);

  tt->UpdateMe();
  int pp=tt->write(AMSDATADIR.amsdatabase);

  return 0;
}

int main(int argc, char *argv[])
{
  if (argc!=4) {
    printf("Usage:  %s  <begin run> <end run> <tkdbc> \n",argv[0]);
    return 1;
  }

  time_t brun = atoi(argv[1]);
  time_t erun = atoi(argv[2]);
  return SaveAlign2DB(brun, erun, argv[3]);
}
