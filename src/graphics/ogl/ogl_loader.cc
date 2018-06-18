#include "graphics/ogl/ogl_loader.h"

#include "graphics/ogl/resources/ogl_shader.h"

#include <foundation/auxiliary/logger.h>
#include <foundation/memory/memory.h>

namespace snuffbox
{
  namespace graphics
  {
    //--------------------------------------------------------------------------
    IRendererLoader::GPUHandle OGLLoader::CreateShader(ShaderTypes type)
    {
      OGLShader* shader = foundation::Memory::Construct<OGLShader>(
        &foundation::Memory::default_allocator(),
        type);

      return shader;
    }

    //--------------------------------------------------------------------------
    bool OGLLoader::LoadShader(
      GPUHandle handle, 
      const uint8_t* buffer, 
      size_t len)
    {
      OGLShader* shader = reinterpret_cast<OGLShader*>(handle);

      if (shader->Load(buffer,len) == false)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kError,
          "Could not load shader, errors:\n{0}",
          shader->error());

        return false;
      }

      return true;
    }

    //--------------------------------------------------------------------------
    void OGLLoader::UnloadShader(GPUHandle handle)
    {
      OGLShader* shader = reinterpret_cast<OGLShader*>(handle);
      shader->Release();
    }

    //--------------------------------------------------------------------------
    void OGLLoader::ReleaseShader(IRendererLoader::GPUHandle shader)
    {
      OGLShader* sh = reinterpret_cast<OGLShader*>(shader);
      foundation::Memory::Destruct(sh);
    }
  }
}