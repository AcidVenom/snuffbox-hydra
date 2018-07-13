#include "engine/graphics/mesh.h"
#include "engine/assets/asset.h"
#include "engine/assets/model_asset.h"

#include "engine/application/application.h"
#include "engine/services/renderer_service.h"

#ifndef SNUFF_NSCRIPTING
#include <sparsed/mesh.gen.cc>
#endif

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    Mesh::Mesh() :
      asset_(nullptr),
      index_(-1),
      gpu_handle_(nullptr),
      renderer_(Application::Instance()->GetService<RendererService>())
    {
      
    }

    //--------------------------------------------------------------------------
    Mesh::Mesh(IAsset* model, int idx) :
      Mesh()
    {
      FromModel(model, idx);
    }

    //--------------------------------------------------------------------------
    Mesh::Mesh(Mesh&& other) noexcept
    {
      *this = eastl::move(other);
    }

    //--------------------------------------------------------------------------
    Mesh& Mesh::operator=(Mesh&& other) noexcept
    {
      if (&other == this)
      {
        return *this;
      }

      Release();

      asset_ = other.asset_;
      index_ = other.index_;

      gpu_handle_ = other.gpu_handle_;
      renderer_ = other.renderer_;

      other.gpu_handle_ = nullptr;
      other.asset_ = nullptr;
      other.index_ = -1;

      return *this;
    }

    //--------------------------------------------------------------------------
    void Mesh::FromModel(IAsset* model, int idx)
    {
      if (model == nullptr || model->type() != compilers::AssetTypes::kModel)
      {
        return;
      }

      Release();

      asset_ = static_cast<ModelAsset*>(model);
      index_ = idx;
    }

    //--------------------------------------------------------------------------
    void Mesh::Set2D(
      const foundation::Vector<graphics::Vertex2D>& verts,
      const foundation::Vector<graphics::Index>& indices)
    {
      asset_ = nullptr;
      index_ = -1;

      Release();

      gpu_handle_ = renderer_->GetLoader()->CreateMesh();
      renderer_->GetLoader()->LoadMesh(gpu_handle_, verts, indices);
    }

    //--------------------------------------------------------------------------
    void Mesh::Set3D(
      const foundation::Vector<graphics::Vertex3D>& verts,
      const foundation::Vector<graphics::Index>& indices)
    {
      asset_ = nullptr;
      index_ = -1;

      Release();

      gpu_handle_ = renderer_->GetLoader()->CreateMesh();
      renderer_->GetLoader()->LoadMesh(gpu_handle_, verts, indices);
    }

    //--------------------------------------------------------------------------
    void* Mesh::GetGPUHandle() const
    {
      if (asset_ != nullptr)
      {
        return asset_->GetGPUHandle(index_);
      }

      if (gpu_handle_ != nullptr)
      {
        return gpu_handle_;
      }

      return nullptr;
    }

    //--------------------------------------------------------------------------
    bool Mesh::IsValid() const
    {
      return GetGPUHandle() != nullptr;
    }

    //--------------------------------------------------------------------------
    ModelAsset* Mesh::asset() const
    {
      return asset_;
    }

    //--------------------------------------------------------------------------
    int Mesh::index() const
    {
      return index_;
    }

    //--------------------------------------------------------------------------
    void Mesh::Release()
    {
      if (gpu_handle_ == nullptr)
      {
        return;
      }

      renderer_->GetLoader()->ReleaseMesh(gpu_handle_);
      gpu_handle_ = nullptr;
    }

    //--------------------------------------------------------------------------
    Mesh::~Mesh()
    {
      Release();
    }

