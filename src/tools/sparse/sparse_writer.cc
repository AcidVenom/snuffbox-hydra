#include "tools/sparse/sparse_writer.h"
#include "tools/sparse/json_header_parser.h"

#include <iostream>

namespace snuffbox
{
  namespace sparse
  {
    //--------------------------------------------------------------------------
    SparseWriter::SparseWriter()
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

      bool success;

      if ((success = WriteAll(header, parser->definitions())) == false)
      {
        std::cerr << "Could not write destination file: " 
          << output << std::endl;
      }
      
      output_.flush();
      output_.close();

      return success;
    }

    //--------------------------------------------------------------------------
    void SparseWriter::WriteComment(const std::vector<ClassDefinition>& defs)
    {
      output_
        << "/**" << std::endl
        << "* == DO NOT MODIFY FILE CONTENTS ==" << std::endl
        << "* This file was generated using Snuffbox's 'sparse'" << std::endl
        << "* The contents of this file should not be modified as" << std::endl
        << "* changes are discarded during the next compilation" << std::endl
        << "* ---------------------------------------------------" << std::endl
        << "* Meta data:" << std::endl
        << "* - Number of classes: " << defs.size() << std::endl
        << "* ---------------------------------------------------" << std::endl
        << "* Class information: " << std::endl;

      for (size_t i = 0; i < defs.size(); ++i)
      {
        const ClassDefinition& d = defs.at(i);

        output_ <<
          "* -> " << "[C++: " << d.c_name << ", ScriptName: " << d.s_name << "]"
          << std::endl <<
          "* -- Functions: " << d.functions.size() << std::endl;
      }

      output_ << "*/" << std::endl << std::endl;
    }

    //--------------------------------------------------------------------------
    bool SparseWriter::WriteAll(
      const std::string& header,
      const std::vector<ClassDefinition>& defs)
    {
      WriteComment(defs);

      output_ << "#include \"" << header << "\"" << std::endl << std::endl;

      return true;
    }
  }
}