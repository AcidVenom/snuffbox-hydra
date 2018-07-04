#pragma once

#include <foundation/io/file.h>

#include <tiny_gltf.h>

#include <cinttypes>

namespace snuffbox
{
  namespace compilers
  {
    /**
    * @brief Used to compile the glTF format into a pure binary format,
    *        readable by the engine
    *
    * @author Daniel Konings
    */
    class GLTFCompiler
    {
      
    public:

      /**
      * @brief Used to contain compiled data of each mesh in the glTF scene
      *
      * @author Daniel Konings
      */
      struct Mesh
      {
        foundation::Vector<uint8_t> vertices; //!< The vertex data of the mesh
        foundation::Vector<uint32_t> indices; //!< The index data of the mesh
      };

      /**
      * @brief Used to set specific vertex attributes by specializing
      *        GLTFCompiler::SetVertexAttribute
      */
      enum class VertexAttribute
      {
        kPosition, //!< A float3 position
        kColor, //!< A float4 color
        kNormal, //!< A float3 normal
        kTangent, //!< A float3 tangent
        kBitangent, //!< A float3 bitangent
        kUV, //!< A float2 texcoord
        kUnknown //!< An unknown vertex attribute
      };

      /**
      * @brief Default constructor
      */
      GLTFCompiler();

      /**
      * @brief Compiles a .gltf file into a pure binary format, outputting
      *        a custom vertex format
      *
      * @tparam The vertex format to use
      *
      * @param[in] file The file to compile from
      *
      * @return Was the compilation a success?
      */
      template <typename T>
      bool Compile(const foundation::File& file);

    protected:

      /**
      * @brief This method is to be specialized to fill in a vertex format's
      *        data accordingly
      *
      * Each time the compiler encounters a primitive with its respective
      * attributes, it will call this function for each available attribute
      * in the GLTFCompiler::VertexAttribute enumeration. It is up to the
      * user to convert these attributes accordingly to their own custom
      * vertex format.
      *
      * @tparam The vertex structure to fill in
      *
      * @param[in] attr The ttype of attribute currently being set
      * @param[in] buffer The data of the attribute
      * @param[in] out The current vertex that's being processed
      */
      template <typename T>
      static void SetVertexAttribute(
        VertexAttribute attr, 
        const uint8_t* buffer, 
        T* out);

      /**
      * @brief Executes a delegate for each primitive within a tinygltf mesh
      *
      * @param[in] mesh The mesh to retrieve the primitives from
      * @param[in] del The delegate to call
      */
      void ForEachPrimitive(
        const tinygltf::Mesh& mesh,
        const foundation::Function<void(const tinygltf::Primitive&)>& del);

      /**
      * @brief Compiles a single in a model into a binary format
      *
      * @tparam The vertex format to use
      *
      * @param[in] model The current glTF model
      * @param[in] mesh The current glTF mesh within the model
      * @param[in] scene_index The scene index of the current mesh
      */
      template <typename T>
      void CompileMesh(
        const tinygltf::Model& model, 
        const tinygltf::Mesh& mesh,
        size_t scene_index);

      /**
      * @brief Retrieves a vertex's attribute data from the accessor within
      *        the model
      *
      * @param[in] model The model to retrieve the binary data from
      * @param[in] accessor The accessor to where the binary data is located
      * @param[out] len The length of the found attribute
      * @param[out] out The buffer to write the data to
      *
      * @remarks The buffer should be big enough to contain the attribute data
      */
      static void GetAttributeData(
        const tinygltf::Model& model,
        int accessor,
        size_t* len,
        uint8_t** out);

      /**
      * @brief Converts a vertex attribute name to the corresponding
      *        vertex attribute enumeration
      *
      * Possible attribute names are:
      * - POSITION: The vertex's position
      * - NORMAL: The vertex's normal
      * - TANGENT: The vertex's tangent
      * - TEXCOORD_0: The vertex's UV coordinates
      *
      * @return The converted vertex attribute
      *
      * @remarks The return value is VertexAttribute::kUnknown if an
      *          unsupported vertex attribute is encountered
      */
      static VertexAttribute GetVertexAttribute(const std::string& name);

    private:

      tinygltf::TinyGLTF ctx_; //!< The curent tinygltf context
      foundation::Vector<Mesh> meshes_; //!< The compiled meshes
      foundation::String error_; //!< The current error message
    };

    //--------------------------------------------------------------------------
    template <typename T>
    inline bool GLTFCompiler::Compile(const foundation::File& file)
    {
      if (file.is_ok() == false)
      {
        error_ = "Could not open GLTF file";
        return false;
      }

      tinygltf::Model model;
      std::string err;
      
      size_t len;
      const uint8_t* str = file.ReadBuffer(&len);

      foundation::String base = file.path().GetBaseDirectory().ToString();
      if (ctx_.LoadASCIIFromString(
        &model, 
        &err, 
        reinterpret_cast<const char*>(str), 
        static_cast<unsigned int>(len), base.c_str()) == false)
      {
        error_ = err.c_str();
        return false;
      }

      meshes_.reserve(model.meshes.size());
      for (size_t i = 0; i < model.meshes.size(); ++i)
      {
        CompileMesh<T>(model, model.meshes.at(i), i);
      }

      return true;
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void GLTFCompiler::CompileMesh(
      const tinygltf::Model& model, 
      const tinygltf::Mesh& mesh,
      size_t scene_index)
    {
      size_t num_primitives = mesh.primitives.size();
      size_t num_vertices = num_primitives * 3;

      Mesh m;

      m.vertices.resize(num_vertices);
      m.indices.resize(num_vertices);

      ForEachPrimitive(mesh, [&](const tinygltf::Primitive& p)
      {
        size_t len;
        uint8_t data[16];
        VertexAttribute attr;

        std::map<std::string, int>::iterator it = p.attributes.begin();
        for (it; it != p.attributes.end(); ++it)
        {
          /*attr = GetVertexAttribute(it->first);
          GetAttributeData(model, it->second, &len, &data);

          SetVertexAttribute(attr, data, )*/
        }
      });

      meshes_.at(scene_index) = m;
    }
  }
}