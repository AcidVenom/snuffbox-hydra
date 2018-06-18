#pragma once

#include "tools/compilers/compilers/compiler.h"
#include "tools/compilers/definitions/asset_types.h"

namespace snuffbox
{
  namespace compilers
  {
    /**
    * @brief Used to compile HLSL code to SPIRV and then back to renderer
    *        specific bytecode (HLSL in Direct3D and it stays SPIRV in OpenGL)
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
      * @param[in] type The type of the shader to compile, valid arguments are:
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

      /**
      * @brief Converts a shader type to a file header magic value
      *
      * @param[in] type The type of the shader, valid arguments are:
      *                 - AssetTypes::kVertexShader
      *                 - AssetTypes::kPixelShader
      *                 - AssetTypes::kGeometryShader
      *
      * @return The converted value
      */
      static FileHeaderMagic ShaderTypeToMagic(AssetTypes type);

    private:

      /**
      * @brief The shader header to contain both HLSL byte code and SPIRV byte
      *        code
      *
      * The header contains offsets to reach the corresponding shader for
      * the current renderer.
      *
      * @author Daniel Konings
      */
      struct ShaderHeader
      {
        bool has_hlsl; //!< Is there HLSL in this file?
        size_t hlsl_size; //!< The HLSL size
        size_t hlsl_offset; //!< The HLSL offset to reach its binary blob
        size_t spirv_size; //!< The SPIRV size
        size_t spirv_offset; //!< The SPIRV offset to reach its source code
      };

      AssetTypes type_; //!< The shader type
    };
  }
}