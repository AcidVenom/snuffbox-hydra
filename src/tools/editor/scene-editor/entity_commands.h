#pragma once

#include "tools/editor/property-editor/property_value.h"

#include <engine/definitions/components.h>

#include <foundation/containers/uuid.h>

#include <QUndoCommand>
#include <QModelIndex>

namespace snuffbox
{
  namespace editor
  {
    class HierarchyView;
    class HierarchyViewItem;

    /**
    * @brief The base class of every command that does something with entities.
    *        This class guarantees that we have a reference to the hierarchy
    *        view and that we have a valid index to operate with.
    *
    * @author Daniel Konings
    */
    class EntityCommand : public QUndoCommand
    {

    protected:

      /**
      * @brief No default constructor
      */
      EntityCommand() = delete;

      /**
      * @brief Construct by UUID and the hierarchy view this command got
      *        created from
      *
      * @param[in] uuid The UUID of the entity to operate on
      * @param[in] view The hierarchy view that invoked this command
      */
      EntityCommand(const foundation::UUID& uuid, HierarchyView* view);

      /**
      * @return The hierarchy view that invoked this command
      */
      HierarchyView* view() const;

      /**
      * @return The UUID of the entity to operate on
      */
      const foundation::UUID& uuid() const;

      /**
      * @brief Called when this command is executed initially or redone
      */
      void redo() override;

      /**
      * @see EntityCommand::redo
      */
      virtual void RedoImpl() = 0;

      /**
      * @brief Called after a redo, after the scene changed callback is 
      *        unlocked
      */
      virtual void PostRedo();

      /**
      * @brief Called when this entity command is undone
      */
      void undo() override;

      /**
      * @see EntityCommand::undo
      */
      virtual void UndoImpl() = 0;

      /**
      * @brief Called after an undo, after the scene changed callback is 
      *        unlocked
      */
      virtual void PostUndo();

      /**
      * @brief Retrieves the hierarchy view item by a UUID
      *
      * @param[in] uuid The UUID to retrieve
      *
      * @return The found item, or nullptr if it doesn't exist, or the UUID
      *         was not valid
      */
      HierarchyViewItem* GetFromUUID(const foundation::UUID& uuid) const;

      /**
      * @return The entity this entity command operates on
      */
      HierarchyViewItem* GetSelf() const;

    private:

      foundation::UUID uuid_; //!< The UUID of the entity to operate on
      HierarchyView* view_; //!< The hierarchy view that invoked this command
    };
    //--------------------------------------------------------------------------

    /**
    * @brief A command to create entities, or delete them when undone
    *
    * @author Daniel Konings
    */
    class CreateEntityCommand : public EntityCommand
    {

    public:
      
      /**
      * @see EntityCommand::EntityCommand
      */
      CreateEntityCommand(const foundation::UUID& uuid, HierarchyView* view);

      /**
      * @see EntityCommand::RedoImpl
      */
      void RedoImpl() override;

      /**
      * @see EntityCommand::UndoImpl
      */
      void UndoImpl() override;
    };
    //--------------------------------------------------------------------------

    /**
    * @brief A command to delete entities, or re-create them when undone
    *
    * @author Daniel Konings
    */
    class DeleteEntityCommand : public EntityCommand
    {

    public:

      /**
      * @see EntityCommand::EntityCommand
      *
      * @param[in] deleted_index The index this entity was deleted from
      */
      DeleteEntityCommand(
        const foundation::UUID& uuid,
        HierarchyView* view,
        int deleted_index);

      /**
      * @see EntityCommand::RedoImpl
      *
      * @remarks This temporarily stops the signal blocking during execution
      *          so that the hierarchy can emit a signal of a selection change
      *          in the case a selected entity was deleted
      */
      void RedoImpl() override;

      /**
      * @see EntityCommand::UndoImpl
      */
      void UndoImpl() override;

    private:

      QString serialization_data_; //!< The data of the deleted entity

      foundation::UUID deleted_from_; //!< The entity we were deleted from
      int deleted_index_; //!< The index this entity was deleted from
    };

    //--------------------------------------------------------------------------

