#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#ifdef SNUFF_WIN32
#define STBI_MSC_SECURE_CRT
#endif

#include "tools/compilers/utils/gltf_compiler.h"

#include <foundation/auxiliary/logger.h>

namespace snuffbox
{
  namespace compilers
  {
    //--------------------------------------------------------------------------
    GLTFCompiler::GLTFCompiler() :
      error_("No errors")
    {

    }

    //--------------------------------------------------------------------------
    void GLTFCompiler::ForEachPrimitive(
      const tinygltf::Mesh& mesh,
      const foundation::Function<void(const tinygltf::Primitive&)>& del)
    {
      for (size_t i = 0; i < mesh.primitives.size(); ++i)
      {
        if (del != nullptr)
        {
          del(mesh.primitives.at(i));
        }
      }
    }

    //--------------------------------------------------------------------------
    void GLTFCompiler::GetAttributeData(
      const tinygltf::Model& model, 
      int accessor,
      size_t* len,
      uint8_t** out)
    {
      const tinygltf::Accessor acc = model.accessors.at(accessor);
      int view = acc.bufferView;

      const tinygltf::BufferView& buffer_view = model.bufferViews.at(view);
      const tinygltf::Buffer& buffer = model.buffers.at(buffer_view.buffer);

      const uint8_t* start = &buffer.data.at(buffer_view.byteOffset);

      if (len != nullptr)
      {
        *len = buffer_view.byteLength;

        foundation::Logger::Assert(*len <= 16,
          "Length of vertex attribute exceeds 16 bytes");

        memcpy(*out, start, *len);
      }
    }
  }
}