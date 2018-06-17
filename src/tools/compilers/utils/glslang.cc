#include "tools/compilers/utils/glslang.h"

#include <foundation/auxiliary/logger.h>
#include <foundation/io/file.h>

#include <glslang/OSDependent/osinclude.h>

#include <SPIRV/GlslangToSpv.h>
#include <SPIRV/disassemble.h>

namespace snuffbox
{
  namespace compilers
  {
    //--------------------------------------------------------------------------
    bool Glslang::is_initialized_ = false;

    /**
    * @see StandAlone/ResourceLimits.cpp in glslang
    */
    TBuiltInResource Glslang::kDefaultResource_ =
    {
      /* .MaxLights = */ 32,
      /* .MaxClipPlanes = */ 6,
      /* .MaxTextureUnits = */ 32,
      /* .MaxTextureCoords = */ 32,
      /* .MaxVertexAttribs = */ 64,
      /* .MaxVertexUniformComponents = */ 4096,
      /* .MaxVaryingFloats = */ 64,
      /* .MaxVertexTextureImageUnits = */ 32,
      /* .MaxCombinedTextureImageUnits = */ 80,
      /* .MaxTextureImageUnits = */ 32,
      /* .MaxFragmentUniformComponents = */ 4096,
      /* .MaxDrawBuffers = */ 32,
      /* .MaxVertexUniformVectors = */ 128,
      /* .MaxVaryingVectors = */ 8,
      /* .MaxFragmentUniformVectors = */ 16,
      /* .MaxVertexOutputVectors = */ 16,
      /* .MaxFragmentInputVectors = */ 15,
      /* .MinProgramTexelOffset = */ -8,
      /* .MaxProgramTexelOffset = */ 7,
      /* .MaxClipDistances = */ 8,
      /* .MaxComputeWorkGroupCountX = */ 65535,
      /* .MaxComputeWorkGroupCountY = */ 65535,
      /* .MaxComputeWorkGroupCountZ = */ 65535,
      /* .MaxComputeWorkGroupSizeX = */ 1024,
      /* .MaxComputeWorkGroupSizeY = */ 1024,
      /* .MaxComputeWorkGroupSizeZ = */ 64,
      /* .MaxComputeUniformComponents = */ 1024,
      /* .MaxComputeTextureImageUnits = */ 16,
      /* .MaxComputeImageUniforms = */ 8,
      /* .MaxComputeAtomicCounters = */ 8,
      /* .MaxComputeAtomicCounterBuffers = */ 1,
      /* .MaxVaryingComponents = */ 60,
      /* .MaxVertexOutputComponents = */ 64,
      /* .MaxGeometryInputComponents = */ 64,
      /* .MaxGeometryOutputComponents = */ 128,
      /* .MaxFragmentInputComponents = */ 128,
      /* .MaxImageUnits = */ 8,
      /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
      /* .MaxCombinedShaderOutputResources = */ 8,
      /* .MaxImageSamples = */ 0,
      /* .MaxVertexImageUniforms = */ 0,
      /* .MaxTessControlImageUniforms = */ 0,
      /* .MaxTessEvaluationImageUniforms = */ 0,
      /* .MaxGeometryImageUniforms = */ 0,
      /* .MaxFragmentImageUniforms = */ 8,
      /* .MaxCombinedImageUniforms = */ 8,
      /* .MaxGeometryTextureImageUnits = */ 16,
      /* .MaxGeometryOutputVertices = */ 256,
      /* .MaxGeometryTotalOutputComponents = */ 1024,
      /* .MaxGeometryUniformComponents = */ 1024,
      /* .MaxGeometryVaryingComponents = */ 64,
      /* .MaxTessControlInputComponents = */ 128,
      /* .MaxTessControlOutputComponents = */ 128,
      /* .MaxTessControlTextureImageUnits = */ 16,
      /* .MaxTessControlUniformComponents = */ 1024,
      /* .MaxTessControlTotalOutputComponents = */ 4096,
      /* .MaxTessEvaluationInputComponents = */ 128,
      /* .MaxTessEvaluationOutputComponents = */ 128,
      /* .MaxTessEvaluationTextureImageUnits = */ 16,
      /* .MaxTessEvaluationUniformComponents = */ 1024,
      /* .MaxTessPatchComponents = */ 120,
      /* .MaxPatchVertices = */ 32,
      /* .MaxTessGenLevel = */ 64,
      /* .MaxViewports = */ 16,
      /* .MaxVertexAtomicCounters = */ 0,
      /* .MaxTessControlAtomicCounters = */ 0,
      /* .MaxTessEvaluationAtomicCounters = */ 0,
      /* .MaxGeometryAtomicCounters = */ 0,
      /* .MaxFragmentAtomicCounters = */ 8,
      /* .MaxCombinedAtomicCounters = */ 8,
      /* .MaxAtomicCounterBindings = */ 1,
      /* .MaxVertexAtomicCounterBuffers = */ 0,
      /* .MaxTessControlAtomicCounterBuffers = */ 0,
      /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
      /* .MaxGeometryAtomicCounterBuffers = */ 0,
      /* .MaxFragmentAtomicCounterBuffers = */ 1,
      /* .MaxCombinedAtomicCounterBuffers = */ 1,
      /* .MaxAtomicCounterBufferSize = */ 16384,
      /* .MaxTransformFeedbackBuffers = */ 4,
      /* .MaxTransformFeedbackInterleavedComponents = */ 64,
      /* .MaxCullDistances = */ 8,
      /* .MaxCombinedClipAndCullDistances = */ 8,
      /* .MaxSamples = */ 4,
      /* .limits = */ {
      /* .nonInductiveForLoops = */ 1,
      /* .whileLoops = */ 1,
      /* .doWhileLoops = */ 1,
      /* .generalUniformIndexing = */ 1,
      /* .generalAttributeMatrixVectorIndexing = */ 1,
      /* .generalVaryingIndexing = */ 1,
      /* .generalSamplerIndexing = */ 1,
      /* .generalVariableIndexing = */ 1,
      /* .generalConstantMatrixVectorIndexing = */ 1,
      }
    };

