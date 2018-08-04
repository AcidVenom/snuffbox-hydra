#pragma once

#include "tools/editor/windows/main_window.h"

#include <engine/application/application.h>
#include <tools/builder/builder.h>

#include <qapplication.h>

#ifdef SNUFF_WIN32
#ifdef CreateService
#undef CreateService
#endif
#endif

namespace snuffbox
{
  namespace editor
  {
    class EditorCamera;

    /**
    * @brief The editor application that runs the engine in editor mode
    *
    * This class derives from QApplication to be useful with all Qt widgets
    * and windows as well. By deriving from Application, we can provide
    * custom functionality while in editor mode or when in "play" mode.
    *
    * @see Application
    *
    * @author Daniel Konings
    */
    class EditorApplication : public engine::Application, public QApplication
    {

    public:

      /**
      * @brief The different states the editor application can be in
      */
      enum class States
      {
        kUninitialized,
        kEditing,
        kPlaying,
        kPaused
      };

      /**
      * @see Application::Application
      *
      * @remarks Apparently QApplication needs the argc argument
      *          to be a reference, thus it is a reference in this
      *          constructor
      */
      EditorApplication(
        int& argc,
        char** argv, 
        const engine::Application::Configuration& cfg);

      /**
      * @see Application::Run
      */
      foundation::ErrorCodes Run() override;

      /**
      * @brief Sets the project directory and starts the builder on that
      *        directory
      *
      * @param[in] path The path to the existing source directory
      *
      * @return Was the source directory a valid directory?
      */
      bool SetProjectDirectory(const foundation::Path& path);

      /**
      * @brief Sets the state of the editor application and changes all
      *        windows to reflect on that change
      *
      * @param[in] state The state to set
      */
      void SwitchState(States state);

      /**
      * @brief Opens a new scene
      *
      * @param[in] prompt Should the user be prompted to save the 
      *                   previous scene?
      *
      * @return Was the action not cancelled?
      */
      bool NewScene(bool prompt = true);

      /**
      * @brief Opens a scene by relative path in the build directory
      *
      * @param[in] path The scene to open
      */
      void OpenScene(const foundation::Path& path);

      /**
      * @brief Saves the current scene
      *
      * @param[in] dialog Do we want to force a file picker dialog?
      *
      * @remarks This opens a file dialog if the scene does not exist yet
      *
      * @return Was the scene actually saved?
      */
      bool SaveCurrentScene(bool dialog = false);

      /**
      * @return The currently loaded scene name
      */
      QString GetLoadedScene();

      /**
      * @return The state of the editor
      */
      States state() const;

    protected:

      /**
      * @see Application::CreateRenderer
      */
      void CreateRenderer() override;

      /**
      * @brief Initializes the asset handles when the build directory
      *        changes
      *
      * @param[in] build_dir The path to the build directory
      */
      void InitializeAssets(const foundation::Path& build_dir);

      /**
      * @brief Called when an asset is reloaded
      *
      * @param[in] item The build item that finished building
      */
      void OnReload(const builder::BuildItem& item);

      /**
      * @brief Called when an asset is removed from the build directory
      *
      * @param[in] item The build item that was removed
      */
      void OnRemoved(const builder::BuildItem& item);

      /**
      * @brief Creates the editor camera for a particular scene as an internal
      *        entity
      *
      * @param[in] scene The scene to create the camera for
      */
      void NewEditorCamera(engine::Scene* scene);

      /**
      * @brief Reloads all script-related content when a new script was added
      *        or reloaded
      *
      * Entities have their callbacks assign anew, whereas the asset system
      * recompiles all current scripts and checks them for errors.
      *
      * @remarks If there are any errors, the playback button will be disabled
      *          until they have been fixed
      */
      void ReloadScripts();

      /**
      * @see Scene::RenderEntities
      */
      void RenderOnly(float dt);

      /**
      * @brief Checks if we had errors during playmode and if so; disable
      *        the play button until those errors are fixed
      */
      void OnStartEditing();

      /**
      * @brief Called when the editor state is initially switched to play mode
      */
      void Play();

      /**
      * @brief Shows a scene save dialog and asks the user whether they
      *        want to cancel as well
      *
      * @return Should we continue?
      */
      bool ShowSceneSaveDialog();

      /**
      * @brief Serializes the current scene, called before playing
      */
      void SerializeCurrentScene();

      /**
      * @brief Deserializes the current scene, called after entering edit mode
      */
      void DeserializeCurrentScene();

      /**
      * @brief Called when a scene has been changed
      *
      * A flag is set to reload the hierarchy view within the editor after
      * finishing the current frame.
      *
      * @param[in] scene The scene that has been changed
      */
      void OnSceneChanged(engine::Scene* scene);

      /**
      * @brief Checks if the scene changed this frame and if it did, updates
      *        the hierarchy in the main window
      */
      void CheckSceneChanged();

      /**
      * @brief Destroys the editor camera
      */
      void OnShutdown() override;

    private:

      builder::Builder builder_; //!< The builder service
      foundation::UniquePtr<MainWindow> window_; //!< The main window
      EditorCamera* camera_; //!< The editor camera

      QString project_dir_; //!< The current project directory
      QString assets_dir_; //!< The current assets directory

      foundation::String loaded_scene_; //!< The currently loaded scene
      foundation::String serialized_scene_; //!< The current scene, serialized

      bool scene_changed_; //!< Has the current scene changed this frame?

      States state_; //!< The state of the editor
      bool has_error_; //!< Are there any errors?
    };
  }
}
