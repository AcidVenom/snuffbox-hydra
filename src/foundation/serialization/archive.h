#pragma once

#include "foundation/serialization/serializable.h"

#include "foundation/containers/vector.h"
#include "foundation/containers/string.h"
#include "foundation/io/path.h"

#include <EASTL/type_traits.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cinttypes>
#include <cstddef>

namespace snuffbox
{
  namespace foundation
  {
    template <typename T>
    struct is_number
    {
      static const bool value = 
        eastl::is_arithmetic<T>::value == true && 
        eastl::is_same<T, bool>::value == false && 
        eastl::is_enum<T>::value == false;
    };

    template <typename T>
    struct is_enum
    {
      static const bool value =
        is_number<T>::value == false && 
        eastl::is_enum<T>::value == true;
    };

    template <typename T>
    struct is_c_string
    {
      static const bool value = 
        eastl::is_same<char[
          std::extent_v<std::remove_reference_t<T>>], T>::value;
    };

    template <typename T>
    struct is_vector
    {
      static const bool value = false;
    };

    template <typename T>
    struct is_vector<Vector<T>>
    {
      static const bool value = true;
    };

    template <typename T>
    struct is_serializable
    {
      static const bool value = eastl::is_base_of<ISerializable, typename eastl::remove_pointer<T>::type>::value;
    };

    template <typename T>
    struct is_unknown
    {
      static const bool value = 
        is_number<T>::value == false && 
        is_enum<T>::value == false && 
        is_c_string<T>::value == false &&
        is_vector<T>::value == false &&
        is_serializable<T>::value == false;
    };

    template <bool C, typename T>
    using enable_t = typename eastl::enable_if<C, T>::type;

    template <typename T>
    using enable_if_number = enable_t<is_number<T>::value == true, T>;

    template <typename T>
    using enable_if_enum = enable_t<is_enum<T>::value == true, T>;

    template <typename T>
    using enable_if_c_string = enable_t<is_c_string<T>::value == true, T>;

    template <typename T>
    using enable_if_unknown = enable_t<is_unknown<T>::value == true, T>;

    template <typename T>
    using enable_if_vector = enable_t<is_vector<T>::value == true, T>;

    template <typename T>
    using enable_if_serializable = 
      enable_t<is_serializable<T>::value == true, T>;

    class SaveArchive
    {

    public:

      SaveArchive(const foundation::Path& path);

      template <typename T, typename ... Args>
      void operator()(const T& value, Args&&... args);

      template <typename T>
      void operator()(const T& value);

      template <typename T>
      static void Serialize(SaveArchive& archive, const T& value);

    protected:

      enum class Identifiers : uint8_t
      {
        kNumber,
        kBoolean,
        kString,
        kArray,
        kObjectStart,
        kObjectEnd
      };

      template <typename T>
      size_t Reserve(size_t size = sizeof(T));

      template <typename T>
      void WriteRaw(const T& value, size_t size = sizeof(T));

      void WriteIdentifier(Identifiers id);

      template <typename T>
      void WriteValue(const T& value, enable_if_number<T>* = nullptr);

      template <typename T>
      void WriteValue(const T& value, enable_if_enum<T>* = nullptr);

      template <typename T>
      void WriteValue(const T& value, enable_if_c_string<T>* = nullptr);

      template <typename T>
      void WriteValue(const T& value, enable_if_vector<T>* = nullptr);

      template <typename T>
      void WriteValue(const T& value, enable_if_serializable<T>* = nullptr);

      template <typename T>
      void WriteValue(const T& value, enable_if_unknown<T>* = nullptr);

      static String WriteJsonValue(size_t& i, const uint8_t* buffer);
      static String WriteJsonNumber(size_t& i, const uint8_t* buffer);
      static String WriteJsonBoolean(size_t& i, const uint8_t* buffer);
      static String WriteJsonString(size_t& i, const uint8_t* buffer);
      static String WriteJsonArray(size_t& i, const uint8_t* buffer);
      static String WriteJsonObject(size_t& i, const uint8_t* buffer);

      void Flush();

    public:

      ~SaveArchive();

    private:

