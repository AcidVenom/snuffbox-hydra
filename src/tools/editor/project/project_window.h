#pragma once

#include <QDialog>

namespace snuffbox
{
  namespace editor
  {
    class Project;

    /**
    * @brief The project window dialog that opens before running the editor
    *        instance, setting the intial project
    *
    * @author Daniel Konings
    */
    class ProjectWindow : public QDialog
    {

      Q_OBJECT;

    public:

      /**
      * @brief Create a project window with a reference to the project
      *        settings
      *
      * @param[in] project The current project settings
      * @param[in] parent The Qt parent widget this window lives in
      */
      ProjectWindow(Project* project, QWidget* parent = nullptr);

    protected slots:

      /**
      * @brief Create a 'new project' dialog
      */
      void NewProject();

      /**
      * @brief Create an 'open project' dialog
      */
      void OpenProject();

      /**
      * @brief General folder dialog that shows a file picker that only accepts
      *        directories.
      *
      * @remarks This sets the recent folder settings in the global QSettings
      *
      * @param[out] valid Was a valid directory chosen?
      *
      * @return The chosen directory
      */
      QString ShowFolderDialog(bool* valid);

    private:

      Project* project_; //!< A reference to the current project settings
      QString recent_folder_; //!< The last folder that was opened in a dialog

      static const int kProjectWindowWidth_; //!< The width of the dialog
      static const int kProjectWindowHeight_; //!< The height of the dialog

      /**
      * @brief The key for the recent folder setting, in the global QSettings
      */
      static const QString kSettingsRecentFolder_;
    };
  }
}