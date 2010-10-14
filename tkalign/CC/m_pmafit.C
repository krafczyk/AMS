// $Id: m_pmafit.C,v 1.1 2010/10/14 09:28:04 shaino Exp $
#include "pmafit.C"
#include <cstdlib>

int main(int argc, char *argv[])
{
  if (argc < 3) {
    cout << "pmafit [fname] [tkdbc] [bits=-1]" << endl;
    cout << "bits= 1: kXpFit" << endl;
    cout << "bits= 2: kYpFit" << endl;
    cout << "bits= 4: kZyFit" << endl;
    cout << "bits= 8: kYtFit" << endl;
    cout << "bits=16: kSensY" << endl;
    exit(1);
  }

  TString sfn = argv[1];
  if (!sfn.Contains(".root")) sfn += "*.root";

  if (argc == 3) pmafit(sfn, argv[2]);
  if (argc == 4) pmafit(sfn, argv[2], atoi(argv[3]));
  exit(0);
  return 0;
}
