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
      * preprocessor directives.
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
      * @brief Writes a comment on that the file was generated and should
      *        not be modified manually, on top of some meta information
      *
      * @param[in] defs The class definition data, used for the meta section
      */
      void WriteComment(const std::vector<ClassDefinition>& defs);

      /**
      * @brief Write all class definitions into the source file in the
      *        required format so the engine can use it
      *
      * The header can be an absolute path, as the file is regenerated on
      * each machine anyway.
      *
      * @param[in] header Full path to the original header file
      * @param[in] defs The list of definitions to write
      *
      * @return Was the writing a success?
      */
      bool WriteAll(
        const std::string& header, 
        const std::vector<ClassDefinition>& defs);

    private:

      std::ofstream output_; //!< The output stream
    };
  }
}