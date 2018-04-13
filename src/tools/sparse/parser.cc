#include "tools/sparse/parser.h"

#include <fstream>
#include <iostream>

namespace snuffbox
{
  namespace sparse
  {
    //--------------------------------------------------------------------------
    Parser::Parser() :
      current_namespace_(""),
      num_namespaces_(0),
      in_class_(false)
    {

    }

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
          std::cerr << 
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
          std::cerr << "Could not parse file: " << input;
        }

        return success;
      }
      
      std::cerr << "Could not open file: " << input << std::endl;
      return false;
    }

    //--------------------------------------------------------------------------
    Parser::ClassDefinition::ClassDefinition() :
      c_name(nullptr),
      s_name(nullptr)
    {

    }

    //--------------------------------------------------------------------------
    bool Parser::ParseDocument(const rapidjson::Document& doc)
    {
      if (doc.IsArray() == false)
      {
        return false;
      }

      rapidjson::GenericArray<true, rapidjson::Value> arr = doc.GetArray();
      bool success = ParseArray(arr);

      for (size_t i = 0; i < definitions_.size(); ++i)
      {
        const ClassDefinition& cl = definitions_.at(i);

        std::cout << "Parsed definition for " <<
          "[Class: " << cl.c_name << ", " 
          "ScriptName: " << cl.s_name << "].." 
          << std::endl;
      }

      return success;
    }

    //--------------------------------------------------------------------------
    bool Parser::ParseArray(
      const rapidjson::GenericArray<true, rapidjson::Value>& arr)
    {
      const char* type = nullptr;
      bool found_name = in_class_ == false;

      for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
      {
        const rapidjson::Value& v = arr[i];

        if ((type = GetType(v)) != nullptr)
        {
          if (strcmp(type, "namespace") == 0)
          {
            if (EnterNamespaces(v) == false)
            {
              std::cerr << "Attempted to enter a namespace but the namespace" <<
                " was invalid" << std::endl;

              return false;
            }
          }
          else if (strcmp(type, "class") == 0)
          {
            if (ParseClass(v) == false)
            {
              std::cerr << "Attempted to parse an invalid class" << std::endl;
              return false;
            }
          }
          else if (strcmp(type, "macro") == 0)
          {
            found_name = true;

            const char* s_name = GetScriptName(v);

            if (s_name == nullptr)
            {
              std::cerr << "Attempted to parse class '" <<
                current_.c_name << "' with an invalid SCRIPT_NAME"
                << std::endl;

              return false;
            }

            current_.s_name = s_name;
          }
        }
      }

      if (found_name == false)
      {
        std::cerr << "Attempted to parse class '" <<
          current_.c_name << "' without a SCRIPT_NAME"
          << std::endl;

        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    const char* Parser::GetType(const rapidjson::Value& val)
    {
      if (val.IsObject() == false)
      {
        return nullptr;
      }

      if (val.HasMember("type") == true)
      {
        const rapidjson::Value& type = val["type"];

        if (type.IsString() == false)
        {
          return nullptr;
        }

        return type.GetString();
      }

      return nullptr;
    }

    //--------------------------------------------------------------------------
    bool Parser::EnterNamespaces(const rapidjson::Value& ns)
    {
      const char* name = ns["name"].GetString();

      if (current_namespace_ == "")
      {
        current_namespace_ += name;
      }
      else
      {
        current_namespace_ += std::string("::") + name;
      }

      ++num_namespaces_;

      const rapidjson::Value& m = ns["members"];

      if (m.IsArray() == false)
      {
        std::cerr << "'members' field of a namespace was not an array type" <<
          std::endl;

        return false;
      }

      const rapidjson::GenericArray<true, rapidjson::Value> arr = m.GetArray();
      return ParseArray(arr);
    }

    //--------------------------------------------------------------------------
    bool Parser::ParseClass(const rapidjson::Value& cl)
    {
      in_class_ = true;
      current_ = ClassDefinition();

      if (cl.HasMember("name") == false)
      {
        std::cerr << "Attempted to parse a class without a name" << std::endl;
        return false;
      }

      const char* c_name = cl["name"].GetString();

      if (DerivesFrom("ScriptClass", cl) == false)
      {
        std::cerr << "Could not parse class '" 
          << c_name 
          << "', it does not derive from snuffbox::scripting::ScriptClass"
          << std::endl;

        return false;
      }

      if (
        cl.HasMember("members") == false ||
        cl["members"].IsArray() == false)
      {
        std::cerr << "Could not parse class '"
          << c_name
          << "', it does not have a 'members' field"
          << std::endl;

        return false;
      }

      const rapidjson::GenericArray<true, rapidjson::Value> members = 
        cl["members"].GetArray();

      current_.c_name = c_name;

      bool success = ParseArray(members);

      if (success == true)
      {
        definitions_.push_back(current_);
      }

      in_class_ = false;
      return success;
    }

    //--------------------------------------------------------------------------
    bool Parser::DerivesFrom(const char* name, const rapidjson::Value& cl)
    {
      if (cl.HasMember("parents") == true)
      {
        const rapidjson::Value& parents = cl["parents"];

        if (parents.IsArray() == true)
        {
          const rapidjson::GenericArray<true, rapidjson::Value>& arr =
            parents.GetArray();

          for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
          {
            if (arr[i].IsObject() == true)
            {
              if (
                arr[i].HasMember("name") == true && 
                arr[i]["name"].IsObject() == true &&
                arr[i]["name"].HasMember("name") == true)
              {
                const rapidjson::Value& inner_name = arr[i]["name"]["name"];

                if (inner_name.IsString() == true)
                {
                  std::string find_in = inner_name.GetString();

                  if (find_in.find("ScriptClass", 0) != std::string::npos)
                  {
                    return true;
                  }
                }
              }
            }
          }
        }
        else
        {
          std::cerr << "'parents' is not of an array type" << std::endl;

          return false;
        }
      }

      return false;
    }

    //--------------------------------------------------------------------------
    const char* Parser::GetScriptName(const rapidjson::Value& m)
    {
      if (
        m.HasMember("meta") == false ||
        m["meta"].IsObject() == false)
      {
        return nullptr;
      }

      const rapidjson::GenericObject<true, rapidjson::Value>& meta = 
        m["meta"].GetObject();

      for (
        rapidjson::GenericObject<true, rapidjson::Value>::MemberIterator it =
        meta.MemberBegin();
        it != meta.MemberEnd();
        ++it)
      {
        return it->name.IsString() == false ? nullptr : it->name.GetString();
      }

      return nullptr;
    }
  }
}