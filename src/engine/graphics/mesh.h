#pragma once

#include <graphics/definitions/vertex.h>

#include <scripting/script_class.h>

#include <foundation/containers/function.h>

#include <cinttypes>

namespace snuffbox
{
  namespace engine
  {
    class IAsset;
    class ModelAsset;
    class RendererService;

    /**
    * @brief A mesh class that can be used both from an asset or as a
    *        dynamic mesh
    *
    * When the mesh is used as an asset, the vertex data is shared between
    * multiple meshes across multiple mesh components.
    *
    * If the mesh is a dynamic mesh, created from code, the mesh's GPU resource
    * is created anew per mesh component. This makes sure that the mesh data
    * is always valid, even when this mesh itself is destructed through
    * its lifetime.
    *
    * Meshes can only be moved to avoid double deletions of the underlying
    * GPU handle.
    *
    * @author Daniel Konings
    */
    SCRIPT_CLASS() class Mesh : public scripting::ScriptClass
    {

    public:

      SCRIPT_NAME(Mesh);
      SCRIPT_CONSTRUCTOR(Mesh);

      /**
      * @brief Default constructor
      */
      Mesh();

      /**
      * @see Mesh::FromModel
      */
      Mesh(IAsset* model, int idx);

      /**
      * @brief Delete copy constructor
      */
      Mesh(const Mesh&) = delete;

      /**
      * @brief Move constructor, sets the GPU handle of the copied
      *        mesh to nullptr to avoid double deletions
      *
      * @param[in] other The copied mesh
      */
      Mesh(Mesh&& other) noexcept;

      /**
      * @brief Delete copy assignment operator
      */
      Mesh& operator=(const Mesh&) = delete;

      /**
      * @brief Move assignment operator
      *
      * @see The move constructor
      */
      Mesh& operator=(Mesh&& other) noexcept;

      /**
      * @brief Sets this mesh based on a model asset and a scene index in the
      *        model
      *
      * @param[in] model The model asset
      * @param[in] idx The scene index within the model
      */
      SCRIPT_FUNC() void FromModel(IAsset* model, int idx);

      /**
      * @brief Sets the 2D vertices of the mesh, together with respective
      *        indices
      *
      * @param[in] verts The 2D vertices to set
      * @param[in] indices The indices to set
      */
      SCRIPT_FUNC(custom) void Set2D(
        const foundation::Vector<graphics::Vertex2D>& verts,
        const foundation::Vector<graphics::Index>& indices);

      /**
      * @brief Sets the 3D vertices of the mesh, together with respective
      *        indices
      *
      * @param[in] verts The 3D vertices to set
      * @param[in] indices The indices to set
      */
      SCRIPT_FUNC(custom) void Set3D(
        const foundation::Vector<graphics::Vertex3D>& verts,
        const foundation::Vector<graphics::Index>& indices);

      /**
      * @return The GPU handle of this mesh, or of the underlying model asset
      */
      void* GetGPUHandle() const;

      /**
      * @return Is this mesh valid for use?
      */
      bool IsValid() const;

      /**
      * @return The underlying model asset
      */
      ModelAsset* asset() const;

      /**
      * @return The scene index of this mesh if it is a model
      */
      int index() const;

    protected:

      /**
      * @brief Frees the GPU handle of this mesh, if available
      *
      * @remarks This does not release the GPU handle contained by a model
      *          asset
      */
      void Release();

    public:

#ifndef SNUFF_NSCRIPTING

      /**
      * @brief Retrieves an indices array from script arguments
      *
      * @param[in] args The arguments to retrieve the indices from
      * @param[in] idx The argument index
      * @param[out] out The retrieved indices
      */
      static void GetIndicesFromScript(
        scripting::ScriptArgs& args,
        uint8_t idx,
        foundation::Vector<graphics::Index>* out);

      /**
      * @brief Call a delegate on each object in a scripting array
      *
      * @param[in] arr The array to loop through
      * @param[in] del The delegate to call
      *
      * @remarks The delegate should return true to continue or false to break
      */
      static bool ForEachObject(
        const scripting::ScriptArray* arr, 
        const foundation::Function<
        bool(const scripting::ScriptObject*, size_t)>& del);

#endif

      /**
      * @see Mesh::Release
      */
      ~Mesh();

    private:

      ModelAsset* asset_; //!< The underlying model asset
      int index_; //!< The scene index of this mesh in the model
      
      void* gpu_handle_; //!< The GPU handle of the mesh if dynamic

      RendererService* renderer_; //!< The renderer service
    };
  }
}