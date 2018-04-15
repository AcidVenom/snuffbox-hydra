#include "tools/sparse/sparse_writer.h"
#include "tools/sparse/json_header_parser.h"

#include <iostream>

namespace snuffbox
{
  namespace sparse
  {
    //--------------------------------------------------------------------------
    SparseWriter::SparseWriter() :
      indent_(0),
      namespaces_(0)
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
    void SparseWriter::WriteComment(const ScriptDefinitions& defs)
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
      output_ << "* - Number of classes: " << defs.classes.size() << std::endl;
      output_ << "* - Number of enums: " << defs.enums.size() << std::endl;
      Splitter();
      WriteLine("* Class information: ");

      for (size_t i = 0; i < defs.classes.size(); ++i)
      {
        const ClassDefinition& d = defs.classes.at(i);

        output_ <<
          "* -> " << "[C++: " << d.c_name << ", ScriptName: " << d.s_name << "]"
          << std::endl <<
          "* -- Functions: " << d.functions.size() << std::endl;
      }

      Splitter();
      WriteLine("* Enum information: ");

      for (size_t i = 0; i < defs.enums.size(); ++i)
      {
        const EnumDefinition& d = defs.enums.at(i);

        output_ <<
          "* -> " << "[" << d.name << "]"
          << std::endl <<
          "* -- Key/value pairs: " << d.values.size() << std::endl;
      }

      WriteLine("*/\n");
    }

    //--------------------------------------------------------------------------
    void SparseWriter::WriteAll(
      const std::string& header,
      const ScriptDefinitions& defs)
    {
      WriteComment(defs);

      output_ << "#include \"" << header << "\"" << std::endl << std::endl;

      for (size_t i = 0; i < defs.classes.size(); ++i)
      {
        WriteClass(defs.classes.at(i));
      }
    }

    //--------------------------------------------------------------------------
    void SparseWriter::WriteClass(const ClassDefinition& d)
    {
      EnterNamespaces(d.ns);

      ExitNamespaces();
    }

    //--------------------------------------------------------------------------
    void SparseWriter::EnterNamespaces(const std::string& ns)
    {
      std::string current = "";
      char c = '\0';

      size_t len = ns.size();

      for (size_t i = 0; i < len; ++i)
      {
        c = ns.at(i);

        if (c == ':' || i == len - 1)
        {
          ++i;

          if (c != ':')
          {
            current += c;
          }

          WriteLine(("namespace " + current).c_str());
          WriteLine("{");

          ++indent_;
          ++namespaces_;

          current = "";

          continue;
        }

        current += c;
      }
    }

    //--------------------------------------------------------------------------
    void SparseWriter::ExitNamespaces()
    {
      for (unsigned int i = 0; i < namespaces_; ++i)
      {
        --indent_;
        WriteLine("}");
      }

      namespaces_ = 0;
    }
  }
}