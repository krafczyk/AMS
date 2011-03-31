// $Id: m_tbaf.C,v 1.2 2011/03/31 10:10:03 haino Exp $
#include "tbafit.C"
#include <cstdlib>

int main(int argc, char *argv[])
{
  if (argc < 3) {
    cout << "tbaf [bits] [tkdbc] "
	 << "[rng=0.01] [fname=tbafit.dat] [selpos=-1]" << endl;
    cout << "bits= 1: kXpFit" << endl;
    cout << "bits= 2: kYpFit" << endl;
    cout << "bits= 4: kZyFit" << endl;
    cout << "bits= 8: kYtFit" << endl;
    cout << "bits=16: kSensY" << endl;
    cout << "bits=32: kZtFit" << endl;
    exit(1);
  }

  if (argc == 3) tbafit(atoi(argv[1]), argv[2]);
  if (argc == 4) tbafit(atoi(argv[1]), argv[2], atof(argv[3]));
  if (argc == 5) tbafit(atoi(argv[1]), argv[2], atof(argv[3]), argv[4]);
  if (argc == 6) tbafit(atoi(argv[1]), argv[2], atof(argv[3]), argv[4],
			                        atoi(argv[5]));
  cout << "Done" << endl;

  exit(0);
  return 0;
}
