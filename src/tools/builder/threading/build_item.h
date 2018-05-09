#pragma once

#include "tools/builder/definitions/asset_types.h"

#include <foundation/io/path.h>
#include <foundation/containers/queue.h>

namespace snuffbox
{
  namespace builder
  {
    /**
    * @brief Used to queue build items and schedule them to the corresponding
    *        build jobs
    *
    * @author Daniel Konings
    */
    struct BuildItem
    {
      AssetTypes type; //!< The type of the build item
      foundation::Path in; //!< The path to the input source file

      /**
      * @brief Used to sort build items by their asset type
      *
      * This is used to make sure that dependencies of different asset types
      * are built before others. The comparision function checks the
      * AssetTypes value that is set to BuildItem::type and sorts on the
      * lesser item first. Thus, the build order that assets are built in
      * are completely dependent on the order of the AssetTypes enumerator.
      *
      * @see AssetTypes
      */
      struct Compare
      {
        /**
        * @brief The comparision function used by EASTL
        *
        * @param[in] a The first item
        * @param[in] b The second item
        *
        * @return Should item a be prioritized?
        */
        bool operator()(const BuildItem& a, const BuildItem& b);
      };
    };

    /**
    * @brief Short hand for the priority queue used to queue build items
    */
    using BuildQueue = 
      foundation::PriorityQueue<BuildItem, BuildItem::Compare>;
  }
}