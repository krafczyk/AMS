// $Id: m_tbafm.C,v 1.1 2010/10/14 09:28:04 shaino Exp $
#include "tbafit.C"
#include <cstdlib>

int main(int argc, char *argv[])
{
  if (argc < 2) {
    cout << "tbafm [fname=tbafit.dat] [rng=0.01]" << endl;
    exit(1);
  }
  if (argc == 2) tbafit(-1, "tkdbc.dat", 0.01,          argv[1]);
  if (argc == 3) tbafit(-1, "tkdbc.dat", atof(argv[2]), argv[1]);
  cout << "Done" << endl;

  exit(0);
  return 0;
}
