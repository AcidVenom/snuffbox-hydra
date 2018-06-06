#include "foundation/serialization/archive.h"
#include "foundation/auxiliary/string_utils.h"

#include "foundation/io/file.h"

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
    String SaveArchive::WriteJsonValue(size_t& i, const uint8_t* buffer)
    {
      Identifiers id = *reinterpret_cast<const Identifiers*>(buffer + i);
      ++i;

      switch (id)
      {

      case Identifiers::kNumber:
        return WriteJsonNumber(i, buffer);

      case Identifiers::kBoolean:
        return WriteJsonBoolean(i, buffer);

      case Identifiers::kString:
        return WriteJsonString(i, buffer);

      case Identifiers::kArray:
        return WriteJsonArray(i, buffer);

      case Identifiers::kObjectStart:
        return WriteJsonObject(i, buffer);

      default:
        return "";
      }

      return "";
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
    String SaveArchive::WriteJsonArray(size_t& i, const uint8_t* buffer)
    {
      foundation::String result = "[";

      size_t size = *reinterpret_cast<const size_t*>(buffer + i);
      
      i += sizeof(size_t);

      size_t e = 0;

      while (e < size)
      {
        result += WriteJsonValue(i, buffer);
        ++e;

        if (e == size)
        {
          continue;
        }

        result += ",";
      }

      result += "]";

      return result;
    }

    //--------------------------------------------------------------------------
    String SaveArchive::WriteJsonObject(size_t& i, const uint8_t* buffer)
    {
      foundation::String result = "{";

      Identifiers id = *reinterpret_cast<const Identifiers*>(buffer + i);

      size_t count = 0;

      while (id != Identifiers::kObjectEnd)
      {
        result += "\"" + StringUtils::ToString(count) + "\" : ";

        result += WriteJsonValue(i, buffer);

        id = *reinterpret_cast<const Identifiers*>(buffer + i);

        ++count;

        if (id == Identifiers::kObjectEnd)
        {
          continue;
        }

        result += ",";
      }

      result += "}";

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