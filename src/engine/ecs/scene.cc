#include "engine/ecs/scene.h"
#include "engine/ecs/entity.h"
#include "engine/components/transform_component.h"

#include <foundation/serialization/archive.h>

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
      if (idx < 0 || idx >= entities_.size())
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kError,
          "Attempted to remove a non-existing entity");

        return;
      }

      entities_.erase(entities_.begin() + idx);
    }

    //--------------------------------------------------------------------------
    void Scene::Clear()
    {
      foundation::Vector<Entity*> copy = entities_;

      Entity* e = nullptr;
      for (int i = static_cast<int>(copy.size()) - 1; i >= 0; --i)
      {
        if (HasEntity(copy.at(i)) == -1)
        {
          continue;
        }

        foundation::Memory::Destruct(copy.at(i));
      }
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
    void Scene::Start()
    {
      for (size_t i = 0; i < entities_.size(); ++i)
      {
        entities_.at(i)->Start();
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
    foundation::Vector<TransformComponent*> Scene::TopLevelTransforms() const
    {
      foundation::Vector<TransformComponent*> result;

      TransformComponent* t = nullptr;

      for (size_t i = 0; i < entities_.size(); ++i)
      {
        t = entities_.at(i)->GetComponent<TransformComponent>();

        if (t->parent() == nullptr)
        {
          result.push_back(t);
        }
      }

      return result;
    }

    //--------------------------------------------------------------------------
    void Scene::ForEachEntity(EntityDelegate del)
    {
      if (del == nullptr)
      {
        return;
      }

      for (size_t i = 0; i < entities_.size(); ++i)
      {
        del(entities_.at(i));
      }
    }

    //--------------------------------------------------------------------------
    void Scene::Serialize(foundation::SaveArchive& archive) const
    {
      foundation::Vector<TransformComponent*> top = TopLevelTransforms();

      foundation::Vector<Entity*> entities;
      entities.resize(top.size());

      for (size_t i = 0; i < top.size(); ++i)
      {
        entities.at(i) = top.at(i)->entity();
      }

      archive(ARCHIVE_PROP(entities));
    }

    //--------------------------------------------------------------------------
    void Scene::Deserialize(foundation::LoadArchive& archive)
    {

    }

    //--------------------------------------------------------------------------
    Scene::~Scene()
    {
      Clear();
    }
  }
}