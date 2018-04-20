#pragma once

#include <string>
#include <fstream>
#include <cstddef>
#include <cinttypes>

namespace snuffbox
{
  namespace bin2h
  {
    /**
    * @brief Used to write files into an output directory as binary headers
    *
    * The binary headers include the binary data as a uint8_t array and
    * their size_t length.
    *
    * @author Daniel Konings
    */
    class BinaryWriter
    {

    public:

      /**
      * @brief Writes the file as binary header to the specified output
      *        directory.
      *
      * The output file name will be the original file name, but with a .h
      * extension instead. The definition will also contain a relative path
      * to create the virtual file system mapping.
      * 
      * Please note there is currently no support for files with 
      * duplicate names. The last file will simply overwrite the previous file.
      *
      * @param[in] file The path to the file to convert
      * @param[in] relative The relative path to store the file under, without
      *                     file name
      * @param[in] out_dir The root output directory
      */
      static bool WriteFile(
        const char* file, 
        const char* relative, 
        const char* out_dir);

    protected:

      /**
      * @brief Retrieves a file name without its path and extension
      *
      * @param[in] path The full or relative path to the file
      *
      * @return The converted file name
      */
      static std::string GetFileName(const char* path);

      /**
      * @brief Retrieves the variable name of a file name
      *
      * Any spaces are converted to underscores, bin2h_ is prepended and
      * illegal symbols are removed
      *
      * @param[in] file The file name, without extension and path
      *
      * @return The converted name
      */
      static std::string GetVariableName(const std::string& file);

      /**
      * @brief Writes the definitions for a file to a binary header
      *
      * @param[in] buffer The binary data of the file
      * @param[in] len The length of the binary data
      * @param[in] name The name of the file, without extension
      * @param[in] relative The relative path, without file name
      * @param[in] fin The current opened file stream for the file
      */
      static void WriteDefinitions(
        uint8_t* buffer, 
        size_t len,
        const std::string& name,
        const char* relative,
        std::fstream& fin);
    };
  }
}