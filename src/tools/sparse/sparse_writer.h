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

    private:

      std::ofstream output_; //!< The output stream
      unsigned int indent_; //!< How much are we currently indented?
      unsigned int namespaces_; //!< How much namespaces have we entered?
    };
  }
}