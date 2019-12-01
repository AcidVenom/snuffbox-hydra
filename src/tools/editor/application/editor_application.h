#pragma once

#include "tools/editor/project/project.h"
#include "tools/editor/windows/main_window.h"
#include "tools/editor/scene-editor/asset_importer.h"

#include <engine/application/application.h>
#include <foundation/auxiliary/timer.h>

#include <QApplication>
#include <QSettings>

namespace snuffbox
{
  namespace builder
  {
    class Builder;
    struct BuildItem;
  }

  namespace editor
  {
    class MainWindow;

    /**
    * @brief The application instance for the editor version of the engine
    *
    * @author Daniel Konings
    */
    class EditorApplication : public engine::Application
    {

    public:

      /**
      * @brief The different states the editor can globally be in
      */
      enum class EditorStates
      {
        kEditing,
        kPlaying,
        kPaused,
        kFrame
      };

      /**
      * @see Application::Application
      *
      * @param[in] qapp The Qt application to update each frame
      */
      EditorApplication(int argc, char** argv, const Configuration& cfg);

      /**
      * @see Application::Instance
      */
      static EditorApplication* Instance();

      /**
      * @see Application::Run
      *
      * Makes sure the engine is ran from within the Qt interfaces, instead
      * of creating a separate window for the renderer instance
      */
      foundation::ErrorCodes Run() override;

      /**
      * @brief Attempts to open a new project through the dialog
      *
      * @remarks If accepted, this will reset the entire application
      */
      void TryOpenProject();

      /**
      * @return The global settings of the application
      */
      QSettings& GlobalSettings() const;

      /**
      * @brief Sets the current scene within the window title
      *
      * @param[in] scene_name The name of the scene
      */
      void SetSceneInWindowTitle(const QString& scene_name);

      /**
      * @brief Switches the state of the editor application to a new one
      *
      * @param[in] state The state to switch to
      */
      void SwitchState(EditorStates state);

      /**
      * @return The main window of the application
      */
      MainWindow* main_window() const;

      /**
      * @return The current asset importer
      */
      AssetImporter* asset_importer() const;

      /**
      * @return The current project
      */
      Project& project();

      /**
      * @return The current state of the editor application
      */
      EditorStates state() const;

    protected:

      /**
      * @brief Sets the default values after a restart
      */
      void OnRestart();

      /**
      * @brief Shows a project window, to pick a new or existing project from
      *
      * @return Should we continue execution, or did the user exit?
      */
      bool ShowProjectWindow();

      /**
      * @brief Creates the main window, that contains the graphics view
      *        and all other editor panes
      *
      * @param[out] An error code, or foundation::ErrorCodes::kSuccess if no
      *             errors occurred
      *
      * @return The created main window, or nullptr if initialization failed
      */
      std::unique_ptr<MainWindow> CreateMainWindow(foundation::ErrorCodes* err);

      /**
      * @brief Called when the builder has finished building one of
      *        its current items
      *
      * @param[in] builder The builder that invoked this call
      * @param[in] item The item that was finished
      */
      void OnBuilderFinished(
        const builder::Builder* builder, 
        const builder::BuildItem& item);

      /**
      * @brief Called when the builder has removed a file because it was no
      *        longer in the source directory, or a directory changed
      *
      * @param[in] builder The builder that invoked this call
      * @param[in] item The item that was removed, or the directory that changed
      */
      void OnBuilderChanged(
        const builder::Builder* builder, 
        const builder::BuildItem& item);

      /**
      * @brief Mark that the current build has changed
      */
      void BuildChanged();

      /**
      * @brief Checks if the build directory has changed and if it did,
      *        refresh the current asset listing
      *
      * @remarks We do not constantly want to update this, so we make sure to
      *          wait a while inbetween changes in the case of mass-deletes
      */
      void CheckForBuildChanges();

      /**
      * @brief Called when we initially start editing again
      */
      void OnStartEditing();

      /**
      * @brief Called when we initially start playing
      *
      * @param[in] old The old state the editor was in
      *
      * @remarks This serializes the scene, to deserialize it when we
      *          go out of play mode
      */
      void OnStartPlaying(EditorStates old);

      /**
      * @brief Serializes the current scene, for deserialization later
      */
      void SerializeCurrentScene();

      /**
      * @brief Deserializes the current scene
      */
      void DeserializeCurrentScene();

      /**
      * @brief Reloads all scripts and sets whether we have an error or not
      */
      void ReloadScripts();

    private:

      QApplication qapp_; //!< The Qt application
      Project project_; //!< The current project

      /**
      * @brief The grace period timer for build changes
      */
      foundation::Timer build_change_timer_;
      bool build_dir_changed_; //!< Has the build directory changed?
      std::mutex build_mutex_; //!< The mutex for any building callbacks

      std::unique_ptr<MainWindow> main_window_; //!< The main window
      std::unique_ptr<AssetImporter> asset_importer_; //!< The asset importer

      bool project_changed_; //!< Was the project changed and should we restart?

      EditorStates state_; //!< The current state of the editor application
      foundation::String serialized_scene_; //!< The currently serialized scene

      bool has_script_error_; //!< Do we currently have a scripting error?

      /**
      * @brief The minimum amount of time, in milliseconds, to wait inbetween
      *        build changes during removal
      */
      static const int kMinBuildChangeWait_;
      static const QString kTitleFormat_; //!< The format of the window title
    };
  }
}