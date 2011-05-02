#include <vector>
#include <iostream>
#include "TH1F.h"
#include "TDirectory.h"
#include "time.h"

#include "root.h"
#include "amschain.h"

#include <string>
#include "ProgBar.h"

using namespace std;
#ifndef DATA_H_
#define DATA_H_
class Data{
public:
	Data(vector<string> hists_name,vector<string> hists_name_summary,vector<string> hists_name_h);
	~Data();
	string data1_filename;
	string data2_filename;
	Bool_t Update_data(Int_t id);
	Bool_t Generate_hist();
	void Set_data1_filename(string n){data1_filename=n;n1_changed=true;}
	void Set_data2_filename(string n){data2_filename=n;n2_changed=true;}
	vector<TH1F*> _hists_1;
	vector<TH1F*> _hists_2;
	vector<TH1F*> _hists_summary;
	vector<string> _hists_name;
	vector<string> _hists_name_summary;
	vector<TH1F*> _hists_h;
	vector<string> _hists_name_h;	
private:
	time_t fpre_time;
	bool n1_changed;
	bool n2_changed;
	Int_t nhist;
};

#endif
