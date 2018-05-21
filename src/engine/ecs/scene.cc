#include "engine/ecs/scene.h"
#include "engine/ecs/entity.h"

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
        entities_.erase(entities_.begin() + idx);
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
    void Scene::Update(float dt)
    {
      for (size_t i = 0; i < entities_.size(); ++i)
      {
        entities_.at(i)->Update(dt);
      }
    }

    //--------------------------------------------------------------------------
    void Scene::Clear()
    {
      Entity* e = nullptr;
      for (size_t i = 0; i < entities_.size(); ++i)
      {
        e = entities_.at(i);

        if (e->is_from_script() == true)
        {
          continue;
        }

        foundation::Memory::Destruct(e);
      }

      entities_.clear();
    }
  }
}