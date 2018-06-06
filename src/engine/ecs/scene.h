#pragma once

#include <foundation/containers/vector.h>
#include <foundation/serialization/serializable.h>

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
    class Scene : public foundation::ISerializable
    {

      friend Entity;
      friend TransformComponent;

    public:

      /**
      * @brief Used to call a function on each entity in the scene
      */
      using EntityDelegate = void(*)(Entity*);

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

    public:

      /**
      * @brief Starts all entities in the scene
      */
      void Start();

      /**
      * @brief Updates all entities in the scene
      *
      * @param[in] dt The current delta-time of the application
      */
      void Update(float dt);

      /**
      * @return The transform hierarchy with the upper-level transforms
      */
      foundation::Vector<TransformComponent*> TopLevelTransforms() const;

      /**
      * @brief Call a function on each entity in the scene
      *
      * @param[in] del The delegate to call
      */
      void ForEachEntity(EntityDelegate del);

      void Serialize(foundation::SaveArchive& archive) const override;

      void Deserialize(foundation::LoadArchive& archive) override;

      /**
      * @brief Default destructor, calls Scene::Clear
      */
      ~Scene();

    private:

      /**
      * @brief All current entities in this scene
      */
      foundation::Vector<Entity*> entities_;
    };
  }
}