#include "foundation/serialization/load_archive.h"
#include "foundation/auxiliary/logger.h"

#include "foundation/io/file.h"

#include <rapidjson/error/en.h>

namespace snuffbox
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    LoadArchive::LoadArchive() :
      is_ok_(false)
    {

    }

    //--------------------------------------------------------------------------
    LoadArchive::LoadArchive(const Path& path) :
      LoadArchive()
    {
      FromFile(path);
    }

    //--------------------------------------------------------------------------
    bool LoadArchive::FromFile(const Path& path)
    {
      File fin(path);

      if (fin.is_ok() == false)
      {
        return false;
      }

      size_t len;
      const uint8_t* buffer = fin.ReadBuffer(&len, true);

      is_ok_ = FromJson(reinterpret_cast<const char*>(buffer));
      return is_ok_;
    }

    //--------------------------------------------------------------------------
    bool LoadArchive::FromJson(const String& json)
    {
      rapidjson::ParseResult res = document_.Parse(json.c_str());

      if (res.IsError() == true)
      {
        Logger::LogVerbosity<1>(
          LogChannel::kEngine,
          LogSeverity::kError,
          "Could not parse JSON for deserialization\n\nError(s):\n{0} ({1})",
          rapidjson::GetParseError_En(res.Code()),
          res.Offset());

        return false;
      }

      is_ok_ = true;

      return true;
    }

    //--------------------------------------------------------------------------
    size_t LoadArchive::GetArraySize(const char* name)
    {
      const JsonValue& current = (*CurrentScope())[name];

      if (current.IsArray() == false)
      {
        return 0;
      }

      rapidjson::SizeType size = current.GetArray().Size();

      return static_cast<size_t>(size);
    }

    //--------------------------------------------------------------------------
    void LoadArchive::EnterScope(const String& token)
    {
      scope_ += "/" + token;
    }

    //--------------------------------------------------------------------------
    const LoadArchive::JsonValue* LoadArchive::CurrentScope()
    {
      return rapidjson::GenericPointer<
        JsonValue, 
        RapidJsonStackAllocator>(scope_.c_str()).Get(document_);
    }

    //--------------------------------------------------------------------------
    void LoadArchive::ExitScope()
    {
      if (scope_.size() == 0)
      {
        return;
      }

      int erase = 0;
      int i = static_cast<int>(scope_.size()) - 1;

      char current = scope_.at(i);
      while (i >= 0 && current != '/')
      {
        current = scope_.at(i);
        ++erase;
        --i;
      }

      scope_.erase(scope_.end() - erase, scope_.end());
    }

    //--------------------------------------------------------------------------
    bool LoadArchive::is_ok() const
    {
      return is_ok_;
    }
  }
}