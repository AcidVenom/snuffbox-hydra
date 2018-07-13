#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#ifdef SNUFF_WIN32
#define STBI_MSC_SECURE_CRT
#endif

#include "tools/compilers/utils/gltf_compiler.h"

#include <foundation/auxiliary/logger.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
    void GLTFCompiler::AffineTransform(const tinygltf::Node& node, Mesh* out)
    {
      if (out == nullptr)
      {
        return;
      }

      out->transform = glm::mat4x4(1.0f);

      if (node.matrix.size() != 0)
      {
        float* start = &(out->transform[0].x);
        for (int i = 0; i < 16; ++i)
        {
          *(start + i) = static_cast<float>(node.matrix.at(i));
        }
      }
      else
      {
        auto GetVectorValue = [&](const std::vector<double>& v, glm::vec4 def)
        {
          glm::vec4 buffer = def;

          if (v.size() != 0)
          {
            for (size_t i = 0; i < 4; ++i)
            {
              if (v.size() <= i)
              {
                break;
              }

              buffer[static_cast<glm::vec4::length_type>(i)] = 
                static_cast<float>(v.at(i));
            }
          }

          return buffer;
        };

        glm::vec4 t = 
          GetVectorValue(node.translation, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        glm::vec4 r =
          GetVectorValue(node.rotation, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        glm::vec4 s =
          GetVectorValue(node.scale, glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));

        out->transform = 
          glm::translate(out->transform, glm::vec3(t.x, t.y, t.z));

        out->transform *= glm::toMat4(glm::quat(r.w, r.x, r.y, r.z));
        out->transform = glm::scale(out->transform, glm::vec3(s.x, s.y, s.z));
      }
    }

    //--------------------------------------------------------------------------
    const float* GLTFCompiler::GetAttributeData(
      const tinygltf::Model& model, 
      int accessor,
      size_t* len,
      size_t* count)
    {
      const tinygltf::Accessor acc = model.accessors.at(accessor);
      const tinygltf::BufferView& view = model.bufferViews.at(acc.bufferView);
      const tinygltf::Buffer& buffer = model.buffers.at(view.buffer);

      const float* start = reinterpret_cast<const float*>(
        &buffer.data.at(view.byteOffset + acc.byteOffset));

      if (count != nullptr)
      {
        *count = acc.count;
      }

      if (len != nullptr)
      {
        switch (acc.type)
        {
        case TINYGLTF_TYPE_SCALAR:
          *len = 1;
          break;

        case TINYGLTF_TYPE_VEC2:
          *len = 2;
          break;

        case TINYGLTF_TYPE_VEC3:
          *len = 3;
          break;

        case TINYGLTF_TYPE_VEC4:
          *len = 4;
          break;

        case TINYGLTF_TYPE_MAT2:
          *len = 4;
          break;

        case TINYGLTF_TYPE_MAT3:
          *len = 9;
          break;

        case TINYGLTF_TYPE_MAT4:
          *len = 16;
          break;

        default:
          *len = -1;
          break;
        }

        foundation::Logger::Assert(*len <= 16, 
          "Vertex attribute size exceeds 16 bytes in a GLTF model");
      }

      return start;
    }

    //--------------------------------------------------------------------------
    GLTFCompiler::VertexAttribute 
      GLTFCompiler::GetVertexAttribute(const std::string& name)
    {
      if (name == "POSITION")
      {
        return VertexAttribute::kPosition;
      }
      else if (name == "COLOR_0")
      {
        return VertexAttribute::kColor;
      }
      else if (name == "NORMAL")
      {
        return VertexAttribute::kNormal;
      }
      else if (name == "TANGENT")
      {
        return VertexAttribute::kTangent;
      }
      else if (name == "TEXCOORD_0")
      {
        return VertexAttribute::kUV;
      }

      return VertexAttribute::kUnknown;
    }

    //--------------------------------------------------------------------------
    void GLTFCompiler::AssignIndices(
      const tinygltf::Model& model,
      const tinygltf::Primitive& primitive,
      foundation::Vector<uint32_t>* out)
    {
      const tinygltf::Accessor& acc = model.accessors.at(primitive.indices);
      const tinygltf::BufferView& view = model.bufferViews.at(acc.bufferView);
      const tinygltf::Buffer& buffer = model.buffers.at(view.buffer);

      bool is_short = 
        acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT;

      const size_t stride = is_short == true ? 
        sizeof(uint16_t) : sizeof(uint32_t);

      const uint8_t* start = &buffer.data.at(view.byteOffset + acc.byteOffset);
      const uint8_t* current = nullptr;

      for (size_t i = 0; i < acc.count; ++i)
      {
        current = &start[i * stride];

        if (is_short == true)
        {
          out->push_back(
            static_cast<uint32_t>(*reinterpret_cast<const uint16_t*>(current)));

          continue;
        }

        out->push_back(*reinterpret_cast<const uint32_t*>(current));
      }
    }

    //--------------------------------------------------------------------------
    const foundation::Vector<GLTFCompiler::Mesh>& GLTFCompiler::meshes() const
    {
      return meshes_;
    }

    //--------------------------------------------------------------------------
    const foundation::String& GLTFCompiler::error() const
    {
      return error_;
    }
  }
}
