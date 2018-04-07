#pragma once

#include <foundation/containers/string.h>

#include <EASTL/type_traits.h>

#include <cstdlib>
#include <cstring>
#include <cctype>

namespace snuffbox
{
  namespace engine
  {
    class CVarService;

    /**
    * @brief The base class for every CVar value to use
    *
    * Supported types are:
    * - Booleans
    * - Strings
    * - Numbers
    *
    * @author Daniel Konings
    */
    class CVarValue
    {
      
      friend CVarService;

    public:

      /**
      * @brief Used with SFINAE to only enable functions for double values
      *
      * @tparam T The type to check
      * @tparam C Should the type be a double or not?
      */
      template <typename T, bool C>
      using is_double =
        typename eastl::enable_if_t<eastl::is_same<T, double>::value == C>;

      /**
      * @brief Used for ranged number values in CVars of a number type
      *
      * @author Daniel Konings
      */
      struct Range
      {
        /**
        * @brief Default constructor
        */
        Range();

        double min; //!< The minimum value
        double max; //!< The maximum value

        bool has_min; //!< Does this value have a minimum?
        bool has_max; //!< Does this value have a maximum?
      };

      /**
      * @brief Construct a CVar by its name
      *
      * @param[in] name The name of the CVar to create
      * @param[in] description The description of this CVar value
      *
      * @remarks The specified name will be used to retrieve the CVar from its
      *          respective manager. The stored name is without the
      *          '-' specifier.
      */
      CVarValue(const char* name, const char* description);

    protected:

      /**
      * @brief Used to parse the CVar value from the CLI string, which followed
      *        the name of a defined CVar
      *
      * This function is called when the name of a registered CVarValue is found
      * in the command line of the application. The value is parsed by string
      * and is done in the derived classes of this base class.
      *
      * @param[in] value The stringified value to parse
      */
      virtual bool ParseFrom(const char* value) = 0;

      /**
      * @brief Logs the usage with error severity if the CVar string value 
      *        assigned was not able to be parsed
      */
      virtual const char* Usage() = 0;

      /**
      * @brief Sets the CVar value based on a string value
      *
      * @see CVarValue::ParseFrom
      *
      * @param[in] value The string value to parse
      */
      void Set(const char* value);

    public:

      /**
      * @return The name of this CVar value
      */
      const foundation::String& name() const;

      /**
      * @return The description of this CVar value
      */
      const foundation::String& description() const;

    private:

      foundation::String name_; //!< The name of this CVar value
      foundation::String description_; //!< The description of this CVar value
    };

    //--------------------------------------------------------------------------


    /**
    * @brief The base class declaration to specialize with SFINAE
    *
    * This is used to make sure that only CVar values with T = double have a
    * range value. This would be non-sensical for any value but numerical
    * values.
    *
    * @see CVarValue::is_double
    *
    * @author Daniel Konings
    */
    template <typename T, typename Enabled = void>
    class CVarBase;

    /**
    * @brief The base class for a range-incompatible value
    *
    * @see CVarBase
    *
    * @author Daniel Konings
    */
    template <typename T>
    class CVarBase<T, CVarValue::is_double<T, false>> : public CVarValue
    {
      friend CVarService;
      
    protected:

      /**
      * @see CVarValue::CVarValue
      */
      CVarBase(const char* name, const char* description) :
        CVarValue(name, description)
      {

      }
    };

    /**
    * @brief The base class for a range-compatible value
    *
    * @see CVarBase
    *
    * @author Daniel Konings
    */
    template <typename T>
    class CVarBase<T, CVarValue::is_double<T, true>> : public CVarValue
    {
      friend CVarService;

    protected:

      /**
      * @see CVarValue::CVarValue
      */
      CVarBase(const char* name, const char* description) :
        CVarValue(name, description)
      {

      }

