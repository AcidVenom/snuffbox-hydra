#pragma once

#include <foundation/containers/vector.h>

namespace snuffbox
{
  namespace engine
  {
    class Entity;

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

    public:

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
      * @brief Updates all entities in the scene
      *
      * @param[in] dt The current delta-time of the application
      */
      void Update(float dt);

    protected:

      /**
      * @brief Checks if an entity exists within the scene
      *
      * @param[in] entity The entity to check for
      *
      * @return The index of the entity, or -1 if it doesn't exist
      */
      int HasEntity(Entity* entity);

    private:

      /**
      * @brief All current entities in this scene
      */
      foundation::Vector<Entity*> entities_;
    };
  }
}