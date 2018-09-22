#include "tools/editor/application/editor_application.h"

#include "tools/editor/project/project_window.h"
#include "tools/editor/application/styling.h"

#include <engine/services/script_service.h>

#include <foundation/auxiliary/logger.h>

#include <QApplication>
#include <memory>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    EditorApplication::EditorApplication(
      int argc, char** argv, const Configuration& cfg)
      :
      engine::Application(argc, argv, cfg),
      qapp_(argc, argv),
      project_(nullptr)
    {
      QCoreApplication::setOrganizationName(
        QStringLiteral("Daniel Konings"));

      QCoreApplication::setOrganizationDomain(
        QStringLiteral("www.danielkonings.com"));

      QCoreApplication::setApplicationName(cfg.application_name);

      Styling::ApplyStyle(&qapp_);
    }

    //--------------------------------------------------------------------------
    EditorApplication* EditorApplication::Instance()
    {
      return static_cast<EditorApplication*>(Application::Instance());
    }

    //--------------------------------------------------------------------------
    foundation::ErrorCodes EditorApplication::Run()
    {
      std::unique_ptr<ProjectWindow> 
        project_window(new ProjectWindow(&project_));

      project_window->show();

      if (project_window->exec() == QDialog::Rejected)
      {
        return foundation::ErrorCodes::kSuccess;
      }

      ApplyConfiguration();

      foundation::Logger::LogVerbosity<2>(
        foundation::LogChannel::kEditor,
        foundation::LogSeverity::kDebug,
        "Initializing the editor"
        );

      foundation::ErrorCodes err = Initialize();

      if (err != foundation::ErrorCodes::kSuccess)
      {
        Shutdown();
        return err;
      }

      SCRIPT_CALLBACK(Start);

      Shutdown();

      return foundation::ErrorCodes::kSuccess;
    }

    //--------------------------------------------------------------------------
    QSettings& EditorApplication::GlobalSettings() const
    {
      const Configuration& cfg = config();

      static QSettings settings(
        QStringLiteral("Daniel Konings"), 
        cfg.application_name);

      return settings;
    }

    //--------------------------------------------------------------------------
    const Project& EditorApplication::project() const
    {
      return project_;
    }

    //--------------------------------------------------------------------------
    void EditorApplication::CreateRenderer()
    {
      //!< @todo Implement this function when we have a 3D view again
    }
  }
}