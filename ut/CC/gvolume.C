#include <gvolume.h>
#include <math.h>
#include <amsgobj.h>
#include <gmat.h>
integer AMSgvolume::debug=1;
geant AMSgvolume::dgeant=1.e-4;


AMSgvolume::AMSgvolume (integer matter,integer rotmno,const char name[], 
           const char shape[] ,   geant par[] , integer npar, 
            geant coo[] ,  number nrm[][3] , const char gonly[] , 
           integer posp,integer gid,integer rel) :
    _matter(matter),_rotmno(rotmno), _npar(npar), _posp(posp),
     _gid(gid),_cooA(coo[0],coo[1],coo[2]),_rel(rel),AMSNode(0){
   setname(name);
   _coo=_cooA;
   if(shape)strcpy(_shape,shape);
   if(gonly)strcpy(_gonly,gonly);
   UCOPY(par,_par,6*sizeof(par[0])/4);
   UCOPY(nrm,_nrm,3*3*sizeof(nrm[0][0])/4);
   UCOPY(nrm,_nrmA,3*3*sizeof(nrm[0][0])/4);
   amsprotected::transpose(_nrmA,_inrmA);
//
//  Part of geant initialization here
//
      int ivol;
      if (gid > 0 && !posp){
       GSVOLU(_name,_shape,matter,_par,npar,ivol);
      }      
       setid(gid);
   
      
}

AMSgvolume::AMSgvolume (char  matter[],integer rotmno,const char name[], 
           const char shape[] ,   geant par[] , integer npar, 
            geant coo[] ,  number nrm[][3] , const char gonly[] , 
           integer posp,integer gid, integer rel) :
    _rotmno(rotmno), _npar(npar), _posp(posp),
     _gid(gid),_cooA(coo[0],coo[1],coo[2]),_rel(rel),AMSNode(0){
      AMSgtmed *p= (AMSgtmed *)AMSgObj::GTrMedMap.getp(AMSID(0,matter));
      if(p)_matter=p->getmedia();
      else{
        cerr<<"AMSgvolume-ctor-F-NoSuchMedium "<<matter<<endl;
        exit(1);
      }

   setname(name);
   _coo=_cooA;
   if(shape)strcpy(_shape,shape);
   if(gonly)strcpy(_gonly,gonly);
   UCOPY(par,_par,6*sizeof(par[0])/4);
   UCOPY(nrm,_nrm,3*3*sizeof(nrm[0][0])/4);
   UCOPY(nrm,_nrmA,3*3*sizeof(nrm[0][0])/4);
   amsprotected::transpose(_nrmA,_inrmA);
//
//  Part of geant initialization here
//
      int ivol;
      if (gid > 0 && !posp){
       GSVOLU(_name,_shape,_matter,_par,npar,ivol);
      }      
       setid(gid);
   
      
}

AMSgvolume::AMSgvolume (char  matter[],integer rotmno,const char name[], 
           const char shape[] ,   geant par[] , integer npar, 
           geant coo[] ,  number nrm1[3] , number nrm2[3], number nrm3[3],
           const char gonly[] , 
           integer posp,integer gid, integer rel) :
    _rotmno(rotmno), _npar(npar), _posp(posp),
     _gid(gid),_cooA(coo[0],coo[1],coo[2]),_rel(rel),AMSNode(0){
      AMSgtmed *p= (AMSgtmed *)AMSgObj::GTrMedMap.getp(AMSID(0,matter));
      if(p)_matter=p->getmedia();
      else{
        cerr<<"AMSgvolume-ctor-F-NoSuchMedium "<<matter<<endl;
        exit(1);
      }

   setname(name);
   _coo=_cooA;
   if(shape)strcpy(_shape,shape);
   if(gonly)strcpy(_gonly,gonly);
   UCOPY(par,_par,6*sizeof(par[0])/4);
   int j;
   for(j=0;j<3;j++){
      _nrm[j][0]=nrm1[j];
      _nrm[j][1]=nrm2[j];
      _nrm[j][2]=nrm3[j];
     _nrmA[j][0]=nrm1[j];
     _nrmA[j][1]=nrm2[j];
     _nrmA[j][2]=nrm3[j];
   }
     amsprotected::transpose(_nrmA,_inrmA);
//
//  Part of geant initialization here
//
      int ivol;
      if (gid > 0 && !posp){
       GSVOLU(_name,_shape,_matter,_par,npar,ivol);
      }      
       setid(gid);
   
      
}

