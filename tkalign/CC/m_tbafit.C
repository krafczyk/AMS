// $Id: m_tbafit.C,v 1.1 2010/10/14 09:28:04 shaino Exp $
#include "tbafit.C"
#include <cstdlib>

int main(int argc, char *argv[])
{
  if (argc < 4) {
    cout << "tbafit [fname] [tkdbc] [bits] [nmax=5000]" << endl;
    cout << "bits= 1: kXpFit" << endl;
    cout << "bits= 2: kYpFit" << endl;
    cout << "bits= 4: kZyFit" << endl;
    cout << "bits= 8: kYtFit" << endl;
    cout << "bits=16: kSensY" << endl;
    exit(1);
  }

  TString sfn = argv[1];
  if (!sfn.Contains(".root")) sfn += "*.root";

  if (argc == 4) tbafit(sfn, argv[2], atoi(argv[3]));
  if (argc >= 5) tbafit(sfn, argv[2], atoi(argv[3]), atoi(argv[4]));
  exit(0);
  return 0;
}
