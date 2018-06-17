#pragma once

#include "tools/compilers/definitions/asset_types.h"

#include <foundation/io/path.h>
#include <foundation/containers/vector.h>

#include <glslang/Public/ShaderLang.h>

#include <cinttypes>
#include <cstddef>

namespace snuffbox
{
  namespace compilers
  {
    /**
    * @brief Used to compile HLSL shaders into SPIR-V
    *
    * The data allocated here is not through Snuffbox's custom memory
    * allocators and are all handled internally by glslang. This class
    * and the corresponding Glslang classes should not be used on
    * systems that have a limited memory allocation pool through 
    * 'new' and 'delete'.
    *
    * Mostly, this is a non-issue as shaders are compiled from within the
    * editor anyway. Meaning we're running on platforms like Windows and
    * Linux; which can freely use non-overridden 'new' and 'delete'.
    *
    * @remarks There might be a memory leak within glslang still, which
    *          will cause this class to leak some memory here and there.
    *          This was an issue with a previous version of Snuffbox, but
    *          could be resolved by now
    *
    * @see 
    * https://github.com/KhronosGroup/glslang/issues/389#issuecomment-345917601
    *
    * @author Daniel Konings
    */
    class Glslang
    {

    protected:

      /**
      * @brief Used to do include directives in the HLSL shader
      *
      * @remarks This is currently limited to local includes
      *
      * @see Glslang
      *
      * @author Daniel Konings
      */
      class Includer : public glslang::TShader::Includer
      {

      public:

        /**
        * @brief Sets the directory of the includer
        *
        * @param[in] root The directory to search for files
        */
        Includer(const foundation::Path& root);

      protected:
        /**
        * @brief Used to include system directories, 
        *        this is unused and will always return nullptr
        *
        * @remarks e.g. when the user includes a shader 
        *          using '#include <path/to/shader>'
        */
        IncludeResult* includeSystem(
          const char* header_name, 
          const char* includer_name, 
          size_t depth) override;

        /**
        * @brief Used to include a local directory, this will search from the 
        *        current root directory of the current shader
        *
        * @remarks e.g. when the user includes a shader using 
        *          '#include "path/to/shader"'
        */
        IncludeResult* includeLocal(
          const char* header_name, 
          const char* includer_name, 
          size_t depth) override;

        /**
        * @brief Releases the used data
        */
        void releaseInclude(IncludeResult* result) override;

      private:

        foundation::Path root_; //!< The directory to search for files
      };

    public:

      /**
      * @brief Initializes glslang for use
      *
      * @remarks This function should only be called once per process
      */
      static bool Initialize();

      /**
      * @brief Tears down glslang and its allocated memory
      *
      * @remarks This function should only be called once per process, after
      *          Glslang::Initialize has been succesfully called
      */
      static void Shutdown();

      /**
      * @brief Default constructor
      */
      Glslang();

      /**
      * @brief Compiles a source HLSL string into SPIRV bytecode
      *
      * @param[in] hlsl The source HLSL code
      * @param[in] path The originating path of the HLSL code
      * @param[in] type The asset type, which should be any of
      *                 - AssetTypes::kVertexShader
      *                 - AssetTypes::kPixelShader
      *                 - AssetTypes::kGeometryShader
      * @param[out] spirv The SPIRV bytecode
      *
      * @remarks This method will fail if glslang is not initialized yet
      *
      * @see Glslang::Initialize
      */
      bool Compile(
        const foundation::String& hlsl, 
        const foundation::Path& path,
        compilers::AssetTypes type,
        foundation::Vector<uint8_t>* spirv);

      /**
      * @return The current error message
      */
      const foundation::String& error() const;

    protected:

      /**
      * @brief Retrieves the current directory by splitting the file path
      *
      * @param[in] path The current path
      *
      * @return The path, stripped of its file
      */
      static foundation::String GetCurrentDirectory(
        const foundation::Path& path);

      /**
      * @brief Creates the shader description for glslang to compile
      *
      * @param[in] lang The shader type
      * @param[in] path The originating path of the HLSL code
      * @param[out] shader The created shader description
      */
      void CreateShaderDesc(
        EShLanguage lang,
        const foundation::Path& path, 
        glslang::TShader* shader);

      /**
      * @brief Compiles a shader from a shader description
      *
      * @see Glslang::CreateShaderDesc
      *
      * @param[in] shader The shader to compile
      * @param[in] hlsl The source HLSL code
      * @param[in] path The root includer path
      * @param[out] msg The shader messages used for compilation
      *
      * @return Were we able to compile the shader?
      */
      bool CompileShader(
        glslang::TShader& shader,
        const foundation::String& hlsl,
        const foundation::Path& path,
        EShMessages* msg);

      /**
      * @brief Links and maps the shader to a program
      *
      * @param[in] shader The shader to link to the respective program
      * @param[in] program The shader program
      * @param[in] msg The shader messages that were used during compilation
      *
      * @see Glslang::CompileShader
      *
      * @return Were we able to link and map the shader?
      */
      bool LinkAndMap(
        glslang::TShader& shader,
        glslang::TProgram& program,
        EShMessages msg);

      /**
      * @brief Converts a Snuffbox asset type into its respective shader type
      *
      * The only valid asset types to convert here are
      * - AssetTypes::kVertexShader
      * - AssetTypes::kPixelShader
      * - AssetTypes::kGeometryShader
      *
      * @return The converted shader type
      */
      static EShLanguage AssetTypesToLanguage(compilers::AssetTypes type);

    private:

      static bool is_initialized_; //!< Is glslang initialized?

      static TBuiltInResource kDefaultResource_; //!< The default resource

      /**
      * @brief The language version
      */
      static const glslang::EShTargetLanguageVersion kLangVersion_;

      /**
      * @brief The client version
      */
      static const glslang::EShTargetClientVersion kClientVersion_;

      static const char* kEntryPoint_; //!< The shader's entry point
      static const int kDesktopVersion_; //!< The desktop version

      foundation::String error_; //!< The current error message
    };
  }
}