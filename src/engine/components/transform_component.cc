#include "engine/components/transform_component.h"
#include "engine/ecs/entity.h"
#include "engine/ecs/scene.h"

#ifndef SNUFF_NSCRIPTING
#include <sparsed/transform_component.gen.cc>
#endif

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    TransformComponent::TransformComponent(Entity* entity) :
      ComponentBase<TransformComponent, Components::kTransform>(entity),
      parent_(nullptr)
    {
      entity->scene()->UpdateHierarchy(this);
    }

    //--------------------------------------------------------------------------
    int TransformComponent::HasChild(TransformComponent* child)
    {
      int idx = -1;
      for (int i = 0; i < static_cast<int>(children_.size()); ++i)
      {
        if (children_.at(i) == child)
        {
          idx = i;
          break;
        }
      }

      return idx;
    }

    //--------------------------------------------------------------------------
    void TransformComponent::Attach(TransformComponent* child)
    {
      if (HasChild(child) >= 0)
      {
        return;
      }

      children_.push_back(child);
    }

    //--------------------------------------------------------------------------
    void TransformComponent::Detach(TransformComponent* child)
    {
      int idx;
      if ((idx = HasChild(child)) < 0)
      {
        return;
      }

      children_.erase(children_.begin() + idx);
      child->SetParentRaw(nullptr);
    }

    //--------------------------------------------------------------------------
    void TransformComponent::DetachAll()
    {
      for (size_t i = 0; i < children_.size(); ++i)
      {
        children_.at(i)->SetParentRaw(nullptr);
      }

      children_.clear();
    }

    //--------------------------------------------------------------------------
    void TransformComponent::DetachAt(int idx)
    {
      unsigned int i = static_cast<unsigned int>(idx);

      if (i >= children_.size())
      {
        return;
      }

      foundation::Vector<TransformComponent*>::iterator it = 
        children_.begin() + i;

      (*it)->SetParentRaw(nullptr);
      children_.erase(it);
    }

    //--------------------------------------------------------------------------
    TransformComponent* TransformComponent::GetChildAt(int idx) const
    {
      unsigned int i = static_cast<unsigned int>(idx);

      if (i >= children_.size())
      {
        return nullptr;
      }

      return children_.at(i);
    }

    //--------------------------------------------------------------------------
    void TransformComponent::SetParent(TransformComponent* parent)
    {
      if (parent != nullptr)
      {
        if (parent->parent() == this)
        {
          return;
        }
      }

      if (parent_ != nullptr)
      {
        parent_->Detach(this);
      }

      if (parent != nullptr)
      {
        parent->Attach(this);
      }

      SetParentRaw(parent);
    }

    //--------------------------------------------------------------------------
    TransformComponent* TransformComponent::parent() const
    {
      return parent_;
    }

    //--------------------------------------------------------------------------
    const foundation::Vector<TransformComponent*>& 
      TransformComponent::children() const
    {
      return children_;
    }

    //--------------------------------------------------------------------------
    void TransformComponent::SetParentRaw(TransformComponent* parent)
    {
      parent_ = parent;
      entity()->scene()->UpdateHierarchy(this);
    }

    //--------------------------------------------------------------------------
    void TransformComponent::Update(float dt)
    {
      if (parent_ != nullptr)
      {
        return;
      }
    }

    //--------------------------------------------------------------------------
    TransformComponent::~TransformComponent()
    {
      SetParent(nullptr);
    }

#ifndef SNUFF_NSCRIPTING
    //--------------------------------------------------------------------------
    SPARSE_CUSTOM(TransformComponent, children)
    {
      TransformComponent* self = args.GetSelf<TransformComponent>();

      if (self == nullptr)
      {
        return false;
      }

      const foundation::Vector<TransformComponent*>& c = self->children();

      scripting::ScriptArrayHandle a = scripting::ScriptValue::CreateArray();
      scripting::ScriptObjectHandle o;
      scripting::ScriptObjectHandle e;
      TransformComponent* child;

      for (size_t i = 0; i < c.size(); ++i)
      {
        child = c.at(i);

        e = scripting::ScriptValue::CreateObject();
        e->SetPointer<Entity>(child->entity());

        o = scripting::ScriptValue::CreateObject();
        o->SetPointer<TransformComponent>(child);

        o->Insert("entity", e);

        a->Add(o);
      }

      args.AddReturnValue(a);

      return true;
    }
#endif
  }
}