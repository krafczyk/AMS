// Oct 07, 1996. ak. first try with Objectivity
//                   method source file of object AMSDBcD
//                   AMBIG, BAD, USED, DELETED are not compared.
//
// Last Edit : Nov 3, 1997. ak.
//

#include <iostream.h>
#include <string.h>
#include <cern.h>


#include <amsdbc.h>
#include <amsdbcD.h>

AMSDBcD:: AMSDBcD() 
{
  integer i, j, k;

  _nlay      = AMSDBc::_nlay;
  _maxnlad   = AMSDBc::_maxnlad;

  for (i = 0; i<nl; i++) {
    for (j=0; j<5; j++) {
      _layd[i][j] = AMSDBc::_layd[i][j];
    }
  }

  for (i=0; i<nl; i++) {
   _xposl[i] = AMSDBc::_xposl[i];
   _yposl[i] = AMSDBc::_yposl[i];
   _zposl[i] = AMSDBc::_zposl[i];
   _nlad[i]  = AMSDBc::_nlad[i];
   _zpos[i]  = AMSDBc::_zpos[i];
   _c2c[i]   = AMSDBc::_c2c[i];
    _silicon_z[i]     = AMSDBc::_silicon_z[i];
   _support_foam_w[i]      = AMSDBc::_support_foam_w[i];
   _support_hc_w[i]      = AMSDBc::_support_hc_w[i];
   _support_hc_r[i]      = AMSDBc::_support_hc_r[i];
   _support_hc_z[i]      = AMSDBc::_support_hc_z[i];
   _nladshuttle[i]     = AMSDBc::_nladshuttle[i];
   _boundladshuttle[i] = AMSDBc::_boundladshuttle[i];
   _halfldist[i]       = AMSDBc::_halfldist[i];
  }

  for (i = 0; i<nl; i++) {
    for (j=0; j<nld; j++) {
      _nsen[i][j]  = AMSDBc::_nsen[i][j];
      _nhalf[i][j] = AMSDBc::_nhalf[i][j];
    }
  }

  for (i = 0; i<nl; i++) {
    for (j=0; j<2; j++) {
      _ssize_active[i][j]    = AMSDBc::_ssize_active[i][j];
      _ssize_inactive[i][j]  = AMSDBc::_ssize_inactive[i][j];
      _nstripssen[i][j]      = AMSDBc::_nstripssen[i][j];
      _nstripsdrp[i][j]      = AMSDBc::_nstripsdrp[i][j];
    }
  }

  for (i = 0; i<nl; i++) {
    for (j=0; j<3; j++) {
      _zelec[i][j]           = AMSDBc::_zelec[i][j];
    }
  }

  for (i=0; i<nl; i++) {
    for (j=0; j<3; j++) {
      for (k=0; k<3; k++) {
        _nrml[i][j][k] = AMSDBc::_nrml[i][j][k];
      }
    }
  }
 
  raddeg  = AMSDBc::raddeg;
  pi      = AMSDBc::pi;
  twopi   = AMSDBc::twopi;

  for (i=0; i<3; i++) {
   ams_size[i] = AMSDBc::ams_size[i];
   ams_coo[i]  = AMSDBc::ams_coo[i];
  }

  AMBIG   = AMSDBc::AMBIG;
  BAD     = AMSDBc::BAD;
  USED    = AMSDBc::USED;
  DELETED = AMSDBc::DELETED;

  BigEndian = BigEndian;

  if(AMSDBc::ams_name) strcpy (ams_name,AMSDBc::ams_name);  
}

