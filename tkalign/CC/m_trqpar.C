// $Id: m_trqpar.C,v 1.1 2010/12/07 00:20:22 shaino Exp $
#include "trqpar.C"
#include <cstdlib>

int main(int argc, char *argv[])
{
  if (argc < 2) {
    cout << "trqpar [fname] [oname=trqpar] [fit=17] [nproc=0]" << endl;
    cout << "fit=  1: kChoutko" << endl;
    cout << "fit=  4: kAlcaraz" << endl;
    cout << "fit= 17: kChoutko | kMultScat" << endl;
    cout << "fit= 20: kAlcaraz | kMultScat" << endl;
    exit(1);
  }

  if (argc == 2) trqpar(argv[1]);
  if (argc == 3) trqpar(argv[1], argv[2]);
  if (argc == 4) trqpar(argv[1], argv[2], atoi(argv[3]));
  if (argc == 5) trqpar(argv[1], argv[2], atoi(argv[3]),
			                  atoi(argv[4]));
  exit(0);
  return 0;
}
