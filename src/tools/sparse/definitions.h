#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace snuffbox
{
  namespace sparse
  {
    /**
    * @brief The different reference types that exist
    */
    enum class RefType
    {
      kReference, //!< e.g. int& MyFunction(float& c)
      kPointer, //!< e.g. int* MyFunction(float* c)
      kLiteral //!< e.g. int MyFunction(float c)
    };

    /**
    * @brief Used to store type information of argument types and return
    *        types of function definitions
    *
    * @see FunctionDefinition
    *
    * @author Daniel Konings
    */
    struct TypeDefinition
    {
      std::string name; //!< The full C++ typename, including any namespaces
      RefType ref_type; //!< The reference type of the type
    };

    /**
    * @brief Used to store argument information for a function definition
    *
    * @author Daniel Konings
    */
    struct ArgumentDefinition
    {
      std::string name; //!< The name of the argument
      TypeDefinition type; //!< The type of the argument
    };

    /**
    * @brief Used to store function information of each member function
    *        a class contains
    *
    * This struct is created every time the parser finds a function in
    * "ParseClassMembers" and "ParseFunction" is called. 
    * Then the function is added to the current class being parsed.
    *
    * @see ClassDefinition
    *
    * @author Daniel Konings
    */
    struct FunctionDefinition
    {
      std::string name; //!< The function name
      bool is_static; //!< Is this function static?
      bool is_custom; //!< Is there a custom implementation specified?

      TypeDefinition ret_val; //!< The type of the return value

      /**
      * @brief The arguments of the function
      */
      std::vector<ArgumentDefinition> arguments;
    };

    /**
    * @brief Used to store class information of the current class
    *        being parsed
    *
    * This struct is created every time the parser finds a class in "ParseRoot"
    * and "ParseClass" is called. The ClassDefinition will be stored in 
    * JsonHeaderParser::class_definitions_ for later use.
    *
    * @author Daniel Konings
    */
    struct ClassDefinition
    {
      std::string c_name; //!< The C++ class name
      std::string s_name; //!< The script name
      std::string ns; //!< The namespace string including '::'

      std::vector<FunctionDefinition> functions; //!< The functions of the class
    };

    /**
    * @brief Used to store enumerator information for the current .json being
    *        parsed
    *
    * This struct is created every time the parser finds an enum in "ParseEnum"
    * and "ParseEnum" is called. It is noteworthy enumerators can be explicitly
    * specified like you'd expect in C++, but cannot be any values other than
    * integer values. 
    * The enumerators are stored in JsonHeaderParser::enum_definitions_ 
    * for later use.
    *
    * @author Daniel Konings
    */
    struct EnumDefinition
    {
      std::string name; //!< The name of the enum
      std::unordered_map<std::string, int> values; //!< The values of the enum
      std::string nested; //!< The namespace and class string, including '::'
      std::string ns; //!< The namespace string including '::'
    };

    /**
    * @brief A structure to hold both class and enumerator defintions
    *        for a parsed .json document
    *
    * @author Daniel Konings
    */
    struct ScriptDefinitions
    {
      std::vector<ClassDefinition> classes; //!< All class definitions
      std::vector<EnumDefinition> enums; //!< All enum definitions
    };
  }
}