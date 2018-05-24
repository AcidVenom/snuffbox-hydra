#include "engine/ecs/scene.h"
#include "engine/ecs/entity.h"
#include "engine/components/transform_component.h"

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    void Scene::AddEntity(Entity* entity)
    {
      if (HasEntity(entity) >= 0)
      {
        return;
      }

      entities_.push_back(entity);
    }

    //--------------------------------------------------------------------------
    void Scene::RemoveEntity(Entity* entity)
    {
      int idx;
      if ((idx = HasEntity(entity)) >= 0)
      {
        RemoveEntityAt(idx);
      }
    }

    //--------------------------------------------------------------------------
    void Scene::RemoveEntityAt(int idx)
    {
      foundation::Logger::Assert(idx >= 0 && idx < entities_.size(),
        "Attempted to remove a non-existing entity");

      Entity* e = entities_.at(idx);
      UpdateHierarchy(e->GetComponent<TransformComponent>(), true);

      entities_.erase(entities_.begin() + idx);
    }

    //--------------------------------------------------------------------------
    void Scene::Clear()
    {
      Entity* e = nullptr;
      for (int i = static_cast<int>(entities_.size()) - 1; i >= 0; --i)
      {
        foundation::Memory::Destruct(entities_.at(i));
      }

      hierarchy_.clear();
    }

    //--------------------------------------------------------------------------
    int Scene::HasEntity(Entity* entity)
    {
      int idx = -1;
      for (int i = 0; i < static_cast<int>(entities_.size()); ++i)
      {
        if (entities_.at(i) == entity)
        {
          idx = i;
          break;
        }
      }

      return idx;
    }
    //--------------------------------------------------------------------------
    int Scene::HasTransform(TransformComponent* transform)
    {
      int idx = -1;
      for (int i = 0; i < static_cast<int>(hierarchy_.size()); ++i)
      {
        if (hierarchy_.at(i) == transform)
        {
          idx = i;
          break;
        }
      }

      return idx;
    }

    //--------------------------------------------------------------------------
    void Scene::UpdateHierarchy(TransformComponent* transform, bool removed)
    {
      removed = transform->entity()->destroyed_ == true ? true : removed;

      if (transform->parent() == nullptr && removed == false)
      {
        if (HasTransform(transform) != -1)
        {
          return;
        }

        hierarchy_.push_back(transform);
        return;
      }

      for (size_t i = 0; i < hierarchy_.size(); ++i)
      {
        if (hierarchy_.at(i) == transform)
        {
          hierarchy_.erase(hierarchy_.begin() + i);
          break;
        }
      }
    }

    //--------------------------------------------------------------------------
    void Scene::Update(float dt)
    {
      for (size_t i = 0; i < entities_.size(); ++i)
      {
        entities_.at(i)->Update(dt);
      }
    }

    //--------------------------------------------------------------------------
    const foundation::Vector<TransformComponent*>& Scene::hierarchy() const
    {
      return hierarchy_;
    }

    //--------------------------------------------------------------------------
    Scene::~Scene()
    {
      Clear();
    }
  }
}