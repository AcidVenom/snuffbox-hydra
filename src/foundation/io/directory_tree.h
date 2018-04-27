#pragma once

#include "foundation/io/path.h"
#include "foundation/io/directory.h"
#include "foundation/containers/vector.h"

namespace snuffbox
{
  namespace foundation
  {
    class DirectoryTree;

    /**
    * @brief A directory tree item to construct a directory tree represented
    *        as a C++ data structure
    *
    * A DirectoryTreeItem can either be a file or a directory. If the item
    * is a directory, its children will contain a multitude of new
    * DirectoryTreeItems that can be traversed appropriately.
    *
    * @author Daniel Konings
    */
    class DirectoryTreeItem
    {

      friend class DirectoryTree;

    protected:

      /**
      * @brief Constructs a directory tree item from a path
      *
      * This calls DirectoryTreeItem::AddChildren if the path is a directory
      *
      * @see DirectoryTreeItem::AddChildren
      *
      * @param[in] path The path to a file or directory
      */
      DirectoryTreeItem(const Path& path);

    public:

      /**
      * @return The path of this directory tree item
      */
      const Path& path() const;

      /**
      * @return Is this item a directory?
      */
      bool is_directory() const;

      /**
      * @return The children items of this item
      */
      const Vector<DirectoryTreeItem>& children() const;

    protected:

      /**
      * @brief Recursively adds the children of this DirectoryTreeItem from
      *        the root path of this item
      *
      * @param[in] dir The path to the directory
      */
      void AddChildren(const Path& dir);

    private:

      Path path_; //!< The path of this directory tree item
      bool is_directory_; //!< Is this item a directory?

      Vector<DirectoryTreeItem> children_; //!< The children items of this item
    };

    /**
    * @brief The root node of a directory tree, containing multiple
    *        DirectoryTreeItems where applicable
    *
    * @author Daniel Konings
    */
    class DirectoryTree
    {

    public:

      /**
      * @brief Creates an empty directory tree to be opened for later use
      */
      DirectoryTree();

      /**
      * @see DirectoryTree::Open
      */
      DirectoryTree(const Path& path);

      /**
      * @brief Opens a directory tree and lists all its items and their
      *        children
      *
      * @param[in] path The root path of the directory tree
      */
      void Open(const Path& path);

      /**
      * @return The root items of this directory tree
      */
      const Vector<DirectoryTreeItem>& items() const;

    protected:

      /**
      * @brief Adds the root items of this directory tree based on the 
      *        found Directory::children
      *
      * The tree is recursed from within the DirectoryTreeItems themselves
      *
      * @remarks The item list is cleared every time a new DirectoryTree::Open
      *          has been called
      *
      * @param[in] items The root items to traverse
      */
      void AddItems(const Vector<Path>& items);

    private:

      /**
      * @brief The root items of this directory tree
      */
      Vector<DirectoryTreeItem> items_;
    };
  }
}