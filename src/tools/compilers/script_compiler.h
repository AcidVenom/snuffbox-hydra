#pragma once

#include "tools/compilers/compiler.h"

namespace snuffbox
{
  namespace compilers
  {
    /**
    * @brief Used to compile raw script files into either a binary format
    *        or an RC4 encrypted version of the raw source string
    *
    * @see RC4
    *
    * @author Daniel Konings
    */
    class ScriptCompiler : public ICompiler
    {

    public:

      /**
      * @brief Default constructor
      */
      ScriptCompiler();

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