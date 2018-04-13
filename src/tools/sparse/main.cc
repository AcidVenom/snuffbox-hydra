#include "tools/sparse/parser.h"

#include <string>

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
  if (p.Parse(input, output) == false)
  {
    return 1;
  }

  return 0;
}