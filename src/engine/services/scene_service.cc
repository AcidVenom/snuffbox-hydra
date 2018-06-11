#include "engine/services/scene_service.h"

#include "engine/application/application.h"

#include "engine/services/asset_service.h"
#include "engine/assets/scene_asset.h"

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    SceneService::SceneService() :
      ServiceBase<SceneService>("SceneService"),
      current_scene_(nullptr),
      loaded_scene_(nullptr),
      on_scene_changed_(nullptr)
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
      if (loaded_scene_ != nullptr && loaded_scene_->scene() != scene)
      {
        loaded_scene_->Unload();
        loaded_scene_ = nullptr;
      }

      if (scene == nullptr)
      {
        default_scene_ = Scene();
        current_scene_ = &default_scene_;
        return;
      }

      current_scene_ = scene;
    }

    //--------------------------------------------------------------------------
    void SceneService::OnSceneChanged(Scene* scene)
    {
      if (on_scene_changed_ != nullptr && scene == current_scene_)
      {
        on_scene_changed_(scene);
      }
    }

    //--------------------------------------------------------------------------
    void SceneService::SwitchScene(const foundation::String& path)
    {
      Application* app = Application::Instance();
      AssetService* as = app->GetService<AssetService>();

      if (loaded_scene_ != nullptr)
      {
        loaded_scene_->Unload();
      }

      loaded_scene_ = static_cast<engine::SceneAsset*>(
          as->Get(compilers::AssetTypes::kScene, path));

      if (loaded_scene_ == nullptr || loaded_scene_->Load() == false)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kEngine,
          foundation::LogSeverity::kError,
          "Could not open scene '{0}', it doesn't exist or failed to load",
          path);

        loaded_scene_ = nullptr;
        SwitchScene(nullptr);

        return;
      }

      SwitchScene(loaded_scene_->scene());
    }

    //--------------------------------------------------------------------------
    void SceneService::set_on_scene_changed(const SceneChanged& cb)
    {
      on_scene_changed_ = cb;
    }

    //--------------------------------------------------------------------------
    Scene* SceneService::current_scene() const
    {
      return current_scene_;
    }
  }
}