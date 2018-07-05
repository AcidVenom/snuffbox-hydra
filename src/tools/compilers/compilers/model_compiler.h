#pragma once

#include "tools/compilers/compilers/compiler.h"
#include "tools/compilers/utils/gltf_compiler.h"

#include <graphics/definitions/vertex.h>

namespace snuffbox
{
  namespace compilers
  {
    /**
    * @brief Compiles models from a provided glTF format, into a binary
    *        format using TinyGLTF
    *
    * @tparam T The vertex format to use
    *
    * Models can be viewed as a scene in 3D modeling software. The scene
    * consists of multiple meshes, which are converted to the Vertex3D format
    * Snuffbox uses for 3D models. 2D meshes are currently not supported.
    *
    * Each mesh can be accessed individually from a model's scene index.
    * This scene index is a unique identifier, following the parenting tree
    * in the model itself. When for instance constructing a model as an
    * engine entity, the MeshComponent simply gets a model asset assigned,
    * along with a scene index within that model to render the appropriate mesh.
    *
    * Each mesh then contains definitions for sub-meshes (material IDs), which
    * are rendered with a different texture. Alongside of this the skeleton
    * and animation data are extracted from the model and are converted to
    * an asset as well.
    *
    * Lastly, materials are automatically
    * created per mesh, which can be replaced by the user during runtime.
    * These materials are stored as a separate asset as well.
    *
    * @remarks TinyGLTF uses STL as its containers, which do not provide
    *          custom memory allocation without overriding 'new' and 'delete'.
    *          As TinyGLTF runs on platforms that don't have a limited memory
    *          pool for malloc and free anyway, this can be disregarded. It
    *          should be noted however to not use the compile functions on
    *          limited memory systems.
    *
    * @author Daniel Konings
    */
    template <typename T>
    class ModelCompiler : public ICompiler
    {

    public:

      /**
      * @see GLTFCompiler::Mesh
      */
      struct Mesh
      {
        foundation::Vector<T> vertices;
        foundation::Vector<uint32_t> indices;
      };

    protected:

      /**
      * @brief The model header describing the model after it
      *
      * @author Daniel Konings
      */
      struct ModelHeader
      {
        size_t num_meshes; //!< The number of meshes
      };

      /**
      * @brief The mesh header to append after the model header for each
      *        mesh
      *
      * @remarks The number of mesh headers are equal to ModelHeader::num_meshes
      *
      * @author Daniel Konings
      */
      struct MeshHeader
      {
        size_t num_vertices; //!< The number of vertices in this mesh
        size_t vertices_offset; //!< The offset to all vertices
        size_t num_indices; //!< The number of indices in this mesh
        size_t indices_offset; //!< The offset to all indices
      };

    public:

      /**
      * @brief Default constructor
      */
      ModelCompiler();

    protected:

      /**
      * @see ICompiler::CompileImpl
      */
      bool CompileImpl(foundation::File& file) override;

      /**
      * @see ICompiler::DecompileImpl
      */
      bool DecompileImpl(foundation::File& file) override;

    public:

      /**
      * @return The currently decompiled meshes
      */
      const foundation::Vector<Mesh>& meshes() const;

    private:

      foundation::Vector<Mesh> meshes_; //!< The currently decompiled meshes
    };

