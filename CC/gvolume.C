#include <gvolume.h>
#include <math.h>
#include <amsgobj.h>
#include <gmat.h>
#ifdef __G4AMS__
#include <commons.h>
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4VSolid.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Para.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4Trap.hh"
#include "G4Polyhedra.hh"
#include "G4BREPSolidPolyhedra.hh"
#include "G4Polycone.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include <geant4.h>
#include "G4VisAttributes.hh"
#endif
integer AMSgvolume::debug=0;
AMSgvolume::_amsrm AMSgvolume::_UnitRM;
AMSgvolume::_amsrm::_amsrm(){
int i,j;
for(i=0;i<3;i++){
 for(j=0;j<3;j++){
  if(i==j)_nrm[i][j]=1;
  else _nrm[i][j]=0;
 }
}
}

AMSgvolume::_amsrm::_amsrm(number nrm[3][3]){
int i,j;
for(i=0;i<3;i++){
 for(j=0;j<3;j++){
  _nrm[i][j]=nrm[i][j];
 }
}
}


AMSgvolume::~AMSgvolume(){
 delete[] _par;
 delete[] _shape;
 delete _nrmA;
if(_nrm != &_UnitRM)delete _nrm;
}

geant AMSgvolume::dgeant=1.e-4;
uinteger AMSgvolume::_GlobalRotMatrixNo=0;
integer AMSgvolume::_LastRotMatrixOutOfOrder=0;
AMSgvolume::AMSgvolume (char  matter[],integer rotmno,const char name[], 
           const char shape[] ,   geant par[] , integer npar, 
            geant coo[] ,  number nrm[][3] , const char gonly[] , 
           integer posp,integer gid, integer rel) :
     _npar(npar), _posp(posp),_rotmno(rotmno),
#ifdef __G4AMS__
   _pg4v(0),_pg4l(0),_pg4rm(0),_offspring(0),_smartless(2),
#endif
     _gid(abs(gid)),_cooA(coo[0],coo[1],coo[2]),_rel(rel),AMSNode(0){
      _pgtmed= (AMSgtmed *)AMSgObj::GTrMedMap.getp(AMSID(0,matter));
      if(_pgtmed)_matter=_pgtmed->getmedia();
      else{
        cerr<<"AMSgvolume-ctor-F-NoSuchMedium "<<matter<<endl;
        exit(1);
      }
   setname(name);
   setid(_gid);
   _coo=_cooA;
   if(_npar){
     _par=new geant[_npar];
     UCOPY(par,_par,sizeof(_par[0])*npar/sizeof(integer));
   }
    else _par=0;
    
   if(shape){
      _shape=new char[strlen(shape)>3?strlen(shape)+1:5];
      strcpy(_shape,shape);
      if(strlen(_shape)<=3){
       _shape[3]=' ';
       _shape[4]=0;
      }
   }
   else _shape=0;

   if(gonly)strcpy(_gonly,gonly);

    if(!_rotmno && !_rel){
        cerr <<"AMSgvolume::ctor-W-RotNatrixZeroInAbsModeIsNotAllowed "<<getname() << " " <<getid()<<endl;
       _rotmno=1;
     }

   if(_rotmno ){
    _nrm= new _amsrm(nrm);
   }
   else _nrm= &_UnitRM;   
   _nrmA= new _amsrm(nrm);
 
      
}



void AMSgvolume::_gl2loc(AMSgvolume * cur, AMSPoint & coo){
cur=cur->up();
number nrm[3][3];
if(cur)_gl2loc(cur,coo);
else return;
 amsprotected::transpose(cur->_nrm->_nrm,nrm);
 amsprotected::mm3(nrm,_nrm->_nrm,0);
 coo=(coo-cur->_coo).mm3(nrm);
}

void AMSgvolume::_init(){


  //
  // Translate global to local if rel==0
  // Translate local  to global if rel!=0 
  AMSgvolume * cur=this;
  number nrm[3][3]={1,0,0,
                    0,1,0,
                    0,0,1};
   AMSPoint coo;
  if(_rel){
    // loc2gl
   coo=0;
   while (cur){
      coo=coo.mm3(cur->_nrm->_nrm)+cur->_coo;
      amsprotected::mm3(cur->_nrm->_nrm,nrm,0);
      cur=cur->up();
   }
   _cooA=coo;
   *_nrmA=_amsrm(nrm);
  }
  else{
    coo=_cooA;
    // gl2loc   // recursion needed
      _gl2loc(cur,coo);
    _coo=coo;     
  }

}

