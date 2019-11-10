#pragma once

#include <foundation/containers/string.h>
#include <foundation/memory/memory.h>

#include <QWidget>

namespace snuffbox
{
  namespace editor
  {
    class PropertyView;
    class PropertyValue;

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
      * @brief Construct through a property view and the widget's parent
      *
      * @param[in] view The view this value edit is owned by
      * @param[in] object The object this property belongs to
      * @param[in] name The name of the property that was in the mapping
      * @param[in] prop The property this edit operates on
      * @param[in] parent The parent of this value edit
      */
      PropertyValueEdit(
        void* object,
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
      * @brief Creates a vector edit widget
      *
      * @param[in] length The length of the input vector
      *
      * @return The created widget
      */
      QWidget* CreateVectorEdit(int length);

      /**
      * @brief Updates the value within this property value edit
      *
      * @param[in] new_data The new data to set
      */
      void UpdateValue(const uint8_t* new_data);

    private:

      /**
      * @brief The maximum data size contained within an editable value
      */
      static const size_t kMaxDataSize_ = 256;
      static const int kSpacing_; //!< The spacing between the name and edit
      static const int kLabelWidth_; //!< The width of the name label
      static const int kMaxEditWidth_; //!< The maximum width of edit boxes

      /**
      * @brief The size of a vector component label
      */
      static const int kVectorLabelSize_;

      /**
      * @brief The maximum spacing between vector components
      */
      static const int kVectorSpacing_;

      QString name_; //!< The name of this property value edit

      void* object_; //!< The object this property belongs to

      /**
      * @brief The property this edit operates on
      */
      foundation::SharedPtr<PropertyValue> prop_;

      /**
      * @brief The old data of the assigned property, used to update the
      *        value edit accordingly
      */
      uint8_t* old_data_[kMaxDataSize_];

      EditTypes type_; //!< The edit type for this value edit
      QWidget* widget_; //!< The widget contained in this value edit
    };
  }
}