// Oct 07, 1996. ak. first try with Objectivity
//                   method source file of object AMSDBcD
//                   AMBIG, BAD, USED, DELETED are not compared.
//
// Last Edit : Sep 15, 1997. ak.
//

#include <iostream.h>
#include <string.h>
#include <cern.h>


#include <amsdbc.h>
#include <amsdbcD.h>
//

AMSDBcD:: AMSDBcD() 
{
  integer i, j;

  _maxstripsD = AMSDBc::_maxstrips;
  _nlayD      = AMSDBc::_nlay;
  _maxnladD   = AMSDBc::_maxnlad;

  for (i = 0; i<nl; i++) {
    for (j=0; j<5; j++) {
      _laydD[i][j] = AMSDBc::_layd[i][j];
    }
  }

  for (i=0; i<nl; i++) {
   _zposlD[i] = AMSDBc::_zposl[i];
   _nladD[i]  = AMSDBc::_nlad[i];
   _zposD[i]  = AMSDBc::_zpos[i];
   _c2cD[i]   = AMSDBc::_c2c[i];
    _silicon_zD[i]     = AMSDBc::_silicon_z[i];
   _support_wD[i]      = AMSDBc::_support_w[i];
   _nladshuttle[i]     = AMSDBc::_nladshuttle[i];
   _boundladshuttle[i] = AMSDBc::_boundladshuttle[i];
   _halfldist[i]       = AMSDBc::_halfldist[i];
  }

  for (i = 0; i<nl; i++) {
    for (j=0; j<nld; j++) {
      _nsenD[i][j]  = AMSDBc::_nsen[i][j];
      _nhalfD[i][j] = AMSDBc::_nhalf[i][j];
    }
  }

  for (i = 0; i<nl; i++) {
    for (j=0; j<2; j++) {
      _ssize_activeD[i][j]    = AMSDBc::_ssize_active[i][j];
      _ssize_inactiveD[i][j]  = AMSDBc::_ssize_inactive[i][j];
      _nstripssenD[i][j]      = AMSDBc::_nstripssen[i][j];
      _nstripsdrpD[i][j]      = AMSDBc::_nstripsdrp[i][j];
      _zelecD[i][j]           = AMSDBc::_zelec[i][j];
    }
  }

  raddegD = AMSDBc::raddeg;
  piD     = AMSDBc::pi;
  twopi   = AMSDBc::twopi;

  for (i=0; i<3; i++) {
   ams_sizeD[i] = AMSDBc::ams_size[i];
   ams_cooD[i]  = AMSDBc::ams_coo[i];
  }

  AMBIG   = AMSDBc::AMBIG;
  BAD     = AMSDBc::BAD;
  USED    = AMSDBc::USED;
  DELETED = AMSDBc::DELETED;

  BigEndian = BigEndian;

  if(AMSDBc::ams_name) strcpy (ams_nameD,AMSDBc::ams_name);  
}

