#include "tools/sparse/parser.h"

#include <fstream>
#include <iostream>

namespace snuffbox
{
  namespace sparse
  {
    //--------------------------------------------------------------------------
    bool Parser::Parse(const std::string& input, const std::string& output)
    {
      rapidjson::Document d;

      std::ifstream fin = std::ifstream(input.c_str(), 
        std::fstream::in | std::ios::ate | std::ios::binary);

      if (fin)
      {
        size_t size = fin.tellg();
        fin.seekg(0);

        char* buffer = new char[size + 1];
        fin.read(buffer, size);

        memset(buffer + size, '\0', sizeof(char));

        rapidjson::ParseResult res = d.Parse(buffer);

        if (res.IsError() == true)
        {
          std::cout << 
            "Could not parse .json file, the contents might be invalid" <<
            std::endl << 
            "This happens when the input header contains invalid definitions" <<
            std::endl << "Errors:" << std::endl <<
            buffer << std::endl;
        }

        bool success = ParseDocument(d);

        delete[] buffer;
        fin.close();

        if (success == false)
        {
          std::cout << "Could not parse file: " << input;
        }

        return success;
      }
      
      std::cout << "Could not open file: " << input << std::endl;
      return false;
    }

    //--------------------------------------------------------------------------
    bool Parser::ParseDocument(const rapidjson::Document& doc)
    {
      return true;
    }
  }
}