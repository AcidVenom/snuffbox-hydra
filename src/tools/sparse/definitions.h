#pragma once

#include <string>
#include <vector>

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

      TypeDefinition ret_val; //!< The type of the return value
      std::vector<TypeDefinition> arguments; //!< The arguments of the function
    };

    /**
    * @brief Used to store class information of the current class
    *        being parsed
    *
    * This struct is created every time the parser finds a class in "ParseRoot"
    * and "ParseClass" is called. The ClassDefinition will be stored in 
    * JsonHeaderParser::definitions_ for later use.
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
  }
}