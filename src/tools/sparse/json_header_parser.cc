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
    bool JsonHeaderParser::Parse(const std::string& input)
    {
      Clear();

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

          Clear();
        }

        return success;
      }
      
      std::cerr << "Could not open file: " << input << std::endl;

      Clear();

      return false;
    }

    //--------------------------------------------------------------------------
    bool JsonHeaderParser::HasDocument() const
    {
      return has_error_ == true ? false : 
        (definitions_.classes.size() > 0 || definitions_.enums.size() > 0);
    }

    //--------------------------------------------------------------------------
    const ScriptDefinitions& JsonHeaderParser::definitions() const
    {
      return definitions_;
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
          else if (strcmp(type, "enum") == 0)
          {
            if (ParseEnum(v, current_ns, nullptr) == false)
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

      definitions_.classes.push_back(d);

      return true;
    }

    //--------------------------------------------------------------------------
    bool JsonHeaderParser::ParseEnum(
      RapidValue v,
      const std::string& ns,
      ClassDefinition* cl)
    {
      if (v.HasMember("name") == false)
      {
        std::cerr << "Enumerator doesn't have a 'name' field" << std::endl;
        return false;
      }

      std::string nested_in = ns;

      if (cl != nullptr)
      {
        nested_in += nested_in.size() == 0 ? cl->c_name : "::" + cl->c_name;
      }

      EnumDefinition d;
      d.name = v["name"].GetString();
      d.nested = nested_in;
      d.ns = ns;

      std::cout << "sparse: " << d.nested << "::" << d.name << std::endl;

      if (v.HasMember("members") == false || v["members"].IsArray() == false)
      {
        definitions_.enums.push_back(d);
        return true;
      }

      RapidArray members = v["members"].GetArray();

      int current = 0;
      ForEach(members, [&](RapidValue val, RapidIdx i)
      {
        const char* key = val["key"].GetString();

        if (val.HasMember("value") == true)
        {
          current = atoi(val["value"].GetString());
        }

        d.values.emplace(std::pair<std::string, int>
        {
          key, current
        });

        ++current;

        return true;
      });

      definitions_.enums.push_back(d);

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
          else if (strcmp(type, "enum") == 0)
          {
            if (ParseEnum(val, d->ns, d) == false)
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
      f.is_static = v.HasMember("static") == true;

      if (v.HasMember("meta") == true)
      {
        RapidObject o = v["meta"].GetObject();

        for (
          RapidObject::MemberIterator it = o.MemberBegin();
          it != o.end();
          ++it)
        {
          if (strcmp(it->name.GetString(), "custom") == 0)
          {
            f.is_custom = true;
            break;
          }
        }
      }

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
        std::string name = val["name"].GetString();
        TypeDefinition type = ParseTypeValue(val["type"]);

        f.arguments.push_back(ArgumentDefinition{ name, type });
        return true;
      });

      d->functions.push_back(f);

      return true;
    }

    //--------------------------------------------------------------------------
    TypeDefinition JsonHeaderParser::ParseTypeValue(RapidValue v)
    {
      const char* type = v["type"].GetString();

      RefType ref = RefType::kLiteral;

      if (strcmp(type, "pointer") == 0)
      { 
        ref = RefType::kPointer;
      }
      else if (strcmp(type, "reference") == 0)
      {
        ref = RefType::kReference;
      }
      else
      {
        type = v["name"].GetString();
      }

      if (ref != RefType::kLiteral)
      {
        type = v["baseType"]["name"].GetString();
      }

      return TypeDefinition{ type, ref };
    }

    //--------------------------------------------------------------------------
    void JsonHeaderParser::Clear()
    {
      definitions_.classes.clear();
      definitions_.enums.clear();
    }
  }
}