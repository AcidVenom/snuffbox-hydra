#pragma once

#include <foundation/io/file.h>

#include <tiny_gltf.h>
#include <glm/glm.hpp>

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
      bool Compile(foundation::File& file);

    protected:

      /**
      * @brief Creates the default vertex for when certain attributes are not
      *        set with the compiler
      *
      * @tparam T The vertex format
      *
      * This function should be specialized like 
      * GLTFCompiler::SetVertexAttribute
      */
      template <typename T>
      static T CreateDefaultVertex();

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
        const float* buffer, 
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
      * @brief Compiles a model's nodes recursively
      *
      * @param[in] model The current model being compiled
      * @param[in] node The root node to start at
      * @param[in] transform The previous transform of the parent node
      */
      template <typename T>
      void CompileNode(
        const tinygltf::Model& model,
        const tinygltf::Node& node,
        const glm::mat4x4& transform = glm::mat4x4(1.0f));

      /**
      * @brief Compiles a single in a model into a binary format
      *
      * @tparam The vertex format to use
      *
      * @param[in] model The current glTF model
      * @param[in] node The current glTF node within the model that contains
      *                 a mesh
      * @param[in] transform The parent hierarchy transformation
      */
      template <typename T>
      void CompileMesh(
        const tinygltf::Model& model, 
        const tinygltf::Node& node,
        const glm::mat4x4& transform);

      /**
      * @brief Affines the transformation of a single node and returns it
      *
      * @remarks If the node doesn't have any transformation data, an identity
      *          matrix is returned
      *
      * @param[in] node The node that contains the transformation data
      */
      static glm::mat4x4 NodeTransform(const tinygltf::Node& node);

      /**
      * @brief Retrieves a vertex's attribute data from the accessor within
      *        the model
      *
      * @param[in] model The model to retrieve the binary data from
      * @param[in] accessor The accessor to where the binary data is located
      * @param[out] len The length of the found attributes
      * @param[out] count The number of attributes in the accessor
      */
      static const float* GetAttributeData(
        const tinygltf::Model& model,
        int accessor,
        size_t* len,
        size_t* count);

      /**
      * @brief Flips the Z-axis accordingly, to left-handed coordinate space
      *        and applies the transformations of the node hierarchy
      *
      * @remarks Transformations are not applied on the color and UV value
      *          of an object, the normal is multiplied with a W component of
      *          0
      *
      * @param[in] attr The current vertex attribute
      * @param[out] data The attribute data to adjust
      * @param[in] transform The node hierarchy transformations
      */
      static void TransformVector(
        VertexAttribute attr, 
        float* data,
        const glm::mat4x4& transform);

      /**
      * @brief Converts a vertex attribute name to the corresponding
      *        vertex attribute enumeration
      *
      * Possible attribute names are:
      * - POSITION: The vertex's position
      * - COLOR_0: The vertex's color
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

      /**
      * @brief Assigns the indices of a model's primitive to the a list of
      *        indices
      *
      * @param[in] model The model to assign the indices from
      * @param[in] primitive The primitive in the model to assign indices from
      * @param[out] out The list of indices to fill
      */
      static void AssignIndices(
        const tinygltf::Model& model, 
        const tinygltf::Primitive& primitive, 
        foundation::Vector<uint32_t>* out);

    public:

      /**
      * @return The compiled meshes
      */
      const foundation::Vector<Mesh>& meshes() const;

      /**
      * @return The current error message
      */
      const foundation::String& error() const;

    private:

      tinygltf::TinyGLTF ctx_; //!< The curent tinygltf context
      foundation::Vector<Mesh> meshes_; //!< The compiled meshes
      foundation::String error_; //!< The current error message
    };

    //--------------------------------------------------------------------------
    template <typename T>
    inline bool GLTFCompiler::Compile(foundation::File& file)
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

      if (model.nodes.size() == 0)
      {
        return true;
      }

      CompileNode<T>(model, model.nodes.at(0));

      return true;
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void GLTFCompiler::CompileNode(
      const tinygltf::Model& model, 
      const tinygltf::Node& node,
      const glm::mat4x4& transform)
    {
      const std::vector<int>& children = node.children;
      glm::mat4x4 root_transform = transform * NodeTransform(node);

      if (node.mesh >= 0)
      {
        CompileMesh<T>(model, node, root_transform);
      }

      for (size_t i = 0; i < children.size(); ++i)
      {
        const tinygltf::Node& child = model.nodes.at(children.at(i));
        CompileNode<T>(model, child, root_transform);
      }
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void GLTFCompiler::CompileMesh(
      const tinygltf::Model& model, 
      const tinygltf::Node& node,
      const glm::mat4x4& transform)
    {
      const tinygltf::Mesh& mesh = model.meshes.at(node.mesh);

      Mesh m;

      ForEachPrimitive(mesh, [&](const tinygltf::Primitive& p)
      {
        size_t len, attr_count;
        const float* buffer;
        VertexAttribute attr;
        float attr_data[16];
        bool is_first = true;

        std::map<std::string, int>::const_iterator it = p.attributes.begin();
        for (; it != p.attributes.end(); ++it)
        {
          attr = GetVertexAttribute(it->first);

          if (attr == VertexAttribute::kUnknown)
          {
            continue;
          }

          buffer = GetAttributeData(model, it->second, &len, &attr_count);

          if (is_first == true)
          {
            m.vertices.resize(sizeof(T) * attr_count);
            for (size_t i = 0; i < attr_count; ++i)
            {
              *reinterpret_cast<T*>(&m.vertices.at(i * sizeof(T))) = 
                CreateDefaultVertex<T>();
            }

            is_first = false;
          }

          for (int i = static_cast<int>(attr_count) - 1; i >= 0; --i)
          {
            memcpy(attr_data, &buffer[i * len], sizeof(float) * len);

            TransformVector(attr, attr_data, transform);

            SetVertexAttribute(
              attr, 
              attr_data, 
              reinterpret_cast<T*>(&(m.vertices.at(i * sizeof(T)))));
          }
        }

        AssignIndices(model, p, &m.indices);
      });

      meshes_.push_back(m);
    }
  }
}