#ifndef SNUFF_NSCRIPTING
    //--------------------------------------------------------------------------
    Mesh::Mesh(scripting::ScriptArgs& args) :
      Mesh()
    {

    }

    //--------------------------------------------------------------------------
    void Mesh::GetIndicesFromScript(
      scripting::ScriptArgs& args,
      uint8_t idx,
      foundation::Vector<graphics::Index>* out)
    {
      scripting::ScriptArray* indices = 
        static_cast<scripting::ScriptArray*>(args.GetArgument(idx));

      if (out == nullptr)
      {
        return;
      }

      out->resize(indices->size());

      scripting::ScriptNumber* num = nullptr;

      for (size_t i = 0; i < indices->size(); ++i)
      {
        if (indices->at(i)->type() != scripting::ScriptValueTypes::kNumber)
        {
          foundation::Logger::LogVerbosity<1>(
            foundation::LogChannel::kScript,
            foundation::LogSeverity::kError,
            "Index for a mesh is not a number type (index number {0})",
            i);

          return;
        }

        num = static_cast<scripting::ScriptNumber*>(indices->at(i).get());
        out->at(i) = static_cast<graphics::Index>(num->value());
      }
    }

    //--------------------------------------------------------------------------
    bool Mesh::ForEachObject(
      const scripting::ScriptArray* arr,
      const foundation::Function<
      bool(const scripting::ScriptObject*, size_t)>& del)
    {
      scripting::ScriptHandle val;
      scripting::ScriptObjectHandle obj;

      for (size_t i = 0; i < arr->size(); ++i)
      {
        val = arr->at(i);
        if (val->type() != scripting::ScriptValueTypes::kObject)
        {
          foundation::Logger::LogVerbosity<1>(
            foundation::LogChannel::kScript,
            foundation::LogSeverity::kError,
            "Unexpected scriptable value type ({0}) at array index {1}",
            static_cast<int>(val->type()),
            i);

          return false;
        }

        obj = eastl::static_shared_pointer_cast<scripting::ScriptObject>(val);

        if (del != nullptr)
        {
          if (del(obj.get(), i) == false)
          {
            break;
          }
        }
      }

      return true;
    }

    //--------------------------------------------------------------------------
    SPARSE_CUSTOM(Mesh, Set2D)
    {
      if (args.Check("AA") == false)
      {
        return false;
      }

      Mesh* self = args.GetSelf<Mesh>();
      if (self == nullptr)
      {
        return false;
      }

      scripting::ScriptArray* arr = 
        static_cast<scripting::ScriptArray*>(args.GetArgument(0));

      foundation::Vector<graphics::Vertex2D> verts;
      verts.resize(arr->size());

      if (Mesh::ForEachObject(arr,
      [&](const scripting::ScriptObject* obj, size_t i)
      {
        graphics::Vertex2D& v = verts.at(i);

        scripting::ScriptValue* value = obj->Get("position");
        v.position = args.GetVectorValue(value, glm::vec3(0.0f, 0.0f, 0.0f));

        value = obj->Get("color");
        v.color = args.GetVectorValue(value, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));

        value = obj->Get("uv");
        v.uv = args.GetVectorValue(value, glm::vec2(0.0f, 0.0f));

        return true;
      }) == false)
      {
        return false;
      }

      foundation::Vector<graphics::Index> indices;
      Mesh::GetIndicesFromScript(args, 1, &indices);

      self->Set2D(verts, indices);

      return true;
    }

    //--------------------------------------------------------------------------
    SPARSE_CUSTOM(Mesh, Set3D)
    {
      if (args.Check("AA") == false)
      {
        return false;
      }

      Mesh* self = args.GetSelf<Mesh>();
      if (self == nullptr)
      {
        return false;
      }

      scripting::ScriptArray* arr = 
        static_cast<scripting::ScriptArray*>(args.GetArgument(0));

      foundation::Vector<graphics::Vertex3D> verts;
      verts.resize(arr->size());

      if (Mesh::ForEachObject(arr,
      [&](const scripting::ScriptObject* obj, size_t i)
      {
        graphics::Vertex3D& v = verts.at(i);

        scripting::ScriptValue* value = obj->Get("position");
        v.position = args.GetVectorValue(value, glm::vec3(0.0f, 0.0f, 0.0f));

        value = obj->Get("color");
        v.color = args.GetVectorValue(value, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));

        value = obj->Get("normal");
        v.normal = args.GetVectorValue(value, glm::vec3(0.0f, 0.0f, 1.0f));

        value = obj->Get("tangent");
        v.tangent = args.GetVectorValue(value, glm::vec3(1.0f, 0.0f, 0.0f));

        value = obj->Get("uv");
        v.uv = args.GetVectorValue(value, glm::vec2(0.0f, 0.0f));

        return true;
      }) == false)
      {
        return false;
      }

      foundation::Vector<graphics::Index> indices;
      Mesh::GetIndicesFromScript(args, 1, &indices);

      self->Set3D(verts, indices);

      return true;
    }
#endif
  }
}