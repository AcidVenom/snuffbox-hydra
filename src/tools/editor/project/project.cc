#include "tools/editor/project/project.h"

#include <foundation/io/directory.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    const QString Project::kProjectFile = "project_settings.json";
    const QString Project::kProjectNameKey_ = "project_name";

    const char* Project::kAssetDirectory = "assets";
    const char* Project::kBuildDirectory = ".build";

    //--------------------------------------------------------------------------
    Project::Project(QObject* parent) :
      QObject(parent)
    {

    }

    //--------------------------------------------------------------------------
    QString Project::GetCurrentAssetsPath() const
    {
      return project_path() + '/' + Project::kAssetDirectory;
    }

    //--------------------------------------------------------------------------
    QString Project::GetCurrentBuildPath() const
    {
      return project_path() + '/' + Project::kBuildDirectory;
    }

    //--------------------------------------------------------------------------
    const QString& Project::project_path() const
    {
      return project_path_;
    }

    //--------------------------------------------------------------------------
    const Project::Settings& Project::project_settings() const
    {
      return project_settings_;
    }

    //--------------------------------------------------------------------------
    bool Project::SetProjectPath(const QString& path)
    {
      project_path_ = "";
      project_settings_ = GetProjectSettings(path);
      
      if (project_settings_.is_project == true)
      {
        project_path_ = path;
        return true;
      }

      return false;
    }

    //--------------------------------------------------------------------------
    Project::Settings Project::GetProjectSettings(const QString& path)
    {
      Project::Settings settings;
      settings.is_project = false;

      QString settings_path = path + "/" + kProjectFile;

      if (QFile::exists(path) == false)
      {
        return settings;
      }

      QFile file(settings_path);

      if (file.open(QFile::ReadOnly) == true)
      {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        
        if (doc.isEmpty() == false && doc.isObject() == true)
        {
          QJsonObject object = doc.object();
          settings.is_project = true;
          settings.name = object[kProjectNameKey_].toString("undefined");
        }

        file.close();
      }

      return settings;
    }

    //--------------------------------------------------------------------------
    bool Project::CreateProject(
      const QString& path, 
      const Project::Settings& settings)
    {
      foundation::Path build_dir = path.toLatin1().data();
      foundation::Path source_dir = build_dir;

      build_dir = build_dir / kBuildDirectory / kAssetDirectory;
      source_dir = source_dir / kAssetDirectory;

      foundation::Directory create_dir;
      create_dir.Open(build_dir);
      create_dir.Open(source_dir);

      QJsonDocument doc;
      QJsonObject root;
      root.insert(kProjectNameKey_, settings.name);

      doc.setObject(root);

      QFile file(path + "/" + kProjectFile);

      if (file.open(QFile::WriteOnly) == true)
      {
        file.write(doc.toJson());
        file.close();
        return true;
      }

      return false;
    }

    //--------------------------------------------------------------------------
    bool Project::Exists(const QString& path)
    {
      return QFile::exists(path + "/" + kProjectFile);
    }
  }
}