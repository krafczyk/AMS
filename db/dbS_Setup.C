// May  05, 1996. ak. Add methods, objectivity first try
// July 29, 1996. ak. Modify AddEvent, split Add and Fill functions
// Aug    , 1996. ak. All time is lost in commit, so ReadStartEnd flag is added
//                    to CopyMCEvent, CopyEvent 
//                    Add function GetNEvents;
//                    Add WriteStartEnd to AddEvent
// Sep    , 1996. ak. Add Geometry, Materials and TMedia.
//                    remove fRunNumber class member from list.ddl
//                    set map per list, not map per dbase as before
// Nov    , 1996. ak. exit, if Addamsdbc fails
// Dec  16, 1996. ak. oocMROW if mode == oocREAD
// Feb    , 1997. ak. mceventItr, AddTDV
//                    no map anymore, use indexes
// Mar  18, 1997. ak. Getmceventg and GetNEvents are modified
//                    setup moved to AMSsetupDB
// May  05, 1997. ak. separate file for setup
// July 01, 1997. ah. CmpGeometry method implemented
//
// last edit Nov 06, 1997, ak.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>

#include <sys/times.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/time.h>

#include <rd45.h>

#include <typedefs.h>
#include <db_comm.h>
#include <dbS.h>

#include <event.h>
#include <job.h>


#include <db_comm.h>

#include <gvolumeD_ref.h>
#include <gmatD_ref.h>
#include <gtmedD_ref.h>
#include <amsdbcD_ref.h>
#include <antidbcD_ref.h>
#include <ctcdbcD_ref.h>
#include <tofdbcD_ref.h>
#include <commonsD_ref.h>
#include <tkdbcV_ref.h>

#include <gvolumeD.h>
#include <gmatD.h>
#include <gtmedD.h>
#include <amsdbcD.h>
#include <antidbcD.h>
#include <ctcdbcD.h>
#include <tofdbcD.h>
#include <commonsD.h>
#include <tkdbcV.h>

#include <amsgobj.h>
#include <gmat.h>
#include <amsdbc.h>
#include <ctcdbc.h>
#include <gsen.h>

// debugging and testing values

integer  event_size;
integer  NN;
integer  NOBJ;


ooHandle(AMSgvolumeD)* geometryHT;
integer*               geometryP;
AMSgvolume**           ptrsGV;

// Containers
ooHandle(ooContObj)  contH;                   // container
ooHandle(ooContObj)  contGeometryH;           // geometry 
ooHandle(ooContObj)  contgmatH;               // material
ooHandle(ooContObj)  contgtmedH;              // tracking media

// ooObj classes
ooHandle(AMSgvolumeD)      geometryH;              
ooHandle(AMSgmatD)         gmatH;                      
ooHandle(AMSgtmedD)        gtmedH;                      
ooHandle(AMSDBcD)          amsdbcH;
ooHandle(AntiDBcD)         antidbcH;
ooHandle(CTCDBcD)          ctcdbcH;
ooHandle(TOFDBcD)          tofdbcH;
ooHandle(AMScommonsD)      commonsH;
ooHandle(TKDBcV)           tkdbcH;

// Iterators
ooItr(AMSgvolumeD)     geometryItr;             // geometry
ooItr(AMSgmatD)        gmatItr;                 // material
ooItr(AMSgtmedD)       gtmedItr;                // tmedia

ooItr(AMSDBcD)         amsdbcItr;               // amsdbc
ooItr(AntiDBcD)        antidbcItr;               // amsdbc
ooItr(TOFDBcD)         tofdbcItr;               // tofdbc
ooItr(AMScommonsD)     commonsItr;              // commons
ooItr(CTCDBcD)         ctcdbcItr;               // ctcdbc
ooItr(TKDBcV)          tkdbcItr;                // tkdbc

