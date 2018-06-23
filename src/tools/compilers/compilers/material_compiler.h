#pragma once

#include "tools/compilers/compilers/compiler.h"

namespace snuffbox
{
  namespace compilers
  {
    /**
    * @brief The material compiler that compiles serialized material data
    *        into a build format
    *
    * @author Daniel Konings
    */
    class MaterialCompiler : public ICompiler
    {

    public:

      /**
      * @brief Default constructor
      */
      MaterialCompiler();

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