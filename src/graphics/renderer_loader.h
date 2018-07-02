#pragma once

#include "graphics/definitions/shader_types.h"
#include "graphics/definitions/vertex.h"

#include <foundation/containers/vector.h>

#include <cinttypes>
#include <cstddef>

namespace snuffbox
{
  namespace graphics
  {
    /**
    * @brief Used as an interface to load GPU memory from within the
    *        native rendering API (shaders, textures, render targets, etc.)
    *
    * @author Daniel Konings
    */
    class IRendererLoader
    {

    public:

      /**
      * @brief A more descriptive name for the GPU handles
      */
      using GPUHandle = void*;

      /**
      * @brief Used to create the shader handles within the native rendering API
      *
      * @param[in] type The type of the shader to load
      *
      * @retunr The created GPU handle
      */
      virtual GPUHandle CreateShader(ShaderTypes type) = 0;

      /**
      * @brief Used to actually load a created shader
      *
      * @see IRendererLoader::CreateShader
      *
      * @param[in] buffer The buffer to read from
      * @param[in] len The length of the buffer
      *
      * @return Were we able to load the shader?
      */
      virtual bool LoadShader(
        GPUHandle handle, 
        const uint8_t* data, 
        size_t len) = 0;

      /**
      * @brief Unloads a loaded shader
      *
      * @param[in] handle The shader to unload
      */
      virtual void UnloadShader(GPUHandle handle) = 0;

      /**
      * @brief Releases a created shader
      *
      * @param[in] shader The shader to release
      */
      virtual void ReleaseShader(GPUHandle shader) = 0;

      /**
      * @return A newly created material 
      */
      virtual GPUHandle CreateMaterial() = 0;

      /**
      * @brief Loads a material from a set of shader handles
      *
      * @param[in] handle The material to load data into
      * @param[in] vs The vertex shader handle
      * @param[in] ps The pixel shader handle
      * @param[in] gs The geometry shader handle, or nullptr if unused
      *
      * @return Was the loading a success?
      */
      virtual bool LoadMaterial(
        GPUHandle handle,
        GPUHandle vs,
        GPUHandle ps,
        GPUHandle gs) = 0;

      /**
      * @brief Unloads a loaded material
      *
      * @param[in] handle The material handle to load
      */
      virtual void UnloadMaterial(GPUHandle handle) = 0;

      /**
      * @brief Releases a created material
      *
      * @param[in] handle The material handle to release
      */
      virtual void ReleaseMaterial(GPUHandle handle) = 0;

      /**
      * @brief Used to create mesh handles within the native rendering API
      */
      virtual GPUHandle CreateMesh() = 0;

      /**
      * @brief Loads a mesh from 2D vertices and index data
      *
      * @param[in] handle The mesh handle
      * @param[in] verts The vertices to load the mesh from
      * @param[in] indices The indices to load the mesh from
      *
      * @return Was the loading of the mesh a success?
      */
      virtual bool LoadMesh(
        GPUHandle handle,
        const foundation::Vector<Vertex2D>& verts,
        const foundation::Vector<Index>& indices) = 0;

      /**
      * @see IRendererLoader::LoadMesh
      *
      * @remarks Vertex3D overload
      */
      virtual bool LoadMesh(
        GPUHandle handle,
        const foundation::Vector<Vertex3D>& verts,
        const foundation::Vector<Index>& indices) = 0;

      /**
      * @brief Unloads a loaded mesh
      *
      * @param[in] handle The mesh handle
      */
      virtual void UnloadMesh(GPUHandle handle) = 0;

      /**
      * @brief Releases a mesh handle
      *
      * @param[in] handle The mesh handle
      */
      virtual void ReleaseMesh(GPUHandle handle) = 0;
    };
  }
}