ooStatus AMSDBcD::CmpConstants() 
{
  integer i, j;
  ooStatus rstatus = oocSuccess;  

  if ( (_maxstripsD != AMSDBc::_maxstrips) ||
       (_nlayD != AMSDBc::_nlay)           ||
       (_maxnladD != AMSDBc::_maxnlad)) {
      rstatus = oocError;
      cout<<" _maxstrips, _nlay, _maxnlad "
          << _maxstripsD<<" "<<_nlayD<<" "<<_maxnladD<< endl;
      cout<<" _maxstrips, _nlay, _maxnlad "
          << AMSDBc::_maxstrips<<" "<<AMSDBc::_nlay<<" "<<AMSDBc::_maxnlad 
          << endl;
  }

  for (i = 0; i<nl; i++) {
    for (j=0; j<5; j++) {
      if(_laydD[i][j] != AMSDBc::_layd[i][j]) 
        {
         rstatus = oocError;
         cout <<"layd "<<_laydD[i]<<" "<<AMSDBc::_layd[i]<<endl;
         break;
        }
    }
  }

  for (i=0; i<nl; i++) {
    if((_zposlD[i] != AMSDBc::_zposl[i]) ||
       (_nladD[i]  != AMSDBc::_nlad[i])  ||
       (_zposD[i]  != AMSDBc::_zpos[i])  ||
       (_c2cD[i]   != AMSDBc::_c2c[i])   ||
       (_silicon_zD[i]      != AMSDBc::_silicon_z[i])       ||
       (_support_wD[i]      != AMSDBc::_support_w[i])       ||
       (_nladshuttle[i]     != AMSDBc::_nladshuttle[i])     ||
       (_boundladshuttle[i] != AMSDBc::_boundladshuttle[i]) ||
       (_halfldist[i]       != AMSDBc::_halfldist[i]))
      {
        rstatus = oocError;
        cout<<"zposl "<<_zposlD[i]<<" "<<AMSDBc::_zposl[i]<<endl;
        cout<<"nlad "<<_nladD[i] <<" "<<AMSDBc::_nlad[i]<<endl;
        cout<<"zpos "<<_zposD[i]<<" "<<AMSDBc::_zpos[i]<<endl;
        cout<<"c2c "<<_c2cD[i]<<" "<<AMSDBc::_c2c[i]<<endl;
        cout<<"silicon z"<<_silicon_zD[i]<<" "<<AMSDBc::_silicon_z[i]<<endl;
        cout<<"support_w"<<_support_wD[i]<<" "<<AMSDBc::_support_w[i]<<endl;
        cout<<"n_ladshuttle "<<_nladshuttle[i]<<" "
             <<AMSDBc::_nladshuttle[i]<<endl;
        cout<<"boundladshuttle "<<_boundladshuttle[i]<<" "
            <<AMSDBc::_boundladshuttle[i]<<endl;
        cout<<"halfldist "<<_halfldist[i]<<" "
            <<AMSDBc::_halfldist[i]<<endl;
        break;
      }
  }

  for (i = 0; i<nl; i++) {
    for (j=0; j<nld; j++) {
     if( (_nsenD[i][j]  != AMSDBc::_nsen[i][j]) ||
         (_nhalfD[i][j] != AMSDBc::_nhalf[i][j]) )
      {
        rstatus = oocError;
        cout<<"nsen "<<_nsenD[i][j]<<" "<<AMSDBc::_nsen[i][j]<<endl;
        cout<<"nhalf "<<_nhalfD[i][j]<<" "<<AMSDBc::_nhalf[i][j]<<endl;
        break;
      }
    }
  }

  for (i = 0; i<nl; i++) {
    for (j=0; j<2; j++) {
     if( (_ssize_activeD[i][j]  != AMSDBc::_ssize_active[i][j]) ||
         (_ssize_inactiveD[i][j]  != AMSDBc::_ssize_inactive[i][j]) ||
         (_nstripssenD[i][j]      != AMSDBc::_nstripssen[i][j]) ||
        (_nstripsdrpD[i][j]      != AMSDBc::_nstripsdrp[i][j]) ||
        (_zelecD[i][j]           != AMSDBc::_zelec[i][j]) )
      {
        rstatus = oocError;
        cout<<"ssize_acive "<<_ssize_activeD[i][j]
            <<AMSDBc::_ssize_active[i][j]<<endl;
        cout<<"ssize_inactive "<<_ssize_inactiveD[i][j]
            <<AMSDBc::_ssize_inactive[i][j]<<endl;
        cout<<"nstripssed "<<_nstripssenD[i][j]
            <<AMSDBc::_nstripssen[i][j]<<endl;
          cout<<"nstripsdrp "<<_nstripsdrpD[i][j]
              <<AMSDBc::_nstripsdrp[i][j]<<endl;
        cout<<"zelec "<<_zelecD[i][j]<<AMSDBc::_zelec[i][j]<<endl;
        break;
      }
    }
  }

  if ( (raddegD != AMSDBc::raddeg) ||
        (piD != AMSDBc::pi)        ||
        (twopi != AMSDBc::twopi))
    {
      rstatus = oocError;
      cout<<"raddeg "<<raddegD<<" "<<AMSDBc::raddeg<<endl;
      cout<<"pi "<<piD<<" "<<AMSDBc::pi<<endl;
      cout<<"twopi "<<twopi<<" "<<AMSDBc::twopi<<endl;
    }

  for (i=0; i<3; i++) {
   if (
       (ams_sizeD[i] != AMSDBc::ams_size[i]) ||
       (ams_cooD[i]  = AMSDBc::ams_coo[i]) )
      {
        cout<<"ams_size "<<ams_sizeD[i]<<" "
            <<AMSDBc::ams_size[i]<<endl;
        cout<<"ams_coo "<<ams_cooD[i]<<" "<<AMSDBc::ams_coo[i]
            <<endl;
        rstatus = oocError;
        break;
      }
  }
  return rstatus;
}


