#include "refit.C"
#include <cstdlib>

int main(int argc, char *argv[])
{
  if (argc < 7) {
    cout << "refit [fname] [oname] [tkdbc] [toffs] [tstep] [norb]" << endl;
    exit(1);
  }

  refit(argv[1], argv[2], argv[3],
	atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
  cout << "Done" << endl;
  exit(0);
  return 0;
}
