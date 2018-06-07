#include "foundation/serialization/archive.h"
#include "foundation/auxiliary/string_utils.h"
#include "foundation/auxiliary/logger.h"

#include "foundation/io/file.h"

#include <memory>

namespace snuffbox
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    SaveArchive::SaveArchive(const foundation::Path& path) :
      path_(path)
    {

    }

    //--------------------------------------------------------------------------
    void SaveArchive::WriteIdentifier(Identifiers id)
    {
      buffer_.push_back(static_cast<uint8_t>(id));
    }

    //--------------------------------------------------------------------------
    void SaveArchive::WriteName(const char* name)
    {
      WriteIdentifier(Identifiers::kName);

      size_t size = strlen(name);
      size_t off = Reserve<uint8_t>(size + 1);

      for (size_t i = 0; i < size; ++i)
      {
        buffer_.at(off + i) = name[i];
      }

      buffer_.at(off + size) = '\0';
    }

    //--------------------------------------------------------------------------
    String SaveArchive::WriteJsonValue(
      size_t& i, 
      const uint8_t* buffer,
      int indent)
    {
      Identifiers id = *reinterpret_cast<const Identifiers*>(buffer + i);
      ++i;

      String result;

      switch (id)
      {

      case Identifiers::kNumber:
        result = WriteJsonNumber(i, buffer);
        break;

      case Identifiers::kBoolean:
        result = WriteJsonBoolean(i, buffer);
        break;

      case Identifiers::kString:
        result = WriteJsonString(i, buffer);
        break;

      case Identifiers::kArray:
        result = WriteJsonArray(i, buffer, indent);
        break;

      case Identifiers::kObjectStart:
        result = WriteJsonObject(i, buffer, indent);
        break;
      }

      return result;
    }

    //--------------------------------------------------------------------------
    String SaveArchive::WriteJsonNumber(size_t& i, const uint8_t* buffer)
    {
      const size_t size = sizeof(double);

      double value = *reinterpret_cast<const double*>(buffer + i);
      i += size;

      String str = StringUtils::ToString<double>(value);

      int idx;
      if ((idx = StringUtils::Contains(str, ",")) >= 0)
      {
        str.at(idx) = '.';
      }

      return str;
    }

    //--------------------------------------------------------------------------
    String SaveArchive::WriteJsonBoolean(size_t& i, const uint8_t* buffer)
    {
      const size_t size = sizeof(bool);

      bool value = *reinterpret_cast<const bool*>(buffer + i);
      i += size;

      return value == true ? "true" : "false";
    }

    //--------------------------------------------------------------------------
    String SaveArchive::WriteJsonString(size_t& i, const uint8_t* buffer)
    {
      char current = buffer[i];

      foundation::String result = "\"";
      while (current != '\0')
      {
        result += current;

        ++i;
        current = buffer[i];
      }

      result += "\"";
      ++i;

      return result;
    }

    //--------------------------------------------------------------------------
    String SaveArchive::WriteJsonArray(
      size_t& i, 
      const uint8_t* buffer, 
      int indent)
    {
      String result = IndentationString(indent) + "[";

      ++indent;

      size_t size = *reinterpret_cast<const size_t*>(buffer + i);
      Identifiers id;
      
      i += sizeof(size_t);

      size_t e = 0;

      String indent_string = "\n" + IndentationString(indent);

      while (e < size)
      {
        id = *reinterpret_cast<const Identifiers*>(buffer + i);

        if (id == Identifiers::kObjectStart || id == Identifiers::kArray)
        {
          result += "\n" + WriteJsonValue(i, buffer, indent);
        }
        else
        {
          result += indent_string + WriteJsonValue(i, buffer, indent);
        }

        ++e;

        if (e == size)
        {
          continue;
        }

        result += ",";
      }

      result += "\n" + IndentationString(indent - 1) + "]";

      return result;
    }

    //--------------------------------------------------------------------------
    String SaveArchive::WriteJsonObject(
      size_t& i, 
      const uint8_t* buffer, 
      int indent)
    {
      String result = IndentationString(indent) + "{";

      ++indent;

      Identifiers id = *reinterpret_cast<const Identifiers*>(buffer + i);

      auto GetName = [&i, &id, buffer]()
      {
        Logger::Assert(id == Identifiers::kName, "Unexpected token in archive");
        ++i;
        String result = WriteJsonString(i, buffer);
        id = *reinterpret_cast<const Identifiers*>(buffer + i);

        return result;
      };

      String indent_string = "\n" + IndentationString(indent);
      while (id != Identifiers::kObjectEnd)
      {
        result += indent_string + GetName() + " : ";
        if (id == Identifiers::kObjectStart || id == Identifiers::kArray)
        {
          result += "\n";
        }
        result += WriteJsonValue(i, buffer, indent);

        id = *reinterpret_cast<const Identifiers*>(buffer + i);

        if (id == Identifiers::kObjectEnd)
        {
          ++i;
          continue;
        }

        result += ",";
      }

      result += "\n" + IndentationString(indent - 1) + "}";

      return result;
    }

    //--------------------------------------------------------------------------
    String SaveArchive::IndentationString(int indent)
    {
      String result = "";

      for (int i = 0; i < indent; ++i)
      {
        result += '\t';
      }

      return result;
    }

    //--------------------------------------------------------------------------
    void SaveArchive::Flush()
    {
      size_t i = 0;
      String json = WriteJsonValue(i, buffer_.data());

      File file(path_, FileFlags::kWrite);

      if (file.is_ok() == false)
      {
        return;
      }

      file.Write(reinterpret_cast<const uint8_t*>(json.c_str()), json.size());
    }

    //--------------------------------------------------------------------------
    SaveArchive::~SaveArchive()
    {
      Flush();
    }
  }
}