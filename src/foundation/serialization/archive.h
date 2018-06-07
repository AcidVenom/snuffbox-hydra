#pragma once

#include "foundation/serialization/serializable.h"

#include "foundation/containers/vector.h"
#include "foundation/containers/string.h"
#include "foundation/io/path.h"
#include "foundation/auxiliary/type_traits.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cinttypes>
#include <cstddef>

#define ARCHIVE_PROP(x) snuffbox::foundation::ArchiveName{ #x }, ## x

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief Used as a unique type to store property names
    *
    * @author Daniel Konings
    */
    struct ArchiveName
    {
      const char* name; //!< The name of the property
    };

    /**
    * @brief Used to check if a type derives from ISerializable
    *
    * @tparam T The type to check
    *
    * @author Daniel Konings
    */
    template <typename T>
    struct is_serializable
    {
      /**
      * @brief Is T serializable?
      */
      static const bool value = 
        eastl::is_base_of<ISerializable, 
        typename eastl::remove_pointer<T>::type>::value;
    };

    /**
    * @brief Used to enable functions when T is of a user-defined type and
    *        serializable
    *
    * @tparam T The type to check
    *
    * @author Daniel Konings
    */
    template <typename T>
    using enable_if_serializable = 
      eastl::enable_if_t<is_serializable<T>::value == true, T>;

    /**
    * @brief Used to enable functions when T is of a user-defined type and
    *        not serializable
    *
    * @tparam T The type to check
    *
    * @author Daniel Konings
    */
    template <typename T>
    using enable_if_n_serializable =
      eastl::enable_if_t<is_user_defined<T>::value == true && 
      is_serializable<T>::value == false, T>;

    /**
    * @brief Used to archive values for serialization
    *
    * This is to be used in conjuction with LoadArchive to serialize and
    * deserialize data. The data can be named so that the serialization works
    * over multiple versions of the data. Of course, these names need to be
    * kept intact for backwards compatibility.
    *
    * The archive stores its data as JSON on disk, which can then be loaded
    * again through a LoadArchive
    *
    * @author Daniel Konings
    */
    class SaveArchive
    {

    public:

      /**
      * @brief Construct a save archive with a path to flush the data to
      *
      * @param[in] path The path to store the data in
      */
      SaveArchive(const foundation::Path& path);

      /**
      * @brief Archives multiple values into the archive
      *
      * @tparam T The current type of the recursive unroll
      * @tparam Args... The other types of the recursive unroll
      *
      * @param[in] value The current value
      * @param[in] args The remaining values
      */
      template <typename T, typename ... Args>
      void operator()(const T& value, Args&&... args);

      /**
      * @see SaveArchive::operator()
      *
      * @remarks The function that is called in the unroll when there is only
      *          one value left
      */
      template <typename T>
      void operator()(const T& value);

      /**
      * @brief Serializes a value by storing individual components of the value
      *        in the archive
      *
      * This function can be specialized to archive user-defined types. This is
      * one of the methods to archive these. You can also let a class derive
      * from ISerializable and override the ISerializable::Serialize and
      * ISerializable::Deserialize functions.
      *
      * @tparam T The type of value to archive
      *
      * @param[in] archive The archive to store the value in
      * @param[in] value The value being archived
      */
      template <typename T>
      static void Serialize(SaveArchive& archive, const T& value);

    protected:

      /**
      * @brief Common identifiers to represent what type of value is followed
      *        in the archive's buffer
      */
      enum class Identifiers : uint8_t
      {
        kName,
        kNumber,
        kBoolean,
        kString,
        kArray,
        kObjectStart,
        kObjectEnd
      };

      /**
      * @brief Reserves space in the archive's buffer
      *
      * @tparam T The type to reserve space for
      *
      * @param[in] size The size of the type, overridable
      *
      * @return The index in the buffer to the reserved block
      */
      template <typename T>
      size_t Reserve(size_t size = sizeof(T));

      /**
      * @brief Writes a type's data as raw bytes into the buffer
      *
      * @tparam T The value type
      *
      * @param[in] value The value containing the data
      * @param[in] size The size of the data to be written, overridable
      */
      template <typename T>
      void WriteRaw(const T& value, size_t size = sizeof(T));

      /**
      * @brief Writes an identifier into the buffer to represent the value
      *        followed in the buffer
      *
      * @param[in] id The identifier to write
      */
      void WriteIdentifier(Identifiers id);

      /**
      * @brief Write the name of a named value into the buffer
      *
      * @param[in] name The name of the value that follows this name
      */
      void WriteName(const char* name);

      /**
      * @brief Writes a number value to the archive's buffer
      *
      * @tparam T The numerical type
      *
      * @param[in] value The value to write
      */
      template <typename T>
      void WriteValue(const T& value, enable_if_number<T>* = nullptr);

      /**
      * @brief Writes an enumerator value to the archive's buffer
      *
      * @tparam T The enumerator type
      *
      * @param[in] value The value to write
      */
      template <typename T>
      void WriteValue(const T& value, enable_if_enum<T>* = nullptr);

      /**
      * @brief Writes a C-style string to the archive's buffer
      *
      * @tparam T The C-style string
      *
      * @param[in] value The value to write
      */
      template <typename T>
      void WriteValue(const T& value, enable_if_c_string<T>* = nullptr);

      /**
      * @brief Writes a vector and its contents to the archive's buffer
      *
      * @tparam T The vector type
      *
      * @param[in] value The value to write
      */
      template <typename T>
      void WriteValue(const T& value, enable_if_vector<T>* = nullptr);

      /**
      * @brief Writes a serializable class to the archive's buffer
      *
      * @remarks This calls ISerializable::Serialize on the class
      *
      * @tparam T The serializable type
      *
      * @param[in] value The value to write
      */
      template <typename T>
      void WriteValue(const T& value, enable_if_serializable<T>* = nullptr);

      /**
      * @brief Writes any other user-defined type to the buffer
      *
      * SaveArchive::Serialize needs to be specialized with the user-defined
      * type for this function to write the data.
      *
      * @tparam T The type
      *
      * @param[in] value The value to write
      */
      template <typename T>
      void WriteValue(const T& value, enable_if_n_serializable<T>* = nullptr);

      /**
      * @brief Writes an unknown value as a JSON string
      *
      * @param[in] i The current index within the buffer
      * @param[in] buffer The current buffer
      * @param[in] indent The amount of indentation
      *
      * @return The stringified JSON value
      */
      static String WriteJsonValue(
        size_t& i, 
        const uint8_t* buffer, 
        int indent = 0);

      /**
      * @brief Writes a number value as a JSON string
      *
      * @remarks These numbers are always of double precision
      *
      * @param[in] i The current index within the buffer
      * @param[in] buffer The current buffer
      *
      * @return The stringified JSON number
      */
      static String WriteJsonNumber(size_t& i, const uint8_t* buffer);

      /**
      * @brief Writes a number value as a JSON boolean
      *
      * @param[in] i The current index within the buffer
      * @param[in] buffer The current buffer
      *
      * @return The stringified JSON boolean
      */
      static String WriteJsonBoolean(size_t& i, const uint8_t* buffer);

      /**
      * @brief Writes a number value as a JSON string
      *
      * @param[in] i The current index within the buffer
      * @param[in] buffer The current buffer
      *
      * @return The stringified JSON string
      */
      static String WriteJsonString(size_t& i, const uint8_t* buffer);

      /**
      * @brief Writes a number value as a JSON array
      *
      * @param[in] i The current index within the buffer
      * @param[in] buffer The current buffer
      * @param[in] indent The amount of indentation
      *
      * @return The stringified JSON array
      */
      static String WriteJsonArray(
        size_t& i, 
        const uint8_t* buffer, 
        int indent);

      /**
      * @brief Writes a number value as a JSON object
      *
      * @param[in] i The current index within the buffer
      * @param[in] buffer The current buffer
      * @param[in] indent The amount of indentation
      *
      * @return The stringified JSON object
      */
      static String WriteJsonObject(
        size_t& i, 
        const uint8_t* buffer, 
        int indent);

      /**
      * @brief Retrieves the indentation string to prepend based on the current
      *        number of indentations
      *
      * @param[in] indent The number of indentations to do
      *
      * @return The resulting string
      */
      static String IndentationString(int indent);

      /**
      * @brief Flushes the contents of the archive to disk, as a JSON string
      */
      void Flush();

    public:

      /**
      * @see SaveArchive::Flush
      */
      ~SaveArchive();

    private:

      Vector<uint8_t> buffer_; //!< The buffer with data
      foundation::Path path_; //!< The path where to save the data
    };

    /**
    * @brief Used to load data archived by SaveArchive
    *
    * The load archive requires a JSON format, created from the SaveArchive.
    * All values can then be retrieved by their original name within the
    * SaveArchive. This is order-independent and thus compatible over multiple
    * versions of serialized data.
    *
    * @author Daniel Konings
    */
    class LoadArchive
    {

    public:

      /**
      * @brief Load a value from the archive
      *
      * @remarks This is a recursive unroll to retrieve multiple values at once
      *
      * @tparam T The current type
      * @tparam Args... The remaining types
      *
      * @param[out] value The value to store the found data in
      * @param[out] args The other values to retrieve
      */
      template <typename T, typename ... Args>
      void operator()(T* value, Args&&... args);

      /**
      * @see LoadArchive::operator()
      *
      * @remarks The last function of the unroll when there is only 
      *          one item left
      */
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
    inline void SaveArchive::WriteValue(
      const T& value, 
      enable_if_serializable<T>*)
    {
      WriteIdentifier(Identifiers::kObjectStart);
      value->Serialize(*this);
      WriteIdentifier(Identifiers::kObjectEnd);
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void SaveArchive::WriteValue(
      const T& value, 
      enable_if_n_serializable<T>*)
    {
      Serialize<T>(*this, value);
    }

    //--------------------------------------------------------------------------
    template <typename T, typename ... Args>
    inline void SaveArchive::operator()(const T& value, Args&&... args)
    {
      operator()(value);
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
      const ArchiveName& value)
    {
      archive.WriteName(value.name);
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