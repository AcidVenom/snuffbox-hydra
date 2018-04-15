#include "tools/sparse/json_header_parser.h"
#include "tools/sparse/sparse_writer.h"

#include <string>
#include <iostream>

using namespace snuffbox;
using namespace sparse;

int main(int argc, char** argv)
{
  std::string input = "";
  std::string output = "";
  std::string header = "";

  auto GetArgument = [argc, argv](const char* id)
  {
    for (int i = 0; i < argc; ++i)
    {
      if (strcmp(argv[i], id) == 0)
      {
        return i + 1 < argc ? argv[i + 1] : "";
      }
    }

    return "";
  };

  input = GetArgument("-i");
  output = GetArgument("-o");
  header = GetArgument("-h");

  JsonHeaderParser p;
  int result = p.Parse(input) == false ? 1 : 0;

  if (result == 0)
  {
    SparseWriter w;
    w.Write(&p, header, output);
    std::cout << "sparse -> " << output << std::endl;
  }

  return result;
}