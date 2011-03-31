// $Id: tkmerge.C,v 1.2 2011/03/31 10:10:06 haino Exp $

#include "TkDBc.h"

class TkDBcD : public TkDBc {

public:
  TkDBcD(const char *fname, Int_t setup = 3) { init(setup, fname); }
};

void tkcomp(void)
{
  TString str1;
  TString str2;

  Int_t blyr = 0;

  for (Int_t i = 0; i < 196+6; i++) {
    str1.ReadToken(std::cin);
    if (std::cin.eof()) break;

    if (!str1.BeginsWith("Plane") && !str1.BeginsWith("L")) {
      std::cerr << "Wrong format[1]: " << str1.Data() << std::endl;
      return;
    }

    str2.ReadToken(std::cin);
    if (str1 != str2) {
      std::cerr << "Wrong format[2]: " << str1.Data() << " != "
		                       << str2.Data() << std::endl;
      return;
    }

    AMSPoint  npos1, npos2;
    AMSPoint  apos1, apos2;
    AMSRotMat rot1, rot2;
    Double_t  rm1[3][3];
    Double_t  rm2[3][3];

    std::cin >> npos1 >> npos2;
/*
    if ((npos1-npos2).norm() > 0) {
      std::cerr << "Wrong format[3]: " << apos1 << " != " << std::endl
		                       << apos2 << std::endl;
      return;
    }
*/
    std::cin >> rot1 >> rot2;

    std::cin >> apos1 >> apos2;
    for (Int_t j = 0; j < 3; j++) {
      for (Int_t k = 0; k < 3; k++) std::cin >> rm1[j][k];
      for (Int_t k = 0; k < 3; k++) std::cin >> rm2[j][k];
    }
    rot1.SetMat(rm1);
    rot2.SetMat(rm2);

    Int_t data[6];
    for (Int_t j = 0; j < 6; j++) std::cin >> data[j];
    for (Int_t j = 0; j < 3; j++) {
      if (data[j] != data[j+3]) {
	std::cerr << "Wrong format[3]: " << j << " "
		  << data[j] << " != " << data[j+3] << std::endl;
	return;
      }
    }

    if (data[1] > 0) {
      AMSPoint dp = (npos1+apos1)-(npos2+apos2);
      dp = dp*1e4;
      if (dp.norm() > 0.9) {
	std::cout << Form("DiffN: %4d %6.1f %6.1f %6.1f", data[0],
			  dp.x(), dp.y(), dp.z()) << std::endl;
      }
      continue;
    }

    if ((apos1-apos2).norm() > 0) {
      Int_t lyr = data[0]/100;
      if (blyr != lyr)
	std::cout << std::endl;// << "Diff:";
      std::cout << Form(" %4d", data[0]);
      std::cout << Form(" %6.1f %6.1f %6.1f", 
			(apos1.x()-apos2.x())*1e4,
			(apos1.y()-apos2.y())*1e4,
			(apos1.z()-apos2.z())*1e4);

      Double_t a1, b1, g1; rot1.GetRotAngles(a1, b1, g1);
      Double_t a2, b2, g2; rot2.GetRotAngles(a2, b2, g2);
      std::cout << Form(" %6.1f %6.1f %6.1f", 
			(a1-a2)*60e4, (b1-b2)*60e4, (g1-g2)*60e4)
		<< std::endl;
      blyr = lyr;
    }
  }
  std::cout << std::endl;
}

void tkmerge(const char *fn1,  // Original
	     const char *fn2)  // Updated
{
  if (fn1[0] == '-' && fn2[0] == '-') {
    tkcomp();
    return;
  }

  TkDBcD tkd1(fn1);
  TkDBcD tkd2(fn2);

  Int_t ntk = tkd1.GetEntries();

  for (Int_t i = 0; i < ntk; i++) {
    TkLadder *lad1 = tkd1.GetEntry(i);
    TkLadder *lad2 = tkd2.GetEntry(i);
    if (!lad1 || !lad2 || lad1->GetTkId() != lad2->GetTkId()) continue;

    Int_t tkid  = lad1->GetTkId();
    Int_t layer = lad1->GetLayer();

    if (tkid == 405) {
      lad1->posA = lad2->posA;
      lad1->rotA = lad2->rotA;
    }
/*
    if (2 <= layer && layer <= 7) {
      lad1->posA = lad2->posA;
      lad1->rotA = lad2->rotA;
    }
*/
  }

  tkd1.write("tkdbc.dat.new");
}

void tkadjst(const char *fn1,  // New
	     const char *fn2)  // Org
{
  TkDBcD tkd1(fn1);
  TkDBcD tkd2(fn2);

  Int_t ntk = tkd1.GetEntries();

  for (Int_t i = 0; i < ntk; i++) {
    TkLadder *lad1 = tkd1.GetEntry(i);
    TkLadder *lad2 = tkd2.GetEntry(i);
    if (!lad1 || !lad2 || lad1->GetTkId() != lad2->GetTkId()) continue;

    cout << Form("%4d %8.5f %8.5f %8.5f", lad1->GetTkId(),
		 lad1->pos.x()-lad2->pos.x(),
		 lad1->pos.y()-lad2->pos.y(),
		 lad1->pos.z()-lad2->pos.z()) << endl;

    lad1->posA = lad1->posA+lad2->pos-lad1->pos;
  }

  tkd1.write("tkdbc.dat.new");
}