    const glslang::EShTargetLanguageVersion Glslang::kLangVersion_ =
      glslang::EShTargetLanguageVersion::EShTargetSpv_1_0;

    const glslang::EShTargetClientVersion Glslang::kClientVersion_ =
      glslang::EShTargetClientVersion::EShTargetVulkan_1_0;

    const int Glslang::kDesktopVersion_ = 110;

    const char* Glslang::kEntryPoint_ = "main";

    //--------------------------------------------------------------------------
    Glslang::Includer::Includer(const foundation::Path& root) :
      root_(root)
    {

    }

    //--------------------------------------------------------------------------
    glslang::TShader::Includer::IncludeResult* Glslang::Includer::includeSystem(
      const char* header_name, 
      const char* includer_name, 
      size_t depth)
    {
      return nullptr;
    }

    //--------------------------------------------------------------------------
    glslang::TShader::Includer::IncludeResult* Glslang::Includer::includeLocal(
      const char* header_name, 
      const char* includer_name, 
      size_t depth)
    {
      IncludeResult* res = nullptr;

      foundation::Path full_path = root_ / header_name;

      foundation::File fin(full_path, foundation::FileFlags::kRead);

      if (fin.is_ok() == true)
      {
        size_t len;
        const char* data = 
          reinterpret_cast<const char*>(fin.ReadBuffer(&len));

        char* contents = new char[len];
        memcpy(contents, data, len);

        res = new IncludeResult(header_name, contents, len, contents);
      }

      return res;
    }

    //--------------------------------------------------------------------------
    void Glslang::Includer::releaseInclude(IncludeResult* result)
    {
      if (result != nullptr)
      {
        delete[] static_cast<const char*>(result->userData);
        delete result;
      }
    }

    //--------------------------------------------------------------------------
    bool Glslang::Initialize()
    {
      if (is_initialized_ == true || glslang::InitializeProcess() == true)
      {
        is_initialized_ = true;
        return true;
      }

      return false;
    }

    //--------------------------------------------------------------------------
    void Glslang::Shutdown()
    {
      glslang::FinalizeProcess();
      is_initialized_ = false;
    }

    //--------------------------------------------------------------------------
    Glslang::Glslang() :
      error_("No errors")
    {
      
    }

    //--------------------------------------------------------------------------
    bool Glslang::Compile(
      const foundation::String& hlsl,
      const foundation::Path& path,
      compilers::AssetTypes type,
      foundation::Vector<uint8_t>* spirv)
    {
      if (is_initialized_ == false)
      {
        error_ = "glslang is not initialized";
        return false;
      }

      EShLanguage lang = AssetTypesToLanguage(type);

      if (lang == EShLanguage::EShLangCount)
      {
        error_ = "Unknown shader type";
        return false;
      }

      glslang::TShader shader(lang);

      CreateShaderDesc(lang, path, &shader);

      EShMessages messages;
      if (CompileShader(shader, hlsl, path, &messages) == false)
      {
        return false;
      }

      glslang::TProgram program;
      if (LinkAndMap(shader, program, messages) == false)
      {
        return false;
      }

      std::vector<unsigned int> spv;
      spv::SpvBuildLogger logger;

      glslang::GlslangToSpv(*program.getIntermediate(lang), spv, &logger);

      if (spirv != nullptr)
      {
        size_t s = spv.size() * 4;
        spirv->resize(s);

        uint8_t* bytes;

        for (size_t i = 0; i < spv.size(); ++i)
        {
          bytes = reinterpret_cast<uint8_t*>(&spv.at(i));

          for (size_t j = 0; j < 4; ++j)
          {
            spirv->at(i * 4 + j) = bytes[j];
          }
        }
      }

      return true;
    }

