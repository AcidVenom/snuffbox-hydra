#pragma once

#include <foundation/containers/vector.h>
#include <foundation/memory/memory.h>

#include "scripting/script_value.h"

namespace snuffbox
{
  namespace scripting
  {
    /**
    * @brief Used to contain a list of script values that were passed through
    *        a native C-function
    *
    * This class contains definitions to check the arguments by a format and
    * how many arguments were passed. It also contains a "callee" definition,
    * to be used if the function was originally a member function
    *
    * @author Daniel Konings
    */
    class ScriptArgs
    {
      
    public:

      struct ReturnPtr
      {
        void* ptr;
        foundation::String type;
      };

      /**
      * @brief Default constructor
      */
      ScriptArgs();

      /**
      * @brief Construct from a list of arguments
      *
      * The arguments should be created by the native scripting API and put
      * in a vector of ScriptValue.
      *
      * @param[in] args The parsed arguments by the 
      *                 native scripting implementation
      * @param[in] callee The callee/self/this that called the method from
      *                   scripting
      *
      * @remarks The callee is only applicable with non-static functions
      */
      ScriptArgs(
        const foundation::Vector<ScriptHandle>& args,
        void* callee);

      /**
      * @brief Checks if the arguments passed into this function are
      *        of the right type and argument length
      *
      * The argument length is simply deducted from the string length of the
      * format string. The supported characters to check for are as follows:
      *
      * N = Number
      * B = Boolean
      * S = String
      * O = Object
      * A = Array
      *
      * e.g. If a function only accepts a string as first argument and a
      * number as second argument, the format would be "SN". The format is
      * case in-sensitive.
      *
      * @param[in] format The format to check
      */
      bool Check(const char* format) const;

      /**
      * @return The number of arguments contained in this class
      */
      uint8_t Count() const;

      /**
      * @brief Retrieves a numerical value
      *
      * @see ScriptArgs::GetImpl
      */
      template <typename T>
      T Get(uint8_t idx, T def = T(), if_number_or_enum<T>* = nullptr) const;
      
      /**
      * @brief Retrieves any other value
      *
      * @see ScriptArgs::GetImpl
      */
      template <typename T>
      T Get(uint8_t idx, T def = T(), if_n_number_and_enum<T>* = nullptr) const;

      /**
      * @brief Retrieves a pointer from a ScriptObject
      *
      * @param[in] idx The argument index to retrieve the pointer from
      *
      * @return The retrieved pointer, or nullptr if there was a type mismatch
      */
      template <typename T>
      T* GetPointer(uint8_t idx) const;

      /**
      * @brief Returns a value to the script state
      *
      * @tparam T The convertible value type
      *
      * @param[in] value The value to return, which will be converted to
      *                  a ScriptValue
      *
      * @remarks If a new return value is specified, the other is lost
      *          implicitly
      */
      template <typename T>
      void AddReturnValue(T value);

      /**
      * @brief Returns a pointer, derived from ScriptClass
      *
      * @param[in] ptr The pointer to return
      *
      * @remarks If a new return value is specified, the other is lost
      *          implicitly
      */
      template <typename T>
      void AddReturnPointer(T* ptr);

      /**
      * @brief Retrieves the callee as a typed pointer
      *
      * @tparam T The type to cast the pointer to
      *
      * @return The callee, or nullptr if it was not of the specified type
      */
      template <typename T>
      T* GetSelf() const;

      /**
      * @return The callee that called this function from scripting
      *
      * @remarks This callee will be nullptr if the function call was non-static
      */
      void* callee() const;

      /**
      * @return The return value that was set in the function call
      *
      * @remarks This is nullptr if it was never set, thus there will be no
      *          return value, unless ScriptArgs::return_ptr_ is set
      */
      ScriptValue* return_value() const;

      /**
      * @return The return pointer that was set in the function call
      *
      * @remarks The .ptr field is nullptr if it was never set, thus there will
      *          be no return value, unless ScriptArgs::return_value_ is set
      */
      ReturnPtr return_ptr() const;

    protected:

      /**
      * @brief Checks if an argument index is in the valid arguments range
      *
      * @param[in] idx The argument index to check for
      *
      * @return Is the index within the valid argument range?
      */
      bool HasArgument(uint8_t idx) const;

