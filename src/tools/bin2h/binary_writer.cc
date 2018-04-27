#include "tools/bin2h/binary_writer.h"

#include <iostream>
#include <cstring>

namespace snuffbox
{
  namespace bin2h
  {
    //--------------------------------------------------------------------------
    bool BinaryWriter::WriteFile(const char* file, const char* relative, const char* out_dir)
    {
      std::string file_path = file;

      std::fstream fin = std::fstream(
        file_path, 
        std::ios::in | std::ios::ate | std::ios::binary);

      if (fin.is_open() == false)
      {
        std::cerr << "Could not open file: " << file << std::endl;
        return false;
      }

      size_t len = fin.tellg();
      uint8_t* buffer = new uint8_t[len];

      fin.seekg(std::ios_base::beg);
      fin.read(reinterpret_cast<char*>(buffer), len);

      fin.close();

      std::string name = GetFileName(file);
      std::string out_path = out_dir;
      out_path += '/' + name + ".h";

      std::fstream fout = std::fstream(
        out_path,
        std::ios::out | std::ios::binary | std::ios::trunc
      );

      if (fout.is_open() == false)
      {
        std::cerr << "Could not create file: " << out_path << std::endl;
        delete[] buffer;
        return false;
      }

      WriteDefinitions(buffer, len, name, relative, fout);

      fout.close();

      delete[] buffer;

      return true;
    }

    //--------------------------------------------------------------------------
    std::string BinaryWriter::GetFileName(const char* path)
    {
      size_t len = strlen(path);

      if (len == 0)
      {
        return "";
      }

      size_t current = len - 1;

      char c = path[current];
      bool read_extension = false;

      std::string res = "";

      while (c != '/' && c != '\\')
      {
        if (read_extension == true)
        {
          res += c;
        }

        if (read_extension == false && c == '.')
        {
          read_extension = true;
        }

        if (current == 0)
        {
          break;
        }

        c = path[--current];
      }

      std::string reverse = "";

      for (int i = static_cast<int>(res.size()) - 1; i >= 0; --i)
      {
        reverse += res.at(i);
      }

      return reverse;
    }

    //--------------------------------------------------------------------------
    std::string BinaryWriter::GetVariableName(const std::string& file)
    {
      std::string copy = file;

      auto IsIllegalCharacter = [](char ch)
      {
        static const char* illegal = "$%!@#^&()-={}[],.";
        static const size_t len = strlen(illegal);

        for (size_t i = 0; i < len; ++i)
        {
          if (ch == illegal[i])
          {
            return true;
          }
        }

        return false;
      };

      char c;
      for (int i = static_cast<int>(copy.size()) - 1; i >= 0; --i)
      {
        c = copy.at(i);

        if (c == ' ')
        {
          copy.at(i) = '_';
          continue;
        }

        if (IsIllegalCharacter(c) == true)
        {
          copy.erase(copy.begin() + i);
        }
      }

      return copy;
    }

    //--------------------------------------------------------------------------
    void BinaryWriter::WriteDefinitions(
      uint8_t* buffer,
      size_t len,
      const std::string& name,
      const char* relative,
      std::fstream& fin)
    {
      std::string var_name = "bin2h_" + GetVariableName(name);

      fin << "#pragma once" << std::endl << std::endl;

      fin << "#include <cinttypes>" << std::endl;
      fin << "#include <cstddef>" << std::endl << std::endl;

      fin << "static const uint8_t " << var_name << "[] = { ";

      std::ios_base::fmtflags flags(fin.flags());

      for (size_t i = 0; i < len; ++i)
      {
        fin << "0x" << std::hex << static_cast<int>(buffer[i]);
        fin.flags(flags);
        
        if (i < len - 1)
        {
          fin << ", ";
          continue;
        }

        fin << " };" << std::endl << std::endl;
      }

      fin << "static const size_t " << var_name << "_size = " << len << ";" 
        << std::endl;
      fin << "static const char* " << var_name << "_path = \"" << relative <<
        "\";" << std::endl;
    }
  }
}
