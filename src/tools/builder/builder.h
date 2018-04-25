#pragma once

namespace snuffbox
{
  namespace builder
  {
    /**
    * @brief The builder to compile all assets into a format readable by
    *        the engine
    *
    * Assets have custom compilers to build files based on their file extension.
    * These files can then be decompiled with the same compilers by the engine
    * itself. This makes for a workflow where the builder builds a file, outputs
    * it to a build directory and rebuilds it accordingly when the source file
    * has changed.
    *
    * The builder will retain the same folder structure as the original source
    * directory's tree.
    *
    * This is a multi-threaded process, where every core of the CPU can build
    * its own file. There is no job system attached, as it's simply input ->
    * output. There should be no data contentation.
    *
    * @see BuildScheduler
    * @see BuildJob
    *
    * @author Daniel Konings
    */
    class Builder
    {

    };
  }
}