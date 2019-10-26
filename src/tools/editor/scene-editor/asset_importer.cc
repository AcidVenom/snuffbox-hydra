#include "tools/editor/scene-editor/asset_importer.h"
#include "tools/editor/application/editor_application.h"

#include <engine/services/asset_service.h>
#include <engine/services/scene_service.h>

#include <foundation/io/file.h>
#include <foundation/serialization/save_archive.h>

#include <QMessageBox>
#include <QFileDialog>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    AssetImporter::AssetImporter(
      EditorApplication* app) :
      app_(app)
    {
      Project& project = app->project();

      build_dir_ = 
        foundation::Path(project.GetCurrentBuildPath().toLatin1().data());

      source_dir_ = 
        foundation::Path(project.GetCurrentAssetsPath().toLatin1().data());

      build_dir_ /= Project::kAssetDirectory;

      NewScene();
    }

    //--------------------------------------------------------------------------
    void AssetImporter::ImportAsset(
      compilers::AssetTypes type, 
      const QString& full_path)
    {
      foundation::Path relative_path = full_path.toLatin1().data();
      relative_path = relative_path.StripPath(build_dir_).NoExtension();

      foundation::String asset_path = relative_path.ToString();

      switch (type)
      {
      case compilers::AssetTypes::kScene:
        OpenSceneDialog(asset_path);
        break;

      default:
        break;
      }
    }

    //--------------------------------------------------------------------------
    void AssetImporter::NewScene()
    {
      app_->GetService<engine::SceneService>()->SwitchScene(nullptr);
      current_scene_ = "";

      app_->SetSceneInWindowTitle("New scene");
    }

    //--------------------------------------------------------------------------
    void AssetImporter::OpenSceneDialog(const foundation::String& scene_path)
    {
      if (current_scene_ == scene_path)
      {
        return;
      }

      QMessageBox::StandardButton choice = QMessageBox::question(
        nullptr,
        "Open scene",
        "Do you want to save the changes within the current scene?"
        "\nAll unsaved changes will be lost",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

      if (choice == QMessageBox::Cancel)
      {
        return;
      }

      if (choice == QMessageBox::Yes)
      {
        SaveCurrentScene();
      }

      OpenScene(scene_path);
    }

    //--------------------------------------------------------------------------
    void AssetImporter::OpenScene(const foundation::String& scene_path)
    {
      engine::SceneService* ss = app_->GetService<engine::SceneService>();
      
      if (ss->SwitchScene(scene_path) == true)
      {
        current_scene_ = scene_path;

        QString changed_scene = current_scene_.c_str();
        app_->SetSceneInWindowTitle(changed_scene);

        emit SceneChanged(changed_scene);
      }
    }

    //--------------------------------------------------------------------------
    void AssetImporter::SaveCurrentScene(bool force_dialog)
    {
      engine::Scene* scene = 
        app_->GetService<engine::SceneService>()->current_scene();

      foundation::Path save_path;

      bool needs_dialog = force_dialog == true || IsAssetSaved(
        compilers::AssetTypes::kScene, 
        current_scene_, 
        &save_path) == false;

      if (needs_dialog == true)
      {
        save_path = foundation::Path(QFileDialog::getSaveFileName(
          nullptr,
          "Save scene",
          source_dir_.ToString().c_str(),
          "Scene files (*.scene)").toLatin1().data());

        if (save_path.ToString().empty() == false)
        {
          current_scene_ =
            save_path.StripPath(source_dir_).NoExtension().ToString();

          app_->SetSceneInWindowTitle(current_scene_.c_str());
        }
      }

      if (save_path.ToString().size() > 0)
      {
        foundation::SaveArchive archive;
        archive(scene);

        if (archive.WriteFile(save_path) == false)
        {
          foundation::Logger::LogVerbosity<1>(
            foundation::LogChannel::kEditor,
            foundation::LogSeverity::kError,
            "Could not save scene file '{0}',\n"
            "do you have the correct permissions?",
            save_path);
        }
      }
    }

    //--------------------------------------------------------------------------
    bool AssetImporter::IsAssetSaved(
      compilers::AssetTypes type, 
      const foundation::String& relative_path,
      foundation::Path* saved_at) const
    {
      if (relative_path.size() > 0)
      {
        const char* ext =
          compilers::AssetTypesToSourceExtension(type);

        foundation::Path old_dir =
          source_dir_ / relative_path + "." + ext;

        if (foundation::File::Exists(old_dir) == true)
        {
          if (saved_at != nullptr)
          {
            *saved_at = old_dir;
          }

          return true;
        }
      }

      return false;
    }
  }
}