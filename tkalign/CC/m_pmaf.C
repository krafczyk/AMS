// $Id: m_pmaf.C,v 1.2 2010/11/09 08:38:54 shaino Exp $
#include "pmafit.C"
#include <cstdlib>

int main(int argc, char *argv[])
{
  if (argc < 3) {
    cout << "pmaf [bits] [tkdbc] [rng=0.01] [fname=pmafit.dat]" << endl;
    cout << "bits= 1: kXpFit" << endl;
    cout << "bits= 2: kYpFit" << endl;
    cout << "bits= 4: kZyFit" << endl;
    cout << "bits= 8: kYtFit" << endl;
    cout << "bits=16: kSensY" << endl;
    exit(1);
  }

  if (argc == 3) pmafit(atoixb(argv[1]), argv[2]);
  if (argc == 4) pmafit(atoixb(argv[1]), argv[2], atof(argv[3]));
  if (argc == 5) pmafit(atoixb(argv[1]), argv[2], atof(argv[3]), argv[4]);
  cout << "Done" << endl;

  exit(0);
  return 0;
}
