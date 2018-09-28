#pragma once

#include <QObject>

namespace snuffbox
{
  namespace editor
  {
    /**
    * @brief Used to store session data of the current project opened in the
    *        editor
    *
    * @author Daniel Konings
    */
    class Project : public QObject
    {
      
      Q_OBJECT;

    public:

      /**
      * @brief Used to contain project settings data that reflects the
      *        JSON in the project directory
      *
      * @author Daniel Konings
      */
      struct Settings
      {
        bool is_project; //!< Is the project a valid project?
        QString name; //!< The name of the project
      };

      /**
      * @brief Construct through a QObject parent
      *
      * @param[in] parent The parent that contains this QObject
      */
      Project(QObject* parent = nullptr);

      /**
      * @return The current project path
      */
      const QString& project_path() const;

      /**
      * @return The current project settings
      */
      const Settings& project_settings() const;

      /**
      * @brief Sets the project path for this project and automatically
      *        retrieves the settings of the project
      *
      * @param[in] path The path to the project directory to set
      *
      * @return Was a valid project path set?
      */
      bool SetProjectPath(const QString& path);

      /**
      * @brief Retrieves the project settings of a given project under
      *        a specified path
      *
      * @param[in] path The path to the project
      */
      static Settings GetProjectSettings(const QString& path);

      /**
      * @brief Creates a valid project in a specified directory
      *
      * @param[in] path The path to create the project in
      * @param[in] settings The settings to create the project with
      *
      * @return Was project creation a success?
      */
      static bool CreateProject(
        const QString& path, 
        const Project::Settings& settings);

      /**
      * @brief Checks if a project exists by its settings file
      *
      * @param[in] path The path to the directory to check
      *
      * @return Does the project exist?
      */
      static bool Exists(const QString& path);

    private:

      QString project_path_; //!< The current project path
      Settings project_settings_; //!< The current project settings

    public:

      /**
      * @brief The file to save project settings in
      */
      static const QString kProjectFile;

    private:

      /**
      * @brief The project name key in the project settings JSON file
      */
      static const QString kProjectNameKey_;
    };
  }
}