#include "graphics/ogl/ogl_loader.h"

#include "graphics/ogl/resources/ogl_shader.h"
#include "graphics/ogl/resources/ogl_material.h"
#include "graphics/ogl/resources/ogl_mesh.h"

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
    void OGLLoader::ReleaseShader(IRendererLoader::GPUHandle handle)
    {
      foundation::Memory::Destruct(reinterpret_cast<OGLShader*>(handle));
    }

    //--------------------------------------------------------------------------
    IRendererLoader::GPUHandle OGLLoader::CreateMaterial()
    {
      OGLMaterial* mat = foundation::Memory::Construct<OGLMaterial>(
        &foundation::Memory::default_allocator());

      return mat;
    }

    //--------------------------------------------------------------------------
    bool OGLLoader::LoadMaterial(
      IRendererLoader::GPUHandle handle,
      IRendererLoader::GPUHandle vs,
      IRendererLoader::GPUHandle ps,
      IRendererLoader::GPUHandle gs)
    {
      OGLMaterial* mat = reinterpret_cast<OGLMaterial*>(handle);

      OGLShader* vs_ptr = reinterpret_cast<OGLShader*>(vs);
      OGLShader* ps_ptr = reinterpret_cast<OGLShader*>(ps);
      OGLShader* gs_ptr = reinterpret_cast<OGLShader*>(gs);

      return mat->Load(vs_ptr, ps_ptr, gs_ptr);
    }

    //--------------------------------------------------------------------------
    void OGLLoader::UnloadMaterial(IRendererLoader::GPUHandle handle)
    {
      reinterpret_cast<OGLMaterial*>(handle)->Release();
    }

    //--------------------------------------------------------------------------
    void OGLLoader::ReleaseMaterial(IRendererLoader::GPUHandle handle)
    {
      foundation::Memory::Destruct(reinterpret_cast<OGLMaterial*>(handle));
    }

    //--------------------------------------------------------------------------
    IRendererLoader::GPUHandle OGLLoader::CreateMesh()
    {
      return foundation::Memory::Construct<OGLMesh>(
        &foundation::Memory::default_allocator());
    }

    //--------------------------------------------------------------------------
    bool OGLLoader::LoadMesh(
      IRendererLoader::GPUHandle handle,
      const foundation::Vector<Vertex2D>& verts,
      const foundation::Vector<Index>& indices)
    {
      OGLMesh* mesh = reinterpret_cast<OGLMesh*>(handle);
      return mesh->Create(verts, indices);
    }

    //--------------------------------------------------------------------------
    bool OGLLoader::LoadMesh(
      IRendererLoader::GPUHandle handle,
      const foundation::Vector<Vertex3D>& verts,
      const foundation::Vector<Index>& indices)
    {
      OGLMesh* mesh = reinterpret_cast<OGLMesh*>(handle);
      return mesh->Create(verts, indices);
    }

    //--------------------------------------------------------------------------
    void OGLLoader::UnloadMesh(IRendererLoader::GPUHandle handle)
    {
      OGLMesh* mesh = reinterpret_cast<OGLMesh*>(handle);
      mesh->Release();
    }

    //--------------------------------------------------------------------------
    void OGLLoader::ReleaseMesh(IRendererLoader::GPUHandle handle)
    {
      foundation::Memory::Destruct(reinterpret_cast<OGLMesh*>(handle));
    }
  }
}