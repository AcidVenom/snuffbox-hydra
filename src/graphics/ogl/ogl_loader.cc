#include "graphics/ogl/ogl_loader.h"

#include "graphics/ogl/ogl_shader.h"

#include <foundation/auxiliary/logger.h>
#include <foundation/memory/memory.h>

namespace snuffbox
{
  namespace graphics
  {
    //--------------------------------------------------------------------------
    IRendererLoader::GPUHandle OGLLoader::LoadShader(
      ShaderTypes type,
      const uint8_t* buffer,
      size_t len)
    {
      OGLShader* shader = foundation::Memory::Construct<OGLShader>(
        &foundation::Memory::default_allocator(),
        type);

      if (shader->Load(foundation::String(
          reinterpret_cast<const char*>(buffer),
          len)) == false)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kError,
          "Could not load shader, errors:\n{0}",
          shader->error());

        foundation::Memory::Destruct(shader);
        shader = nullptr;
      }

      return shader;
    }

    //--------------------------------------------------------------------------
    void OGLLoader::ReleaseShader(IRendererLoader::GPUHandle shader)
    {
      OGLShader* sh = reinterpret_cast<OGLShader*>(shader);
      foundation::Memory::Destruct(sh);
    }
  }
}