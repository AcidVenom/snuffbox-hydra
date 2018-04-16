#include "tools/sparse/sparse_writer.h"
#include "tools/sparse/json_header_parser.h"

#include <iostream>

namespace snuffbox
{
  namespace sparse
  {
    //--------------------------------------------------------------------------
    std::unordered_map<std::string, char> SparseWriter::kFormats_ =
    {
      { "int", 'N' },
      { "float", 'N' },
      { "double", 'N' },
      { "char", 'N' },
      { "bool", 'B' },
      { "String", 'S' },
      { "ScriptObject", 'O' },
      { "ScriptArray", 'A' },
      { "vec", 'V' },
      { "quat", 'Q' },
      { "mat", 'M' }
    };

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

      output_ << "#include \"" << header << "\"" << std::endl;
      output_ << "#include <scripting/script_register.h>" << std::endl << std::endl;

      for (size_t i = 0; i < defs.classes.size(); ++i)
      {
        WriteClass(defs.classes.at(i));
      }

      WriteLine("");

      EnterNamespaces("snuffbox::scripting");
      for (size_t i = 0; i < defs.enums.size(); ++i)
      {
        WriteEnum(defs.enums.at(i));
      }
      ExitNamespaces();
    }

    //--------------------------------------------------------------------------
    void SparseWriter::WriteClass(const ClassDefinition& d)
    {
      EnterNamespaces(d.ns);

      for (size_t i = 0; i < d.functions.size(); ++i)
      {
        WriteFunction(d.functions.at(i), d.c_name);
      }

      WriteFunctionRegister(d);

      ExitNamespaces();
    }

    //--------------------------------------------------------------------------
    void SparseWriter::WriteFunctionRegister(const ClassDefinition& d)
    {
      std::string decl =
        "RegisterScriptFunctions(snuffbox::scripting::ScriptRegister* reg)";

      WriteLine(("void " + d.c_name + "::" + decl).c_str());
      WriteLine("{");
      ++indent_;

      WriteLine("snuffbox::scripting::ScriptFunctionRegister funcs[] =");
      WriteLine("{");

      ++indent_;

      std::string name = "";
      std::string fname = "";

      for (size_t i = 0; i < d.functions.size(); ++i)
      {
        name = d.functions.at(i).name;
        fname = FormatFunctionName(name, d.c_name);

        WriteLine(("{ \"" + name + "\" , " + fname + " },").c_str());
      }

      WriteLine("{ nullptr, nullptr }");
      --indent_;
      WriteLine("};");

      WriteLine("");
      WriteLine(("reg->RegisterFunctions<" + d.c_name + ">(funcs);").c_str());

      --indent_;
      WriteLine("}");
    }

