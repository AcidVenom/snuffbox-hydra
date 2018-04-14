#include "tools/sparse/json_header_parser.h"

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

  snuffbox::sparse::JsonHeaderParser p;
  int result = p.Parse(input, output) == false ? 1 : 0;

  if (result == 0)
  {
    std::cout << "sparse -> " << output << std::endl;
  }

  return result;
}