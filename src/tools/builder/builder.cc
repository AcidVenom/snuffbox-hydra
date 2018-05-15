#include "tools/builder/builder.h"

#include <foundation/io/file.h>
#include <foundation/auxiliary/string_utils.h>

namespace snuffbox
{
  namespace builder
  {
    //--------------------------------------------------------------------------
    const char* Builder::kBuildFolder_ = ".build";
    const char* Builder::kStampExtension_ = "time";

    //--------------------------------------------------------------------------
    Builder::Builder() :
      is_ok_(false),
      build_directory_("")
    {

    }

    //--------------------------------------------------------------------------
    bool Builder::Initialize(const foundation::Path& source_dir)
    {
      listener_.Stop();

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
      build_directory_ = source_directory_ / kBuildFolder_;

      if (CreateBuildDirectory() == false)
      {
        return false;
      }

      is_ok_ = true;

      SyncDirectories();
      FindFileChanges(source_tree_.items());

      listener_.SetCallbacks(
        [&](const foundation::Path&)
        { 
          SyncDirectories(); 
        },
        [&](const foundation::Path&)
        {
          FindFileChanges(source_tree_.items());
        });

      listener_.Listen(source_directory_);

      return true;
    }

    //--------------------------------------------------------------------------
    void Builder::IdleNotification()
    {
      scheduler_.IdleNotification(this);
    }

    //--------------------------------------------------------------------------
    void Builder::Write(
      const BuildItem& item, 
      const uint8_t* buffer, 
      size_t size)
    {
      foundation::Path build = 
        build_directory_ /
        item.relative.NoExtension() + 
        "." + 
        AssetTypesToExtension(item.type);

      foundation::File fout(build, foundation::FileFlags::kWrite);

      if (fout.is_ok() == false)
      {
        foundation::Logger::LogVerbosity<1>(
          foundation::LogChannel::kBuilder,
          foundation::LogSeverity::kError,
          "Cannot save file '{0}'",
          build);

        return;
      }

      fout.Write(buffer, size);
    }

    //--------------------------------------------------------------------------
    void Builder::Shutdown()
    {
      if (is_ok_ == false)
      {
        return;
      }

      listener_.Stop();
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
      bool exists = foundation::Directory::Exists(build_directory_);

      if (is_ok_ == false || CreateBuildDirectory() == false)
      {
        return;
      }

      build_tree_.Open(build_directory_);
      RemoveOld(build_tree_.items());
      build_tree_.Open(build_directory_);

      source_tree_.Open(source_directory_);
      SyncItems(source_tree_.items());

      if (exists == false)
      {
        FindFileChanges(source_tree_.items());
      }
    }

    //--------------------------------------------------------------------------
    void Builder::FindFileChanges(const ItemList& items)
    {
      if (is_ok_ == false || CreateBuildDirectory() == false)
      {
        return;
      }

      foundation::Path current;

      for (size_t i = 0; i < items.size(); ++i)
      {
        const foundation::DirectoryTreeItem& item = items.at(i);
        const foundation::Path& item_path = item.path();

        current = item_path.StripPath(source_directory_);

        if (current == kBuildFolder_)
        {
          continue;
        }

        if (item.is_directory() == true)
        {
          FindFileChanges(item.children());
          continue;
        }

        if (HasChanged(item_path) == true)
        {
          QueueForBuild(item_path);
        }
      }
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

          if (current == kBuildFolder_)
          {
            continue;
          }

          current_build = build_directory_ / current;

          if (foundation::Directory::Exists(current_build) == false)
          {
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
      foundation::Path source_file = "";

      for (size_t i = 0; i < build_items.size(); ++i)
      {
        const foundation::DirectoryTreeItem& item = build_items.at(i);
        const foundation::Path& item_path = item.path();

        current = item_path.StripPath(build_directory_);
        current_source = source_directory_ / current;

        if (item.is_directory() == true)
        {
          if (foundation::Directory::Exists(current_source) == false)
          {
            foundation::Directory::Remove(item_path);
            continue;
          }

          RemoveOld(item.children());
          continue;
        }

        if (item_path.extension() == kStampExtension_)
        {
          source_file = current_source.NoExtension();

          if (foundation::File::Exists(source_file) == false)
          {
            foundation::File::Remove(item_path);
          }
        }
      }
    }

    //--------------------------------------------------------------------------
    bool Builder::HasChanged(const foundation::Path& path) const
    {
      if (
        foundation::File::Exists(path) == false ||
        path.is_directory() == true)
      {
        return false;
      }

      foundation::Path relative = path.StripPath(source_directory_);

      foundation::File source(path, foundation::FileFlags::kRead);
      time_t current_time = source.last_modified();

      foundation::Path stamp = 
        build_directory_ / relative + "." + kStampExtension_;
      
      foundation::File f;

      if (foundation::File::Exists(stamp) == true)
      {
        f.Open(stamp, foundation::FileFlags::kRead);

        if (f.is_ok() == false)
        {
          return false;
        }

        size_t len;
        const FileTime* ft = 
          reinterpret_cast<const FileTime*>(f.ReadBuffer(&len));

        foundation::Logger::Assert(len == sizeof(FileTime),
          "Attempted to read an invalid time stamp in the builder");

        if (difftime(current_time, ft->last_modified) <= 0.0)
        {
          return false;
        }
      }

      f.Open(stamp, foundation::FileFlags::kWrite);

      if (f.is_ok() == false)
      {
        return false;
      }

      FileTime cft;
      cft.last_modified = current_time;

      f.Write(reinterpret_cast<uint8_t*>(&cft), sizeof(FileTime));
      
      return true;
    }

    //--------------------------------------------------------------------------
    void Builder::QueueForBuild(const foundation::Path& path)
    {
      if (path.is_directory() == true)
      {
        return;
      }

      BuildItem item;
      item.type = TypeFromExtension(path.extension());
      item.in = path;
      item.relative = path.StripPath(source_directory_);

      if (item.type == AssetTypes::kUnknown)
      {
        foundation::Logger::LogVerbosity<2>(
          foundation::LogChannel::kBuilder,
          foundation::LogSeverity::kDebug,
          "Ignoring '{0}' for rebuild, it is of an unknown type",
          path);

        return;
      }

      foundation::Logger::LogVerbosity<3>(
        foundation::LogChannel::kBuilder,
        foundation::LogSeverity::kDebug,
        "Marked '{0}' for rebuild",
        path);

      scheduler_.Queue(item);
    }

    //--------------------------------------------------------------------------
    AssetTypes Builder::TypeFromExtension(const foundation::String& ext)
    {
      if (ext == "js")
      {
        return AssetTypes::kScript;
      }

      return AssetTypes::kUnknown;
    }

    //--------------------------------------------------------------------------
    Builder::~Builder()
    {
      Shutdown();
    }
  }
}
