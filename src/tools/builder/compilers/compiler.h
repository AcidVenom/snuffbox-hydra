#pragma once

#include <foundation/io/path.h>
#include <foundation/containers/vector.h>

#include <cinttypes>
#include <cstddef>

namespace snuffbox
{
  namespace builder
  {
    /**
    * @brief The compiler interface for every compiler to use
    *
    * Compilers are scheduled for compilation from the BuildScheduler.
    * A BuildJob is instantiated along with a compiler to prevent any data
    * contentation.
    *
    * @author Daniel Konings
    */
    class ICompiler
    {

    public:

      /**
      * @brief Construct a compiler with its supported extensions
      *
      * @remarks The extensions should be without the preceding '.'
      *
      * @param[in] supported_extensions The supported extensions for the
      *                                 ICompiler::Compile call of this compiler
      *
      * @param[in] out_extension The output extension for this compiler
      */
      ICompiler(
        const foundation::Vector<foundation::String>& supported_extensions, 
        const foundation::String& out_extension);

      /**
      * @brief Compiles a file from a provided file path
      *
      * The file path is first checked for the appropriate extension, before
      * actually starting the compilation. The base compiler interface also
      * checks if the file actually exists, along with checking if the path
      * is a file and not a directory.
      *
      * The derived class should override ICompiler::CompileImpl to actually
      * compile the file if this function passes that check.
      *
      * The compilation implementation should simply change the underlying
      * data of the compiler. The actual base interface will write the file
      * to disk.
      *
      * @param[in] path The path to the file to compile
      *
      * @return Was the compilation a success?
      */
      bool Compile(const foundation::Path& path);

      /**
      * @brief Decompiles a file from a provided file path
      *
      * The file path is first checked for the appropriate extension, before
      * actually starting the decompilation. The base compiler interface also
      * checks if the file actually exists, along with checking if the path
      * is a file and not a directory.
      *
      * The derived class should override ICompiler::DecompileImpl to actually
      * decompile the file if this function passes that check.
      *
      * The decompilation implementation should load all necessary data into
      * memory, where the base compiler can use that data to interact with
      * the engine side of the application.
      *
      * @param[in] path The path to the file to decompile
      *
      * @return Was the decompilation a success?
      */
      bool Decompile(const foundation::Path& path);

      /**
      * @brief Retrieves the current data of the compiler along with its size
      *
      * @param[out] size The size of the data in the compiler
      *
      * @return The data currently contained in this compiler
      */
      const uint8_t* Data(size_t* size) const;

    protected:

      /**
      * @brief Checks if the extension of a file path is listed in the
      *        supported extensions of this compiler
      *
      * The path is checked for its file extension based on compilation or
      * decompilation, along with a check to make sure the path is a file
      * and not a directory.
      *
      * @param[in] path The path to check
      * @param[in] compile Are we compiling or decompiling?
      *
      * @return Is the path valid for use?
      */
      bool IsSupported(const foundation::Path& path, bool compile) const;

      /**
      * @see ICompiler::Compile
      */
      virtual bool CompileImpl(const foundation::Path& path) = 0;

      /**
      * @see ICompiler::Decompile
      */
      virtual bool DecompileImpl(const foundation::Path& path) = 0;

      /**
      * @brief Sets the current error message of this compiler
      *
      * This function should be called from within ICompiler::CompileImpl or
      * ICompiler::DecompileImpl to send any error messages back to the user 
      * when a compilation or decompilation fails.
      *
      * @param[in] error The error message to set
      */
      void set_error(const foundation::String& error);

    private:

      /**
      * @brief The supported extensions of this compiler
      */
      foundation::Vector<foundation::String> supported_extensions_;

      /**
      * @brief The out extension for a compiled file
      */
      foundation::String out_extension_;

      foundation::String error_; //!< The current error message of this compiler

      uint8_t* data_; //!< The data currently contained in the compiler
      size_t size_; //!< The size of the contained data
    };
  }
}