void CopyByPos(ooHandle(AMSgvolumeD)& ptr, ooMode mode)
{
  ooHandle(AMSgvolumeD) cur;
  ooHandle(AMSgvolumeD) tmp;
  //static integer nnn = 0;

  //if (ptr != NULL ) cout <<"ptr -> Pos "<< ptr -> getContPos()<<endl;
  //cout <<"in "<<++nnn<<endl;
  cur = ptr;
  while(cur != NULL) {
    geometryHT[NN] = cur;
    geometryP[NN]  = cur -> getContPos();
    NN++;
    //cout <<"NN... "<<NN<<" contPos "<<cur -> getContPos();
    //cout<<"down "<<cur-> _Down<<", next "<<cur-> _Next<<endl;
    tmp = cur -> _Down;
    if (tmp != NULL) CopyByPos(tmp, mode);
    cur = cur -> _Next;
  }   
  // cout <<"exit "<<nnn--<<endl;
}


ooStatus LMS::WriteGeometry()
{
  ooStatus  rstatus = oocError;
  ooMode    mode    = oocUpdate;
  ooItr(AMSgvolumeD)       geometryItrT;           // and iterator
  AMSgvolume *             cur;
  integer                  i=0;
  integer                  pos;

  AMSgvolume * pg = AMSJob::gethead() -> getgeom();
  if (pg == NULL) return rstatus;
 
  // get setup and container names
  //char* setup = AMSJob::gethead() -> getsetup();
  char *setup = StrDup(_setup);
  if(!_setup) Fatal("WriteGeometry : invalid setup name");
  if(dbg_prtout) cout <<"WriteGeometry -I- setup "<<setup<<endl;
  char* contName = StrCat("Geometry_",setup);

  // check container
  ooHandle(ooDBObj)    dbH = setupdb();
  int status = Container(dbH, contName, contGeometryH);
  if (status < 0) {
    Fatal("WriteGeometry : cannot open/create geometry container");
  } else {
    if (status == 1) {
     cout<<"WriteGeometry -I- Create container "<<contName<< endl;
    } 
    if (status == 0) {
     cout<<"AddGeometry -I- Found container "<<contName<< endl;
     geometryItr.scan(contGeometryH, mode);
     if (geometryItr.next()) {
      Warning("WriteGeometry : geometry container is not empty. Quit");
      return oocSuccess;
     }
    }
  }

  Message ("WriteGeometry : write new geometry");
  integer   id;
  char*     name;

  ooHandle(AMSgvolumeD) *geometryHT;

// save the whole tree first
  integer nobj =  0;     // number of geometry objects
  for (;;)
  {
    cur =  (AMSgvolume*)AMSgObj::GVolMap.getid(nobj);
    if (cur != NULL) nobj++;
    if (cur == NULL) break;
  }

  cout<<"WriteGeometry - I - found "<<nobj<<" geometry objects"
      <<endl;

  geometryHT = new ooHandle(AMSgvolumeD)[nobj];

  for (;;)
  {
    cur =  (AMSgvolume*)AMSgObj::GVolMap.getid(i);
    if (cur != NULL) {
     id = cur -> getid();
     name = cur -> getname();
     i++;
     cur -> setContPos(i);
     geometryH = new(contGeometryH) AMSgvolumeD(id, cur, name, i);
     rstatus = geometryH -> CmpGeometry(id, cur);
     if(rstatus != oocSuccess)
     Fatal("AddGeometry : comparison failed, please write new setup");
     geometryHT[i-1] = geometryH;
  } else {
      break;
   }
  }

// and now set pointers
  i=0;

  geometryItr.scan(contGeometryH, mode);

  while (geometryItr.next())
  {
    cur =  (AMSgvolume*)AMSgObj::GVolMap.getid(i);
    if (cur != NULL) {
     if (cur -> next() != NULL) {
       pos = cur -> next() -> getContPos(); geometryItr -> setNext(pos); 
       
     }
     if (cur -> prev() != NULL) {
      pos = cur -> prev() -> getContPos(); geometryItr -> setPrev(pos);
     }
     if (cur -> up()   != NULL) {
      pos = cur -> up()   -> getContPos(); geometryItr -> setUp(pos);
     }
     if (cur -> down() != NULL) {
      pos = cur -> down() -> getContPos(); geometryItr -> setDown(pos);
     }
     i++;    
    } 
  }

  geometryItr.scan(contGeometryH, mode);
  while (geometryItr.next())
  {
   pos = geometryItr -> getNext();
   if (pos > 0) geometryItr -> _Next = geometryHT[pos-1];
  
   pos = geometryItr -> getPrev();
   if (pos > 0) geometryItr -> _Prev = geometryHT[pos-1];

   pos = geometryItr -> getUp();
   if (pos > 0) geometryItr -> _Up = geometryHT[pos-1];

   pos = geometryItr -> getDown();
   if (pos > 0) geometryItr -> _Down = geometryHT[pos-1];

  }

  if(contName) delete [] contName;  
  return oocSuccess;
}

