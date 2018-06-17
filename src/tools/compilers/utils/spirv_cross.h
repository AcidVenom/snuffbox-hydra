#pragma once

#include "tools/compilers/definitions/asset_types.h"

#include <foundation/containers/vector.h>
#include <foundation/containers/string.h>

#include <cinttypes>
#include <cstddef>

namespace snuffbox
{
  namespace compilers
  {
    /**
    * @brief A utility class to compile SPIRV bytecode into HLSL bytecode and
    *        GLSL source code to present to the driver
    *
    * @author Daniel Konings
    */
    class SPIRVCross
    {

    public:

      /**
      * @brief A structure to contain compiled HLSL and GLSL code
      *
      * HLSL is compiled through the usage of D3DCompiler and GLSL is presented
      * as its source format, because as far as I know, GLSL doesn't have an
      * intermediate format like HLSL does.
      *
      * @author Daniel Konings
      */
      struct ShaderData
      {
        bool has_hlsl; //!< Was HLSL compiled as well?
        foundation::Vector<uint8_t> hlsl; //!< The HLSL bytecode
        foundation::String glsl; //!< The source GLSL code
      };

      /**
      * @brief Compiles SPIRV bytecode into both HLSL bytecode and GLSL
      *        source code
      *
      * @remarks HLSL bytecode is only available on the Windows platform, as
      *          D3DCompiler is required
      *
      * @param[in] spirv The SPIRV bytecode
      * @param[in] len The length of the bytecode
      * @param[in] type The type of shader to compile, valid arguments are:
      *                 - AssetTypes::kVertexShader
      *                 - AssetTypes::kPixelShader
      *                 - AssetTypes::kGeometryShader
      * @param[out] data The compiled data
      *
      * @return Was the compilation succesful?
      */
      bool Compile(
        const uint8_t* spirv, 
        size_t len,
        AssetTypes type,
        ShaderData* data);

      /**
      * @return The current error
      */
      const foundation::String& error() const;

    protected:

      /**
      * @brief Used to contain the source code for both HLSL and GLSL
      *
      * @author Daniel Konings
      */
      struct SourceData
      {
        foundation::String hlsl; //!< HLSL source code
        foundation::String glsl; //!< GLSL source code
      };

      /**
      * @brief Compiles SPIRV into respective HLSL and GLSL source formats
      *
      * @param[in] spirv The SPIRV bytecode
      * @param[in] len The length of the bytecode
      * @param[out] data The compiled data, as source code strings
      *
      * @return Was compilation a success?
      */
      static bool ToSourceCode(
        const uint8_t* spirv, 
        size_t len, 
        SourceData* data);

    private:

      static const int kGLSLVersion_; //!< The GLSL version to use
      static const int kShaderModel_; //!< The HLSL shader model

      foundation::String error_; //!< The current error
    };
  }
}