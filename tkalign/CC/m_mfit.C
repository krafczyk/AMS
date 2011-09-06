#include "mfit.C"
#include <cstdlib>

int main(int argc, char *argv[])
{
  if (argc < 8) {
    cout << "mfit [layer] [fname] [oname] [extalign.root] "
                 "[mode] [dt] [range]" << endl;
    exit(1);
  }

  mfit(atoi(argv[1]), argv[2], argv[3], argv[4], atoi(argv[5]),
       atof(argv[6]), atof(argv[7]));
  cout << "Done" << endl;
  exit(0);
  return 0;
}
