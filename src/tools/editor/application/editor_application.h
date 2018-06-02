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
      * @brief Reloads scripted entities their callbacks so they use
      *        any new versions of scripts if they are reloaded
      */
      void ReloadScriptedEntities();

      /**
      * @brief Called when the editor state is initially switched to play mode
      */
      void Play();

    private:

      builder::Builder builder_; //!< The builder service
      foundation::UniquePtr<MainWindow> window_; //!< The main window

      States state_; //!< The state of the editor
    };
  }
}