      /**
      * @brief Enable the setting of range for double values
      *
      * @see CVar::is_double
      *
      * This function fails if the minimum value and maximum value is set,
      * but the minimum is greater than the maximum. On failure, the default
      * Range is set.
      *
      * @param[in] range The range to set
      *
      * @return Was a valid range set?
      */
      bool SetRange(const CVarValue::Range& range)
      {
        if (range.has_min == true && range.has_max && range.min > range.max)
        {
          range_ = CVarValue::Range();
          return false;
        }

        range_ = range;
        return true;
      }

      CVarValue::Range range_; //!< The range of this
    };

    //--------------------------------------------------------------------------
    
    /**
    * @brief The actual typed CVar values, which contain the expressions for
    *        parsing the value from the command line.
    *
    * @tparam T The type to assign under this CVar
    *
    * Technically a CVar of every template argument could be created by defining
    * a parsing expression for the respective type. This would allow for parsing
    * of arbritrary data inputted from the CLI.
    *
    * @author Daniel Konings
    */
    template <typename T>
    class CVar : public CVarBase<T>
    {

      friend CVarService;

    public:

      /**
      * @see CVarValue::CVarValue
      *
      * @brief Registers a typed CVar, with a default value of that type
      *
      * @param[in] def The default value if this CVar is not set from the
      *                command line
      */
      CVar(const char* name, const char* description, const T& def);

    protected:

      /**
      * @see CVarValue::ParseFrom
      */
      bool ParseFrom(const char* value) override;
    
      /**
      * @see CVarValue::Usage 
      */
      const char* Usage() override;

    public:

      /**
      * @brief Sets the value of the CVar from the type it was created with
      *
      * @param[in] value The value to set
      */
      void set_value(const T& value);

      /**
      * @return The value contained in this CVar value
      */
      const T& value() const;

    private:

      T value_; //!< The value contained in this CVar value
    };

    //--------------------------------------------------------------------------
    template <typename T>
    inline CVar<T>::CVar(
      const char* name, 
      const char* description, 
      const T& def) 
      :
      CVarBase<T>(name, description),
      value_(def)
    {

    }

    //--------------------------------------------------------------------------
    template <>
    inline bool CVar<bool>::ParseFrom(const char* value)
    {
      size_t len = strlen(value);

      if (len == 0 || len > 5)
      {
        return false;
      }

      char buffer[6];

      for (size_t i = 0; i < len; ++i)
      {
        buffer[i] = static_cast<char>(tolower(value[i]));
      }

      buffer[len] = '\0';
      
      if (strcmp(buffer, "true") == 0)
      {
        value_ = true;
        return true;
      }
      
      if (strcmp(buffer, "false") == 0)
      {
        value_ = false;
        return true;
      }

      return false;
    }

    //--------------------------------------------------------------------------
    template <>
    inline const char* CVar<bool>::Usage()
    {
      return "Boolean (case insensitive): true | false";
    }

    //--------------------------------------------------------------------------
    template <>
    inline bool CVar<double>::ParseFrom(const char* value)
    {
      char* result;
      double v = strtod(value, &result);

      if ((*result) != 0)
      {
        return false;
      }

      if (range_.has_min == true && v < value_)
      {
        return false;
      }

      if (range_.has_max == true && v > value_)
      {
        return false;
      }

      value_ = v;

      return true;
    }

    //--------------------------------------------------------------------------
    template <>
    inline const char* CVar<double>::Usage()
    {
      return 
        "Number: 0x* | * | *.** | *e* | *e-*";
    }

    //--------------------------------------------------------------------------
    template <>
    inline bool CVar<foundation::String>::ParseFrom(const char* value)
    {
      value_ = value;
      return true;
    }

    //--------------------------------------------------------------------------
    template <>
    inline const char* CVar<foundation::String>::Usage()
    {
      return "String: This should always work";
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void CVar<T>::set_value(const T& value)
    {
      value_ = value;
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline const T& CVar<T>::value() const
    {
      return value_;
    }
  }
}