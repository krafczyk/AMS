#include "tkmerge.C"
#include <cstdlib>

int main(int argc, char *argv[])
{
  if (argc < 3) {
    cout << "tkmerge [tkdbc1] [tkdbc2]" << endl;
    exit(1);
  }

  if (argc == 4)
    tkadjst(argv[1], argv[2]);
  else
    tkmerge(argv[1], argv[2]);

  exit(0);
  return 0;
}
