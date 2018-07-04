#pragma once

#include "tools/compilers/compilers/compiler.h"

namespace snuffbox
{
  namespace compilers
  {
    /**
    * @brief Compiles models from a provided glTF format, into a binary
    *        format using TinyGLTF
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
    class ModelCompiler : public ICompiler
    {

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
    };
  }
}