#pragma once

namespace snuffbox
{
  namespace scripting
  {
    class ScriptArgs;

    /**
    * @brief The different type of script values that exist
    */
    enum class ScriptValueTypes
    {
      kNull, //!< Null/undefined value
      kBoolean, //!< Boolean values
      kNumber, //!< Numerical values, they are always double
      kString, //!< String values stored as an EASTL string
      kObject, //!< Object values by key/value pairs
      kArray, //!< Array values by indices
    };

    /**
    * @brief A type definition for a scripting free-function pointer
    *
    * @param[in] args The arguments that were parsed from the script state
    *
    * @return Was the call a success?
    */
    using ScriptFunction = bool(*)(ScriptArgs& args);

    /**
    * @brief Used to register functions to the script state by name
    *
    * This is to be used in conjuction with the ScriptRegister to expose
    * C++ classes to the scripting environment.
    *
    * @author Daniel Konings
    */
    struct ScriptFunctionRegister
    {
      const char* name; //!< The name of the function
      ScriptFunction func; //!< The actual function
    };

    /**
    * @brief Used to register enumerator values to the script state by name
    *
    * This is used in conjuction with the ScriptRegister to expose C++ enums
    * to the scripting environment. Please note enumerators can only be declared
    * as "enum class MyEnum" or "enum MyEnum" when ran through the script
    * parsing utility.
    *
    * @author Daniel Konings
    */
    struct ScriptEnumRegister
    {
      const char* key; //!< The key of the enumerator value
      int value; //!< The actual value
    };

    /**
    * @brief Used to register an enumerator to the script state by name
    *
    * @see ScriptEnumRegister
    *
    * @author Daniel Konings
    */
    struct ScriptEnum
    {
      const char* name; //!< The name of the enumerator in script
      ScriptEnumRegister* keys; //!< The key/value pairs
    };
  }
}