    /**
    * @brief Used to re-parent entities or move their position around in
    *        the hierarchy view
    *
    * @author Daniel Konings
    */
    class ReparentEntityCommand : public EntityCommand
    {

    public:

      /**
      * @see EntityCommand::EntityCommand
      *
      * @param[in] from The item this item was moved from
      * @param[in] to The item this item was moved to
      * @param[in] index_from The index from which this item was moved
      * @parampin] index_to The index to which this item was moved
      */
      ReparentEntityCommand(
        const foundation::UUID& uuid,
        HierarchyView* view,
        const foundation::UUID& from,
        const foundation::UUID& to,
        int index_from,
        int index_to);

      /**
      * @see EntityCommand::RedoImpl
      */
      void RedoImpl() override;

      /**
      * @see EntityCommand::UndoImpl
      */
      void UndoImpl() override;

      /**
      * @brief Updates the sort indices from a specific index, shifting them
      *        by a positive value of 1 for everything after the item, or
      *        by a negative value of 1 for everything before
      *
      * @remarks Indices before the index are not shifted in any way
      *
      * @param[in] target The target to shift from
      * @param[in] index The index of the target
      */
      void ShiftSortIndices(const foundation::UUID& target, int index);

    private:

      foundation::UUID from_; //!< The item this item was moved from
      foundation::UUID to_; //!< The item this item was moved to

      int index_from_; //!< The index from which this item was moved
      int index_to_; //!< The index to which this item was moved
    };

    //--------------------------------------------------------------------------

    /**
    * @brief Used to add components to entities by component ID
    *
    * @author Daniel Konings
    */
    class AddComponentEntityCommand : public EntityCommand
    {

    public:

      /**
      * @see EntityCommand::EntityCommand
      *
      * @param[in] comp The ID of the component to add
      */
      AddComponentEntityCommand(
        const foundation::UUID& uuid,
        HierarchyView* view,
        engine::Components comp);

      /**
      * @see EntityCommand::RedoImpl
      */
      void RedoImpl() override;

      /**
      * @see EntityCommand::UndoImpl
      */
      void UndoImpl() override;

    private:

      engine::Components comp_; //!< The component type to add
      int new_index_; //!< The index that the component will be inserted at
    };

    //--------------------------------------------------------------------------

    /**
    * @brief Used to remove components from their respective entity
    *
    * @author Daniel Konings
    */
    class RemoveComponentEntityCommand : public EntityCommand
    {

    public:

      /**
      * @see EntityCommand::EntityCommand
      *
      * @param[in] comp The ID of the component to remove
      * @param[in] to_remove The index of the component to remove
      */
      RemoveComponentEntityCommand(
        const foundation::UUID& uuid,
        HierarchyView* view,
        engine::Components comp,
        int to_remove);

      /**
      * @see EntityCommand::RedoImpl
      */
      void RedoImpl() override;

      /**
      * @see EntityCommand::UndoImpl
      */
      void UndoImpl() override;

    private:

      engine::Components comp_; //!< The component to be removed
      int removed_from_; //!< The index the component was removed from

      QString serialization_data_; //!< The serialized component
    };

    //--------------------------------------------------------------------------

    /**
    * @brief Used to set different properties on an entity, both in the
    *        entity itself and its components
    *
    * @author Daniel Konings
    */
    class PropertyEntityCommand : public EntityCommand
    {

    private:

      /**
      * @brief The different property types that exist
      */
      enum class PropertyTypes
      {
        kBoolean,
        kNumber,
        kString,
        kUUID,
        kEnum,
        kVec2,
        kVec3,
        kVec4,
        kAsset
      };

      /**
      * @brief The maximum number of bytes that can be stored within a
      *        property command's data fields
      */
      static const int kMaxDataSize_ = 256;

    public:

      /**
      * @see EntityCommand::EntityCommand
      *
      * @brief Construct through a property name to set and the indexes to
      *        reach the affected component, or unspecified for edits on the
      *        root entity itself
      *
      * @param[in] prop_name The name of the property to edit
      * @param[in] component_type The type of the component to edit,
      *                           default = Components::kCount
      * @param[in] component_index The index of the component to edit,
      *                            default = -1
      */
      PropertyEntityCommand(
        const foundation::UUID& uuid,
        HierarchyView* view,
        const foundation::String& prop_name,
        engine::Components component_type = engine::Components::kCount,
        int component_index = -1);

