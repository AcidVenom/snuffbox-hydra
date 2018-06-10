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