ooStatus LMS::CopyGeometry()
{
  ooStatus                 rstatus = oocError;
  ooMode                   mode = oocRead;

  AMSgvolume *             cur;
  AMSgvolume *             ptr;
 
  cout <<"LMS::CopyGeometry - let's copy geometry"<<endl;
 
  NN = 0;

  // get setup name
  //char* setup = AMSJob::gethead() -> getsetup();
  char *setup = StrDup(_setup);
  cout <<"LMS::CopyGeometry -I- setup name "<<setup<<endl;

  char* contName = StrCat("Geometry_",setup);
  cout <<"LMS::CopyGeometry -I- container "<<contName<<endl;

  ooHandle(ooDBObj) dbH = setupdb();
  int status = Container(dbH, contName, contGeometryH);
  if (status < 0) 
          Fatal("CopyGeometry : cannot find or open geometry container ");

  integer nobj = 0;
  geometryItr.scan(contGeometryH, mode);
  while (geometryItr.next())
  {
    nobj++;
  }

  if (nobj < 1) Fatal("CopyGeometry : geometry container is empty");

  NOBJ = nobj;
  cout <<"CopyGeometry -I- found "<<NOBJ<<" objects"<<endl;


  geometryHT = new ooHandle(AMSgvolumeD)[nobj+1];
  geometryP  = new integer[nobj+1];
  ptrsGV     = new AMSgvolume*[nobj+1];
  
  // find mother (or virtual top)
  geometryItr.scan(contGeometryH, mode);
  while (geometryItr.next())
  {
    if (geometryItr -> getUp() == 0  && geometryItr -> _Up == NULL) 
    {
        cout <<"mother  "<<geometryItr -> getid()<<" "
             <<geometryItr -> _name<<endl;
        //cout <<"position "<<geometryItr -> getContPos()<<endl;
        if (geometryItr -> _Next != 0) {
         geometryItr -> _Next = NULL;
         geometryItr -> setNext(0); 
         cout 
          <<"LMS::CopyGeometry -I- Next ptr of mother set to 0"<<endl;
        }
        CopyByPos(geometryItr, mode);
        break;
    }
  }

  //integer     j;
  AMSNode*    p;
  AMSgvolume* mother;

   for (integer j = 0; j<NN; j++)
  {
   AMSgvolume* p;
   AMSPoint                 coo[1];
   geant                    par[6];
   geant                    cooAG[3];
   geant                    cooG[3];
   number                   nbuff0[9];
   number                   nrmA[3][3];
   number                   nrm[3][3];
   char                     gonly[5];
   char                     shape[5];

   integer id     = geometryHT[j] -> getid();
   integer matter = geometryHT[j] -> getmatter();
   integer rotmno = geometryHT[j] -> getrotmno();
   char*   name   = geometryHT[j] -> getname();
                    geometryHT[j] -> getshape(shape);
                    geometryHT[j] -> getpar(par);

                    geometryHT[j] -> getcoo(coo);
                    cooG[0] = coo[0] [0];
                    cooG[1] = coo[0] [1];
                    cooG[2] = coo[0] [2];

                    geometryHT[j] -> getcooA(coo);
                    cooAG[0] = coo[0] [0];
                    cooAG[1] = coo[0] [1];
                    cooAG[2] = coo[0] [2];

                    geometryHT[j] -> getgonly(gonly);
   integer posp   = geometryHT[j] -> getposp();
   integer gid    = geometryHT[j] -> getgid();
   integer npar   = geometryHT[j] -> getnpar();

                    geometryHT[j] -> getnrmA(nbuff0);
                    UCOPY(nbuff0,nrmA,sizeof(number)*9/4);

                    geometryHT[j] -> getnrm(nbuff0);
                    UCOPY(nbuff0,nrm,sizeof(number)*9/4);

   integer rel    = geometryHT[j] -> getrel();
   if (rel == 1) 
    p = new AMSgvolume (matter, rotmno, name, shape, par, npar, 
                        cooG, nrm, gonly, posp, gid, rel);
   if (rel == 0) 
    p = new AMSgvolume (matter, rotmno, name, shape, par, npar, 
                        cooAG, nrmA, gonly, posp, gid, rel);
   
   integer pos = geometryHT[j] -> getContPos();
   p -> setContPos(pos);

   ptrsGV[j] = p;

   if (dbg_prtout > 1) cout <<"# "<<j<<"  "<<id<<" "<<name<<" "<<pos;

   if( geometryHT[j] -> getUp() != 0) 
    {
     integer posUp = geometryHT[j] -> _Up -> getContPos();
     for (integer l=0; l<j;l++)
      { 
       if (geometryP[l] == posUp) 
        {
         AMSgvolume* motherT = ptrsGV[l];
         motherT -> add(p);
         break;
        } 
      } 
    } else { 
    if (dbg_prtout) cout <<" virtual top"<<endl; 
    AMSJob::gethead() -> addup(p);
    mother = p;
    }
  }

  cout <<"LMS::CopyGeometry -I- Copy done "<<endl;
  GGCLOS();
  AMSgObj::GVolMap.map(*mother);

  if(contName) delete [] contName;  
  if(setup)    delete [] setup;  

  return oocSuccess;
}


