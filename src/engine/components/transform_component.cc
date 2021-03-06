#include "engine/components/transform_component.h"
#include "engine/ecs/entity.h"
#include "engine/ecs/scene.h"

#include <foundation/serialization/save_archive.h>
#include <foundation/serialization/load_archive.h>

#ifndef SNUFF_NSCRIPTING
#include <sparsed/transform_component.gen.cc>
#endif

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

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
      position_(glm::vec3{ 0.0f, 0.0f, 0.0f }),
      rotation_(glm::vec3{ 0.0f, 0.0f, 0.0f }),
      euler_angles_(glm::vec3{ 0.0f, 0.0f, 0.0f }),
      scale_(glm::vec3{ 1.0f, 1.0f, 1.0f }),
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
      if (child == nullptr || child == this || HasChild(child) >= 0)
      {
        return;
      }

      glm::vec3 old_position = child->GetPosition();
      glm::vec3 old_up = child->Up();
      glm::vec3 old_forward = child->Forward();
      glm::vec3 old_scale = child->GetWorldScale();

      if (child->parent_ != nullptr)
      {
        child->parent_->Detach(child);
      }

      child->entity()->set_sort_index(static_cast<int>(children_.size()));

      children_.push_back(child);
      child->SetParentRaw(this);

      child->SetWorldScale(old_scale);
      child->SetPosition(old_position);
      child->SetWorldRotation(
        InvTransformDirection(old_forward),
        InvTransformDirection(old_up));

      UpdateFromTop();
    }

    //--------------------------------------------------------------------------
    void TransformComponent::Detach(TransformComponent* child)
    {
      int idx;
      if (child == nullptr || child == this || (idx = HasChild(child)) < 0)
      {
        return;
      }

      child->entity()->set_sort_index(
        static_cast<int>(entity()->scene()->TopLevelTransforms().size()));

      glm::vec3 old_position = child->GetPosition();
      glm::vec3 old_up = child->Up();
      glm::vec3 old_forward = child->Forward();
      glm::vec3 old_scale = child->GetWorldScale();

      children_.erase(children_.begin() + idx);
      child->SetParentRaw(nullptr);

      child->SetPosition(old_position);
      child->SetWorldRotation(old_forward, old_up);
      child->SetWorldScale(old_scale);

      UpdateFromTop();
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
        parent->Attach(this);
        return;
      }

      if (parent_ != nullptr)
      {
        parent_->Detach(this);
      }
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
      if (parent_ != nullptr)
      {
        position_ = parent_->InvTransformPoint(position);
      }
      else
      {
        position_ = position;
      }
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
      euler_angles_ = glm::degrees(glm::eulerAngles(rotation_));
      MarkDirty(DirtyFlags::kSelf);
    }

    //--------------------------------------------------------------------------
    void TransformComponent::SetWorldRotation(
      const glm::vec3& forward,
      const glm::vec3& up)
    {
      SetRotation(glm::quatLookAtLH(forward, up));
    }

    //--------------------------------------------------------------------------
    void TransformComponent::SetRotationEuler(const glm::vec3& rotation)
    {
      euler_angles_ = rotation;

      for (glm::length_t i = 0; i < euler_angles_.length(); ++i)
      {
        float& comp = euler_angles_[i];
        while (comp < -360.0f)
        {
          comp += 360.0f;
        }

        while (comp > 360.0f)
        {
          comp -= 360.0f;
        }
      }

      rotation_ = glm::quat(glm::radians(euler_angles_));
      MarkDirty(DirtyFlags::kSelf);
    }

    //--------------------------------------------------------------------------
    void TransformComponent::RotateAxis(const glm::vec3& axis, float angle)
    {
      rotation_ *= glm::angleAxis(angle, axis);
      euler_angles_ = glm::degrees(glm::eulerAngles(rotation_));
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
      return euler_angles_;
    }

    //--------------------------------------------------------------------------
    void TransformComponent::SetScale(const glm::vec3& scale)
    {
      scale_ = scale;
      MarkDirty(DirtyFlags::kSelf);
    }

    //--------------------------------------------------------------------------
    void TransformComponent::SetWorldScale(const glm::vec3& scale)
    {
      glm::vec3 to_set = scale;
      if (parent_ != nullptr)
      {
        to_set = parent_->GetWorldScale();

        if (glm::length(to_set) != 0.0f)
        {
          to_set = scale * (glm::vec3(1.0f, 1.0f, 1.0f) / to_set);
        }
      }

      SetScale(to_set);
    }

    //--------------------------------------------------------------------------
    const glm::vec3& TransformComponent::GetScale() const
    {
      return scale_;
    }

    //--------------------------------------------------------------------------
    glm::vec3 TransformComponent::GetWorldScale() const
    {
      glm::vec3 scale = scale_;
      TransformComponent* p = parent_;

      while (p != nullptr)
      {
        scale *= p->scale_;
        p = p->parent_;
      }

      return scale;
    }

    //--------------------------------------------------------------------------
    glm::vec3 TransformComponent::TransformPoint(const glm::vec3& point)
    {
      UpdateFromTop();
      glm::vec4 to_transform = glm::vec4(point, 1.0f);
      return local_to_world_ * to_transform;
    }

    //--------------------------------------------------------------------------
    glm::vec3 TransformComponent::InvTransformPoint(const glm::vec3& point)
    {
      UpdateFromTop();
      glm::vec4 to_transform = glm::vec4(point, 1.0f);
      return world_to_local_ * to_transform;
    }

    //--------------------------------------------------------------------------
    glm::vec3 TransformComponent::TransformDirection(const glm::vec3& direction)
    {
      UpdateFromTop();
      glm::vec4 to_transform = glm::vec4(glm::normalize(direction), 0.0f);

      return glm::normalize(local_to_world_ * to_transform);
    }

    //--------------------------------------------------------------------------
    glm::vec3 TransformComponent::InvTransformDirection(
      const glm::vec3& direction)
    {
      UpdateFromTop();
      glm::mat4 mat = glm::transpose(world_to_local_);
      glm::vec4 to_transform = glm::vec4(glm::normalize(direction), 0.0f);

      return glm::normalize(to_transform * mat);
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
    const glm::mat4x4& TransformComponent::local_to_world() const
    {
      return local_to_world_;
    }

    //--------------------------------------------------------------------------
    const glm::mat4x4& TransformComponent::world_to_local() const
    {
      return world_to_local_;
    }

    //--------------------------------------------------------------------------
    void TransformComponent::SetParentRaw(TransformComponent* parent)
    {
      parent_ = parent;
      MarkDirty(DirtyFlags::kSelf);
      entity()->scene()->OnSceneChanged();
    }

    //--------------------------------------------------------------------------
    void TransformComponent::MarkDirty(DirtyFlags flag)
    {
      is_dirty_ |= flag;

      if (flag == DirtyFlags::kSelf)
      {
        TransformComponent* p = parent_;

        while (p != nullptr)
        {
          p->MarkDirty(DirtyFlags::kChild);
          p = p->parent();
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

        if (parent_ != nullptr)
        {
          local_to_world_ = parent_->local_to_world();
        }

        local_to_world_ = glm::translate(local_to_world_, position_);
        local_to_world_ *= glm::toMat4(rotation_);
        local_to_world_ = glm::scale(local_to_world_, scale_);

        for (size_t i = 0; i < children_.size(); ++i)
        {
          children_.at(i)->MarkDirty(DirtyFlags::kSelf);
          children_.at(i)->UpdateMatrices();
        }

        updated = true;
      }

      if ((is_dirty_ & DirtyFlags::kChild) == DirtyFlags::kChild)
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
      std::sort(children_.begin(), children_.end(), 
        [](const TransformComponent* a, const TransformComponent* b)
      {
        return a->entity()->sort_index() < b->entity()->sort_index();
      });

      if (parent_ != nullptr)
      {
        return;
      }

      UpdateMatrices();
    }

    //--------------------------------------------------------------------------
    void TransformComponent::Serialize(foundation::SaveArchive& archive) const
    {
      glm::vec3 rotation = GetRotationEuler();

      archive(
        SET_ARCHIVE_PROP(position_),
        SET_ARCHIVE_PROP(rotation),
        SET_ARCHIVE_PROP(scale_));

      foundation::Vector<Entity*> children;
      children.resize(children_.size());

      for (size_t i = 0; i < children_.size(); ++i)
      {
        children.at(i) = children_.at(i)->entity();
      }

      archive(SET_ARCHIVE_PROP(children));
    }

    //--------------------------------------------------------------------------
    void TransformComponent::Deserialize(foundation::LoadArchive& archive)
    {
      glm::vec3 rotation;

      archive(
        GET_ARCHIVE_PROP(position_),
        GET_ARCHIVE_PROP(rotation),
        GET_ARCHIVE_PROP(scale_));

      foundation::Vector<Entity*> children;
      size_t n = archive.GetArraySize("children");

      children.resize(n);

      foundation::IAllocator* alloc = &foundation::Memory::default_allocator();

      Entity* ent = nullptr;

      for (size_t i = 0; i < n; ++i)
      {
        ent = foundation::Memory::Construct<Entity>(alloc, entity()->scene());
        ent->GetComponent<TransformComponent>()->SetParent(this);

        children.at(i) = ent;
      }

      archive(GET_ARCHIVE_PROP(children));

      SetRotationEuler(rotation);
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

    //--------------------------------------------------------------------------
    SPARSE_CUSTOM(TransformComponent, SetParent)
    {
      TransformComponent* self = args.GetSelf<TransformComponent>();

      if (self == nullptr)
      {
        return false;
      }

      if (args.Check("U", true) == true)
      {
        self->SetParent(nullptr);
        return true;
      }

      TransformComponent* parent = nullptr;
      if (
        args.Check("O") == false ||
        (parent = args.GetPointer<TransformComponent>(0)) == nullptr)
      {
        return false;
      }

      self->SetParent(parent);

      return true;
    }
#endif
  }
}