    //--------------------------------------------------------------------------
    void SparseWriter::WriteEnum(const EnumDefinition& d)
    {
      std::string type = d.nested + "::" + d.name;

      WriteLine("template <>");
      WriteLine(
        ("void ScriptClass::RegisterScriptEnum<" + type + 
          ">(ScriptRegister* reg)").c_str()
      );
      WriteLine("{");
      ++indent_;

      WriteLine("ScriptEnumRegister values[] =");
      WriteLine("{");
      ++indent_;

      for (
        std::unordered_map<std::string, int>::const_iterator it =
        d.values.begin();
        it != d.values.end();
        ++it)
      {
        WriteLine(
          ("{ \"" + it->first + "\" , " + std::to_string(it->second) + " },")
          .c_str());
      }

      WriteLine("{ nullptr , -1 }");

      --indent_;

      WriteLine("};");

      WriteLine("");

      WriteLine("ScriptEnum e");
      WriteLine("{");
      ++indent_;
      WriteLine(("\"" + d.name + "\"" + ",").c_str());
      WriteLine("values");
      --indent_;
      WriteLine("};");
      WriteLine("");
      WriteLine("reg->RegisterEnum(e);");
      --indent_;
      WriteLine("}");
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

    //--------------------------------------------------------------------------
    std::string SparseWriter::FormatFunctionName(
      const std::string& func,
      const std::string& cl)
    {
      return "sparse_" + cl + "_" + func;
    }

    //--------------------------------------------------------------------------
    void SparseWriter::WriteArgumentCheck(
      const std::vector<ArgumentDefinition>& args)
    {
      if (args.size() == 0)
      {
        return;
      }

      std::string format = "";
      std::string type = "";

      for (size_t i = 0; i < args.size(); ++i)
      {
        const ArgumentDefinition& a = args.at(i);

        if (a.type.ref_type == RefType::kPointer)
        {
          format += 'O';
          continue;
        }

        type = a.type.name;
        
        for (
          ArgFormats::const_iterator it = kFormats_.begin();
          it != kFormats_.end();
          ++it)
        {
          if (type.find(it->first, 0) != std::string::npos)
          {
            format += it->second;
            break;
          }
        }
      }

      WriteLine(("if (args.Check(\"" + format + "\") == false)").c_str());
      WriteLine("{");
      ++indent_;
      WriteLine("return false;");
      --indent_;
      WriteLine("}");
      WriteLine("");
    }

    //--------------------------------------------------------------------------
    void SparseWriter::WriteSelf(
      const FunctionDefinition& d, 
      const std::string& cl)
    {
      if (d.is_static == true)
      {
        return;
      }

      WriteLine((cl + "* self = args.GetSelf<" + cl + ">();").c_str());
      WriteLine("");
      WriteLine("if (self == nullptr)");
      WriteLine("{");
      ++indent_;
      WriteLine("return false;");
      --indent_;
      WriteLine("}");
      WriteLine("");
    }

    //--------------------------------------------------------------------------
    void SparseWriter::WriteArgumentList(const FunctionDefinition& d)
    {
      std::string get_func = "";
      std::string get = "";
      std::string type = "";
      std::string aidx = "";

      for (size_t i = 0; i < d.arguments.size(); ++i)
      {
        const ArgumentDefinition& a = d.arguments.at(i);

        if (a.type.ref_type == RefType::kPointer)
        {
          get_func = "GetPointer";
        }
        else
        {
          get_func = "Get";
        }

        type = a.type.name;

        get = "args." + get_func + "<" + type + ">(" + std::to_string(i) + ");";

        WriteLine((type + (a.type.ref_type == RefType::kPointer ? "*" : "") +
          " " + a.name + " = " + get).c_str());
      }
    }

    //--------------------------------------------------------------------------
    void SparseWriter::WriteFunctionCall(
      const FunctionDefinition& d, 
      const std::string& cl)
    {
      std::string call = "";

      bool has_return = false;
      if (d.ret_val.name != "void")
      {
        call += d.ret_val.name;

        if (d.ret_val.ref_type == RefType::kPointer)
        {
          call += "*";
        }

        call += " return_value = ";
        has_return = true;
      }

      if (d.is_static == false)
      {
        call += "self->";
      }
      else
      {
        call += cl + "::";
      }

      std::string args = "";

      size_t len = d.arguments.size();

      for (size_t i = 0; i < len; ++i)
      {
        args += d.arguments.at(i).name;

        if (i == len - 1)
        {
          continue;
        }

        args += ',';
      }

      call += d.name + "(" + args + ");";

      WriteLine(call.c_str());

      if (has_return == true)
      {
        call = d.ret_val.ref_type != RefType::kPointer ?
          "AddReturnValue" :
          "AddReturnPointer";

        WriteLine(("args." + call + "(return_value);").c_str());
      }
    }

    //--------------------------------------------------------------------------
    void SparseWriter::WriteFunction(
      const FunctionDefinition& d,
      const std::string& cl)
    {
      std::string func_name = FormatFunctionName(d.name, cl);
      std::string def = 
        ("bool " + func_name + "(snuffbox::scripting::ScriptArgs& args)");

      if (d.is_custom == true)
      {
        def += ";";
        WriteLine("// Provide a custom implementation for this function");
      }

      WriteLine(def.c_str());

      if (d.is_custom == true)
      {
        WriteLine("");
        return;
      }

      WriteLine("{");
      ++indent_;

      WriteArgumentCheck(d.arguments);
      WriteSelf(d, cl);
      WriteArgumentList(d);

      if (d.arguments.size() > 0)
      {
        WriteLine("");
      }

      WriteFunctionCall(d, cl);

      WriteLine("");
      WriteLine("return true;");

      --indent_;
      WriteLine("}");
      WriteLine("");
    }
  }
}