      /**
      * @brief Retrieves the pointer to the argument at an index
      *
      * @param[in] idx THe argument index to retrieve the value from
      *
      * @return The value at the given index, or nullptr if it doesn't exist
      */
      ScriptValue* GetArgument(uint8_t idx) const;

      /**
      * @brief Checks an argument and logs an error if the expected argument
      *        type was not found
      *
      * @param[in] expected The expected type
      * @param[in] type The argument type
      * @param[in] idx The argument index
      *
      * @return Was the argument of a valid type?
      */
      static bool CheckArg(
        ScriptValueTypes expected, 
        ScriptValueTypes type,
        uint8_t idx);

      /**
      * @brief Retrieves a certain argument as a certain type, but only if
      *        the argument retrieved is of the same type as the template
      *        parameter (or castable).
      *
      * @tparam T The type of the argument to retrieve
      * @tparam Y The ScriptValue derived class to cast to
      * @tparam U The type enumeration to check against
      *
      * @param[in] idx The argument index to retrieve the value from
      * @param[in] def The default value if the argument is invalid
      */
      template <typename T, typename Y, ScriptValueTypes U>
      T GetImpl(uint8_t idx, T def) const;

    private:

      /**
      * @brief The parsed arguments by the native scripting implementation
      */
      foundation::Vector<ScriptHandle> arguments_;

      void* callee_; //!< The callee that called this function from scripting

      /**
      * @brief The return value that was set in the function call
      */
      ScriptHandle return_value_;

      /**
      * @brief The return pointer that was set in the function call
      */
      ReturnPtr return_ptr_;
    };

    //--------------------------------------------------------------------------
    template <typename T>
    inline T ScriptArgs::Get(uint8_t idx, T def, if_number_or_enum<T>*) const
    {
      return static_cast<T>(
        GetImpl<double, ScriptNumber, ScriptValueTypes::kNumber>(idx, def));
    }

    //--------------------------------------------------------------------------
    template <>
    inline bool ScriptArgs::Get(
      uint8_t idx, 
      bool def, 
      if_n_number_and_enum<bool>*) const
    {
      return 
        GetImpl<bool, ScriptBoolean, ScriptValueTypes::kBoolean>(idx, def);
    }

    //--------------------------------------------------------------------------
    template <>
    inline foundation::String ScriptArgs::Get(
      uint8_t idx, 
      foundation::String def, 
      if_n_number_and_enum<foundation::String>*) const
    {
      return 
        GetImpl<
        foundation::String, 
        ScriptString, 
        ScriptValueTypes::kString>(idx, def);
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline T* ScriptArgs::GetPointer(uint8_t idx) const
    {
      ScriptValue* v = arguments_.at(idx).get();

      if (v->type() != ScriptValueTypes::kObject)
      {
        return nullptr;
      }

      ScriptObject* o = static_cast<ScriptObject*>(v);

      void* ptr = nullptr;
      if ((ptr = o->GetPointer(T::ScriptName())) == nullptr)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kScript,
          foundation::LogSeverity::kError,
          "Type mismatch for pointer at argument index '{0}'",
          static_cast<uint32_t>(idx)
          );

        return nullptr;
      }

      return reinterpret_cast<T*>(ptr);
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void ScriptArgs::AddReturnValue(T value)
    {
      if (return_ptr_.ptr != nullptr)
      {
        return_ptr_ = ReturnPtr{ nullptr, "" };
      }

      return_value_ = ScriptValue::From<T>(value);
    }

    //--------------------------------------------------------------------------
    template <typename T>
    void ScriptArgs::AddReturnPointer(T* ptr)
    {
      if (return_value_ != nullptr)
      {
        return_value_.reset();
      }

      return_ptr_ = ReturnPtr{ ptr, T::ScriptName() };
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline T* ScriptArgs::GetSelf() const
    {
      if (callee_ == nullptr)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kScript,
          foundation::LogSeverity::kError,
          "Invalid 'self' for a script function, callee was nullptr"
          );
      }
      return reinterpret_cast<T*>(callee_);
    }

    //--------------------------------------------------------------------------
    template <typename T, typename Y, ScriptValueTypes U>
    inline T ScriptArgs::GetImpl(uint8_t idx, T def) const
    {
      ScriptValue* val = GetArgument(idx);

      if (val != nullptr && val->type() == U)
      {
        return static_cast<Y*>(val)->value();
      }
      
      return def;
    }
  }
}