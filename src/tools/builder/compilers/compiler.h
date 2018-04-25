#pragma once

#include <foundation/io/path.h>
#include <foundation/containers/vector.h>

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
      * checks if the file actually exists.
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
      * checks if the file actually exists.
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

    protected:

      /**
      * @brief Sets the current error message of this compiler
      *
      * This function should be called from within ICompiler::CompileImpl or
      * ICompiler::DecompileImpl to send any error messages back to the user 
      * when a compilation or decompilation fails.
      *
      * @param[in] error The error message to set
      */
      void SetError(const foundation::String& error);

      /**
      * @brief Checks if the extension of a file path is listed in the
      *        supported extensions of this compiler
      */
      bool IsSupported(const foundation::Path& path);

      /**
      * @see ICompiler::Compile
      */
      virtual bool CompileImpl(const foundation::Path& path) = 0;

      /**
      * @see ICompiler::Decompile
      */
      virtual bool DecompileImpl(const foundation::Path& path) = 0;

    private:

      /**
      * @brief The supported extensions of this compiler
      */
      foundation::Vector<foundation::String> supported_extensions_;

      /**
      * @brief The out extension for a compiled file
      */
      foundation::String out_extension_;
    };
  }
}