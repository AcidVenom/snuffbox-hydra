#include "engine/ecs/entity.h"
#include "engine/ecs/scene.h"

#include "engine/services/scene_service.h"
#include "engine/application/application.h"

#include "engine/components/transform_component.h"
#include "engine/components/mesh_component.h"
#include "engine/components/mesh_renderer_component.h"
#include "engine/components/camera_component.h"

#include <foundation/serialization/save_archive.h>
#include <foundation/serialization/load_archive.h>

#ifndef SNUFF_NSCRIPTING
#include "engine/components/script_component.h"
#include <sparsed/entity.gen.cc>
#endif

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    const char* Entity::kDefaultName_ = "Entity";

    //--------------------------------------------------------------------------
    Entity::Entity() :
      Entity(
        Application::Instance()->GetService<SceneService>()->current_scene())
    {
      
    }

    //--------------------------------------------------------------------------
    Entity::Entity(Scene* scene, bool internal) :
      name_(kDefaultName_),
      destroyed_(false),
      active_(true),
      is_internal_(internal),
      scene_(scene),
      id_(0)
    {
      AddComponentInternal(Components::kTransform);
      scene_->AddEntity(this);
      scene_->OnSceneChanged();
    }

    //--------------------------------------------------------------------------
    IComponent* Entity::AddComponentInternal(Components id)
    {
      IComponent* ptr = CreateComponentByID(id);
      ptr->Create();

      components_[static_cast<int>(id)]
        .push_back(foundation::Memory::MakeUnique(ptr));

      return ptr;
    }

    //--------------------------------------------------------------------------
    IComponent* Entity::AddComponent(Components id)
    {
      if (
        id == Components::kTransform &&
        HasComponent(Components::kTransform) == true)
      {
        return GetComponent(Components::kTransform);
      }

      return AddComponentInternal(id);
    }

    //--------------------------------------------------------------------------
    void Entity::RemoveComponent(Components id)
    {
      if (id == Components::kTransform)
      {
        return;
      }

      ComponentArray& arr = components_[static_cast<int>(id)];

      if (arr.size() == 0)
      {
        return;
      }

      ComponentArray::iterator it = arr.begin();
      (*it)->Destroy();

      arr.erase(it);
    }

    //--------------------------------------------------------------------------
    void Entity::RemoveComponentByRef(IComponent* component)
    {
      int id = static_cast<int>(Components::kTransform) + 1;
      const int last = static_cast<int>(Components::kCount);

      IComponent* current = nullptr;
      bool found = false;

      while (id != last && found == false)
      {
        ComponentArray& arr = components_[id];
        for (size_t i = 0; i < arr.size(); ++i)
        {
          current = arr.at(i).get();

          if (current == component)
          {
            found = true;
            arr.erase(arr.begin() + i);
            break;
          }
        }

        ++id;
      }
    }

    //--------------------------------------------------------------------------
    void Entity::RemoveComponents(Components id)
    {
      while (HasComponent(id) == true)
      {
        RemoveComponent(id);
      }
    }

    //--------------------------------------------------------------------------
    bool Entity::HasComponent(Components id) const
    {
      const ComponentArray& arr = GetComponentArray(id);
      return arr.size() > 0;
    }

    //--------------------------------------------------------------------------
    IComponent* Entity::GetComponent(Components id) const
    {
      if (HasComponent(id) == false)
      {
        return nullptr;
      }

      return GetComponentArray(id).at(0).get();
    }

    //--------------------------------------------------------------------------
    foundation::Vector<IComponent*> Entity::GetComponents(Components id) const
    {
      foundation::Vector<IComponent*> result;

      const ComponentArray& arr = GetComponentArray(id);
      result.resize(arr.size());

      for (size_t i = 0; i < arr.size(); ++i)
      {
        result.at(i) = arr.at(i).get();
      }

      return result;
    }

    //--------------------------------------------------------------------------
    void Entity::Destroy()
    {
      if (destroyed_ == true)
      {
        return;
      }

      destroyed_ = true;

      TransformComponent* current = GetComponent<TransformComponent>();

      const foundation::Vector<TransformComponent*>& children =
        current->children();

      Entity* child = nullptr;
      for (size_t i = 0; i < children.size(); ++i)
      {
        child = children.at(i)->entity();
        child->Destroy();
      }

      for (size_t i = 0; i < static_cast<size_t>(Components::kCount); ++i)
      {
        components_[i].clear();
      }

      scene_->OnSceneChanged();
      scene_->RemoveEntity(this);
    }

    //--------------------------------------------------------------------------
    void Entity::set_name(const foundation::String& name)
    {
      name_ = name;
      scene_->OnSceneChanged();
    }

    //--------------------------------------------------------------------------
    const foundation::String& Entity::name() const
    {
      return name_;
    }

    //--------------------------------------------------------------------------
    void Entity::set_active(bool active)
    {
      active_ = active;
      scene_->OnSceneChanged();
    }

    //--------------------------------------------------------------------------
    bool Entity::active() const
    {
      return active_;
    }

    //--------------------------------------------------------------------------
    bool Entity::IsActive() const
    {
      TransformComponent* parent = GetComponent<TransformComponent>()->parent();

      while (parent != nullptr)
      {
        if (parent->entity()->IsActive() == false)
        {
          return false;
        }

        parent = parent->parent();
      }

      return active_;
    }

    //--------------------------------------------------------------------------
    bool Entity::is_internal() const
    {
      return is_internal_;
    }

    //--------------------------------------------------------------------------
    TransformComponent* Entity::transform() const
    {
      return GetComponent<TransformComponent>();
    }

    //--------------------------------------------------------------------------
    void Entity::IDCheck(Components id)
    {
      foundation::Logger::Assert(
        id < Components::kCount && id >= Components::kTransform,
        "Attempted to index a component type that is larger than the maximum\
         number of allowed components");
    }

    //--------------------------------------------------------------------------
    const Entity::ComponentArray& Entity::GetComponentArray(Components id) const
    {
      IDCheck(id);
      return components_[static_cast<size_t>(id)];
    }

    //--------------------------------------------------------------------------
    const Entity::ComponentCreateArray& Entity::ComponentCreators()
    {
      static ComponentCreateArray funcs;

      if (funcs.size() == 0)
      {
        funcs.resize(static_cast<size_t>(Components::kCount));
        AssignComponentCreator<Components::kTransform>(funcs);
      }

      return funcs;
    }

    //--------------------------------------------------------------------------
    IComponent* Entity::CreateComponentByID(Components id)
    {
      const ComponentCreateArray& c = ComponentCreators();
      return c.at(static_cast<size_t>(id))(this);
    }

    //--------------------------------------------------------------------------
    void Entity::set_id(size_t id)
    {
      id_ = id;
    }


    //--------------------------------------------------------------------------
    void Entity::Start()
    {
      for (
        int i = static_cast<int>(Components::kTransform);
        i != static_cast<int>(Components::kCount);
        ++i)
      {
        ComponentArray& arr = components_[i];

        for (size_t j = 0; j < arr.size(); ++j)
        {
          arr.at(j)->Start();
        }
      }
    }

    //--------------------------------------------------------------------------
    void Entity::Update(float dt)
    {
      if (IsActive() == false)
      {
        return;
      }

      IComponent* comp = nullptr;

      for (
        unsigned int i = 0; 
        i < static_cast<unsigned int>(Components::kCount); 
        ++i)
      {
        ComponentArray& arr = components_[i];
        for (size_t c = 0; c < arr.size(); ++c)
        {
          comp = arr.at(c).get();
          if (comp->active() == false)
          {
            continue;
          }

          comp->Update(dt);
        }
      }

      OnUpdate(dt);
    }

    //--------------------------------------------------------------------------
    void Entity::OnUpdate(float dt)
    {

    }

    //--------------------------------------------------------------------------
    Scene* Entity::scene() const
    {
      return scene_;
    }

    //--------------------------------------------------------------------------
    size_t Entity::id() const
    {
      return id_;
    }

    //--------------------------------------------------------------------------
    void Entity::Serialize(foundation::SaveArchive& archive) const
    {
      archive(
        SET_ARCHIVE_PROP(name_), 
        SET_ARCHIVE_PROP(active_),
        SET_ARCHIVE_PROP(id_));

      foundation::Vector<SerializedComponent> components;

      size_t off = 0;

      for (
        int i = static_cast<int>(Components::kTransform);
        i < static_cast<int>(Components::kCount);
        ++i)
      {
        const ComponentArray& arr = components_[i];
        off = components.size();
        components.resize(components.size() + arr.size());

        for (size_t j = 0; j < arr.size(); ++j)
        {
          SerializedComponent& sc = components.at(off + j);
          sc.type = static_cast<Components>(i);
          sc.data = arr.at(j).get();
        }
      }

      archive(SET_ARCHIVE_PROP(components));
    }

    //--------------------------------------------------------------------------
    void Entity::Deserialize(foundation::LoadArchive& archive)
    {
      archive(
        GET_ARCHIVE_PROP(name_), 
        GET_ARCHIVE_PROP(active_),
        GET_ARCHIVE_PROP(id_));

      foundation::Vector<SerializedComponent> components;

      size_t n = archive.GetArraySize("components");
      components.resize(n);

      SerializedComponent sc;

      for (size_t i = 0; i < n; ++i)
      {
        sc.data = nullptr;
        archive.GetArrayValue("components", i, &sc);
        sc.data = AddComponent(sc.type);

        components.at(i) = sc;
      }

      archive(
        GET_ARCHIVE_PROP(components));
    }

    //--------------------------------------------------------------------------
    Entity::~Entity()
    {
      Destroy();
    }

