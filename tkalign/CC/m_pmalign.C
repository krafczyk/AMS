// $Id: m_pmalign.C,v 1.1 2010/10/14 09:28:04 shaino Exp $
#include "pmalign.C"
#include <cstdlib>

int main(int argc, char *argv[])
{
  if (argc < 3) {
    std::cout << "tbalign [fname] [oname] [tkdbc=0] [nproc=0]"
	      << std::endl;
    exit(1);
  }

  if (argc == 3) pmalign(argv[1], argv[2]);
  if (argc == 4) pmalign(argv[1], argv[2], argv[3]);
  if (argc == 5) pmalign(argv[1], argv[2], argv[3], atoi(argv[4]));

  exit(0);
  return 0;
}
