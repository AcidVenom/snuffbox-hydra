#pragma once

#include <string>
#include <rapidjson/document.h>

namespace snuffbox
{
  namespace sparse
  {
    /**
    * @brief The class that handles all parsing functionality from a .json
    *        generated by header-parser
    *
    * @author Daniel Konings
    */
    class Parser
    {

    public:

      /**
      * @brief Default constructor
      */
      Parser();

      /**
      * @brief Parse the contents of a .json and output the parse result
      *        to a specific directory
      *
      * The parser automatically logs errors to std::cerr, so that they can be
      * viewed during compilation.
      *
      * @param[in] input The path to the .json from the current 
      *                  working directory
      * @param[in] output The path to the file to output in the binary directory
      *
      * @return Was the parsing a success?
      */
      bool Parse(const std::string& input, const std::string& output);

    protected:

      /**
      * @brief Parses the .json document after it has been parsed by
      *        rapidjson
      *
      * @param[in] doc The rapidjson document to parse
      *
      * @return Was the parsing a success?
      */
      bool ParseDocument(const rapidjson::Document& doc);

      /**
      * @brief Parses a .json array and finds all respective members to parse
      *
      * @param[in] arr The array to parse
      *
      * @return Was the parsing a success?
      */
      bool ParseArray(
        const rapidjson::GenericArray<true, rapidjson::Value>& arr);

      /**
      * @brief Retrieves the "type" field from a .json value as a string
      *
      * @param[in] val The value to retrieve the type from
      *
      * @return The type name, or nullptr if it doesn't exist
      */
      const char* GetType(const rapidjson::Value& val);

      /**
      * @brief Enters a namespace and appends it to the current namespace
      *
      * The parsing is then progressed from the 'members' field of the namespace
      *
      * @param[in] ns The namespace value to enter
      *
      * @return Was the parsing a success?
      */
      bool EnterNamespaces(const rapidjson::Value& ns);

      /**
      * @brief Parses a class and all its members
      *
      * @param[in] cl The class value to enter
      *
      * @return Was the parsing a success?
      */
      bool ParseClass(const rapidjson::Value& cl);

      /**
      * @brief Checks if a class value is derived from a specified class
      *        name
      *
      * The name is check without namespaces, there is no need to specify them
      *
      * @param[in] name The name of the base class
      * @param[in] cl The class value
      *
      * @return Is the class derived from the specified base class?
      */
      bool DerivesFrom(const char* name, const rapidjson::Value& cl);

    private:

      std::string current_namespace_; //!< The current namespace
      unsigned int num_namespaces_; //!< The number of namespaces entered
    };
  }
}