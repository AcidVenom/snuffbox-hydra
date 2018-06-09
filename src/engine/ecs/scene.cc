#include "engine/ecs/scene.h"
#include "engine/ecs/entity.h"
#include "engine/components/transform_component.h"

#include <foundation/serialization/save_archive.h>
#include <foundation/serialization/load_archive.h>

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    Scene::Scene() :
      current_id_(0)
    {

    }

    //--------------------------------------------------------------------------
    void Scene::AddEntity(Entity* entity)
    {
      if (HasEntity(entity) >= 0)
      {
        return;
      }

      entity->set_id(current_id_);
      ++current_id_;

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
    size_t Scene::GetNextAvailableID()
    {
      if (entities_.size() == 0)
      {
        return 0;
      }

      size_t last = 0;

      ForEachEntity([&last](Entity* e)
      {
        size_t id = e->id();
        if (id > last)
        {
          last = id;
        }

        return true;
      });

      return last + 1;
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
    void Scene::ForEachEntity(const EntityDelegate& del)
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
    Entity* Scene::FindEntityByID(size_t id)
    {
      Entity* found = nullptr;

      ForEachEntity([&found, id](Entity* e)
      {
        if (e->id() == id)
        {
          found = e;
          return false;
        }

        return true;
      });

      return found;
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

      archive(SET_ARCHIVE_PROP(entities));
    }

    //--------------------------------------------------------------------------
    void Scene::Deserialize(foundation::LoadArchive& archive)
    {
      size_t n = archive.GetArraySize("entities");

      foundation::Vector<Entity*> entities;
      foundation::IAllocator* alloc = &foundation::Memory::default_allocator();

      for (size_t i = 0; i < n; ++i)
      {
        foundation::Memory::Construct<Entity>(alloc, this);
      }

      foundation::Vector<TransformComponent*> top = TopLevelTransforms();
      entities.resize(top.size());

      for (size_t i = 0; i < top.size(); ++i)
      {
        entities.at(i) = top.at(i)->entity();
      }

      archive(GET_ARCHIVE_PROP(entities));

      current_id_ = GetNextAvailableID();
    }

    //--------------------------------------------------------------------------
    Scene::~Scene()
    {
      Clear();
    }
  }
}