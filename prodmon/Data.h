#include <vector>
#include <iostream>
#include "TH1F.h"
#include "TDirectory.h"


#include "root.h"
#include "amschain.h"

#include <string>
#include "ProgBar.h"

using namespace std;
#ifndef DATA_H_
#define DATA_H_
class Data{
public:
	Data(vector<string> hists_name,vector<string> hists_name_summary);
	~Data();
	string data1_filename;
	string data2_filename;
	Bool_t Update_data(Int_t id);
	Bool_t Generate_hist();
	vector<TH1F*> _hists_1;
	vector<TH1F*> _hists_2;
	vector<TH1F*> _hists_summary;
	vector<string> _hists_name;
	vector<string> _hists_name_summary;
	TH1F* _hists_h;	
private:
	
	Int_t nhist;
};

#endif