    //--------------------------------------------------------------------------
    template <typename T>
    inline ModelCompiler<T>::ModelCompiler()
    {

    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline bool ModelCompiler<T>::CompileImpl(foundation::File& file)
    {
      GLTFCompiler comp;
      if (comp.Compile<T>(file) == false)
      {
        set_error(comp.error());
        return false;
      }

      const foundation::Vector<GLTFCompiler::Mesh>& meshes = comp.meshes();
      foundation::Vector<uint8_t> buffer;

      ModelHeader header;
      header.num_meshes = meshes.size();

      MeshHeader mesh_header;
      size_t offset = 0;

      buffer.resize(sizeof(ModelHeader));
      memcpy(&buffer.at(offset), &header, sizeof(ModelHeader));
      offset += sizeof(ModelHeader);

      size_t mesh_headers_offset = offset;

      for (size_t i = 0; i < meshes.size(); ++i)
      {
        buffer.resize(buffer.size() + sizeof(MeshHeader));

        const GLTFCompiler::Mesh& mesh = meshes.at(i);

        mesh_header.num_vertices = mesh.vertices.size() / sizeof(T);
        mesh_header.vertices_offset = 0;

        mesh_header.num_indices = mesh.indices.size();
        mesh_header.indices_offset = 0;

        memcpy(&buffer.at(offset), &mesh_header, sizeof(MeshHeader));

        offset += sizeof(MeshHeader);
      }

      MeshHeader* headers = nullptr;

      size_t v_size, i_size;

      for (size_t i = 0; i < meshes.size(); ++i)
      {
        headers = 
          reinterpret_cast<MeshHeader*>(&buffer.at(mesh_headers_offset));

        MeshHeader& h = headers[i];
        const GLTFCompiler::Mesh& mesh = meshes.at(i);

        h.vertices_offset = offset;
        v_size = h.num_vertices * sizeof(T);

        h.indices_offset = offset + v_size;
        i_size = h.num_indices * sizeof(uint32_t);

        buffer.resize(buffer.size() + v_size + i_size);

        memcpy(&buffer.at(offset), mesh.vertices.data(), v_size);
        memcpy(&buffer.at(offset + v_size), mesh.indices.data(), i_size);

        offset += v_size + i_size;
      }
      
      SourceFileData fd;
      fd.magic = FileHeaderMagic::kModel;

      if (AllocateSourceFile(buffer.data(), buffer.size(), &fd) == false)
      {
        return false;
      }

      SetData(fd.data, fd.total_size);

      return true;
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline bool ModelCompiler<T>::DecompileImpl(foundation::File& file)
    {
      BuildFileData fd;
      fd.magic = FileHeaderMagic::kModel;

      if (ReadBuildFile(file, &fd) == false)
      {
        return false;
      }

      SetData(fd.block, fd.length);

      ModelHeader header = *reinterpret_cast<ModelHeader*>(fd.block);
      size_t offset = sizeof(ModelHeader);

      const MeshHeader* mesh_header;

      meshes_.resize(header.num_meshes);
      for (size_t i = 0; i < header.num_meshes; ++i)
      {
        Mesh& mesh = meshes_.at(i);
        mesh_header = reinterpret_cast<const MeshHeader*>(&fd.block[offset]);

        mesh.vertices.resize(mesh_header->num_vertices);
        mesh.indices.resize(mesh_header->num_indices);

        memcpy(
          mesh.vertices.data(), 
          &fd.block[mesh_header->vertices_offset],
          mesh_header->num_vertices * sizeof(T));

        memcpy(
          mesh.indices.data(),
          &fd.block[mesh_header->indices_offset],
          mesh_header->num_indices * sizeof(uint32_t));
        
        offset += sizeof(MeshHeader);
      }

      return true;
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline const foundation::Vector<typename ModelCompiler<T>::Mesh>& 
      ModelCompiler<T>::meshes() const
    {
      return meshes_;
    }
  }

  namespace compilers
  {
    //--------------------------------------------------------------------------
    template <>
    inline graphics::Vertex3D GLTFCompiler::CreateDefaultVertex()
    {
      graphics::Vertex3D v;
      v.position = glm::vec3(0.0f, 0.0f, 0.0f);
      v.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
      v.normal = glm::vec3(0.0f, 0.0f, 1.0f);
      v.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
      v.uv = glm::vec2(0.0f, 0.0f);

      return v;
    }

    //--------------------------------------------------------------------------
    template <>
    inline void GLTFCompiler::SetVertexAttribute<graphics::Vertex3D>(
      GLTFCompiler::VertexAttribute attr,
      const float* data,
      graphics::Vertex3D* out)
    {
      if (out == nullptr)
      {
        return;
      }

      switch (attr)
      {
      case VertexAttribute::kPosition:
        memcpy(&(out->position.x), data, sizeof(float) * 3);
        break;

      case VertexAttribute::kNormal:
        memcpy(&(out->normal.x), data, sizeof(float) * 3);
        break;

      case VertexAttribute::kTangent:
        memcpy(&(out->tangent.x), data, sizeof(float) * 3);
        break;

      case VertexAttribute::kUV:
        memcpy(&(out->uv.x), data, sizeof(float) * 2);
        break;

      default:
        foundation::Logger::Assert(false, 
          "Unknown vertex attribute in SetVertexAttribute");
        break;
      }
    }
  }
}