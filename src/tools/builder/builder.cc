#include "tools/builder/builder.h"

namespace snuffbox
{
  namespace builder
  {
    //--------------------------------------------------------------------------
    Builder::Builder() :
      is_ok_(false),
      build_directory_("")
    {

    }

    //--------------------------------------------------------------------------
    bool Builder::Initialize(const foundation::Path& source_dir)
    {
      if (foundation::Directory::Exists(source_dir) == false)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kBuilder,
          foundation::LogSeverity::kError,
          "Source directory '{0}' doesn't exist",
          source_dir);

        return false;
      }

      source_directory_ = source_dir;
      build_directory_ = source_directory_ / "build";

      if (CreateBuildDirectory() == false)
      {
        return false;
      }

      is_ok_ = true;

      SyncDirectories();

      return true;
    }

    //--------------------------------------------------------------------------
    bool Builder::is_ok() const
    {
      return is_ok_;
    }

    //--------------------------------------------------------------------------
    bool Builder::CreateBuildDirectory() const
    {
      if (foundation::Directory::Exists(build_directory_) == false)
      {
        foundation::Directory build_dir(build_directory_);

        if (build_dir.is_ok() == false)
        {
          foundation::Logger::LogVerbosity<1>(
            foundation::LogChannel::kBuilder,
            foundation::LogSeverity::kError,
            "Could not create build directory '{0}'",
            build_directory_);

          return false;
        }
      }

      return true;
    }

    //--------------------------------------------------------------------------
    void Builder::SyncDirectories()
    {
      if (is_ok_ == false)
      {
        return;
      }

      if (CreateBuildDirectory() == false)
      {
        return;
      }

      build_tree_.Open(build_directory_);
      RemoveOld(build_tree_.items());

      source_tree_.Open(source_directory_);
      SyncItems(source_tree_.items());
    }

    //--------------------------------------------------------------------------
    void Builder::SyncItems(const ItemList& source_items) const
    {
      foundation::Path current = "";
      foundation::Path current_build = "";
      foundation::Directory dir;

      for (size_t i = 0; i < source_items.size(); ++i)
      {
        const foundation::DirectoryTreeItem& item = source_items.at(i);

        if (item.is_directory() == true)
        {
          current = item.path().StripPath(source_directory_);

          if (current == "build")
          {
            continue;
          }

          current_build = build_directory_ / current;

          if (foundation::Directory::Exists(current_build) == false)
          {
            printf("Adding %s\n", current_build.ToString().c_str());
            dir.Open(current_build);

            if (dir.is_ok() == false)
            {
              continue;
            }
          }

          SyncItems(item.children());
        }
      }
    }

    //--------------------------------------------------------------------------
    void Builder::RemoveOld(const ItemList& build_items) const
    {
      foundation::Path current = "";
      foundation::Path current_source = "";
      foundation::Path item_path = "";

      for (size_t i = 0; i < build_items.size(); ++i)
      {
        const foundation::DirectoryTreeItem& item = build_items.at(i);

        if (item.is_directory() == true)
        {
          item_path = item.path();
          current = item_path.StripPath(build_directory_);

          current_source = source_directory_ / current;

          if (foundation::Directory::Exists(current_source) == false)
          {
            printf("Deleting %s\n", item_path.ToString().c_str());
            foundation::Directory::Remove(item_path);
            continue;
          }

          RemoveOld(item.children());
        }
      }
    }
  }
}