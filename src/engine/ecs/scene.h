#pragma once

#include <foundation/containers/vector.h>
#include <foundation/serialization/serializable.h>
#include <foundation/containers/function.h>

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
      using EntityDelegate = foundation::Function<bool(Entity*)>;

      /**
      * @brief Default constructor
      */
      Scene();

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
      * @brief Checks if an entity exists within the scene
      *
      * @param[in] entity The entity to check for
      *
      * @return The index of the entity, or -1 if it doesn't exist
      */
      int HasEntity(Entity* entity);

      /**
      * @brief Finds the highest ID in the entity list and increments it by 1
      *
      * @return The unique ID
      */
      size_t GetNextAvailableID();

      /**
      * @brief Removes all null-entities after the update of a frame
      */
      void RemoveNullEntities();

    public:

      /**
      * @brief Starts all entities in the scene
      */
      void Start();

      /**
      * @brief Updates all entities in the scene
      *
      * This call also calls Scene::RemoveNullEntities after the frame to make
      * sure any null-references are removed from the scene. This makes sure
      * the contents of the entity vector don't get changed during execution.
      *
      * @param[in] dt The current delta-time of the application
      */
      void Update(float dt);

      /**
      * @brief Only renders all entities in the scene whenever they are active,
      *        internal cameras are rendered as well
      *
      * This call doesn't actually execute any other components other than
      * rendering components. This method is not to be used in conjuction
      * with Scene::Update and is primarily used by the editor while editing
      * the scene.
      *
      * This call also calls Scene::RemoveNullEntities after the frame to make
      * sure any null-references are removed from the scene. This makes sure
      * the contents of the entity vector don't get changed during execution.
      *
      * @param[in] dt The current delta-time of the application
      */
      void RenderEntities(float dt);

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
      * @return The transform hierarchy with the upper-level transforms
      */
      foundation::Vector<TransformComponent*> TopLevelTransforms() const;

      /**
      * @brief Call a function on each entity in the scene
      *
      * @param[in] del The delegate to call
      *
      * @remarks The delegate should return whether to continue the loop or not,
      *          where true = continue and false = break
      */
      void ForEachEntity(const EntityDelegate& del);

      /**
      * @brief Finds an entity by ID and returns it
      *
      * @param[in] id The ID to search for
      *
      * @return The found entity, or nullptr if it doesn't exist in this scene
      */
      Entity* FindEntityByID(size_t id);

      /**
      * @brief Called from either the Entity or TransformComponent when
      *        their scene properties/hierarchy have changed
      */
      void OnSceneChanged();

      /**
      * @see ISerializable::Serialize
      */
      void Serialize(foundation::SaveArchive& archive) const override;

      /**
      * @see ISerializable::Deserialize
      */
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

      size_t current_id_; //!< The next available ID for an entity
      bool deleted_; //!< Were any entities deleted this frame?
    };
  }
}