ooStatus AMSDBcD::CmpConstants() 
{
  integer i, j, k;
  ooStatus rstatus = oocSuccess;  

  if ( (_nlay    != AMSDBc::_nlay) || (_maxnlad != AMSDBc::_maxnlad)) {
      rstatus = oocError;
      cout<<" _nlay, _maxnlad "<<_nlay<<" "<<_maxnlad<< endl;
      cout<<" _nlay, _maxnlad "<<AMSDBc::_nlay<<" "<<AMSDBc::_maxnlad<< endl;
  }

  for (i = 0; i<nl; i++) {
    for (j=0; j<5; j++) {
      if(_layd[i][j] != AMSDBc::_layd[i][j]) 
        {
         rstatus = oocError;
         cout <<"layd "<<_layd[i]<<" "<<AMSDBc::_layd[i]<<endl;
         break;
        }
    }
  }

  for (i=0; i<nl; i++) {
    if((_zposl[i] != AMSDBc::_zposl[i]) ||
       (_xposl[i] != AMSDBc::_xposl[i]) ||
       (_yposl[i] != AMSDBc::_yposl[i]) ||
       (_nlad[i]  != AMSDBc::_nlad[i])  ||
       (_zpos[i]  != AMSDBc::_zpos[i])  ||
       (_c2c[i]   != AMSDBc::_c2c[i])   ||
       (_silicon_z[i]           != AMSDBc::_silicon_z[i])       ||
       (_support_foam_w[i]      != AMSDBc::_support_foam_w[i])  ||
       (_support_hc_w[i]        != AMSDBc::_support_hc_w[i])    ||
       (_support_hc_r[i]        != AMSDBc::_support_hc_r[i])    ||
       (_support_hc_z[i]        != AMSDBc::_support_hc_z[i])    ||
       (_nladshuttle[i]         != AMSDBc::_nladshuttle[i])     ||
       (_boundladshuttle[i]     != AMSDBc::_boundladshuttle[i]) ||
       (_halfldist[i]           != AMSDBc::_halfldist[i]))
      {
        rstatus = oocError;
        cout<<"xposl "<<_xposl[i]<<" "<<AMSDBc::_xposl[i]<<endl;
        cout<<"yposl "<<_yposl[i]<<" "<<AMSDBc::_yposl[i]<<endl;
        cout<<"zposl "<<_zposl[i]<<" "<<AMSDBc::_zposl[i]<<endl;
        cout<<"nlad "<<_nlad[i] <<" "<<AMSDBc::_nlad[i]<<endl;
        cout<<"zpos "<<_zpos[i]<<" "<<AMSDBc::_zpos[i]<<endl;
        cout<<"c2c "<<_c2c[i]<<" "<<AMSDBc::_c2c[i]<<endl;
        cout<<"silicon z"<<_silicon_z[i]<<" "<<AMSDBc::_silicon_z[i]<<endl;
        cout<<"support_foam_w"<<_support_foam_w[i]<<" "
            <<AMSDBc::_support_foam_w[i]<<endl;
        cout<<"support_hc_w"<<_support_hc_w[i]<<" "<<AMSDBc::_support_hc_w[i]<<endl;
        cout<<"support_hc_r"<<_support_hc_r[i]<<" "<<AMSDBc::_support_hc_r[i]<<endl;
        cout<<"support_hc_z"<<_support_hc_z[i]<<" "<<AMSDBc::_support_hc_z[i]<<endl;
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
     if( (_nsen[i][j]  != AMSDBc::_nsen[i][j]) ||
         (_nhalf[i][j] != AMSDBc::_nhalf[i][j]) )
      {
        rstatus = oocError;
        cout<<"nsen "<<_nsen[i][j]<<" "<<AMSDBc::_nsen[i][j]<<endl;
        cout<<"nhalf "<<_nhalf[i][j]<<" "<<AMSDBc::_nhalf[i][j]<<endl;
        break;
      }
    }
  }

  for (i = 0; i<nl; i++) {
    for (j=0; j<2; j++) {
     if( (_ssize_active[i][j]  != AMSDBc::_ssize_active[i][j]) ||
         (_ssize_inactive[i][j]  != AMSDBc::_ssize_inactive[i][j]) ||
         (_nstripssen[i][j]      != AMSDBc::_nstripssen[i][j]) ||
         (_nstripsdrp[i][j]      != AMSDBc::_nstripsdrp[i][j]) )
      {
        rstatus = oocError;
        cout<<"ssize_acive "<<_ssize_active[i][j]
            <<AMSDBc::_ssize_active[i][j]<<endl;
        cout<<"ssize_inactive "<<_ssize_inactive[i][j]
            <<AMSDBc::_ssize_inactive[i][j]<<endl;
        cout<<"nstripssed "<<_nstripssen[i][j]
            <<AMSDBc::_nstripssen[i][j]<<endl;
          cout<<"nstripsdrp "<<_nstripsdrp[i][j]
              <<AMSDBc::_nstripsdrp[i][j]<<endl;
        break;
      }
    }
  }

  for (i = 0; i<nl; i++) {
    for (j=0; j<3; j++) {
     if( (_zelec[i][j] != AMSDBc::_zelec[i][j]) )
      {
        rstatus = oocError;
        cout<<"zelec "<<_zelec[i][j]<<AMSDBc::_zelec[i][j]<<endl;
        break;
      }
    }
  }

  if ( (raddeg != AMSDBc::raddeg) ||
        (pi != AMSDBc::pi)        ||
        (twopi != AMSDBc::twopi))
    {
      rstatus = oocError;
      cout<<"raddeg "<<raddeg<<" "<<AMSDBc::raddeg<<endl;
      cout<<"pi "<<pi<<" "<<AMSDBc::pi<<endl;
      cout<<"twopi "<<twopi<<" "<<AMSDBc::twopi<<endl;
    }

  for (i=0; i<3; i++) {
   if (
       (ams_size[i] != AMSDBc::ams_size[i]) ||
       (ams_coo[i]  = AMSDBc::ams_coo[i]) )
      {
        cout<<"ams_size "<<ams_size[i]<<" "
            <<AMSDBc::ams_size[i]<<endl;
        cout<<"ams_coo "<<ams_coo[i]<<" "<<AMSDBc::ams_coo[i]
            <<endl;
        rstatus = oocError;
        break;
      }
  }

  for (i=0; i<nl; i++) {
    for (j=0; j<3; j++) {
      for (k=0; k<3; k++) {
        if(_nrml[i][j][k] != AMSDBc::_nrml[i][j][k]) {
          rstatus = oocError;
          cout<<"nrml["<<i<<"]["<<j<<"]["<<k<<"] "
              <<_nrml[i][j][k]<<", "<<AMSDBc::_nrml[i][j][k]<<endl;
          break;
        }
      }
    }
  }
  return rstatus;
}


