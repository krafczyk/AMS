// Objectivity class Nov 06, 1997 ak
//
// Last edit : Nov 6, 1997 ak
//

#include <typedefs.h> 
#include <tkdbc.h>
#include <tkdbcV.h>


TKDBcV::TKDBcV() 
{
  int     i, j, k;
  TKDBcD  tkdbc;

  _ReadOK = TKDBc::_ReadOK;
  
// sensors
  int nsen = TKDBc::_NumberSen;
  if (nsen) {
   _HeadSensor.resize(nsen);
   for (i=0; i<nsen; i++) {
    tkdbc._status = TKDBc::_HeadSensor[i]._status;
    tkdbc._gid    = TKDBc::_HeadSensor[i]._gid;
    for (j=0; j<3; j++) {
     tkdbc._coo[j] = TKDBc::_HeadSensor[i]._coo[j];
     for (k=0; k<3; k++) tkdbc._nrm[j][k] = TKDBc::_HeadSensor[i]._nrm[j][k];
    }
   _HeadSensor.set(i,tkdbc);
  }
 }

// layers
  int nlayer = TKDBc::_NumberLayer;
  if (nlayer) {
   _HeadLayer.resize(nlayer);
   for (i=0; i<nlayer; i++) {
    tkdbc._status = TKDBc::_HeadLayer[i]._status;
    tkdbc._gid    = TKDBc::_HeadLayer[i]._gid;
    for (j=0; j<3; j++) {
     tkdbc._coo[j] = TKDBc::_HeadLayer[i]._coo[j];
     for (k=0; k<3; k++) tkdbc._nrm[j][k] = TKDBc::_HeadLayer[i]._nrm[j][k];
   }
   _HeadLayer.set(i,tkdbc);
  }
 }

// ladders
  int nladder = TKDBc::_NumberLadder;
  if (nladder) {
   _HeadLadder0.resize(nladder);
   _HeadLadder1.resize(nladder);
   for (int ii=0; ii<2; ii++) {
    for (i=0; i<nladder; i++) {
     tkdbc._status = TKDBc::_HeadLadder[ii][i]._status;
     tkdbc._gid    = TKDBc::_HeadLadder[ii][i]._gid;
     for (j=0; j<3; j++) {
      tkdbc._coo[j] = TKDBc::_HeadLadder[ii][i]._coo[j];
      for (k=0; k<3; k++) tkdbc._nrm[j][k] = TKDBc::_HeadLadder[ii][i]._nrm[j][k];
    }
    if (ii == 0) _HeadLadder0.set(i,tkdbc);
    if (ii == 1) _HeadLadder1.set(i,tkdbc);
   }
  }
 }

}

