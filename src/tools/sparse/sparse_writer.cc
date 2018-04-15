#include "tools/sparse/sparse_writer.h"
#include "tools/sparse/json_header_parser.h"

#include <iostream>

namespace snuffbox
{
  namespace sparse
  {
    //--------------------------------------------------------------------------
    SparseWriter::SparseWriter() :
      indent_(0)
    {

    }

    //--------------------------------------------------------------------------
    bool SparseWriter::Write(
      JsonHeaderParser* parser, 
      const std::string& header,
      const std::string& output)
    {
      if (parser == nullptr || parser->HasDocument() == false)
      {
        return false;
      }

      output_ = std::ofstream(
        output, 
        std::fstream::out | std::ios::binary | std::fstream::trunc);

      if (output_.is_open() == false)
      {
        std::cerr << "Could not open destination file: " 
          << output << std::endl;
        return false;
      }

      WriteAll(header, parser->definitions());
      
      output_.flush();
      output_.close();

      return true;
    }

    //--------------------------------------------------------------------------
    void SparseWriter::WriteLine(const char* line)
    {
      for (unsigned int i = 0; i < indent_; ++i)
      {
        output_ << '\t';
      }

      output_ << line << std::endl;
    }

    //--------------------------------------------------------------------------
    void SparseWriter::WriteComment(const std::vector<ClassDefinition>& defs)
    {
      auto Splitter = [=]()
      {
        WriteLine("* ---------------------------------------------------");
      };

      WriteLine("/**");
      WriteLine("* == DO NOT MODIFY FILE CONTENTS ==");
      WriteLine("* This file was generated using Snuffbox's 'sparse'");
      WriteLine("* The contents of this file should not be modified as");
      WriteLine("* changes are discarded during the next compilation");
      Splitter();
      WriteLine("* Meta data:");
      output_ << "* - Number of classes: " << defs.size() << std::endl;
      Splitter();
      WriteLine("* Class information: ");

      for (size_t i = 0; i < defs.size(); ++i)
      {
        const ClassDefinition& d = defs.at(i);

        output_ <<
          "* -> " << "[C++: " << d.c_name << ", ScriptName: " << d.s_name << "]"
          << std::endl <<
          "* -- Functions: " << d.functions.size() << std::endl;
      }

     WriteLine("*/");
    }

    //--------------------------------------------------------------------------
    void SparseWriter::WriteAll(
      const std::string& header,
      const std::vector<ClassDefinition>& defs)
    {
      WriteComment(defs);

      output_ << "#include \"" << header << "\"" << std::endl << std::endl;

      for (size_t i = 0; i < defs.size(); ++i)
      {
        WriteClass(defs.at(i));
      }
    }

    //--------------------------------------------------------------------------
    void SparseWriter::WriteClass(const ClassDefinition& d)
    {
      
    }
  }
}