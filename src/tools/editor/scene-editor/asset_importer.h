#pragma once

#include <tools/compilers/definitions/asset_types.h>

#include <foundation/io/path.h>

#include <QString>
#include <QObject>

namespace snuffbox
{
  namespace editor
  {
    class EditorApplication;

    /**
    * @brief Used to import assets into the current scene, or open scenes
    *
    * @author Daniel Konings
    */
    class AssetImporter : public QObject
    {

      Q_OBJECT;

    public:

      /**
      * @brief Construct by specifying the current build directory
      *
      * @param[in] app The current application
      */
      AssetImporter(EditorApplication* app);

      /**
      * @brief Imports an asset by type and its full path
      *
      * @param[in] type The type of the asset
      * @param[in] full_path The full path (in the build directory) to the asset
      */
      void ImportAsset(compilers::AssetTypes type, const QString& full_path);

      /**
      * @brief Switches to a new scene
      *
      * @remarks This doesn't do any checking on the current scene as to whether
      *          it should be saved or not
      */
      void NewScene();

      /**
      * @brief Opens a scene and sets it as the current scene, with a prompt
      *        if the user is sure to open a new scene
      *
      * @param[in] scene_path The path to the new scene
      */
      void OpenSceneDialog(const foundation::String& scene_path);

      /**
      * @brief Opens a scene from a specified path, without a prompt to save
      *        the current scene
      *
      * @param[in] scene_path The path to the new scene
      */
      void OpenScene(const foundation::String& scene_path);

      /**
      * @brief Saves the current scene, with a file picker dialog of the scene
      *        was not already saved to disk
      *
      * @param[in] force_dialog Should a dialog be force as to where to save
      *                         the scene?
      */
      void SaveCurrentScene(bool force_dialog = false);

      /**
      * @brief Checks whether or not an asset is already saved to the current
      *        source directory
      *
      * @param[in] type The type of the asset
      * @param[in] relative_path The relative path to the asset
      * @param[out] saved_at The location where the file is saved, if it does
      *                      exist
      */
      bool IsAssetSaved(
        compilers::AssetTypes type, 
        const foundation::String& relative_path,
        foundation::Path* saved_at) const;

    signals:

      /**
      * @brief Emitted when the scene was changed
      */
      void SceneChanged(const QString& new_scene);

    private:

      EditorApplication* app_; //!< The current application

      foundation::Path build_dir_; //!< The current build directory
      foundation::Path source_dir_; //!< The current source directory
      
      foundation::String current_scene_; //!< The name of the current scene
    };
  }
}