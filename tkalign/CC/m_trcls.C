#include "trcls.C"
#include <cstdlib>

int main(int argc, char *argv[])
{
  if (argc < 4) {
    cout << "trcls [fname] [oname] [tkdbc]" << endl;
    exit(1);
  }

  trcls(argv[1], argv[2], argv[3]);
  cout << "Done" << endl;
  exit(0);
  return 0;
}
