#include "engine/ecs/entity.h"
#include "engine/ecs/scene.h"

#include "engine/services/scene_service.h"
#include "engine/application/application.h"

#include "engine/components/transform_component.h"

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
      name_(kDefaultName_),
      destroyed_(false),
      active_(true),
      scene_(
        Application::Instance()->GetService<SceneService>()->current_scene())
    {
      AddComponentInternal(Components::kTransform);

      scene_->AddEntity(this);
    }

    //--------------------------------------------------------------------------
    IComponent* Entity::AddComponentInternal(Components id)
    {
      IComponent* ptr = CreateComponentByID(id);
      ptr->Create();

      GetComponentArray(id).push_back(foundation::Memory::MakeUnique(ptr));

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

      ComponentArray& arr = GetComponentArray(id);

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
        ComponentArray& arr = GetComponentArray(static_cast<Components>(id));
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
    bool Entity::HasComponent(Components id)
    {
      ComponentArray& arr = GetComponentArray(id);
      return arr.size() > 0;
    }

    //--------------------------------------------------------------------------
    IComponent* Entity::GetComponent(Components id)
    {
      if (HasComponent(id) == false)
      {
        return nullptr;
      }

      return GetComponentArray(id).at(0).get();
    }

    //--------------------------------------------------------------------------
    foundation::Vector<IComponent*> Entity::GetComponents(Components id)
    {
      foundation::Vector<IComponent*> result;

      ComponentArray& arr = GetComponentArray(id);
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

        if (child->is_from_script() == false)
        {
          foundation::Memory::Destruct<Entity>(child);
        }
      }

      scene_->RemoveEntity(this);

      for (size_t i = 0; i < static_cast<size_t>(Components::kCount); ++i)
      {
        components_[i].clear();
      }
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
    Entity::ComponentArray& Entity::GetComponentArray(Components id)
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
    void Entity::Update(float dt)
    {
      if (active_ == false)
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
    }

    //--------------------------------------------------------------------------
    Scene* Entity::scene() const
    {
      return scene_;
    }

    //--------------------------------------------------------------------------
    void Entity::set_name(const foundation::String& name)
    {
      name_ = name;
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
    }

    //--------------------------------------------------------------------------
    bool Entity::active() const
    {
      return active_;
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

      scripting::ScriptObjectHandle ent = 
        scripting::ScriptValue::CreateObject();

      ent->SetPointer<Entity>(e);

      comp->SetPointer(c, c->GetScriptName());
      comp->Insert("entity", ent);

      if (type != Components::kTransform)
      {
        scripting::ScriptObjectHandle transform = 
          CreateScriptComponent(
            e, 
            e->GetComponent<TransformComponent>(),
            Components::kTransform);

        comp->Insert("transform", transform);
      }

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
}