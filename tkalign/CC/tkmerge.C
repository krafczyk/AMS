
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

    AMSPoint  pos1, pos2;
    AMSRotMat rot1, rot2;

    std::cin >> pos1 >> pos2;
    if ((pos1-pos2).norm() > 0) {
      std::cerr << "Wrong format[3]: " << pos1 << " != " << std::endl
		                       << pos2 << std::endl;
      return;
    }
    std::cin >> rot1 >> rot2;

    std::cin >> pos1 >> pos2;
    std::cin >> rot1 >> rot2;

    Int_t data[6];
    for (Int_t j = 0; j < 6; j++) std::cin >> data[j];
    for (Int_t j = 0; j < 3; j++) {
      if (data[j] != data[j+3]) {
	std::cerr << "Wrong format[3]: " << j << " "
		  << data[j] << " != " << data[j+3] << std::endl;
	return;
      }
    }

    if ((pos1-pos2).norm() > 0) {
      Int_t lyr = data[0]/100;
      if (blyr != lyr)
	std::cout << std::endl << "Diff:";
      std::cout << Form(" %4d", data[0]);
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

    if (2 <= layer && layer <= 7) {
      lad1->posA = lad2->posA;
      lad1->rotA = lad2->rotA;
    }
  }

  tkd1.write("tkdbc.dat.new");
}
