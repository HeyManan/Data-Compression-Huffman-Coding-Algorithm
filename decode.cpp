#include "huffman.hpp"

using namespace std;

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    cout << "Failed to detect Files";
    exit(1);
  }

  Huffman h(argv[1], argv[2]);
  h.decompress();
  cout << "Decompressed Successfully" << endl;

  return 0;
}