ostream & AMSgvolume::print(ostream & stream)const{
return(AMSID::print(stream) << _shape <<  " GV" << endl);}


AMSPoint AMSgvolume::gl2loc(AMSPoint vect){
#ifndef __AMSDEBUG__
vect=(vect-_cooA).mm3i(_nrmA->_nrm);
return vect;
#else

AMSPoint small(1.e-4,1.e-4,1.e-4);
AMSPoint loc=(vect-_cooA).mm3i(_nrmA->_nrm);
AMSPoint gl=loc2gl(loc);
AMSPoint diff=gl-vect;
if(diff.abs() >= small){
    cerr <<"AMSgvolume::loc2gl-E-Invert operation error "
    <<vect<<" "<<loc<<" "<<gl<<endl;
}
return loc;
#endif

} 

AMSPoint AMSgvolume::loc2gl(AMSPoint xv){
    xv=xv.mm3(_nrmA->_nrm)+_cooA;
    return xv;
    //AMSPoint small(1.e-4,1.e-4,1.e-4);
    //AMSPoint gl=xv.mm3(_nrmA->getdta())+_cooA;
    //AMSPoint loc=gl2loc(gl);
    //AMSPoint diff=loc-xv;
    //if(diff.abs() >= small){
    //    cerr <<"AMSgvolume::loc2gl-E-Invert operation error "
    //    <<xv<<" "<<gl<<" "<<loc<<endl;
    //}
    //return loc;
}

#ifdef __G4AMS__

void AMSgvolume::removeboolean(){
// remove all boolean
   if(up() && up()->offspring()){
     if(next())next()->removeboolean();
     if(!prev()){
        up()->_offspring=0;
     }
     delete this;
   } 

}

void AMSgvolume::addboolean(AMSgvolume * ptr){

  AMSgvolume *cur;
  cur=_offspring;
  if(cur){
   while(cur->next())cur=cur->next();
   cur->_next=ptr;
   ptr->_prev=cur;
  }
  else{
   _offspring=ptr;
   ptr->_prev=0;
  }
  ptr->_up=this;
  ptr->_init();


}

 amsg4rm * AMSgvolume::_pg4rmU=0;
