#include "engine/ecs/scene.h"
#include "engine/ecs/entity.h"
#include "engine/components/transform_component.h"
#include "engine/components/mesh_renderer_component.h"
#include "engine/components/camera_component.h"
#include "engine/application/application.h"
#include "engine/services/scene_service.h"

#include <foundation/serialization/save_archive.h>
#include <foundation/serialization/load_archive.h>

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    Scene::Scene() :
      current_id_(0),
      deleted_(false)
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

      Entity* e = entities_.at(idx);

      if (e->is_from_script() == false)
      {
        foundation::Memory::Destruct(e);
      }

      entities_.at(idx) = nullptr;
      deleted_ = true;
    }

    //--------------------------------------------------------------------------
    int Scene::HasEntity(Entity* entity)
    {
      int idx = -1;
      if (entity == nullptr)
      {
        return idx;
      }

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
    void Scene::RemoveNullEntities()
    {
      foundation::Vector<Entity*> result;
      size_t old_size = entities_.size();
      result.resize(old_size);

      size_t current = 0;
      Entity* entity = nullptr;

      for (size_t i = 0; i < old_size; ++i)
      {
        entity = entities_.at(i);

        if (entity != nullptr)
        {
          result.at(current) = entity;
          ++current;
        }
      }

      result.erase(result.begin() + current, result.end());
      entities_ = eastl::move(result);
    }

    //--------------------------------------------------------------------------
    void Scene::Start()
    {
      ForEachEntity([](Entity* e)
      {
        e->Start();
        return true;
      });
    }

    //--------------------------------------------------------------------------
    void Scene::Update(float dt)
    {
      ForEachEntity([dt](Entity* e)
      {
        e->Update(dt);
        return true;
      });
      
      if (deleted_ == true)
      {
        RemoveNullEntities();
      }
    }

    //--------------------------------------------------------------------------
    void Scene::RenderEntities(float dt)
    {
      ForEachEntity([dt](Entity* e)
      {
        if (e->active() == false)
        {
          return true;
        }

        if (e->HasComponent<MeshRendererComponent>() == true)
        {
          e->GetComponent<MeshRendererComponent>()->Update(dt);
        }
        else if (
          e->HasComponent<CameraComponent>() == true &&
          e->is_internal() == true)
        {
          e->Update(dt);
        }

        e->GetComponent<TransformComponent>()->Update(dt);

        return true;
      });
    }

    //--------------------------------------------------------------------------
    void Scene::Clear()
    {
      Entity* e = nullptr;
      for (int i = static_cast<int>(entities_.size()) - 1; i >= 0; --i)
      {
        e = entities_.at(i);
        if (HasEntity(e) == -1 || e->is_internal() == true)
        {
          continue;
        }

        e->Destroy();
        entities_.at(i) = nullptr;
      }

      RemoveNullEntities();
    }

    //--------------------------------------------------------------------------
    foundation::Vector<TransformComponent*> Scene::TopLevelTransforms() const
    {
      foundation::Vector<TransformComponent*> result;

      Entity* e = nullptr;
      TransformComponent* t = nullptr;

      for (size_t i = 0; i < entities_.size(); ++i)
      {
        e = entities_.at(i);

        if (e != nullptr && e->is_internal() == false)
        {
          t = e->GetComponent<TransformComponent>();

          if (t->parent() == nullptr)
          {
            result.push_back(t);
          }
        }
      };

      return result;
    }

    //--------------------------------------------------------------------------
    void Scene::ForEachEntity(const EntityDelegate& del)
    {
      if (del == nullptr)
      {
        return;
      }

      Entity* current = nullptr;
      for (size_t i = 0; i < entities_.size(); ++i)
      {
        current = entities_.at(i);

        if (current != nullptr)
        {
          del(entities_.at(i));
        }
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
    void Scene::OnSceneChanged()
    {
      Application::Instance()->GetService<SceneService>()->OnSceneChanged(this);
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
      Clear();

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