#include "foundation/io/directory_tree.h"

namespace snuffbox
{
  namespace foundation
  {
    //--------------------------------------------------------------------------
    DirectoryTreeItem::DirectoryTreeItem(const Path& path) :
      path_(path),
      is_directory_(path.is_directory())
    {
      if (is_directory_ == true)
      {
        AddChildren(path);
      }
    }

    //--------------------------------------------------------------------------
    void DirectoryTreeItem::AddChildren(const Path& dir)
    {
      if (dir.is_directory() == false)
      {
        return;
      }

      Directory d(dir);

      if (d.is_ok() == false)
      {
        return;
      }

      const Vector<Path>& children = d.children();

      for (size_t i = 0; i < children.size(); ++i)
      {
        children_.push_back(DirectoryTreeItem(children.at(i)));
      }
    }

    //--------------------------------------------------------------------------
    DirectoryTree::DirectoryTree(const Path& path)
    {
      Open(path);
    }

    //--------------------------------------------------------------------------
    void DirectoryTree::Open(const Path& path)
    {
      if (path.is_virtual() == true)
      {
        return;
      }

      Directory root_dir(path);

      if (root_dir.is_ok() == false)
      {
        return;
      }

      AddItems(root_dir.children());
    }

    //--------------------------------------------------------------------------
    void DirectoryTree::AddItems(const Vector<Path>& items)
    {
      items_.clear();

      for (size_t i = 0; i < items.size(); ++i)
      {
        items_.push_back(items.at(i));
      }
    }
  }
}