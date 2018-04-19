#pragma once

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

    };
  }
}