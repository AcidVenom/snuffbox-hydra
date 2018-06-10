#pragma once

#include "tools/compilers/compiler.h"

namespace snuffbox
{
  namespace compilers
  {
    /**
    * @brief The scene compiler that compiles serialized JSON data into
    *        a binary format, which the decompiler can convert back to
    *        a usable scene
    *
    * @remarks A binary format is the most feasible, but for now the compiler
    *          simply outputs the JSON into the build directory, with
    *          a magic number prefixed that it is indeed a scene
    *
    * @author Daniel Konings
    */
    class SceneCompiler : public ICompiler
    {

    public:

      /**
      * @brief Default constructor
      */
      SceneCompiler();

    protected:

      /**
      * @see ICompiler::CompileImpl
      */
      bool CompileImpl(foundation::File& file) override;

      /**
      * @see ICompiler::DecompileImpl
      */
      bool DecompileImpl(foundation::File& file) override;
    };
  }
}