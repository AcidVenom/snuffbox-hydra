#pragma once

#include "tools/compilers/definitions/magic.h"

#include <foundation/io/file.h>
#include <foundation/containers/vector.h>

#include <cinttypes>
#include <cstddef>

namespace snuffbox
{
  namespace compilers
  {
    /**
    * @brief The compiler interface for every compiler to use
    *
    * Compilers are scheduled for compilation from the BuildScheduler.
    * A BuildJob is instantiated along with a compiler to prevent any data
    * contentation.
    *
    * Allocated data should only be freed in this base class and the default
    * memory allocator should be used for consistency. Due to the nature of
    * the memory interface it is however possible to use a different allocator
    * if necessary.
    *
    * @author Daniel Konings
    */
    class ICompiler
    {

    public:

      /**
      * @brief Default constructor
      */
      ICompiler();

      /**
      * @brief Compiles a file from a provided file path
      *
      * The base compiler interface checks if the file actually exists, 
      * along with checking if the path is a file and not a directory.
      *
      * The derived class should override ICompiler::CompileImpl to actually
      * compile the file if this function passes that check.
      *
      * The compilation implementation should simply change the underlying
      * data of the compiler. The actual builder will write the file
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
      * The base compiler interface checks if the file actually exists, 
      * along with checking if the path is a file and not a directory.
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

      /**
      * @return The current error message of the compiler, if any
      */
      const foundation::String& error() const;

    protected:

      /**
      * @brief Opens a file, but checks first if the path meets all requirements
      *        for either compilation or decompilation
      *
      * @param[in] path The path to the file
      * @param[in] compile Are we compiling or decompiling?
      * @param[out] file The loaded file if the path was valid
      *
      * @return Was the path a valid path for this compiler?
      */
      bool OpenFile(
        const foundation::Path& path, 
        bool compile, 
        foundation::File* file);

      /**
      * @brief Used by the derived classes to set the underlying data
      *        of the compiler
      *
      * @remarks This frees any previously set data
      *
      * @param[in] data The data to set
      * @param[in] size The size of the data
      */
      void SetData(uint8_t* data, size_t size);

      /**
      * @brief Clears the underlying buffer if there is currently data set
      */
      void Clear();

      /**
      * @brief Allocates a buffer with a magic number as header
      *
      * @param[in] magic The magic number to put at the start of the block
      * @param[in] size The size to allocate after the magic number
      * @param[out] block The pointer to the address after the magic number
      * @param[out] total_size The total size of the newly allocated block
      *
      * @return The pointer to the first address of the total block
      */
      static uint8_t* AllocateWithMagic(
        FileHeaderMagic magic, 
        size_t size, 
        uint8_t** block,
        size_t* total_size);

      /**
      * @brief Retrieves the magic number header from a memory block
      *
      * @param[in] buffer The buffer to retrieve the magic number from
      * @param[in] size The size of the buffer
      * @param[out] block The memory block after the magic number header
      * @param[out] block_size The size of the data after the header
      *
      * @return The FileHeaderMagic value found at the start of the block
      */
      static FileHeaderMagic AllocateWithoutMagic(
        const uint8_t* buffer,
        size_t size,
        uint8_t** block,
        size_t* block_size);

      /**
      * @brief Used to read source file data with a magic number in the header
      *
      * @author Daniel Konings
      */
      struct SourceFileData
      {
        FileHeaderMagic magic; //!< The expected magic number
        uint8_t* block; //!< The pointer to the block after the header
        uint8_t* data; //!< The pointer to all data, including the header
        size_t length; //!< The length of block after the header
        size_t total_size; //!< The length of the file
      };

      /**
      * @brief Used to read build file data with a magic number in the header
      *
      * @author Daniel Konings
      */
      struct BuildFileData
      {
        FileHeaderMagic magic; //!< The magic number to write
        uint8_t* block; //!< The pointer to the block after the header
        size_t length; //!< The length of the block after the header
      };

      /**
      * @brief Allocates the data of a file after a magic header using
      *        a file data description
      *
      * @param[in] file The file to read
      * @param[in|out] fd The file data
      *
      * @return Was the file valid and did the magic number match?
      */
      static bool AllocateSourceFile(
        foundation::File& file, 
        SourceFileData* fd);

      /**
      * @brief Allocates user data after a magic header using a file
      *        data description
      *
      * @param[in] data The data to allocate
      * @param[in] len The length of the data
      * @param[in|out] fd The file data
      *
      * @return Was the data allocated succesfully?
      */
      static bool AllocateSourceFile(
        const uint8_t* data, 
        size_t len, 
        SourceFileData* fd);

      /**
      * @brief Reads a build file with a magic number header as a file data
      *        description
      *
      * @param[in] file The file to read
      * @param[out] fd The file data
      *
      * @return Were we able to open the file and read the data?
      */
      static bool ReadBuildFile(foundation::File& file, BuildFileData* fd);

      /**
      * @see ICompiler::Compile
      *
      * @param[in] file The opened file from the compilation call
      */
      virtual bool CompileImpl(foundation::File& file) = 0;

      /**
      * @see ICompiler::Decompile
      *
      * @param[in] file The opened file from the decompilation call
      */
      virtual bool DecompileImpl(foundation::File& file) = 0;

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

    public:

      /**
      * @brief Virtual destructor
      */
      virtual ~ICompiler();

    private:

      foundation::String error_; //!< The current error message of this compiler

      uint8_t* data_; //!< The data currently contained in the compiler
      size_t size_; //!< The size of the contained data
    };
  }
}