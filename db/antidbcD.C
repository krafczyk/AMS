// June 05, 1997. ak. first try with Objectivity
//                   method source file of object AntiDBcD
//
//
//Last Edit : Sep 15, 1997. ak.
//

#include <iostream.h>
#include <string.h>
#include <cern.h>
#include <rd45.h>

#include <antidbc.h>
#include <antidbcD.h>
//

AntiDBcD::AntiDBcD() 
{
  _scradi  =  ANTIDBc::_scradi;
  _scinth  =  ANTIDBc::_scinth;
  _scleng  =  ANTIDBc::_scleng;
  _wrapth  =  ANTIDBc:: _wrapth;
  _groovr  =  ANTIDBc::_groovr;
  _pdlgap  =  ANTIDBc::_pdlgap;
  _stradi  =  ANTIDBc:: _stradi;
  _stleng  =  ANTIDBc:: _stleng;
  _stthic  =  ANTIDBc::_stthic;
  _tdcabw  =  ANTIDBc::_tdcabw;
  _shprdt  =  ANTIDBc::_shprdt;
  _ftpulw  =  ANTIDBc::_ftpulw;
}

ooStatus AntiDBcD::CmpConstants() 
{
  ooStatus rstatus = oocSuccess;  
  
  Message("AntiDBcD::CmpConstants : start comparison ");

   if( _scradi  !=  ANTIDBc::_scradi){
    cout<<"AntiDBcD::CmpConstants : _scradi "<<_scradi<<", "
        <<ANTIDBc::_scradi<<". Set to "<<_scradi<<endl;
    ANTIDBc::_scradi = _scradi;
   }
   if( _scinth  !=  ANTIDBc::_scinth){
    cout<<"ANTIDBcD::CmpConstants : _scinth "<<_scinth<<", "
        <<ANTIDBc::_scinth<<". Set to "<<_scinth<<endl;
    ANTIDBc::_scinth = _scinth;
   }
   if( _scleng  !=  ANTIDBc::_scleng){
    cout<<"ANTIDBcD::CmpConstants : _scleng "<<_scleng<<", "
        <<ANTIDBc::_scleng<<". Set to "<<_scleng<<endl;
    ANTIDBc::_scleng = _scleng;
   }

   if( _wrapth  !=  ANTIDBc:: _wrapth){
    cout<<"ANTIDBcD::CmpConstants : _wrapth "<<_wrapth<<", "
        <<ANTIDBc::_wrapth<<". Set to "<<_wrapth<<endl;
    ANTIDBc::_wrapth = _wrapth;

   }

   if( _groovr  !=  ANTIDBc::_groovr){
    cout<<"ANTIDBcD::CmpConstants : _groovr "<<_groovr<<", "
        <<ANTIDBc::_groovr<<". Set to "<<_groovr<<endl;
    ANTIDBc::_groovr = _groovr;
   }

   if( _pdlgap  !=  ANTIDBc::_pdlgap){
    cout<<"ANTIDBcD::CmpConstants : _pdlgap "<<_pdlgap<<", "
        <<ANTIDBc::_pdlgap<<". Set to "<<_pdlgap<<endl;
    ANTIDBc::_pdlgap = _pdlgap;
   }

   if( _stradi  !=  ANTIDBc:: _stradi){
    cout<<"ANTIDBcD::CmpConstants : _stradi "<<_stradi<<", "
        <<ANTIDBc::_stradi<<". Set to "<<_stradi<<endl;
    ANTIDBc::_stradi = _stradi;
   }

   if( _stleng  !=  ANTIDBc:: _stleng){
    cout<<"ANTIDBcD::CmpConstants : _stleng "<<_stleng<<", "
        <<ANTIDBc::_stleng<<". Set to "<<_stleng<<endl;
    ANTIDBc::_stleng = _stleng;
   }

   if( _stthic  !=  ANTIDBc::_stthic){
    cout<<"ANTIDBcD::CmpConstants : _stthic "<<_stthic<<", "
        <<ANTIDBc::_stthic<<". Set to "<<_stthic<<endl;
    ANTIDBc::_stthic = _stthic;
   }


   if( _tdcabw  !=  ANTIDBc::_tdcabw){
    cout<<"ANTIDBcD::CmpConstants : _tdcabw "<<_tdcabw<<", "
        <<ANTIDBc::_tdcabw<<". Set to "<<_tdcabw<<endl;
    ANTIDBc::_tdcabw = _tdcabw;
   }

   if( _shprdt  !=  ANTIDBc::_shprdt){
    cout<<"ANTIDBcD::CmpConstants : _shprdt "<<_shprdt<<", "
        <<ANTIDBc::_shprdt<<". Set to "<<_shprdt<<endl;
    ANTIDBc::_shprdt = _shprdt;
   }

   if( _ftpulw  !=  ANTIDBc::_ftpulw){
    cout<<"ANTIDBcD::CmpConstants : _ftpulw "<<_ftpulw<<", "
        <<ANTIDBc::_ftpulw<<". Set to "<<_ftpulw<<endl;
    ANTIDBc::_ftpulw = _ftpulw;
   }


  rstatus == oocSuccess;
  Message("ANTIDBcD::CmpConstants : comparison done ");

  return rstatus;

}
