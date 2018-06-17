#pragma once

#include "tools/compilers/compilers/compiler.h"
#include "tools/compilers/definitions/asset_types.h"

namespace snuffbox
{
  namespace compilers
  {
    /**
    * @brief Used to compile HLSL code to SPIRV and then back to renderer
    *        specific bytecode (HLSL in Direct3D and GLSL in OpenGL)
    *
    * @see Glslang
    *
    * @author Daniel Konings
    */
    class ShaderCompiler : public ICompiler
    {

    public:

      /**
      * @brief Construct through a shader type to compile
      *
      * @param[in] type The type of the shader to compile
      *                 Valid arguments are:
      *                 - AssetTypes::kVertexShader
      *                 - AssetTypes::kPixelShader
      *                 - AssetTypes::kGeometryShader
      */
      ShaderCompiler(AssetTypes type);

    protected:

      /**
      * @see ICompiler::CompileImpl
      *
      * @remarks Compilation requires Glslang to be initialized
      *
      * @see Glslang::Initialize
      */
      bool CompileImpl(foundation::File& file) override;

      /**
      * @see ICompiler::DecompileImpl
      */
      bool DecompileImpl(foundation::File& file) override;

    private:

      AssetTypes type_; //!< The shader type
    };
  }
}