ooStatus   LMS::AddGeometry()
{
	ooStatus 	       rstatus = oocError;	// Return status
        ooHandle(ooDBObj)      dbH;


        StartUpdate();

       dbH = setupdb();
       if (dbH == NULL) Fatal("AddGeom: Cannot open setup dbase mode");

       rstatus = WriteGeometry();

end:
        if (rstatus == oocSuccess) {
	  Commit(); 	           // Commit the transaction
        } else {
         Abort();  // or Abort it
         Fatal("AddGeometry: Quit");
        }

	// Return the status (oocSuccess or oocError)
	return rstatus;
}

ooStatus   LMS::AddMaterial()
{
	ooStatus 	     rstatus = oocError;	// Return status
        char                 err_mess[80];
        ooHandle(ooDBObj)      dbH;

// Start the transaction
        StartUpdate();
// Get pointer to default database
         dbH = setupdb();
         if (dbH == NULL) {
           Abort();
           Fatal("Cannot open setup dbase in oocUpdate mode");
         }

         AMSNode*  p;
         AMSgmat*  pp;
         integer   id;
         geant     a[10];
         geant     z[10];
         geant     w[10];
         char*     name;


         // get setup and container names
         //char* setup = AMSJob::gethead() -> getsetup();
         char *setup = StrDup(_setup);
         char* contName = StrCat("Materials_",setup);
         if(setup) delete [] setup;  

         // check container
         cout<<"AddMaterial -I- container name "<<contName<<endl;
         int status = Container(dbH, contName, contgmatH);
         if (status == 0) {
          cout << "AddMaterial -I- Found container "<<contName<< endl;
          p = AMSJob::gethead()->getnodep(AMSID("Materials:",0));
          if (p == NULL) 
                  Fatal("AddMaterial : cannot find the virtual top of gmat");

            p = p -> down();
            integer nobj = 0;
            gmatItr.scan(contgmatH, Mode());
            while (gmatItr.next()) {
            if (p == NULL) Fatal("AddMaterials : AMSgmat == NULL");

            pp = (AMSgmat*)p;
            id = p -> getid();
            rstatus = gmatItr -> CmpMaterials(id, pp);
            if (rstatus != oocSuccess) 
             Fatal("AddMaterials : comparison failed, please write new setup");
              p = p -> next();
              nobj++;
            }

            if (nobj == 0) {
             Warning("AddMaterials : container is empty");
             Message("AddMaterials : write new Materials objects");
             goto newsetup;
            }
          rstatus = oocSuccess;
          goto end;
         }

newsetup:
 // get pointer to the top

  p = AMSJob::gethead()->getnodep(AMSID("Materials:",0));
  if (p == NULL) Fatal("AddMaterial : cannot find the virtual top of gmat");

   p = p -> down();
   while (p != NULL) {
     if (dbg_prtout > 1) cout<<p -> getid() <<" "<<p -> getname();
     pp = (AMSgmat*)p;
     integer npar = pp -> getnpar();
     id = p -> getid();
     name = p -> getname();
     pp -> getNumbers(a, z, w);
     gmatH = new(contgmatH) AMSgmatD(id, pp, a, z, w, name);
     p = p -> next();
   }
 
   rstatus = oocSuccess;

end:
        if (rstatus == oocSuccess) {
          if(contName) delete [] contName;
	  Commit(); 	           // Commit the transaction
        } else {
         Abort();  // or Abort it
         Fatal(err_mess);
        }
	return rstatus;
}

