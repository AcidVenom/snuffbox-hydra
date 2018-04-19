#include "tools/bin2h/binary_writer.h"

#include <iostream>

using namespace snuffbox;
using namespace bin2h;

int main(int argc, char** argv)
{
  if (argc < 3)
  {
    std::cerr << "Usage: snuffbox-bin2h input_file output_folder" << std::endl;
    return 1;
  }

  const char* file_path = argv[1];
  const char* output_dir = argv[2];

  if (BinaryWriter::WriteFile(file_path, output_dir) == false)
  {
    return 2;
  }

  return 0;
}