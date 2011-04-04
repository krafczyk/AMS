#include "TGFrame.h"
#include "TCanvas.h"
#include "TRootEmbeddedCanvas.h"

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include "math.h"

using namespace std;
#ifndef TAB_H_
#define TAB_H_
class Tab_Frame: public TGVerticalFrame{
public:
	Tab_Frame(const TGWindow* p,vector<string>& pads);
	~Tab_Frame();
	Int_t Getpid(string name){
		map<string,Int_t>::iterator it; 
		it=fpads.find(name); 
		if(it==fpads.end()){
			cout<<"-1";
			return -1;
		}
		else{
			cout<<it->second<<endl;
			return it->second;
		}
	};
	void test(){
		
		cout<<"test "<<0<<endl;	
	}
	TCanvas* GetCanvas(){return fEcanvas->GetCanvas();}
private:
	TRootEmbeddedCanvas* fEcanvas;
	Int_t _nPads;
	vector<string> fpads_name_list;
	map<string,Int_t> fpads;
};
#endif