ooStatus   LMS::AddTMedia()
{
	ooStatus 	     rstatus = oocError;	// Return status
        ooHandle(ooDBObj)    dbH;

        StartUpdate(); // Start the transaction
        dbH = setupdb();
        if (dbH == NULL) 
  	       Fatal("AddTMedia: Cannot open setup dbase in oocUpdate mode");

          integer   id;
          char*     name;
          AMSNode*  p;
          AMSgtmed* pp;

          // get setup and container names
          //char* setup = AMSJob::gethead() -> getsetup();
          char *setup = StrDup(_setup);
          if(!setup) Fatal("AddTMedia: invalid setup name");
          if(dbg_prtout) cout <<"AddTmedia -I- setup "<<setup<<endl;
          char* contName = StrCat("TMedia_",setup);
          if(setup) delete [] setup;  

          // check container
          cout<<"AddTMedia -I- container name "<<contName<<endl;
          int status = Container(dbH, contName, contgtmedH);
          if (status == 0) {
           cout << "AddTMedia -I- Found container "<<contName<< endl;
           p = AMSJob::gethead()->getnodep(AMSID("TrackingMedia:",0));
           if (p == NULL) 
                   Fatal("AddTMedia : cannot find the virtual top of gtmed");

            p = p -> down();
            integer nobj = 0;
            gtmedItr.scan(contgtmedH, Mode());
            while (gtmedItr.next()) {
             if (p == NULL) Fatal("AddTMedia : AMSgtmed == NULL");
             pp = (AMSgtmed*)p;
             id = p -> getid();
             rstatus = gtmedItr -> CmpTrMedia(id, pp);
             if (rstatus != oocSuccess) 
              Fatal("AddTrMedia : comparison failed. Pease, write new setup");
             p = p -> next();
             nobj++;
            }
           if (nobj == 0) {
            Message("AddTMedia : container is empty");
            Message("AddTMedia : write new TMedia objects ");
            goto newsetup;
           }
           rstatus = oocSuccess;
           goto end;
          }
newsetup:
  // get pointer to the top
  p = AMSJob::gethead()->getnodep(AMSID("TrackingMedia:",0));
  if (p == NULL)  Fatal("AddTMedia : cannot find the virtual top of gtmed");

   p = p -> down();
   while (p != NULL) {
    cout<<p -> getid() <<" "<<p -> getname();
     pp = (AMSgtmed*)p;
     id = p -> getid();
     name = p -> getname();
     gtmedH = new(contgtmedH) AMSgtmedD(id, pp, name);

     p = p -> next();
   }
 
  rstatus = oocSuccess;

end:
        if (rstatus == oocSuccess) {
          if(contName) delete [] contName;
	  Commit(); 	           // Commit the transaction
        } else {
         Abort();  // or Abort it
         Fatal("Quit in AddTMedia");
        }
	return rstatus;
}