ooStatus TKDBcV::CmpConstants() 
{
  ooStatus  rstatus = oocError;
  int       i, j, k, ii;
  TKDBcD    tkdbc;

  integer nsen    = _HeadSensor.size();
  integer nlayer  = _HeadLayer.size();
  integer nladder = _HeadLadder0.size();

  if (nsen == TKDBc::_NumberSen) {
   cout<<"TKDBcV::CmpConstants -I- sensors"<<endl;
   for (i=0; i<nsen; i++) {
    tkdbc = _HeadSensor[i];
    if(tkdbc._status) {
     if ( tkdbc._status != TKDBc::_HeadSensor[i]._status) {
      cerr<<"TKDBcV::CmpConstants -E- status "<<tkdbc._status
          <<", "<<TKDBc::_HeadSensor[i]._status<<endl;
      return rstatus;
     }
     if (tkdbc._gid    != TKDBc::_HeadSensor[i]._gid) {
      cerr<<"TKDBcV::CmpConstants -E- gid "<<tkdbc._gid
          <<", "<<TKDBc::_HeadSensor[i]._gid<<endl;
      return rstatus;
     }
     for (j=0; j<3; j++) {
      if( tkdbc._coo[j] != TKDBc::_HeadSensor[i]._coo[j]) {
       cerr<<"TKDBcV::CmpConstants -E- coo "<<tkdbc._coo[j]
           <<", "<<TKDBc::_HeadSensor[i]._coo[j]<<endl;
       return rstatus;
      }
     }
    for (j=0; j<3; j++) {
     for (k=0; k<3; k++) {
      if (tkdbc._nrm[j][k] != TKDBc::_HeadSensor[i]._nrm[j][k]) {
       cerr<<"TKDBcV::CmpConstants -E- _nrm "<<tkdbc._nrm[j][k]
           <<", "<<TKDBc::_HeadSensor[i]._nrm[j][k]<<endl;
       return rstatus;
      }
     }
    }
   }
  }
 } else {
   cerr<<"TKDBcV::CmpConstants -E- number of sensors is different "
       <<nsen<<", "<<TKDBc::_NumberSen;
   return rstatus;
 }
 
 if (nlayer == TKDBc::_NumberLayer) {
   cout<<"TKDBcV::CmpConstants -I- layers"<<endl;
   for (i=0; i<nlayer; i++) {
    tkdbc = _HeadLayer[i];
    if ( tkdbc._status != TKDBc::_HeadLayer[i]._status) {
      cerr<<"TKDBcV::CmpConstants -E- status "<<tkdbc._status
          <<", "<<TKDBc::_HeadLayer[i]._status<<endl;
      return rstatus;
    }
    if (tkdbc._gid    != TKDBc::_HeadLayer[i]._gid) {
      cerr<<"TKDBcV::CmpConstants -E- gid "<<tkdbc._gid
          <<", "<<TKDBc::_HeadLayer[i]._gid<<endl;
      return rstatus;
    }

    for (j=0; j<3; j++) {
     if( tkdbc._coo[j] != TKDBc::_HeadLayer[i]._coo[j]) {
      cerr<<"TKDBcV::CmpConstants -E- coo "<<tkdbc._coo[j]
          <<", "<<TKDBc::_HeadLayer[i]._coo[j]<<endl;
      return rstatus;
    }
   }
    for (j=0; j<3; j++) {
     for (k=0; k<3; k++) {
      if (tkdbc._nrm[j][k] != TKDBc::_HeadLayer[i]._nrm[j][k]) {
       cerr<<"TKDBcV::CmpConstants -E- _nrm "<<tkdbc._nrm[j][k]
           <<", "<<TKDBc::_HeadLayer[i]._nrm[j][k]<<endl;
       return rstatus;
     }
    }
   }
  }
 } else {
   cerr<<"TKDBcV::CmpConstants -E- number of layers is different "
       <<nlayer<<", "<<TKDBc::_NumberLayer;
   return rstatus;
 }

  if (nladder == TKDBc::_NumberLadder) {
   cout<<"TKDBcV::CmpConstants -I- ladders"<<endl;
   for (int ii=0; ii<2; ii++) {
    for (i=0; i<nladder; i++) {
     if (ii==0) tkdbc = _HeadLadder0[i];
     if (ii==1) tkdbc = _HeadLadder1[i];
     if ( tkdbc._status != TKDBc::_HeadLadder[ii][i]._status) {
      cerr<<"TKDBcV::CmpConstants -E- status "<<tkdbc._status
          <<", "<<TKDBc::_HeadLadder[ii][i]._status<<endl;
      return rstatus;
    }
    if (tkdbc._gid    != TKDBc::_HeadLadder[ii][i]._gid) {
      cerr<<"TKDBcV::CmpConstants -E- gid "<<tkdbc._gid
          <<", "<<TKDBc::_HeadLadder[ii][i]._gid<<endl;
      return rstatus;
    }

    for (j=0; j<3; j++) {
     if( tkdbc._coo[j] != TKDBc::_HeadLadder[ii][i]._coo[j]) {
      cerr<<"TKDBcV::CmpConstants -E- coo "<<tkdbc._coo[j]
          <<", "<<TKDBc::_HeadLadder[ii][i]._coo[j]<<endl;
      return rstatus;
    }
   }

    for (j=0; j<3; j++) {
     for (k=0; k<3; k++) {
      if (tkdbc._nrm[j][k] != TKDBc::_HeadLadder[ii][i]._nrm[j][k]) {
       cerr<<"TKDBcV::CmpConstants -E- _nrm "<<tkdbc._nrm[j][k]
           <<", "<<TKDBc::_HeadLadder[ii][i]._nrm[j][k]<<endl;
       return rstatus;
     }
    }
   }
  }
 }
 } else {
   cerr<<"TKDBcV::CmpConstants -E- number of ladders is different "
       <<nladder<<", "<<TKDBc::_NumberLadder;
   return rstatus;
 }

  rstatus = oocSuccess;

  return rstatus;
}

