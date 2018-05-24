#pragma once

#include <foundation/containers/vector.h>

namespace snuffbox
{
  namespace engine
  {
    class Entity;
    class TransformComponent;

    /**
    * @brief A scene to contain all entities and to load and unload required
    *        assets appropriately for the ones that are referenced
    *
    * Switching scenes should guarantee a full clean up of the previous scene,
    * this way scenes can be used to switch between menus, levels, etc. in an
    * isolated environment.
    *
    * Persistent data can easily be done through the power of scripting, which
    * the user can then define the behavior of themselves.
    *
    * For the user to not have to explicitly add entities to the scene
    * themselves, the scenes are statically available through the SceneService
    * living in the Application.
    *
    * @author Daniel Konings
    */
    class Scene
    {

      friend Entity;
      friend TransformComponent;

    protected:

      /**
      * @brief Adds an entity to the current list of entities
      *
      * @param[in] entity The entity to add
      */
      void AddEntity(Entity* entity);

      /**
      * @brief Removes an entity from the current list of entities
      *
      * @param[in] entity The entity to remove
      */
      void RemoveEntity(Entity* entity);

      /**
      * @brief Removes an entity from a provided index
      *
      * @see Scene::RemoveEntity
      *
      * @param[in] idx The index to remove the entity from
      */
      void RemoveEntityAt(int idx);

      /**
      * @brief Clears all entities in the scene
      *
      * The entities that were created from script are not destructed here,
      * they are however removed from the list of entities and thus not
      * updated anymore.
      *
      * Entities created from C++-side still get destructed appropriately.
      */
      void Clear();

      /**
      * @brief Checks if an entity exists within the scene
      *
      * @param[in] entity The entity to check for
      *
      * @return The index of the entity, or -1 if it doesn't exist
      */
      int HasEntity(Entity* entity);

      /**
      * @brief Checks if a transform exists within the hierarchy
      *
      * @param[in] transform The transform to check for
      *
      * @return The index of the transform, or -1 if it doesn't exist
      */
      int HasTransform(TransformComponent* transform);

      /**
      * @brief Updates the transform hierarchy whenever a new transform
      *        is created or re-parented
      *
      * Only the root-level transforms should be in the list of the hierarchy.
      * From here, the transform components can be updated recursively through
      * the children tree. Whenever a transform is reparented to have no
      * parent, or a transform is added; it is added to the list.
      *
      * If a transform component is parented while it previously had a parent;
      * the transform is removed from the list.
      *
      * @param[in] transform The new or updated transform component
      * @param[in] removed Was the transform removed?
      */
      void UpdateHierarchy(TransformComponent* transform, bool removed = false);

    public:

      /**
      * @brief Updates all entities in the scene
      *
      * @param[in] dt The current delta-time of the application
      */
      void Update(float dt);

      /**
      * @return The transform hierarchy with the upper-level transforms
      */
      const foundation::Vector<TransformComponent*>& hierarchy() const;

      /**
      * @brief Default destructor, calls Scene::Clear
      */
      ~Scene();

    private:

      /**
      * @brief All current entities in this scene
      */
      foundation::Vector<Entity*> entities_;

      /**
      * @brief The transform hierarchy with the upper-level transforms
      */
      foundation::Vector<TransformComponent*> hierarchy_;
    };
  }
}