ooStatus   LMS::Addamsdbc()
{
	ooStatus 	     rstatus = oocError;	// Return status
        ooHandle(ooDBObj)    dbH;
        ooHandle(ooContObj)  contH;

//
        StartUpdate(); // Start the transaction
        dbH = setupdb();
        if (dbH == NULL) Fatal("Addamsdbc : Cannot open setup(create) dbase.");

        // get setup and container names
        //char* setup = AMSJob::gethead() -> getsetup();
        char *setup = StrDup(_setup);
        cout <<"LMS::Addamsdb -I- setup name "<<setup<<endl;
        char* contName = StrCat("amsdbc_",setup);
        if(setup) delete [] setup;  

        // check container
        int status = Container(dbH, contName, contH);
        if (status == 1) 
        {
         cout << "Addamsdbc -I- Create container "<<contName<< endl;
         amsdbcH  = new(contH) AMSDBcD();
         commonsH = new(contH) AMScommonsD();
         ctcdbcH  = new(contH) CTCDBcD();
         tofdbcH  = new(contH) TOFDBcD();
         antidbcH = new(contH) AntiDBcD();
         tkdbcH   = new(contH) TKDBcV(); 
         rstatus = oocSuccess;
        } else {
          cout << "Addamsdbc -I- Found container "<<contName<< endl;
          amsdbcItr.scan(contH, Mode());
          if (amsdbcItr.next()) {
           Message("Addamsdbc : check amsdbc");
           rstatus = amsdbcItr -> CmpConstants();
           if (rstatus != oocSuccess) 
                              Fatal("Addamsdbc : amsdbc comparison failed");
          }

          integer Write = 1;
          commonsItr.scan(contH, Mode());
          if (commonsItr.next()) {
           Message("Addamsdbc : check commons");
           rstatus = commonsItr -> CmpConstants(Write);
           if (rstatus != oocSuccess) Fatal
           ("Addamsdbc : commons comparison failed. Please, write new setup.");
          }

          ctcdbcItr.scan(contH, Mode());
          if (ctcdbcItr.next()) {
           Message("Addamsdbc : check ctcdbc");
           rstatus = ctcdbcItr -> CmpConstants();
           if (rstatus != oocSuccess) Fatal
             ("Addamsdbc : ctcdbc comparison failed. Please, write new setup");
          }

          tofdbcItr.scan(contH, Mode());
          if (tofdbcItr.next()) {
           Message("Addamsdbc : check tofdbc");
           rstatus = tofdbcItr -> CmpConstants();
           if (rstatus != oocSuccess) Fatal(
            "Addamsdbc: tofdbc comparison failed. Please, write new setup");
          }


          antidbcItr.scan(contH, Mode());
          if (antidbcItr.next()) {
           Message("Addamsdbc : check antidbc");
           rstatus = antidbcItr -> CmpConstants();
           if (rstatus != oocSuccess) Fatal(
            "Addamsdbc: antidbc comparison failed. Please, write new setup");
          }

          tkdbcItr.scan(contH, Mode());
          if (tkdbcItr.next()) {
           Message("Addamsdbc : check tkdbc");
           rstatus = tkdbcItr -> CmpConstants();
           if (rstatus != oocSuccess) 
                              Fatal("Addamsdbc : tkdbc comparison failed");
         }
        }

end:
        if (rstatus == oocSuccess) {
	  Commit(); 	           // Commit the transaction
          if(contName) delete [] contName;  
        } else {
         Abort();  // or Abort it
         Fatal("Addamsdbc : Quit");
        }
	return rstatus;
        
}


ooStatus   LMS::ReadGeometry()
{
	ooStatus 	       rstatus = oocError;	// Return status
        ooMode                 mode = oocRead;
        ooHandle(ooDBObj)      dbH;
        ooHandle(ooContObj)    contH;

        StartRead(oocMROW);
        dbH = setupdb();
        if (dbH == NULL) Fatal("Cannot open setup dbase "); 
        rstatus = CopyGeometry();

end:
        if (rstatus == oocSuccess) {
	  Commit(); 	           
        } else {
          Abort();
          Fatal("ReadGeometry: Quit");
        }
	return rstatus;
}