      Vector<uint8_t> buffer_;
      foundation::Path path_;
    };

    class LoadArchive
    {

    public:

      template <typename T, typename ... Args>
      void operator()(T* value, Args&&... args);

      template <typename T>
      void operator()(T* value);
    };

    //--------------------------------------------------------------------------
    template <typename T>
    inline size_t SaveArchive::Reserve(size_t size)
    {
      size_t old = buffer_.size();

      buffer_.resize(buffer_.size() + size);

      return old;
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void SaveArchive::WriteRaw(const T& value, size_t size)
    {
      size_t start_at = Reserve<T>(size);
      const uint8_t* data = reinterpret_cast<const uint8_t*>(&value);
      
      for (size_t i = 0; i < size; ++i)
      {
        buffer_.at(start_at + i) = data[i];
      }
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void SaveArchive::WriteValue(const T& value, enable_if_number<T>*)
    {
      Serialize<double>(*this, static_cast<double>(value));
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void SaveArchive::WriteValue(const T& value, enable_if_enum<T>*)
    {
      WriteValue<int>(static_cast<int>(value));
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void SaveArchive::WriteValue(const T& value, enable_if_c_string<T>*)
    {
      Serialize<String>(*this, String(value));
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void SaveArchive::WriteValue(const T& value, enable_if_vector<T>*)
    {
      WriteIdentifier(Identifiers::kArray);

      size_t size = value.size();
      WriteRaw<size_t>(size);

      for (size_t i = 0; i < size; ++i)
      {
        WriteValue<T::value_type>(value.at(i));
      }
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void SaveArchive::WriteValue(const T& value, enable_if_serializable<T>*)
    {
      WriteIdentifier(Identifiers::kObjectStart);
      value->Serialize(*this);
      WriteIdentifier(Identifiers::kObjectEnd);
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void SaveArchive::WriteValue(const T& value, enable_if_unknown<T>*)
    {
      Serialize<T>(*this, value);
    }

    //--------------------------------------------------------------------------
    template <typename T, typename ... Args>
    inline void SaveArchive::operator()(const T& value, Args&&... args)
    {
      WriteValue<T>(value);
      operator()(eastl::forward<Args>(args)...);
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void SaveArchive::operator()(const T& value)
    {
      WriteValue<T>(value);
    }

    //--------------------------------------------------------------------------
    template <>
    inline void SaveArchive::Serialize(
      SaveArchive& archive, 
      const double& value)
    {
      archive.WriteIdentifier(Identifiers::kNumber);
      archive.WriteRaw<double>(value);
    }

    //--------------------------------------------------------------------------
    template <>
    inline void SaveArchive::Serialize(
      SaveArchive& archive, 
      const bool& value)
    {
      archive.WriteIdentifier(Identifiers::kBoolean);
      archive.WriteRaw<bool>(value);
    }

    //--------------------------------------------------------------------------
    template <>
    inline void SaveArchive::Serialize(
      SaveArchive& archive, 
      const String& value)
    {
      archive.WriteIdentifier(Identifiers::kString);
      archive.WriteRaw<char>(*value.c_str(), value.size() + 1);
    }

    //--------------------------------------------------------------------------
    template <>
    inline void SaveArchive::Serialize(
      SaveArchive& archive,
      const glm::vec2& value)
    {
      archive(foundation::Vector<float>({ value.x, value.y }));
    }

    //--------------------------------------------------------------------------
    template <>
    inline void SaveArchive::Serialize(
      SaveArchive& archive, 
      const glm::vec3& value)
    {
      archive(foundation::Vector<float>({ value.x, value.y, value.z }));
    }

    //--------------------------------------------------------------------------
    template <>
    inline void SaveArchive::Serialize(
      SaveArchive& archive, 
      const glm::vec4& value)
    {
      archive(foundation::Vector<float>({ value.x, value.y, value.z, value.w}));
    }

    //--------------------------------------------------------------------------
    template <>
    inline void SaveArchive::Serialize(
      SaveArchive& archive, 
      const glm::quat& value)
    {
      archive(foundation::Vector<float>({ value.x, value.y, value.z, value.w}));
    }
  }
}