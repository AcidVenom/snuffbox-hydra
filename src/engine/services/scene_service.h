#pragma once

#include "engine/services/service.h"
#include "engine/ecs/scene.h"

namespace snuffbox
{
  namespace engine
  {
    class Entity;
    class SceneAsset;

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
      * @brief Used to call a function when the scene has changed
      */
      using SceneChanged = foundation::Function<void(Scene*)>;

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
      * @brief Unloads all assets, but scripts and scenes
      */
      void UnloadAssets();

    public:

      /**
      * @brief Calls Entity::Start on every entity in the current scene
      */
      void Start();

      /**
      * @brief Switches the current scene
      *
      * @remarks This calls SceneService::UnloadAssets to make sure all assets
      *          of the previous scene are unloaded
      *
      * @param[in] scene The scene to switch to
      *
      * If the scene parameter is nullptr, the scene service will
      * switch back to the default scene. The default scene is then reset.
      *
      * @remarks This unloads the previously loaded scene
      */
      void SwitchScene(Scene* scene);

      /**
      * @brief Switches the current scene from a scene asset
      *
      * @param[in] path The path to the scene to switch to
      *
      * @see SceneService::SwitchScene
      */
      void SwitchScene(const foundation::String& path);

      /**
      * @brief Called from a Scene when the respective scene has changed
      *
      * @param[in] scene The scene that was changed
      *
      * @remarks This calls the on_scene_changed_ callback, if the current
      *          scene is the scene that changed
      */
      void OnSceneChanged(Scene* scene);

      /**
      * @brief Sets the callback for when the scene has changed
      *
      * @param[in] cb The callback to set
      */
      void set_on_scene_changed(const SceneChanged& cb);

      /**
      * @return The current scene that is being updated
      */
      Scene* current_scene() const;

    private:

      Scene default_scene_; //!< The default scene
      Scene* current_scene_; //!< The current scene that is being updated
      SceneAsset* loaded_scene_; //!< The currently loaded scene
      SceneChanged on_scene_changed_; //!< The scene changed callback
    };
  }
}