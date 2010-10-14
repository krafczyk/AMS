// $Id: m_tbafcp.C,v 1.1 2010/10/14 09:28:04 shaino Exp $
#include "tbafit.C"
#include <cstdlib>

int main(int argc, char *argv[])
{
  if (argc < 3) {
    cout << "tbafcp [bits] [tkdbc] [rng=0.01] [fname=tbafit.root]" << endl;
    exit(1);
  }

  if (argc == 3) copyproc(atoi(argv[1]), argv[2]);
  if (argc == 4) copyproc(atoi(argv[1]), argv[2], atof(argv[3]));
  if (argc == 5) copyproc(atoi(argv[1]), argv[2], atof(argv[3]), argv[4]);
  exit(0);
  return 0;
}