    //--------------------------------------------------------------------------
    const foundation::String& Glslang::error() const
    {
      return error_;
    }

    //--------------------------------------------------------------------------
    foundation::String Glslang::GetCurrentDirectory(
      const foundation::Path& path)
    {
      foundation::String spath = path.ToString();
      size_t i;

      if (
        (i = foundation::StringUtils::FindLastOf(spath, '/')) != 
        foundation::String::npos)
      {
        spath.erase(spath.begin() + i, spath.end());
      }
      else
      {
        spath = "";
      }

      return spath;
    }

    //--------------------------------------------------------------------------
    void Glslang::CreateShaderDesc(
      EShLanguage lang,
      const foundation::Path& path,
      glslang::TShader* shader)
    {
      if (shader == nullptr || lang == EShLanguage::EShLangCount)
      {
        return;
      }

      shader->setSourceEntryPoint(kEntryPoint_);
      shader->setEntryPoint(kEntryPoint_);

      shader->setEnvTarget(
        glslang::EShTargetLanguage::EshTargetSpv,
        kLangVersion_);

      shader->setEnvClient(
        glslang::EShClient::EShClientVulkan, 
        kClientVersion_);

      shader->setEnvInput(
        glslang::EShSource::EShSourceHlsl, 
        lang, 
        glslang::EShClient::EShClientVulkan, 
        kClientVersion_);

      shader->setShiftSamplerBinding(0);
      shader->setShiftTextureBinding(0);
      shader->setShiftImageBinding(0);
      shader->setShiftUboBinding(0);
      shader->setShiftSsboBinding(0);
      shader->setShiftUavBinding(0);
      shader->setFlattenUniformArrays(false);
      shader->setNoStorageFormat(false);

      std::vector<std::string> rsb;
      shader->setResourceSetBinding(rsb);
    }

    //--------------------------------------------------------------------------
    bool Glslang::CompileShader(
      glslang::TShader& shader,
      const foundation::String& hlsl,
      const foundation::Path& path,
      EShMessages* msg)
    {
      EShMessages messages = static_cast<EShMessages>(
        EShMessages::EShMsgDefault | 
        EShMessages::EShMsgVulkanRules | 
        EShMessages::EShMsgSpvRules | 
        EShMessages::EShMsgReadHlsl);

      if (msg != nullptr)
      {
        *msg = messages;
      }

      int l = static_cast<int>(hlsl.size());

      const char* c_hlsl = hlsl.c_str();
      const char* c_path = path.ToString().c_str();

      shader.setStringsWithLengthsAndNames(&c_hlsl, &l, &c_path, 1);

      foundation::String dir = GetCurrentDirectory(path);

      Includer includer(dir.c_str());

      bool success = shader.parse(
        &kDefaultResource_, 
        kDesktopVersion_, 
        false,
        messages, 
        includer);

      if (success == false)
      {
        error_ = "Could not compile shader\n\n";
        error_ += shader.getInfoLog();

        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    bool Glslang::LinkAndMap(
      glslang::TShader& shader, 
      glslang::TProgram& program, 
      EShMessages msg)
    {
      program.addShader(&shader);

      if (program.link(msg) == false || program.mapIO() == false)
      {
        error_ = "Could not link shader\n\n";
        error_ += program.getInfoLog();

        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    EShLanguage Glslang::AssetTypesToLanguage(compilers::AssetTypes type)
    {
      EShLanguage lang;

      switch (type)
      {
      case compilers::AssetTypes::kVertexShader:
        lang = EShLanguage::EShLangVertex;
        break;

      case compilers::AssetTypes::kPixelShader:
        lang = EShLanguage::EShLangFragment;
        break;

      case compilers::AssetTypes::kGeometryShader:
        lang = EShLanguage::EShLangGeometry;
        break;

      default:
        lang = EShLanguage::EShLangCount;
        break;
      }

      return lang;
    }
  }
}