integer AMSgvolume::_Norp=0;
 G4VSolid* AMSgvolume::_MakeG4Volumes(){

    // This is main routine for geant4 like volumes
    // Quite boring

    //  rotmatrix if necessary
    if (!_pg4rm){
      //check if zero
      if(!_rotmno){
         _pg4rm=0;
      }
      else{
        // Here the headache
        _pg4rm= new AMSG4RotationMatrix(_nrm->_nrm);
        _pg4rm->invert();
      }
    }

    number maxstep=DBL_MAX;
     G4VSolid * psolid=0;
    if(!_pg4l){
     G4String shape(_shape);
     if( shape == "BOX "){
//     cout <<_par[0]<<" "<<_par[1]<<" "<<_par[2]<<" "<<_name<<" "<<_id<<endl;
       psolid=new G4Box(G4String(_name),_par[0]*cm,_par[1]*cm,_par[2]*cm);
       for(int par=0;par<_npar;par++){
         if(maxstep>_par[par] && _par[par]>0)maxstep=_par[par];
       }
     }
     else if (shape =="TUBE"){
       psolid=new G4Tubs(G4String(_name),_par[0]*cm,_par[1]*cm,_par[2]*cm,0*degree,360.*degree);
       for(int par=0;par<_npar;par++){
         if(maxstep>_par[par] && _par[par]>0)maxstep=_par[par];
       }
     }
     else if (shape =="TUBS"){
       for(int par=0;par<3;par++){
         if(maxstep>_par[par] && _par[par]>0)maxstep=_par[par];
       }
       psolid=new G4Tubs(G4String(_name),_par[0]*cm,_par[1]*cm,_par[2]*cm,_par[3]*degree,_par[4]*degree-_par[3]*degree);
     }
     else if (shape =="CONE"){
       psolid=new G4Cons(G4String(_name),_par[1]*cm,_par[2]*cm,_par[3]*cm,_par[4]*cm,_par[0]*cm,0*degree,360.*degree);
       for(int par=0;par<_npar;par++){
         if(maxstep>_par[par] && _par[par]>0)maxstep=_par[par];
       }
     }
     else if (shape =="CONS"){
       psolid=new G4Cons(G4String(_name),_par[1]*cm,_par[2]*cm,_par[3]*cm,_par[4]*cm,_par[0]*cm,_par[5]*degree,_par[6]*degree-_par[5]*degree);
       for(int par=0;par<5;par++){
         if(maxstep>_par[par] && _par[par]>0)maxstep=_par[par];
       }
     }
     else if (shape =="TRD1"){
       psolid=new G4Trd(G4String(_name),_par[0]*cm,_par[1]*cm,_par[2]*cm,_par[2]*cm,_par[3]*cm);
     }
     else if (shape =="TRD2"){
       psolid=new G4Trd(G4String(_name),_par[0]*cm,_par[1]*cm,_par[2]*cm,_par[3]*cm,_par[4]*cm);
     }
     else if (shape =="PARA"){
       psolid=new G4Para(G4String(_name),_par[0]*cm,_par[1]*cm,_par[2]*cm,_par[3]*degree,_par[4]*degree,_par[5]*degree);
     }
     else if (shape =="SPHE"){
       psolid=new G4Sphere(G4String(_name),_par[0]*cm,_par[1]*cm,_par[2]*degree,_par[3]*degree-_par[2]*degree,_par[4]*degree,_par[5]*degree-_par[4]*degree);
         if(maxstep>_par[1]-_par[0] && _par[1]-_par[0]>0 )maxstep=_par[1]-_par[0];
     }
     else if (shape =="PGON"){
       integer nsurf=_par[3];
       number * z = new number[nsurf];
       number * rmin = new number[nsurf];
       number * rmax = new number[nsurf];
       for(int i=0;i<nsurf;i++){
        z[i]=_par[4+3*i]*cm;
        rmin[i]=_par[5+3*i]*cm;
        rmax[i]=_par[6+3*i]*cm;
        if(maxstep>rmax[i]-rmin[i])maxstep=rmax[i]-rmin[i];
       }       
       psolid=new G4Polyhedra(G4String(_name),_par[0]*degree,_par[1]*degree,int(_par[2]),nsurf,z,rmin,rmax);
//       psolid=new G4BREPSolidPolyhedra(G4String(_name),_par[0]*degree,_par[1]*degree,int(_par[2]),nsurf,z[0],z,rmin,rmax);
       delete[] z;
       delete[] rmin;
       delete[] rmax;
     }
     else if (shape =="PCON"){
       integer nsurf=_par[2];
       number * z = new number[nsurf];
       number * rmin = new number[nsurf];
       number * rmax = new number[nsurf];
       for(int i=0;i<nsurf;i++){
        z[i]=_par[3+3*i]*cm;
        rmin[i]=_par[4+3*i]*cm;
        rmax[i]=_par[5+3*i]*cm;
       }       
       psolid=new G4Polycone(G4String(_name),_par[0]*degree,_par[1]*degree,nsurf,z,rmin,rmax);
       delete[] z;
       delete[] rmin;
       delete[] rmax;
     }
     else if(shape=="TRAP"){
       psolid=new G4Trap(G4String(_name),_par[0]*cm,_par[1]*degree,_par[2]*degree,_par[3]*cm,_par[4]*cm,_par[5]*cm,_par[6]*degree,_par[7]*cm,_par[8]*cm,_par[9]*cm,_par[10]*degree);
     }
     else {
       cerr<<" AMSgvolume::G3ToG4Interface-F-NoSuchShapeYet "<<_shape<<endl;
       exit(1);
     }



     //Check For MANY
    if(G4String(_gonly) !="ONLY"){
      if(strstr(_gonly,"BOO")){
         // check if it the last one
         if(_gonly[3]=='L' && !offspring()){
          cerr<<" AMSgvolume::AMSG4Interface-F-TheOnlyBooleanVolumeIsNotAllowed"<<endl;
          exit(1);
         }
         else{
           AMSgvolume *cur=offspring();
           G4String name(getname());
           while (cur){
            G4VSolid *ps=cur->_MakeG4Volumes();
            G4ThreeVector trans(cur->getcoo(0),cur->getcoo(1),cur->getcoo(2));
             name+=G4String(cur->_gonly[3]);
             name+=G4String(cur->getname());
            if(cur->_gonly[3]=='+'){
             psolid=new G4UnionSolid(name,psolid,ps,cur->_pg4rm,trans);
            }           
            else if(cur->_gonly[3]=='-'){
             psolid=new G4SubtractionSolid(name,psolid,ps,cur->_pg4rm,trans);
            }           
            else if(cur->_gonly[3]=='/'){
             psolid=new G4IntersectionSolid(name,psolid,ps,cur->_pg4rm,trans);
            }           
            else {
             cerr<<"AMSgvolume::AMSG4Interface-F-BooleanOpIsNotDefined "<<cur->_gonly<<endl;
             exit(1);
            }
            cur=cur->next();
           }   
           // delete volumes
           if(offspring()){
            offspring()->removeboolean();
           }
           else return psolid;
         }
         
      }
      else{
       cerr<<"AMSgvolume::_MakeG4Volumes-F-MANYIsNotAllowedInG4 "<<_name<<" "<<_gid<<endl;
       exit(1);
      }
    }  


     // Now logical volume
     _pg4l= new G4LogicalVolume(psolid,_pgtmed->getpgmat()->getpamsg4m(),G4String(_name));    
     if(_pgtmed->IsSensitive()){
      _pg4l->SetSensitiveDetector(AMSG4DummySD::pSD()); 
//      _pg4l->SetUserLimits(new G4UserLimits(2*maxstep*cm));
     }
// Add user limits 
     else {
      _pg4l->SetUserLimits(new G4UserLimits(maxstep*cm));
     }    
     _pg4l->SetSmartless(_smartless);
//     cout <<" smart "<<getname()<<" "<<_pg4l->GetSmartless()<<endl;

   }
    // Now placement 
    
    _pg4v=new G4PVPlacement(up()?_pg4rm:0,G4ThreeVector(_coo[0]*cm,_coo[1]*cm,_coo[2]*cm),G4String(_name),_pg4l,up()?up()->_pg4v:0,false,_gid);
    if(!up())_pg4l-> SetVisAttributes (G4VisAttributes::Invisible);
    else if(!(up()->up()))_pg4l-> SetVisAttributes (G4VisAttributes::Invisible);
    if(!up() && _Norp){
       // fatal logic error
       cerr<< " AMSgvolume::_MakeG4Volumes-F-LogicErrorAmbigWorldVolume for "<<
       getname()<<" "<<getid()<<endl;
       exit(1);
    }
    else if(!up())_Norp++;

    return psolid;

}

