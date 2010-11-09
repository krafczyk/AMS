#include "pmrinv.C"
#include <cstdlib>

int main(int argc, char *argv[])
{
  if (argc < 3) {
    cout << "pmaf [fname] [tkdbc]" << endl;
    exit(1);
  }

  pmrinv(argv[1], argv[2]);
  cout << "Done" << endl;

  exit(0);
  return 0;
}