      /**
      * @brief Sets a boolean value for this command
      *
      * @param[in] value The value to set
      */
      void Set(bool value);

      /**
      * @brief Sets a number value for this command
      *
      * @param[in] value The value to set
      */
      void Set(double value);

      /**
      * @brief Sets a string value for this command
      *
      * @param[in] value The value to set
      */
      void Set(const foundation::String& value);

      /**
      * @brief Sets a UUID value for this command
      *
      * @param[in] value The value to set
      */
      void Set(const foundation::UUID& value);

      /**
      * @brief Sets a vec2 value for this command
      *
      * @param[in] value The value to set
      */
      void Set(const glm::vec2& value);

      /**
      * @brief Sets a vec3 value for this command
      *
      * @param[in] value The value to set
      */
      void Set(const glm::vec3& value);

      /**
      * @brief Sets a vec4 value for this command
      *
      * @param[in] value The value to set
      */
      void Set(const glm::vec4& value);

      /**
      * @brief Sets an asset value for this command
      *
      * @param[in] value The value to set
      */
      void Set(const engine::SerializableAsset& value);

      /**
      * @brief Sets a combo box value for this command
      *
      * @param[in] value The value to set
      */
      void SetComboValue(EnumProperty value);
      
      /**
      * @see EntityCommand::RedoImpl
      */
      void RedoImpl() override;

      /**
      * @see EntityCommand::UndoImpl
      */
      void UndoImpl() override;
      
    protected:

      /**
      * @brief Used to internally setup the old data of the entity and its
      *        corresponding new data
      *
      * @remarks This uses raw retrieval of data, thus it could potentially
      *          go wrong if input data is > kMaxDataSize, this is however
      *          checked at runtime
      *
      * @param[in] value The new data to set
      */
      template <typename T>
      void SetupData(const T& value);

    private:

      /**
      * @brief Retrieves the corresponding property value from the list
      *        of mappings in PropertyMapping
      *
      * @param[out] object The object associated with the property value
      *
      * @return The property value, or nullptr if not found
      */
      PropertyValue* GetProp(void** object);

      /**
      * @brief Sets the data of a property, based on our typing and the buffer
      *        that is specified
      *
      * @param[in] object The object to operate on
      * @param[in] prop The property to set the data for
      * @param[in] data The data buffer to be set, which will be casted
      *                 internally following our current type
      */
      void SetPropertyData(void* object, PropertyValue* prop, uint8_t* data);

    private:

      foundation::String prop_name_; //!< The name of the property to set

      /**
      * @brief The component type, only valid for component-based operations
      */
      engine::Components component_type_;

      /**
      * @brief The component index in the entity,
      *        only valid for component-based operations
      */
      int component_index_;

      PropertyTypes type_; //!< The type of value we are setting

      uint8_t old_data_[kMaxDataSize_]; //!< The old data before setting
      uint8_t data_[kMaxDataSize_]; //!< The new data to set
    };

    //--------------------------------------------------------------------------
    template <typename T>
    inline void PropertyEntityCommand::SetupData(const T& value)
    {
      size_t size = PropertyDataCopy<T>::Required(value);
      foundation::Logger::Assert(size <= kMaxDataSize_,
        "Attempted to set property data for an entity that exceeds"
        " the maximum allowed size");

      if (size > kMaxDataSize_)
      {
        return;
      }

      PropertyDataCopy<T>::Copy(data_, value, size);

      void* object = nullptr;
      PropertyValue* prop = GetProp(&object);

      if (prop == nullptr)
      {
        return;
      }

      size_t required = 0;
      if (prop->GetRaw(object, nullptr, &required) == true)
      {
        foundation::Logger::Assert(required <= kMaxDataSize_,
          "Attempted to set old property data for an entity that exceeds"
          " the maximum allowed size");

        if (required > kMaxDataSize_)
        {
          return;
        }

        prop->GetRaw(object, old_data_, &required);
      }
    }
  }
}