void AMSgvolume::MakeG4Volumes(){


 if(!_pg4rmU){
   _pg4rmU=new AMSG4RotationMatrix();   
#ifdef __AMSDEBUG__
   cout <<_pg4rmU->xx()<<_pg4rmU->xy()<<_pg4rmU->xz()<<_pg4rmU->yx()<<_pg4rmU->yy()<<_pg4rmU->yz()<<_pg4rmU->zx()<<_pg4rmU->zy()<<_pg4rmU->zz()<<endl;
#endif
   AMSG4RotationMatrix::Test();
 }



// Recursive Routine

  _Nlog++;
  _Nrm++;
  _Nph++;
  AMSgvolume *cur = up() && prev()?prev():up();
    int newrm=1;
    while (cur){
     if(VolumeHasSameRotationMatrixAs(cur)){
      pg4rm()=cur->pg4rm();
      _Nrm--;
      newrm=0;
      break;
     }
     cur=cur->up() && cur->prev()?cur->prev():cur->up();
    }
    if(newrm){
     cur = up() && prev()?prev():0;
     while (cur){
      if(VolumeHasSameRotationMatrixAs(cur)){
       pg4rm()=cur->pg4rm();
       _Nrm--;
       break;
      }
      cur=cur->up() && cur->prev()?cur->prev():cur->down();
     }
    }
    int newv=1;
    cur = up() && prev()?prev():up();
    while (cur){
      if(VolumeHasSameG4AttributesAs(cur)){
       pg4l()=cur->pg4l(); 
       _Nlog--;
       newv=0;
       break;  
      }
     cur=cur->up() && cur->prev()?cur->prev():cur->up();
    }
    if(newv){
     cur = up() && prev()?prev():0;
     while (cur){
      if(VolumeHasSameG4AttributesAs(cur)){
       pg4l()=cur->pg4l(); 
       _Nlog--;
       break;  
      }
      cur=cur->up() && cur->prev()?cur->prev():cur->down();
     }
    }
    _MakeG4Volumes();
     if(down())down()->MakeG4Volumes();
     if(up() && next())next()->MakeG4Volumes();
     
}

int AMSgvolume::VolumeHasSameG4AttributesAs(AMSgvolume* o ){
if(strcmp(getname(),o->getname()))return 0;
for (int i=0;i<_npar;i++){
 if(getpar(i)!= o->getpar(i))return 0;
}
if(strcmp(_shape,o->_shape))return 0;
if(strcmp(_gonly,o->_gonly))return 0;
if(_pgtmed != o->_pgtmed)return 0;
if(strstr(_gonly,"BOO")){
 if(_gonly[3]=='L'){
  return offspring()->VolumeHasSameG4AttributesAs(o->offspring());
 }
  else {
  return next() && o->next()?next()->VolumeHasSameG4AttributesAs(o->next()):0;
  }
}
return 1;
}

