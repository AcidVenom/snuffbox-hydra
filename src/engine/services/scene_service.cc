#include "engine/services/scene_service.h"

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    SceneService::SceneService() :
      ServiceBase<SceneService>("SceneService"),
      current_scene_(nullptr)
    {
      current_scene_ = &default_scene_;
    }

    //--------------------------------------------------------------------------
    foundation::ErrorCodes SceneService::OnInitialize(Application& app)
    {
      return foundation::ErrorCodes::kSuccess;
    }

    //--------------------------------------------------------------------------
    void SceneService::OnUpdate(Application& app, float dt)
    {
      if (current_scene_ == nullptr)
      {
        return;
      }

      current_scene_->Update(dt);
    }

    //--------------------------------------------------------------------------
    void SceneService::OnShutdown(Application& app)
    {

    }

    //--------------------------------------------------------------------------
    void SceneService::Start()
    {
      if (current_scene_ == nullptr)
      {
        return;
      }

      current_scene_->Start();
    }

    //--------------------------------------------------------------------------
    void SceneService::SwitchScene(Scene* scene)
    {
      if (scene == nullptr)
      {
        current_scene_ = &default_scene_;
        return;
      }

      current_scene_ = scene;
    }

    //--------------------------------------------------------------------------
    Scene* SceneService::current_scene() const
    {
      return current_scene_;
    }
  }
}