#include "tools/sparse/json_header_parser.h"

#include <fstream>
#include <iostream>

namespace snuffbox
{
  namespace sparse
  {
    //--------------------------------------------------------------------------
    JsonHeaderParser::JsonHeaderParser()
    {

    }

    //--------------------------------------------------------------------------
    bool JsonHeaderParser::Parse(
      const std::string& input, 
      const std::string& output)
    {
      definitions_.clear();

      rapidjson::Document d;

      std::ifstream fin = std::ifstream(input.c_str(), 
        std::fstream::in | std::ios::ate | std::ios::binary);

      if (fin.is_open() == true)
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
    void JsonHeaderParser::ForEach(
      const RapidArray& arr, 
      const std::function<RapidForEach>& del)
    {
      assert(del != nullptr);

      for (RapidIdx i = 0; i < arr.Size(); ++i)
      {
        const rapidjson::Value& v = arr[i];
        if (del(v, i) == false)
        {
          break;
        }
      }
    }

    //--------------------------------------------------------------------------
    bool JsonHeaderParser::ParseDocument(const rapidjson::Document& doc)
    {
      if (doc.IsArray() == false)
      {
        return false;
      }

      RapidArray arr = doc.GetArray();
      bool success = ParseRoot(arr);

      return success;
    }

    //--------------------------------------------------------------------------
    bool JsonHeaderParser::ParseRoot(
      const RapidArray& arr, 
      const std::string& ns)
    {
      std::string current_ns = ns;
      bool success = true;
      
      ForEach(arr, [&](RapidValue v, RapidIdx i)
      {
        const char* type = nullptr;

        if ((type = GetType(v)) != nullptr)
        {
          if (strcmp(type, "namespace") == 0)
          {
            assert(v.HasMember("name") == true);
            
            const char* name = v["name"].GetString();
            current_ns += current_ns.size() == 0 ?
              name :
              std::string("::") + name;

            if (ParseRoot(v["members"].GetArray(), current_ns) == false)
            {
              success = false;
              return false;
            }
          }
          else if (strcmp(type, "class") == 0)
          {
            if (ParseClass(v, current_ns) == false)
            {
              success = false;
              return false;
            }
          }
        }

        return true;
      });

      return success;
    }

    //--------------------------------------------------------------------------
    const char* JsonHeaderParser::GetType(RapidValue v)
    {
      if (v.IsObject() == false)
      {
        return nullptr;
      }

      if (v.HasMember("type") == true)
      {
        RapidValue type = v["type"];

        if (type.IsString() == false)
        {
          return nullptr;
        }

        return type.GetString();
      }

      return nullptr;
    }

    //--------------------------------------------------------------------------
    bool JsonHeaderParser::DerivesFrom(const char* name, RapidValue v)
    {
      if (v.HasMember("parents") == false)
      {
        return false;
      }

      bool found = false;

      ForEach(v["parents"].GetArray(), [&](RapidValue val, RapidIdx i)
      {
        if (
          val.HasMember("name") == true &&
          val["name"].HasMember("name") == true)
        {
          std::string parent = val["name"]["name"].GetString();

          if (parent.find(name, 0) != std::string::npos)
          {
            found = true;
            return false;
          }
        }

        return true;
      });

      return found;
    }

    //--------------------------------------------------------------------------
    bool JsonHeaderParser::ParseClass(RapidValue v, const std::string& ns)
    {
      ClassDefinition d;
      d.ns = ns;

      assert(v.HasMember("name") == true);
      d.c_name = v["name"].GetString();

      const char* base = "ScriptClass";

      if (DerivesFrom(base, v) == false)
      {
        std::cerr << 
          "Class '" << d.c_name << "' does not derive from '" << base << "'" <<
          std::endl;

        return false;
      }

      std::cout << "sparse: " << d.ns << "::" << d.c_name << std::endl;

      if (ParseClassMembers(&d, v) == false)
      {
        std::cerr << "Could not parse class members for class '" << d.c_name <<
          "'" << std::endl;

        return false;
      }

      definitions_.push_back(d);

      return true;
    }

    //--------------------------------------------------------------------------
    bool JsonHeaderParser::ParseClassMembers(ClassDefinition* d, RapidValue v)
    {
      assert(d != nullptr);
      
      if (v.HasMember("members") == false)
      {
        return false;
      }

      bool success = true;
      ForEach(v["members"].GetArray(), [&](RapidValue val, RapidIdx i)
      {
        const char* type = nullptr;

        if ((type = GetType(val)) != nullptr)
        {
          if (strcmp(type, "macro") == 0)
          {
            if (SetScriptName(d, val) == false)
            {
              success = false;
              return false;
            }
          }
          else if (strcmp(type, "function") == 0)
          {
            if (ParseFunction(d, val) == false)
            {
              success = false;
              return false;
            }
          }
        }

        return true;
      });

      return success;
    }

    //--------------------------------------------------------------------------
    bool JsonHeaderParser::SetScriptName(ClassDefinition* d, RapidValue v)
    {
      if (d->s_name.size() > 0)
      {
        std::cerr << "Duplicate 'SCRIPT_NAME' found for class '" 
          << d->c_name <<
          "'" << std::endl;

        return false;
      }

      if (
        v.HasMember("meta") == false ||
        v["meta"].IsObject() == false)
      {
        return false;
      }

      RapidObject meta = v["meta"].GetObject();

      RapidObject::ConstMemberIterator it = meta.MemberBegin();
      if (it == meta.MemberEnd())
      {
        std::cerr << "SCRIPT_NAME for class '" << d->c_name << "' is empty" <<
          std::endl;

        return false;
      }

      d->s_name = it->name.GetString();

      if (d->s_name.size() == 0)
      {
        std::cerr << "Empty script name for class '" << d->c_name << "'" 
          << std::endl;

        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    bool JsonHeaderParser::ParseFunction(ClassDefinition* d, RapidValue v)
    {
      FunctionDefinition f;

      f.name = v["name"].GetString();
      f.ret_val = ParseTypeValue(v["returnType"]);

      if (
        v.HasMember("arguments") == false ||
        v["arguments"].IsArray() == false ||
        v["arguments"].GetArray().Size() == 0)
      {
        return true;
      }

      RapidArray arr = v["arguments"].GetArray();

      ForEach(arr, [&](RapidValue val, RapidIdx i)
      {
        f.arguments.push_back(ParseTypeValue(val));
        return true;
      });

      return true;
    }

    //--------------------------------------------------------------------------
    TypeDefinition JsonHeaderParser::ParseTypeValue(RapidValue v)
    {
      return TypeDefinition();
    }
  }
}