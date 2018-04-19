#pragma once

#include "tools/sparse/definitions.h"

#include <string>
#include <fstream>

namespace snuffbox
{
  namespace sparse
  {
    class JsonHeaderParser;

    /**
    * @brief Writes the definitions from JsonHeaderParser as a C++ source file
    *        for the scripting environment to compile
    *
    * @author Daniel Konings
    */
    class SparseWriter
    {
      
    public:

      /**
      * @brief Default constructor
      */
      SparseWriter();

      /**
      * @brief Writes the output of a parser to the C++ source file definition
      *
      * The header path is required for the output file to include it in its
      * preprocessor directives. The only error this can really raise is
      * that the output file cannot be opened, or that the initial data was
      * invalid.
      *
      * @param[in] parser The parser that parsed an input header file
      * @param[in] header The path of the original header
      * @param[in] output The output path to write the generated file to
      *
      * @return Was the writing a success?
      */
      bool Write(
        JsonHeaderParser* parser, 
        const std::string& header,
        const std::string& output);

    protected:

      /**
      * @brief Writes a line with the current indentation to the output file
      *
      * @param[in] line The line to write
      */
      void WriteLine(const char* line);

      /**
      * @brief Writes a comment on that the file was generated and should
      *        not be modified manually, on top of some meta information
      *
      * @param[in] defs The definition data, used for the meta section
      */
      void WriteComment(const ScriptDefinitions& defs);

      /**
      * @brief Write all class and enum definitions into the source file in the
      *        required format so the engine can use it
      *
      * The header can be an absolute path, as the file is regenerated on
      * each machine anyway.
      *
      * @param[in] header Full path to the original header file
      * @param[in] defs The list of definitions to write
      */
      void WriteAll(
        const std::string& header, 
        const ScriptDefinitions& defs);

      /**
      * @brief Writes a class's function bodies to the output file
      *
      * @param[in] d The class definition to write the functions of
      */
      void WriteClass(const ClassDefinition& d);

      /**
      * @brief Writes the function register for a class
      *
      * @param[in] d The class definition
      */
      void WriteFunctionRegister(const ClassDefinition& d);

      /**
      * @brief Writes an enum's function bodies to the output file
      *
      * @param[in] d The enum definition to write the key/value pairs of
      */
      void WriteEnum(const EnumDefinition& d);

      /**
      * @brief Enters the current namespaces before writing
      *
      * This makes sure we're at the right indentation and that the
      * function bodies are wrapped in the corresponding namespace.
      *
      * @param[in] ns The namespace to enter
      */
      void EnterNamespaces(const std::string& ns);

      /**
      * @brief Close off the current namespaces
      *
      * This resets the current indentation and writes the closing brackets
      * of each namespace entered in SparseWriter::EnterNamespaces
      */
      void ExitNamespaces();

      /**
      * @brief Formats a function name to be a C-style free function
      *
      * @param[in] func The function name
      * @param[in] cl The class this function is nested in
      */
      std::string FormatFunctionName(
        const std::string& func, 
        const std::string& cl);

      /**
      * @brief Writes the argument check
      *
      * @param[in] args The arguments of the current function
      */
      void WriteArgumentCheck(const std::vector<ArgumentDefinition>& args);

      /**
      * @brief Write the "self" declaration and check if the function is
      *        non-static
      *
      * @param[in] d The function definition
      * @param[in] cl The class of the "self" pointer
      */
      void WriteSelf(const FunctionDefinition& d, const ClassDefinition& cl);
      
      /**
      * @brief Writes the list of arguments and their value retrieval to the
      *        function body after the check and self-retrieval
      *
      * @param[in] d The function definition
      */
      void WriteArgumentList(const FunctionDefinition& d);

      /**
      * @brief Writes the actual function call with the retrieved arguments
      *
      * @param[in] d The function definition
      * @param[in] cl The class definition
      */
      void WriteFunctionCall(
        const FunctionDefinition& d, 
        const ClassDefinition& cl);

      /**
      * @brief Writes the static function to be registered by the script state
      *
      * @param[in] d The function definition
      * @param[in] cl The class definition
      */
      void WriteFunction(
        const FunctionDefinition& d, 
        const ClassDefinition& cl);

    private:

      std::ofstream output_; //!< The output stream
      unsigned int indent_; //!< How much are we currently indented?
      unsigned int namespaces_; //!< How much namespaces have we entered?

      /**
      * @brief Shorthand for a map of argument formats based on a supported
      *        type
      */
      using ArgFormats = std::unordered_map<std::string, char>;

      /**
      * @brief The different format characters that exist per supported type
      *
      * @remarks Argument formats are only checked for containment, so that
      *          they don't clash with namespaces when checking for a fully
      *          qualified string
      */
      static ArgFormats kFormats_;
    };
  }
}