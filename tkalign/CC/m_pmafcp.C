// $Id: m_pmafcp.C,v 1.1 2010/10/14 09:28:04 shaino Exp $
#include "pmafit.C"
#include <cstdlib>

int main(int argc, char *argv[])
{
  if (argc < 3) {
    cout << "pmafcp [bits] [tkdbc] [rng=0.01] [fname=pmafit.root]" << endl;
    exit(1);
  }

  if (argc == 3) copyproc(atoi(argv[1]), argv[2]);
  if (argc == 4) copyproc(atoi(argv[1]), argv[2], atof(argv[3]));
  if (argc == 5) copyproc(atoi(argv[1]), argv[2], atof(argv[3]), argv[4]);
  exit(0);
  return 0;
}
