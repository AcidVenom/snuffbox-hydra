#include "engine/components/transform_component.h"
#include "engine/ecs/entity.h"
#include "engine/ecs/scene.h"

#ifndef SNUFF_NSCRIPTING
#include <sparsed/transform_component.gen.cc>
#endif

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    const glm::vec3 TransformComponent::kWorldUp_{ 0.0f, 1.0f, 0.0f };
    const glm::vec3 TransformComponent::kWorldForward_{ 0.0f, 0.0f, 1.0f };
    const glm::vec3 TransformComponent::kWorldRight_{ 1.0f, 0.0f, 0.0f };

    //--------------------------------------------------------------------------
    TransformComponent::TransformComponent(Entity* entity) :
      ComponentBase<TransformComponent, Components::kTransform>(entity),
      parent_(nullptr),
      position_(glm::vec3{0.0f, 0.0f, 0.0f}),
      rotation_(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
      scale_(glm::vec3{1.0f, 1.0f, 1.0f}),
      local_to_world_(glm::mat4x4(1.0f)),
      world_to_local_(glm::mat4x4(1.0f)),
      is_dirty_(DirtyFlags::kNone)
    {

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
        if (parent->parent() == this || parent == this)
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
    void TransformComponent::SetPosition(const glm::vec3& position)
    {
      position_ = InvTransformPoint(position);
      MarkDirty(DirtyFlags::kSelf);
    }

    //--------------------------------------------------------------------------
    void TransformComponent::SetLocalPosition(const glm::vec3& position)
    {
      position_ = position;
      MarkDirty(DirtyFlags::kSelf);
    }

    //--------------------------------------------------------------------------
    glm::vec3 TransformComponent::GetPosition()
    {
      return TransformPoint(glm::vec3{ 0.0f, 0.0f, 0.0f });
    }

    //--------------------------------------------------------------------------
    const glm::vec3& TransformComponent::GetLocalPosition()
    {
      return position_;
    }

    //--------------------------------------------------------------------------
    void TransformComponent::SetRotation(const glm::quat& rotation)
    {
      rotation_ = rotation;
      MarkDirty(DirtyFlags::kSelf);
    }

    //--------------------------------------------------------------------------
    void TransformComponent::SetRotationEuler(const glm::vec3& rotation)
    {
      rotation_ = glm::quat(rotation);
      MarkDirty(DirtyFlags::kSelf);
    }

    //--------------------------------------------------------------------------
    void TransformComponent::RotateAxis(const glm::vec3& axis, float angle)
    {
      rotation_ = glm::angleAxis(angle, axis);
      MarkDirty(DirtyFlags::kSelf);
    }

    //--------------------------------------------------------------------------
    const glm::quat& TransformComponent::GetRotation() const
    {
      return rotation_;
    }

    //--------------------------------------------------------------------------
    glm::vec3 TransformComponent::GetRotationEuler() const
    {
      return glm::eulerAngles(rotation_);
    }

    //--------------------------------------------------------------------------
    void TransformComponent::SetScale(const glm::vec3& scale)
    {
      scale_ = scale;
      MarkDirty(DirtyFlags::kSelf);
    }

    //--------------------------------------------------------------------------
    const glm::vec3& TransformComponent::GetScale() const
    {
      return scale_;
    }

    //--------------------------------------------------------------------------
    glm::vec3 TransformComponent::TransformPoint(const glm::vec3& point)
    {
      UpdateFromTop();
      glm::vec4 to_transform{ point.x, point.y, point.z, 1.0f };
      return local_to_world_ * to_transform;
    }

    //--------------------------------------------------------------------------
    glm::vec3 TransformComponent::InvTransformPoint(const glm::vec3& point)
    {
      UpdateFromTop();
      glm::vec4 to_transform{ point.x, point.y, point.z, 1.0f };
      return world_to_local_ * to_transform;
    }

    //--------------------------------------------------------------------------
    glm::vec3 TransformComponent::TransformDirection(const glm::vec3& direction)
    {
      UpdateFromTop();
      glm::vec4 to_transform{ direction.x, direction.y, direction.z, 0.0f };
      return local_to_world_ * to_transform;
    }

    //--------------------------------------------------------------------------
    glm::vec3 TransformComponent::InvTransformDirection(
      const glm::vec3& direction)
    {
      UpdateFromTop();
      glm::vec4 to_transform{ direction.x, direction.y, direction.z, 0.0f };
      return world_to_local_ * to_transform;
    }

    //--------------------------------------------------------------------------
    glm::vec3 TransformComponent::Up()
    {
      UpdateFromTop();
      return TransformDirection(kWorldUp_);
    }

    //--------------------------------------------------------------------------
    glm::vec3 TransformComponent::Forward()
    {
      UpdateFromTop();
      return TransformDirection(kWorldForward_);
    }

    //--------------------------------------------------------------------------
    glm::vec3 TransformComponent::Right()
    {
      UpdateFromTop();
      return TransformDirection(kWorldRight_);
    }

    //--------------------------------------------------------------------------
    void TransformComponent::TranslateLocal(const glm::vec3& translation)
    {
      UpdateFromTop();

      glm::vec3 up = TransformDirection(kWorldUp_) * translation;
      glm::vec3 forward = TransformDirection(kWorldForward_) * translation;
      glm::vec3 right = TransformDirection(kWorldRight_) * translation;

      glm::vec3 movement = up + forward + right;

      position_ += movement;
      MarkDirty(DirtyFlags::kSelf);
    }

    //--------------------------------------------------------------------------
    void TransformComponent::Translate(const glm::vec3& translation)
    {
      glm::vec3 position = GetPosition();
      position += translation;

      SetPosition(position);
    }

    //--------------------------------------------------------------------------
    void TransformComponent::SetParentRaw(TransformComponent* parent)
    {
      parent_ = parent;

      if (parent_ != nullptr)
      {
        parent_->MarkDirty(DirtyFlags::kChild);
      }
    }

    //--------------------------------------------------------------------------
    void TransformComponent::MarkDirty(DirtyFlags flag)
    {
      is_dirty_ |= flag;

      if (flag == DirtyFlags::kSelf)
      {
        TransformComponent* current = parent_;

        while (current != nullptr)
        {
          current->MarkDirty(DirtyFlags::kChild);
          current = current->parent_;
        }

        for (size_t i = 0; i < children_.size(); ++i)
        {
          children_.at(i)->MarkDirty(DirtyFlags::kParent);
        }
      }
    }

    //--------------------------------------------------------------------------
    void TransformComponent::UpdateFromTop()
    {
      TransformComponent* top = this;
      TransformComponent* current = parent_;

      while (current != nullptr)
      {
        top = current;
        current = current->parent_;
      }

      top->UpdateMatrices();
    }

    //--------------------------------------------------------------------------
    void TransformComponent::UpdateMatrices()
    {
      if (is_dirty_ == DirtyFlags::kNone)
      {
        return;
      }

      bool updated = false;

      if ((is_dirty_ & DirtyFlags::kSelf) == DirtyFlags::kSelf)
      {
        local_to_world_ = glm::mat4x4(1.0f);

        local_to_world_ = glm::translate(local_to_world_, position_);
        local_to_world_ *= glm::toMat4(rotation_);
        local_to_world_ = glm::scale(local_to_world_, scale_);

        if (parent_ != nullptr)
        {
          local_to_world_ *= parent_->local_to_world_;
        }

        updated = true;
      }

      if ((is_dirty_ & DirtyFlags::kParent) == DirtyFlags::kParent)
      {
        local_to_world_ = local_to_world_ * parent_->local_to_world_;

        updated = true;
      }

      if ((is_dirty_ & DirtyFlags::kChild) == DirtyFlags::kChild == true)
      {
        for (size_t i = 0; i < children_.size(); ++i)
        {
          children_.at(i)->UpdateMatrices();
        }
      }

      if (updated == true)
      {
        world_to_local_ = glm::inverse(local_to_world_);
      }

      is_dirty_ = DirtyFlags::kNone;
    }

    //--------------------------------------------------------------------------
    void TransformComponent::Update(float dt)
    {
      if (parent_ != nullptr)
      {
        return;
      }

      UpdateMatrices();
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