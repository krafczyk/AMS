#include <gvolume.h>
#include <math.h>
#include <amsgobj.h>
#include <gmat.h>
integer AMSgvolume::debug=1;
geant AMSgvolume::dgeant=1.e-4;
void AMSgvolume::setcoo(geant coo[]){
_cooA=AMSPoint(coo[0],coo[1],coo[2]);
_coo=_cooA;
  AMSgvolume * cur;
   cur=up();
   while (cur){
    _coo=_coo-cur->_coo;
    cur=cur->up();
   }
}
void AMSgvolume::setnrm(number nrm[3][3]){
   UCOPY(nrm,_nrm,3*3*sizeof(nrm[0][0])/4);
   UCOPY(nrm,_inrm,3*3*sizeof(nrm[0][0])/4);
  AMSgvolume * cur;
   cur=up();
   while (cur){
    mm3(cur->_nrm,_inrm);
    cur=cur->up();
   }
}

AMSgvolume::AMSgvolume (integer matter,integer rotmno,const char name[], 
           const char shape[] ,   geant par[] , integer npar, 
            geant coo[] ,  number nrm[][3] , const char gonly[] , 
           integer posp,integer gid, integer abs) :
    _matter(matter),_rotmno(rotmno), _npar(npar), _posp(posp),
     _gid(gid),_cooA(coo[0],coo[1],coo[2]),_Absolute(abs),AMSNode(0){
   setname(name);
   _coo=_cooA;
   if(shape)strcpy(_shape,shape);
   if(gonly)strcpy(_gonly,gonly);
   UCOPY(par,_par,6*sizeof(par[0])/4);
   UCOPY(nrm,_nrm,3*3*sizeof(nrm[0][0])/4);
   UCOPY(nrm,_inrm,3*3*sizeof(nrm[0][0])/4);
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
           integer posp,integer gid , integer abs) :
    _rotmno(rotmno), _npar(npar), _posp(posp),
     _gid(gid),_cooA(coo[0],coo[1],coo[2]), _Absolute(abs), AMSNode(0){
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
   UCOPY(nrm,_inrm,3*3*sizeof(nrm[0][0])/4);
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
           integer posp,integer gid, integer abs) :
    _rotmno(rotmno), _npar(npar), _posp(posp),
     _gid(gid),_cooA(coo[0],coo[1],coo[2]), _Absolute(abs), AMSNode(0){
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
     _inrm[j][0]=nrm1[j];
     _inrm[j][1]=nrm2[j];
     _inrm[j][2]=nrm3[j];
   }
//
//  Part of geant initialization here
//
      int ivol;
      if (gid > 0 && !posp){
       GSVOLU(_name,_shape,_matter,_par,npar,ivol);
      }      
       setid(gid);
   
      
}

void AMSgvolume::_init(){
  AMSgvolume * cur;
   cur=up();
   while (cur){
    if(_Absolute==0)
    _coo=_coo-cur->_coo;
    else
    _cooA=_cooA+cur->_coo;
    mm3(cur->_nrm,_inrm);
    cur=cur->up();
   }


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
    if(_posp){
     GSVOLU(_name,_shape,_matter,_par,0,ivol);
    }
   }
   if(newr){
    geant r[3],sph,cth,cph,sth,theta[3],phi[3];
    integer rt;
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
AMSPoint loc=vect-_coo;
AMSgvolume *cur=up();
number nrm[3][3];
UCOPY(_nrm,nrm,sizeof(nrm[0][0])/4*9);
loc=loc.mm3(nrm);
while (cur){
 mm3(cur->_nrm,nrm);
 loc=loc.mm3(cur->_nrm)-cur->_coo.mm3(nrm);
 cur=cur->up();
}
return loc;
} 

AMSPoint AMSgvolume::loc2gl(AMSPoint xv){
number inrm[3][3];
transpose(_nrm,inrm);
xv=xv.mm3(inrm)+_coo;
AMSgvolume *cur=up();
while (cur){
transpose(cur->_nrm,inrm);
xv=xv.mm3(inrm)+cur->_coo;
cur=cur->up();
}
return xv;
}
