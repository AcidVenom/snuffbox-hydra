#pragma once

#include "foundation/serialization/serializable.h"
#include "foundation/definitions/archive.h"

#include "foundation/io/path.h"
#include "foundation/containers/vector.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/pointer.h>

namespace snuffbox
{
  namespace foundation
  {
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
      * @brief Default constructor
      */
      LoadArchive();

      /**
      * @see LoadArchive::FromFile
      */
      LoadArchive(const Path& path);

      /**
      * @brief Loads an archive from a JSON file
      *
      * @param[in] path The path to the file
      */
      bool FromFile(const Path& path);

      /**
      * @brief Loads an archive from a JSON string
      *
      * @param[in] json The JSON string
      */
      bool FromJson(const String& json);

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
      void operator()(T value, Args&&... args);

      /**
      * @see LoadArchive::operator()
      *
      * @remarks The last function of the unroll when there is only 
      *          one item left
      */
      template <typename T>
      void operator()(T value);

      /**
      * @brief Retrieves the array size of an array in the current scope
      *
      * @param[in] name The name of the array to retrieve
      *
      * @return The size of the array
      */
      size_t GetArraySize(const char* name);

      /**
      * @brief Retrieves an array value of an array in the current scope
      *
      * @tparam The value type to be retrieved
      *
      * @param[in] name The name of the array to retrieve the value from
      * @param[in] i The index within the array to retrieve the value from
      * @param[out] out The retrieved value
      */
      template <typename T>
      void GetArrayValue(const char* name, size_t i, T out);

      /**
      * @brief Deserializes a value by retrieving individual properties of
      *        the archive
      *
      * This function can be specialized to archive user-defined types. This is
      * one of the methods to archive these. You can also let a class derive
      * from ISerializable and override the ISerializable::Serialize and
      * ISerializable::Deserialize functions.
      *
      * @tparam T The type of value to archive
      *
      * @param[in] archive The archive to load the values from
      *
      * @return The deserialized value
      */
      template <typename T>
      static void Deserialize(LoadArchive& archive, T* out);

    protected:

      /**
      * @brief Reads a JSON value when the requested value type is a number
      *
      * @tparam A numerical type
      *
      * @param[in] v The current JSON value
      * @param[in] out The retrieved value
      */
      template <typename T>
      void ReadValue(
        const rapidjson::Value& v, 
        T* out, 
        enable_if_number<T>* = nullptr);

      /**
      * @brief Reads a JSON value when the requested value type is an enumerator
      *
      * @tparam An enumerator type
      *
      * @param[in] v The current JSON value
      * @param[in] out The retrieved value
      */
      template <typename T>
      void ReadValue(
        const rapidjson::Value& v, 
        T* out, 
        enable_if_enum<T>* = nullptr);

      /**
      * @brief Reads a JSON value when the requested value type is a vector
      *
      * @tparam A vector type
      *
      * @param[in] v The current JSON value
      * @param[in] out The retrieved value
      */
      template <typename T>
      void ReadValue(
        const rapidjson::Value& v, 
        T* out, 
        enable_if_vector<T>* = nullptr);

      /**
      * @brief Reads a JSON value when the requested value type is serializable
      *
      * @tparam A serializable pointer type
      *
      * @remarks This call doesn't actually adjust the out parameter, it
      *          simply calls ISerializable::Deserialize on the pointer
      *
      * @param[in] v The current JSON value
      * @param[in] out The retrieved value
      */
      template <typename T>
      void ReadValue(
        const rapidjson::Value& v, 
        T* out, 
        enable_if_serializable<T>* = nullptr);

      /**
      * @brief Reads a JSON value when the requested value type is unknown
      *
      * @tparam A user-defined type
      *
      * @remarks This call requires specialization of LoadArchive::Deserialize
      *          to compile
      *
      * @param[in] v The current JSON value
      * @param[in] out The retrieved value
      */
      template <typename T>
      void ReadValue(
        const rapidjson::Value& v, 
        T* out, 
        enable_if_n_serializable<T>* = nullptr);

      /**
      * @brief Enters a JSON object by token name
      *
      * @param[in] token The token to traverse the JSON tree with
      */
      void EnterScope(const String& token);

      /**
      * @return The current JSON scope we're in
      */
      const rapidjson::Value* CurrentScope();

      /**
      * @brief Exit the current scope and pop one token off of the current
      *        token URI
      */
      void ExitScope();

    public:

      /**
      * @return Can we use this archive for deserialization?
      */
      bool is_ok() const;

    private:

      bool is_ok_; //!< Can we use this archive for deserialization?
      rapidjson::Document document_; //!< The loaded Json document

      String scope_; //!< The current URI to the current JSON scope
    };

