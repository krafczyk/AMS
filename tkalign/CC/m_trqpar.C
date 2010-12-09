// $Id: m_trqpar.C,v 1.3 2010/12/09 00:55:46 shaino Exp $
#include "trqpar.C"
#include <cstdlib>

int main(int argc, char *argv[])
{
  if (argc < 2) {
    cout << "trqpar [fname] [oname=trqpar.root] [fit=17] [sname=0] [nproc=0]"
	 << endl;
    cout << "fit=  1: kChoutko" << endl;
    cout << "fit=  4: kAlcaraz" << endl;
    cout << "fit= 17: kChoutko | kMultScat" << endl;
    cout << "fit= 20: kAlcaraz | kMultScat" << endl;
    exit(1);
  }

  if (argc == 2) trqpar(argv[1]);
  if (argc == 3) trqpar(argv[1], argv[2]);
  if (argc == 4) trqpar(argv[1], argv[2], atoi(argv[3]));
  if (argc == 5) trqpar(argv[1], argv[2], atoi(argv[3]), argv[4]);
  if (argc == 6) trqpar(argv[1], argv[2], atoi(argv[3]), argv[4], 
			                  atoi(argv[5]));
  exit(0);
  return 0;
}
