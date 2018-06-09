#pragma once

#include "foundation/definitions/archive.h"

#include "foundation/containers/vector.h"
#include "foundation/containers/string.h"
#include "foundation/io/path.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cinttypes>
#include <cstddef>


namespace snuffbox
{
  namespace foundation
  {
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
      * @brief Default constructor 
      */
      SaveArchive();

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

    public:

      /**
      * @brief Clears the archive of all its contents
      *
      * @remarks This checks if we're currently archiving, as that might
      *          cause undefined behavior due to the nature of the abstract
      *          buffer
      */
      void Clear();

      /**
      * @brief Flushes the contents of the archive to a file, as a JSON string
      *
      * @param[in] path The path to the file
      *
      * @return Were we able to save the file?
      */
      bool WriteFile(const Path& path) const;

      /**
      * @return Retrieves the contents of the archive as a JSON string
      */
      String ToMemory() const;

    private:

      int archiving_; //!< Are we currently archiving?
      Vector<uint8_t> buffer_; //!< The buffer with data
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
    inline void SaveArchive::WriteValue(
      const T& value, 
      enable_if_number<T>*)
    {
      WriteIdentifier(Identifiers::kNumber);
      WriteRaw<double>(static_cast<double>(value));
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
        WriteValue<typename T::value_type>(value.at(i));
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
      WriteIdentifier(Identifiers::kObjectStart);
      Serialize<T>(*this, value);
      WriteIdentifier(Identifiers::kObjectEnd);
    }

    //--------------------------------------------------------------------------
    template <>
    inline void SaveArchive::WriteValue( 
      const bool& value,
      enable_if_n_serializable<bool>*)
    {
      WriteIdentifier(Identifiers::kBoolean);
      WriteRaw<bool>(value);
    }

    //--------------------------------------------------------------------------
    template <>
    inline void SaveArchive::WriteValue(
      const String& value,
      enable_if_n_serializable<String>*)
    {
      WriteIdentifier(Identifiers::kString);
      WriteRaw<char>(*value.c_str(), value.size() + 1);
    }

    //--------------------------------------------------------------------------
    template <>
    inline void SaveArchive::WriteValue(
      const ArchiveName& value,
      enable_if_n_serializable<ArchiveName>*)
    {
      WriteName(value.name);
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
      ++archiving_;
      WriteValue<T>(value);
      --archiving_;
    }

    //--------------------------------------------------------------------------
    template <>
    inline void SaveArchive::Serialize(
      SaveArchive& archive,
      const glm::vec2& value)
    {
      float x = value.x;
      float y = value.y;

      archive(
        SET_ARCHIVE_PROP(x), 
        SET_ARCHIVE_PROP(y));
    }

    //--------------------------------------------------------------------------
    template <>
    inline void SaveArchive::Serialize(
      SaveArchive& archive, 
      const glm::vec3& value)
    {
      float x = value.x;
      float y = value.y;
      float z = value.z;

      archive(
        SET_ARCHIVE_PROP(x), 
        SET_ARCHIVE_PROP(y),
        SET_ARCHIVE_PROP(z));
    }

    //--------------------------------------------------------------------------
    template <>
    inline void SaveArchive::Serialize(
      SaveArchive& archive, 
      const glm::vec4& value)
    {
      float x = value.x;
      float y = value.y;
      float z = value.z;
      float w = value.w;

      archive(
        SET_ARCHIVE_PROP(x), 
        SET_ARCHIVE_PROP(y),
        SET_ARCHIVE_PROP(z),
        SET_ARCHIVE_PROP(w));
    }

    //--------------------------------------------------------------------------
    template <>
    inline void SaveArchive::Serialize(
      SaveArchive& archive, 
      const glm::quat& value)
    {
      Serialize<glm::vec4>(archive, { value.x, value.y, value.z, value.w });
    }
  }
}