#include <server.h>
AMSServer* AMSServer::_Head=0;

int main(int argc, char * argv[]){

 try{
    AMSServer::Singleton()=new AMSServer(argc,argv);
    for(;;){
     sleep(1);
     AMSServer::Singleton()->getOrb()->perform_work();
          
    }
 }
 catch (CORBA::SystemException &a){
   return 1;
 }
 return 0;
}


AMSServer::AMSServer(int argc, char* argv[]){

 char * ior=0;
 uinteger uid=0;
 char *rfile=0;
 for (char *pchar=0; argc>1 &&(pchar=argv[1],*pchar=='-'); (argv++,argc--)){
    pchar++;
    switch (*pchar){
    case 'I':   //ior
     ior=pchar;
     break;
    case 'R':   //RunFileName
     rfile=++pchar;
     break;
    case 'U':   //uid
     uid=atoi(++pchar);
     break;
  }
 }
  _orb=CORBA::ORB_init(argc,argv);
   CORBA::Object_var obj=_orb->resolve_initial_references("RootPOA");
  _poa=PortableServer::POA::_narrow(obj);
  _mgr=_poa->the_POAManager();
  _mgr->activate();
  Server_impl* _simp= new Server_impl();
 if(ior==0){      //  Primary
  if(rfile){  
   Producer_impl * _pimp=new Producer_impl();
  }  
  
 }
 else{
 }


}
