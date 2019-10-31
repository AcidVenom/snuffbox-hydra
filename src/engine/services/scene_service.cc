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
    void SceneService::UnloadAssets()
    {
      AssetService* as = Application::Instance()->GetService<AssetService>();

      compilers::AssetTypes type;
      for (int i = 0; i < static_cast<int>(compilers::AssetTypes::kCount); ++i)
      {
        type = static_cast<compilers::AssetTypes>(i);

        if (
          type == compilers::AssetTypes::kScript || 
          type == compilers::AssetTypes::kScene)
        {
          continue;
        }

        as->UnloadAll(type);
      }
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
    bool SceneService::SwitchScene(Scene* scene)
    {
      if (current_scene_ == &default_scene_)
      {
        current_scene_->Clear();
      }

      UnloadAssets();

      if (loaded_scene_ != nullptr && loaded_scene_->scene() != scene)
      {
        loaded_scene_->Unload();
        loaded_scene_ = nullptr;
      }

      if (scene == nullptr)
      {
        default_scene_ = Scene();
        current_scene_ = &default_scene_;
        return true;
      }

      current_scene_ = scene;
      current_scene_->OnSceneChanged();

      return true;
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
    bool SceneService::SwitchScene(const foundation::String& path)
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

        return false;
      }

      return SwitchScene(loaded_scene_->scene());
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