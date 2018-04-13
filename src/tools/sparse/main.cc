#include "tools/sparse/parser.h"

#include <string>
#include <iostream>

int main(int argc, char** argv)
{
  std::string input = "";
  std::string output = "";

  for (int i = 0; i < argc; ++i)
  {
    if (strcmp(argv[i], "-i") == 0)
    {
      input = i + 1 < argc ? argv[i + 1] : "";
    }
    else if (strcmp(argv[i], "-o") == 0)
    {
      output = i + 1 < argc ? argv[i + 1] : "";
    }
  }

  snuffbox::sparse::Parser p;
  int result = p.Parse(input, output) == false ? 1 : 0;

  std::cout << "sparse -> " << output << std::endl;
  return result;
}