#ifndef SNUFF_NSCRIPTING
    //--------------------------------------------------------------------------
    Entity::Entity(scripting::ScriptArgs& args) :
      Entity()
    {
      
    }

    //--------------------------------------------------------------------------
    scripting::ScriptObjectHandle Entity::CreateScriptComponent(
      Entity* e,
      IComponent* c,
      Components type)
    {
      scripting::ScriptObjectHandle comp = 
        scripting::ScriptValue::CreateObject();

      comp->SetPointer(c, c->GetScriptName());
      
      return comp;
    }

    //--------------------------------------------------------------------------
    SPARSE_CUSTOM(Entity, AddComponent)
    {
      Entity* self = args.GetSelf<Entity>();
      if (self == nullptr || args.Check("N") == false)
      {
        return false;
      }

      Components c = args.Get<Components>(0, Components::kCount);
      IComponent* ptr = self->AddComponent(c);

      scripting::ScriptObjectHandle h = 
        Entity::CreateScriptComponent(self, ptr, c);

      args.AddReturnValue(h);

      return true;
    }

    //--------------------------------------------------------------------------
    SPARSE_CUSTOM(Entity, GetComponent)
    {
      Entity* self = args.GetSelf<Entity>();
      if (self == nullptr || args.Check("N") == false)
      {
        return false;
      }

      Components c = args.Get<Components>(0, Components::kCount);
      IComponent* ptr = self->GetComponent(c);

      if (ptr == nullptr)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kScript,
          foundation::LogSeverity::kError,
          "Attempted to retrieve a component but it didn't exist"
          );

        return false;
      }

      scripting::ScriptObjectHandle h = 
        Entity::CreateScriptComponent(self, ptr, c);

      args.AddReturnValue(h);

      return true;
    }

    //--------------------------------------------------------------------------
    SPARSE_CUSTOM(Entity, GetComponents)
    {
      Entity* self = args.GetSelf<Entity>();
      if (self == nullptr || args.Check("N") == false)
      {
        return false;
      }

      Components c = args.Get<Components>(0, Components::kCount);
      foundation::Vector<IComponent*> comps = self->GetComponents(c);

      foundation::SharedPtr<scripting::ScriptArray> arr = 
        scripting::ScriptValue::CreateArray();

      for (size_t i = 0; i < comps.size(); ++i)
      {
        arr->Add(Entity::CreateScriptComponent(self, comps.at(i), c));
      }

      args.AddReturnValue(arr);

      return true;
    }
#endif
  }

  namespace foundation
  {
    //--------------------------------------------------------------------------
    template <>
    inline void SaveArchive::Serialize(
      foundation::SaveArchive& archive, 
      const engine::Entity::SerializedComponent& value)
    {
      engine::Components type = value.type;
      engine::IComponent* data = value.data;

      archive(
        SET_ARCHIVE_PROP(type),
        SET_ARCHIVE_PROP(data));
    }

    //--------------------------------------------------------------------------
    template <>
    inline void LoadArchive::Deserialize(
      foundation::LoadArchive& archive,
      engine::Entity::SerializedComponent* out)
    {
      engine::Components type;
      engine::IComponent* data = out->data;

      if (data == nullptr)
      {
        archive(
          GET_ARCHIVE_PROP(type));
      }
      else
      {
        archive(
          GET_ARCHIVE_PROP(type),
          GET_ARCHIVE_PROP(data));
      }

      engine::Entity::SerializedComponent& sc = *out;
      sc.type = type;
    }
  }
}