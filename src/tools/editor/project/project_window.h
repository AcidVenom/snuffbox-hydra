#pragma once

#include <QDialog>
#include <QList>

class QLabel;
class QListWidget;

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
      void OnNewProject();

      /**
      * @brief Create an 'open project' dialog
      */
      void OnOpenProject();

    protected:

      /**
      * @brief Load recently opened projects and show them in the window
      */
      void LoadRecentlyOpened();

      /**
      * @brief Refreshes the recently opened list widget
      */
      void RefreshRecentlyOpened();

      /**
      * @brief Adds a project to the list of recently opened proejcts
      *
      * @param[in] path The path to the recent project to add
      */
      void AddRecentProject(const QString& path);

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

      /**
      * @brief Saves the recently opened projects
      */
      void SaveRecentProjects();

    public:

      /**
      * @brief Save all on-close settings in the destructor
      */
      ~ProjectWindow();

    private:

      Project* project_; //!< A reference to the current project settings
      QString recent_folder_; //!< The last folder that was opened in a dialog
      QListWidget* recent_projects_; //!< The list of recent projects

      /**
      * @brief The list of all loaded recent projects
      */
      QList<QString> recent_project_paths_;

      static const int kProjectWindowWidth_; //!< The width of the dialog
      static const int kProjectWindowHeight_; //!< The height of the dialog

      /**
      * @brief The key for the recent folder setting, in the global QSettings
      */
      static const QString kSettingsRecentFolder_;

      /**
      * @brief The key for the recently opened projects, in the global QSettings
      */
      static const QString kSettingsRecentlyOpened_;

      /**
      * @brief The file to save project settings in
      */
      static const QString kProjectFile_;

      /**
      * @brief The maximum number of recently opened projects to show
      */
      static const int kMaxRecentlyOpened_;
    };

    /**
    * @brief This dialog is shown when the user creates a new project
    *
    * When no name is specified for the project, the user is not allowed
    * to continue
    *
    * @author Daniel Konings
    */
    class NewProjectDialog : public QDialog
    {

      Q_OBJECT;

    public:

      /**
      * @brief Construct through a parent widget
      *
      * @param[in] parent The parent widget that owns this dialog
      */
      NewProjectDialog(QWidget* parent = nullptr);

      /**
      * @return The current project name
      */
      const QString& project_name() const;

    protected slots:

      /**
      * @brief Called when the input box's text was changed. This validates
      *        the text as a valid project name, it also sets the
      *        NewProjectDialog::project_name_ member
      *
      * @param[in] name The name of the project
      */
      void OnNameChanged(const QString& name);

    private:

      QString project_name_; //!< The current project name

      QPushButton* button_create_; //!< The create button
      QLabel* error_; //!< The current error message

      static const int kNewProjectDialogWidth_; //!< The width of the dialog
      static const int kNewProjectDialogHeight_; //!< The height of the dialog
    };
  }
}