ooStatus   LMS::ReadMaterial()
{
	ooStatus 	       rstatus = oocError;	// Return status
        char                   err_mess[80];
        ooHandle(ooDBObj)      dbH;

        StartRead(oocMROW);

        dbH = setupdb();
        if (dbH == NULL) Fatal("Cannot open setup dbase "); 

        // get setup name
        char *setup = StrDup(_setup);
        if(dbg_prtout) cout <<"ReadMaterial -I- setup "<<setup<<endl;
        char* contName = StrCat("Materials_",setup);
        if(setup) delete [] setup;  
        if(dbg_prtout) cout <<"ReadMaterial -I-  container "<<contName<<endl;
        integer status = Container(dbH, contName, contgmatH);
        if (status == -1) {
         cout <<"ReadMaterial -E-  cannot find or open container "<<contName
              <<endl;
         exit(1);
        }

         integer              id, imate;
         geant                rho, radl, absl;
         geant                a[10];
         geant                z[10];
         geant                w[10];
         integer              npar;

         static AMSgmat mat;

         mat.setname("Materials:");
         AMSJob::gethead() -> addup(&mat);

         gmatItr.scan(contgmatH, Mode());
         while (gmatItr.next())
         {
           npar = gmatItr -> getnpar();
           if (npar > 10) Fatal("ReadMaterial : npar > 10. Quit ");
           gmatItr -> getNumbers(id, imate, rho, radl, absl, a, z, w);
           char* name = gmatItr -> getname();

           cout <<" Add "<<name<<" with id "<<id<<" imate "<<imate<<endl;

            if (npar > 1) mat.add (new AMSgmat(imate, name, 
                                               a, z, w, npar, 
                                               rho, radl, absl));
            if (npar < 2) mat.add (new AMSgmat(imate, name, a[0], z[0], 
                                               rho, radl, absl));
         }
 
         rstatus = oocSuccess;
         Message("ReadMaterials : read done");
 
end:
        if (rstatus == oocSuccess) {
          if(contName) delete [] contName;
	  Commit(); 	           // Commit the transaction
        } else {
          Abort();
          Fatal(err_mess);
        }
	return rstatus;
}


ooStatus   LMS::ReadTMedia()
{
	ooStatus 	       rstatus = oocError;	// Return status
        char                   err_mess[80];
        ooHandle(ooDBObj)      dbH;

        StartRead(oocMROW);

        dbH = setupdb();
        if (dbH == NULL) Fatal("Cannot open setup dbase "); 

        Message("ReadTMedia : read started");
        char *setup = StrDup(_setup);
        if(dbg_prtout) cout <<"ReadTMedia -I- setup "<<setup<<endl;
        char* contName = StrCat("TMedia_",setup);
        if(dbg_prtout) cout <<"ReadTMedia -I- container "<<contName<<endl;
        integer status = Container(dbH, contName, contgtmedH);
        if (status == -1) {
         Abort();
         Fatal("ReadTMedia : cannot find or open container ");
        }

        integer              ibuff[4];
        geant                gbuff[10], birks[3];
        char                 yb;

        static AMSgtmed  tmed;
        tmed.setname("TrackingMedia:");
        AMSJob::gethead() -> addup(&tmed);

        gtmedItr.scan(contgtmedH, Mode());
        while (gtmedItr.next())
        {
         gtmedItr -> getNumbers(ibuff, gbuff);
         yb = gtmedItr -> getyb();
         char* name = gtmedItr -> getname();

         cout <<" Add "<<name<<endl;
         for (integer i=0; i<3; i++) {birks[i] = gbuff[7+i];}
          tmed.add (new AMSgtmed(ibuff[1], name, ibuff[2], ibuff[0], yb, 
                           birks, ibuff[3], gbuff[0], gbuff[1], gbuff[2],
                           gbuff[3], gbuff[4], gbuff[5]));
        }
         AMSgObj::GTrMedMap.map(tmed);
         Message("ReadTMedia : read done");
         rstatus = oocSuccess;
end:
        if (rstatus == oocSuccess) {
          if(setup)    delete [] setup;  
          if(contName) delete [] contName;
	  Commit(); 	           // Commit the transaction
        } else {
         Abort();
         Fatal(err_mess);
        }
	return rstatus;
}