void AMSgvolume::_gl2loc(AMSgvolume * cur, AMSPoint & coo){
cur=cur->up();
number nrm[3][3];
if(cur)_gl2loc(cur,coo);
else return;
 amsprotected::transpose(cur->_nrm,nrm);
 amsprotected::mm3(nrm,_nrm,0);
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
      coo=coo.mm3(cur->_nrm)+cur->_coo;
      amsprotected::mm3(cur->_nrm,nrm,0);
      cur=cur->up();
   }
   _cooA=coo;
   int i,j;
   for(i=0;i<3;i++){
     for(j=0;j<3;j++){
      _nrmA[i][j]=nrm[i][j];
     }
   }
  }
  else{
    coo=_cooA;
    // gl2loc   // recursion needed
      _gl2loc(cur,coo);
    _coo=coo;     
  }
   amsprotected::transpose(_nrmA,_inrmA);

  if (_gid > 0) {
   int newv=0;
   int newr=0;
   if(prev()){
    if(prev()->_rotmno != _rotmno) {
     newr=1;
    }
    cur=prev();
    newv=1;
    while (cur){
     if(!strcmp(cur->getname(),getname())){
      newv=0;
     }
     cur=(AMSgvolume*)cur->prev();
    } 
   }
   else {
    newv=1;
    if (up()){
     if(up()->_rotmno != _rotmno){
      newr=1;

     }
    }
   }
   int ivol;
   if(newv) {
    if(_posp > 0){
     GSVOLU(_name,_shape,_matter,_par,0,ivol);
    }
   }
   if(newr){
    geant r[3],sph,cth,cph,sth,theta[3],phi[3];
    integer rt=0;
    for (int j=0;j<3;j++){
    for (int k=0;k<3;k++) r[k]=_nrm[k][j];
     GFANG(r, cth,  sth, cph, sph,  rt);
     theta[j]=atan2((double)sth,(double)cth)*AMSDBc::raddeg;
     phi[j]=atan2((double)sph,(double)cph)*AMSDBc::raddeg;
    }
    GSROTM(_rotmno,theta[0],phi[0],theta[1],phi[1],theta[2],phi[2]);
#ifdef __AMSDEBUG__
    if(debug)GPROTM(_rotmno);  
#endif
   }    
   //   char * nn=(char*)getname();
   //   char * pp=(char*)up()->getname();
   geant coo[3];
   _coo.getp(coo[0],coo[1],coo[2]);
   if(_posp){
    GSPOSP(_name,_gid,up()->_name, 
          coo[0],coo[1],coo[2],_rotmno,_gonly, 
          _par,_npar);
   }
   else{
    GSPOS(_name,_gid,up()->_name, 
          coo[0],coo[1],coo[2],_rotmno,_gonly);
   }
  }

}
ostream & AMSgvolume::print(ostream & stream)const{
return(AMSID::print(stream) << _shape <<  " GV" << endl);}


AMSPoint AMSgvolume::gl2loc(AMSPoint vect){
#ifndef __AMSDEBUG__
vect=(vect-_cooA).mm3(_inrmA);
return vect;
#else

AMSPoint small(1.e-4,1.e-4,1.e-4);
AMSPoint loc=(vect-_cooA).mm3(_inrmA);
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
    xv=xv.mm3(_nrmA)+_cooA;
    return xv;
    //AMSPoint small(1.e-4,1.e-4,1.e-4);
    //AMSPoint gl=xv.mm3(_nrmA)+_cooA;
    //AMSPoint loc=gl2loc(gl);
    //AMSPoint diff=loc-xv;
    //if(diff.abs() >= small){
    //    cerr <<"AMSgvolume::loc2gl-E-Invert operation error "
    //    <<xv<<" "<<gl<<" "<<loc<<endl;
    //}
    //return loc;
}
