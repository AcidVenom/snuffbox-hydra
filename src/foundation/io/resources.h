#pragma once

#include "foundation/io/path.h"
#include "foundation/containers/map.h"

#include <cinttypes>

#define SNUFF_MAKE_RESOURCE(name)                                              \
snuffbox::foundation::Resources::Register(                                     \
bin2h_##name, bin2h_##name##_size, bin2h_##name##_path);

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief Used as a virtual file system to store binary data 
    *        in the executable
    *
    * The file system can be accessed through virtual paths, starting
    * with the virtual path prefix. The files should be converted to binary
    * data using Snuffbox's resource compiler; snuffbox-bin2h.
    *
    * The data is stored in application memory to be used throughout the
    * application and file access immediately fills a File::buffer_. There
    * is a direct 'file' to pointer mapping, so no memory needs to be
    * copied.
    *
    * @see File
    *
    * @author Daniel Konings
    */
    class Resources
    {

      friend class File;

    public:

      /**
      * @brief Registers a buffer with a provided size under a virtual path
      *
      * @param[in] buffer The buffer to register
      * @param[in] size The size of the buffer
      * @param[in] path The path to reference the buffer with, excluding the
      *                 virtual path prefix
      */
      static void Register(
        const uint8_t* buffer, 
        size_t size, 
        const Path& path);

      /**
      * @brief Make sure all memory of the resources system is cleared
      *        before the memory allocators get shut down
      */
      static void Shutdown();

    protected:

      /**
      * @brief Contains data of a resource
      *
      * @author Daniel Konings
      */
      struct ResourceData
      {
        const uint8_t* buffer; //!< The buffer of the resource
        size_t size; //!< The size of the resource
      };

      /**
      * @brief Retrieves an internal resource from a virtual path
      *
      * @param[in] path The virtual path to load the resource from, without
      *                 the virtual prefix
      *
      * @return The resource data, or nullptr if it doesn't exist
      */
      static const ResourceData* GetResource(const Path& path);

    private:

      /**
      * @brief A mapping from a file path to a resource
      */
      static UMap<String, ResourceData> resources_;
    };
  }
}