#endif

int AMSgvolume::VolumeHasSameRotationMatrixAs(AMSgvolume * o ){
for (int i=0;i<3;i++){
 for (int j=0;j<3;j++){
   if(getnrm(i,j) != o->getnrm(i,j))return 0;
 }
}
return 1;

}


int AMSgvolume::VolumeHasSameG3AttributesAs(AMSgvolume* o ){
if(strcmp(getname(),o->getname()))return 0;
if(strcmp(_shape,o->_shape))return 0;
if(_pgtmed != o->_pgtmed)return 0;
if(abs(_posp)!=abs(o->_posp))return 0;
if(_posp==0){
 for (int i=0;i<_npar;i++){
  if(getpar(i)!= o->getpar(i))return 0;
 }
}
return 1;
}

integer AMSgvolume::_Nph=0;
integer AMSgvolume::_Nlog=0;
integer AMSgvolume::_Nrm=0;






void AMSgvolume::MakeG3Volumes(){


//ignore boolean volumes
if(!strstr(_gonly,"BOO")){
// Recursive Routine

  _Nlog++;
  _Nrm++;
  _Nph++;
  AMSgvolume *cur = up() && prev()?prev():up();
    int newrm=1;
    while (cur){
     if(VolumeHasSameRotationMatrixAs(cur)){
      _Nrm--;
      newrm=0;
      _rotmno=cur->getrotmatrixno();
      break;
     }
     cur=cur->up() && cur->prev()?cur->prev():cur->up();
    }
    if(newrm){
      cur =up() && prev()?prev():0;
      while (cur){
      if(VolumeHasSameRotationMatrixAs(cur)){
       _Nrm--;
       newrm=0;
       _rotmno=cur->getrotmatrixno();
       break;
      }
      cur=cur->up() && cur->prev()?cur->prev():cur->down();
      }
    }
    if(newrm){
      if(_rotmno){
        _rotmno=++_GlobalRotMatrixNo;
        geant r[3],sph,cth,cph,sth,theta[3],phi[3];
        integer rt=0;
        for (int j=0;j<3;j++){
         for (int k=0;k<3;k++) r[k]=(_nrm->_nrm)[k][j];
         GFANG(r, cth,  sth, cph, sph,  rt);
         theta[j]=atan2((double)sth,(double)cth)*AMSDBc::raddeg;
         phi[j]=atan2((double)sph,(double)cph)*AMSDBc::raddeg;
        }
        GSROTM(_rotmno,theta[0],phi[0],theta[1],phi[1],theta[2],phi[2]);
#ifdef __AMSDEBUG__
        if(debug)GPROTM(_rotmno);  
#endif
      }
      else _Nrm--;
    }

    cur = up() && prev()?prev():up();
    int newv=1;
    while (cur){
      if(VolumeHasSameG3AttributesAs(cur)){
       _Nlog--;
       newv=0;
       break;  
      }
     cur=cur->up() && cur->prev()?cur->prev():cur->up();
    }
    if(newv){
     cur = up() && prev()?prev():0;
     while (cur){
      if(VolumeHasSameG3AttributesAs(cur)){
       _Nlog--;
       newv=0;
       break;  
      }
      cur=cur->up() && cur->prev()?cur->prev():cur->down();
     }
    }
    if(newv){
       int ivol;
       if(!_posp)GSVOLU(_name,_shape,_matter,_par,_npar,ivol);
       else if(_posp>0)  GSVOLU(_name,_shape,_matter,_par,0,ivol);
//       else   GSVOLU(_name,_shape,_matter,_par,0,ivol);
    }    
// Position
    if(up()){
     geant coo[3];
     _coo.getp(coo[0],coo[1],coo[2]);
     // check gonly;
     if(_posp){
       GSPOSP(_name,_gid,up()->_name,coo[0],coo[1],coo[2],_rotmno,_gonly,
          _par,_npar);
     }
     else {
      GSPOS(_name,_gid,up()->_name,coo[0],coo[1],coo[2],_rotmno,_gonly);
     } 
    }
 }
    if(down())down()->MakeG3Volumes();
    if(up() && next())next()->MakeG3Volumes();
     
}