void LMS::CheckConstants()
{
	ooStatus 	     rstatus = oocError;      // Return status
        ooHandle(ooContObj)  contH;                   // container
        ooItr(AMSDBcD)       amsdbcItr;               // amsdbc
        ooItr(CTCDBcD)       ctcdbcItr;               // ctcdbc
        ooItr(TOFDBcD)       tofdbcItr;               // tofdbc
        ooItr(AntiDBcD)      antidbcItr;              // antidbc
        ooItr(TKDBcV)        tkdbcItr;

        ooHandle(ooDBObj)    dbH;
        char*                contName;
        integer              Read = 0;

        StartRead(oocMROW);

        dbH = setupdb();
        if (dbH == NULL) Fatal("CheckConstants : Cannot open setup dbase ");

        char *setup = StrDup(_setup);
        contName = StrCat("amsdbc_",setup);
        if(setup) delete [] setup;  
        if (!contH.exist(dbH, contName, oocRead)) {
         rstatus = oocError;
         cout<<"CheckConstants : cannot open a container "<<contName<<endl;
         goto end;
        }

      amsdbcItr.scan(contH, Mode());
      if (amsdbcItr.next()) {
       Message("CheckConstants : check amsdbc");
       rstatus = amsdbcItr -> CmpConstants();
       if (rstatus != oocSuccess) goto end;
      } 
      
      ctcdbcItr.scan(contH, Mode());
      if (ctcdbcItr.next()) {
       Message("CheckConstants : check ctcdbc");
       rstatus = ctcdbcItr -> CmpConstants();
       if (rstatus != oocSuccess) {
        rstatus = oocError;
        goto end;
       }
      } 


      tofdbcItr.scan(contH, Mode());
      if (tofdbcItr.next()) {
       Message("CheckConstants: check tofdbc");
       rstatus = tofdbcItr -> CmpConstants();
       if (rstatus != oocSuccess) {
        rstatus = oocError;
        goto end;
       }
      }

      antidbcItr.scan(contH, Mode());
      if (antidbcItr.next()) {
       Message("CheckConstants : check antidbc");
       rstatus = antidbcItr -> CmpConstants();
       if (rstatus != oocSuccess) {
        rstatus = oocError;
        goto end;
       }
      }

end:
        if (contName) delete [] contName;
        if (rstatus == oocSuccess) {
	  Commit(); 	           // Commit the transaction
          if (rstatus != oocSuccess) exit (1);
        } else {
          Abort();
          Fatal("CheckConstants : Quit");
        }
}

void LMS::CheckCommons()
{
	ooStatus 	     rstatus = oocError;      // Return status
        ooHandle(ooContObj)  contH;                   // container
        ooItr(AMScommonsD)   commonsItr;              // commons
        ooHandle(ooDBObj)    dbH;
        char*                contName;
        integer              Read = 0;

        StartRead(oocMROW);

        dbH = setupdb();
        if (dbH == NULL) Fatal("CheckConstants : Cannot open setup dbase ");

        char *setup = StrDup(_setup);
        contName = StrCat("amsdbc_",setup);
        if(setup) delete [] setup;  
        if (!contH.exist(dbH, contName, oocRead)) {
         rstatus = oocError;
         cout<<"CheckCommons : cannot open a container "<<contName<<endl;
         goto end;
        }

      commonsItr.scan(contH, Mode());
      if (commonsItr.next()) {
       Message("CheckConstants : check commons");
       rstatus = commonsItr -> CmpConstants(Read);
       if (rstatus != oocSuccess) {
        rstatus = oocError;
        goto end;
       }
      } 

end:
        if (contName) delete [] contName;
        if (rstatus == oocSuccess) {
	  Commit(); 	           // Commit the transaction
          if (rstatus != oocSuccess) exit (1);
        } else {
          Abort();
          Fatal("CheckConstants : Quit");
        }
}

ooStatus LMS::ReadTKDBc()
{
	ooStatus 	     rstatus = oocError;      // Return status
        ooHandle(ooContObj)  contH;                   // container
        ooItr(TKDBcV)        tkdbcItr;

        ooHandle(ooDBObj)    dbH;
        char*                contName;
        integer              Read = 0;

        StartRead(oocMROW);

        dbH = setupdb();
        if (dbH == NULL) Fatal("ReadTKDBC : Cannot open setup dbase ");

        char *setup = StrDup(_setup);
        contName = StrCat("amsdbc_",setup);
        if(setup) delete [] setup;  
        if (!contH.exist(dbH, contName, oocRead)) {
         rstatus = oocError;
         cout<<"ReadTKDBC : cannot open a container "<<contName<<endl;
         goto end;
        }


        tkdbcItr.scan(contH, Mode());
        if (tkdbcItr.next()) {
         Message("CheckTKDBC : copy tkdbc");
         rstatus = tkdbcItr -> ReadTKDBc();
         if (rstatus != oocSuccess) {
          rstatus = oocError;
          goto end;
        }
       }

end:
        if (contName) delete [] contName;
        if (rstatus == oocSuccess) {
	  Commit(); 	           // Commit the transaction
          if (rstatus != oocSuccess) exit (1);
          return rstatus;
        } else {
          Abort();
          Fatal("ReadTKDBc : Quit");
        }
}