ooStatus TKDBcV::ReadTKDBc() 
{
  ooStatus  rstatus = oocError;
  int       i, j, k, ii;
  TKDBcD    tkdbc;

  integer nsen    = _HeadSensor.size();
  integer nlayer  = _HeadLayer.size();
  integer nladder = _HeadLadder0.size();

  if (nsen == TKDBc::_NumberSen) {
   cout<<"TKDBcV::ReadTKDBc -I- sensors"<<endl;
   for (i=0; i<nsen; i++) {
    tkdbc = _HeadSensor[i];
    TKDBc::_HeadSensor[i]._status = tkdbc._status;
    TKDBc::_HeadSensor[i]._gid   = tkdbc._gid;    
    for (j=0; j<3; j++) {
     TKDBc::_HeadSensor[i]._coo[j] = tkdbc._coo[j];
     for (k=0; k<3; k++) TKDBc::_HeadSensor[i]._nrm[j][k] = tkdbc._nrm[j][k];
    }
   }
  } else {
    cerr<<"TKDBcV::CmpConstants -E- number of sensors is different "
        <<nsen<<", "<<TKDBc::_NumberSen<<endl;
    return rstatus;
  }
 
  if (nlayer == TKDBc::_NumberLayer) {
   cout<<"TKDBcV::ReadTKDBc -I- layers"<<endl;
   for (i=0; i<nlayer; i++) {
    tkdbc = _HeadLayer[i];
    TKDBc::_HeadLayer[i]._status = tkdbc._status;
    TKDBc::_HeadLayer[i]._gid    = tkdbc._gid;    
    for (j=0; j<3; j++) {
     TKDBc::_HeadLayer[i]._coo[j] = tkdbc._coo[j];
     for (k=0; k<3; k++) TKDBc::_HeadLayer[i]._nrm[j][k] = tkdbc._nrm[j][k];
    }
   }
  } else {
   cerr<<"TKDBcV::CmpReadTKDBc -E- number of layers is different "
       <<nlayer<<", "<<TKDBc::_NumberLayer<<endl;;
   return rstatus;
 }

  if (nladder == TKDBc::_NumberLadder) {
   cout<<"TKDBcV::ReadTKDBc -I- ladders"<<endl;
   for (int ii=0; ii<2; ii++) {
    for (i=0; i<nladder; i++) {
     if (ii==0) tkdbc = _HeadLadder0[i];
     if (ii==1) tkdbc = _HeadLadder1[i];
     TKDBc::_HeadLadder[ii][i]._status = tkdbc._status;
     TKDBc::_HeadLadder[ii][i]._gid    = tkdbc._gid;
     for (j=0; j<3; j++) {
      TKDBc::_HeadLadder[ii][i]._coo[j] = tkdbc._coo[j];
      for (k=0; k<3; k++) 
                    TKDBc::_HeadLadder[ii][i]._nrm[j][k] = tkdbc._nrm[j][k];
      }
     }
   }
  } else {
   cerr<<"TKDBcV::readTKDbc -E- number of ladders is different "
       <<nladder<<", "<<TKDBc::_NumberLadder<<endl;
   return rstatus;
 }

  TKDBc::_ReadOK = _ReadOK;

  rstatus = oocSuccess;

  return rstatus;
}

