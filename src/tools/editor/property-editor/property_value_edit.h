#pragma once

#include <engine/definitions/components.h>

#include <foundation/containers/string.h>
#include <foundation/memory/memory.h>

#include <QWidget>

namespace snuffbox
{
  namespace engine
  {
    class Entity;
    class IComponent;
  }

  namespace editor
  {
    class PropertyView;
    class PropertyValue;
    class PropertyEntityCommand;
    class HierarchyView;

    /**
    * @brief Used to edit property values with their respective Qt widget
    *
    * For instance, when a string value is found, a QLineEdit is used; but
    * for a more proprietary value (like an asset), custom editors are used.
    *
    * @author Daniel Konings
    */
    class PropertyValueEdit : public QWidget
    {
      
      Q_OBJECT;

    private:

      /**
      * @brief The different types of edit widgets that exist
      */
      enum class EditTypes
      {
        kCheckBox, //!< Used for booleans
        kComboBox, //!< Used for enumerators
        kNumberEdit, //!< Used for numbers (doubles/floats)
        kLineEdit, //!< Used for strings and UUIDs
        kVec2Edit, //!< Used for Vector2 values
        kVec3Edit, //!< Used for Vector3 values
        kVec4Edit, //!< Used for Vector4 values
        kCount
      };

    public:

      /**
      * @brief Construct through a hierarchy view and the widget's parent
      *
      * @param[in] hierarchy The hierarchy the editable entity lives in
      * @param[in] entity The entity this property edit belongs to
      * @param[in] component The component this property belongs to
      * @param[in] name The name of the property that was in the mapping
      * @param[in] prop The property this edit operates on
      * @param[in] parent The parent of this value edit
      */
      PropertyValueEdit(
        HierarchyView* hierarchy,
        engine::Entity* entity,
        engine::IComponent* component,
        const foundation::String& name,
        const foundation::SharedPtr<PropertyValue>& prop,
        QWidget* parent = nullptr);

      /**
      * @brief Updates this value edit. This will check if the value was
      *        changed and if so, update the inner value.
      */
      void Update();

    protected:

      /**
      * @brief Creates the appropriate widget based on the edit type
      *
      * @return The created widget
      */
      QWidget* CreateWidget();

      /**
      * @brief Updates the value within this property value edit
      *
      * @param[in] new_data The new data to set
      */
      void UpdateValue(const uint8_t* new_data);

      /**
      * @return Returns the actual object to be modified, which can either
      *         be the entity or the component
      */
      void* GetRawObject() const;

      /**
      * @return A new set command to set a value within
      */
      PropertyEntityCommand* CreateSetCommand();

    protected slots:

      /**
      * @brief Called when a checkbox has been toggled
      */
      void OnCheckboxChanged(bool toggled);

      /**
      * @brief Called when the index within a combo box value changes
      *
      * @param[in] index The new index
      */
      void OnComboBoxChanged(int index);

      /**
      * @brief Called when a number within a number edit value changes
      *
      * @param[in] number The new number
      */
      void OnNumberChanged(double number);

      /**
      * @brief Called when a string value edit has changed
      *
      * @param[in] str The new value
      */
      void OnStringChanged(const QString& str);

      /**
      * @brief Called when a vector value has changed
      *
      * @param[in] comp The component that was changed
      * @param[in] value The new value of that component
      */
      void OnVectorChanged(int comp, double value);

    private:

      /**
      * @brief The maximum data size contained within an editable value
      */
      static const size_t kMaxDataSize_ = 256;
      static const int kSpacing_; //!< The spacing between the name and edit
      static const int kLabelWidth_; //!< The width of the name label
      static const int kMaxEditWidth_; //!< The maximum width of edit boxes

      QString name_; //!< The name of this property value edit

      HierarchyView* hierarchy_; //!< The hierarchy to operate on
      engine::Entity* entity_; //!< The entity to edit with this value
      engine::IComponent* component_; //!< The component to edit, if any

      /**
      * @brief The property this edit operates on
      */
      foundation::SharedPtr<PropertyValue> prop_;

      EditTypes type_; //!< The edit type for this value edit

      QWidget* widget_; //!< The widget contained in this value edit

      /**
      * @brief The old data of the assigned property, used to update the
      *        value edit accordingly
      */
      uint8_t old_data_[kMaxDataSize_];
    };
  }
}