    //--------------------------------------------------------------------------
    template <typename T, typename ... Args>
    inline void LoadArchive::operator()(T value, Args&&... args)
    {
      operator()(value);
      operator()(eastl::forward<Args>(args)...);
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void LoadArchive::operator()(T value)
    {
      if (is_ok_ == false)
      {
        return;
      }

      static_assert(eastl::is_pointer<T>::value == true,
        "Out parameters for a LoadArchive should be of a de-referencable\
         pointer type");

      ReadValue<typename eastl::remove_pointer<T>::type>(
        *CurrentScope(),
        value);

      ExitScope();
    }

    //--------------------------------------------------------------------------
    template <>
    inline void LoadArchive::operator()(ArchiveName value)
    {
      EnterScope(value.name);
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void LoadArchive::ReadValue(
      const rapidjson::Value& v,
      T* out,
      enable_if_number<T>*)
    {
      if (v.IsNumber() == false)
      {
        return;
      }

      *out = static_cast<T>(v.GetDouble());
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void LoadArchive::ReadValue(
      const rapidjson::Value& v,
      T* out,
      enable_if_enum<T>*)
    {
      ReadValue<int>(v, reinterpret_cast<int*>(out));
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void LoadArchive::ReadValue(
      const rapidjson::Value& v,
      T* out,
      enable_if_vector<T>*)
    {
      if (v.IsArray() == false)
      {
        return;
      }

      const rapidjson::GenericArray<true, rapidjson::Value::ValueType>& arr = 
        v.GetArray();

      rapidjson::SizeType size = arr.Size();

      T& vec = *out;

      using Type = typename T::value_type;

      for (rapidjson::SizeType i = 0; i < size; ++i)
      {
        EnterScope(StringUtils::ToString(i));

        if (vec.size() <= i)
        {
          vec.push_back(Type());
          ReadValue<Type>(v[i], vec.end());
        }
        else
        {
          ReadValue<Type>(v[i], &vec.at(i));
        }

        ExitScope();
      }
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void LoadArchive::ReadValue(
      const rapidjson::Value& v,
      T* out,
      enable_if_serializable<T>*)
    {
      (*out)->Deserialize(*this);
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void LoadArchive::ReadValue(
      const rapidjson::Value& v,
      T* out,
      enable_if_n_serializable<T>*)
    {
      Deserialize<T>(*this, out);
    }

    //--------------------------------------------------------------------------
    template <>
    inline void LoadArchive::ReadValue(
      const rapidjson::Value& v,
      bool* out,
      enable_if_n_serializable<bool>*)
    {
      if (v.IsBool() == false)
      {
        *out = false;
        return;
      }

      *out = v.GetBool();
    }

    //--------------------------------------------------------------------------
    template <>
    inline void LoadArchive::ReadValue(
      const rapidjson::Value& v,
      String* out,
      enable_if_n_serializable<String>*)
    {
      if (v.IsString() == false)
      {
        *out = "";
        return;
      }

      *out = v.GetString();
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void LoadArchive::GetArrayValue(const char* name, size_t i, T out)
    {
      EnterScope(name);

      const rapidjson::Value& current = *CurrentScope();
      
      if (current.IsArray() == false)
      {
        return;
      }

      const rapidjson::GenericArray<true, rapidjson::Value::ValueType>& arr =
        current.GetArray();

      if (i > arr.Size())
      {
        return;
      }

      EnterScope(StringUtils::ToString(i));

      ReadValue<typename eastl::remove_pointer<T>::type>(
        arr[static_cast<rapidjson::SizeType>(i)], out);

      ExitScope();

      ExitScope();
    }

    //--------------------------------------------------------------------------
    template <>
    inline void LoadArchive::Deserialize(LoadArchive& archive, glm::vec2* out)
    {
      float x, y;

      archive(
        GET_ARCHIVE_PROP(x),
        GET_ARCHIVE_PROP(y));

      glm::vec2& v = *out;
      v.x = x;
      v.y = y;
    }

    //--------------------------------------------------------------------------
    template <>
    inline void LoadArchive::Deserialize(LoadArchive& archive, glm::vec3* out)
    {
      float x, y, z;

      archive(
        GET_ARCHIVE_PROP(x),
        GET_ARCHIVE_PROP(y),
        GET_ARCHIVE_PROP(z));

      glm::vec3& v = *out;
      v.x = x;
      v.y = y;
      v.z = z;
    }

    //--------------------------------------------------------------------------
    template <>
    inline void LoadArchive::Deserialize(LoadArchive& archive, glm::vec4* out)
    {
      float x, y, z, w;

      archive(
        GET_ARCHIVE_PROP(x),
        GET_ARCHIVE_PROP(y),
        GET_ARCHIVE_PROP(z),
        GET_ARCHIVE_PROP(w));


      glm::vec4& v = *out;
      v.x = x;
      v.y = y;
      v.z = z;
      v.w = w;
    }

    //--------------------------------------------------------------------------
    template <>
    inline void LoadArchive::Deserialize(LoadArchive& archive, glm::quat* out)
    {
      glm::vec4 v;
      Deserialize<glm::vec4>(archive, &v);

      glm::quat& q = *out;

      q.x = v.x;
      q.y = v.y;
      q.z = v.z;
      q.w = v.w;
    }
  }
}