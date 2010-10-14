// $Id: m_tbalign.C,v 1.1 2010/10/14 09:28:04 shaino Exp $
#include "tbalign.C"
#include <cstdlib>

int main(int argc, char *argv[])
{
  if (argc < 7) {
    std::cout << "tbalign [fname] [oname] [tkdbc] [posid] [ipos] [fpos] "
                                         "[psel=1105] [nproc=0] [ievt=0]"
	      << std::endl;
    exit(1);
  }

  if (argc == 7)
    tbalign(argv[1], argv[2], argv[3], atoi(argv[4]), 
	                               atoi(argv[5]), atoi(argv[6]));
  if (argc == 8) 
    tbalign(argv[1], argv[2], argv[3], atoi(argv[4]), atoi(argv[5]),
	                               atoi(argv[6]), atoi(argv[7]));
  if (argc == 9) 
    tbalign(argv[1], argv[2], argv[3], atoi(argv[4]), atoi(argv[5]),
	                               atoi(argv[6]), atoi(argv[7]),
	                                              atoi(argv[8]));
  if (argc == 10) 
    tbalign(argv[1], argv[2], argv[3], atoi(argv[4]), atoi(argv[5]),
	                               atoi(argv[6]), atoi(argv[7]),
	                               atoi(argv[8]), atoi(argv[9]));
  exit(0);
}
