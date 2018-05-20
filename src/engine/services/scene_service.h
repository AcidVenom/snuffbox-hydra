#pragma once

#include "engine/services/service.h"
#include "engine/ecs/scene.h"

namespace snuffbox
{
  namespace engine
  {
    class Entity;

    /**
    * @brief The scene service to manage all scenes that get created
    *
    * @author Daniel Konings
    */
    class SceneService : public ServiceBase<SceneService>
    {

      friend Entity;

    public:

      /**
      * @see IService::IService
      */
      SceneService();

    protected:

      /**
      * @see IService::OnInitialize
      */
      foundation::ErrorCodes OnInitialize(Application& app) override;

      /**
      * @see IService::OnUpdate
      */
      void OnUpdate(Application& app, float dt) override;

      /**
      * @see IService::OnShutdown
      */
      void OnShutdown(Application& app) override;

      /**
      * @return The current scene that is being updated
      */
      Scene* current_scene() const;

    private:

      Scene default_scene_; //!< The default scene
      Scene* current_scene_; //!< The current scene that is being updated
    };
  }
}