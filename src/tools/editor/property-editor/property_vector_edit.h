#pragma once

#include <glm/vec4.hpp>

#include <QWidget>

namespace snuffbox
{
  namespace editor
  {
    class PropertyNumberEdit;

    /**
    * @brief Used to edit vector values of a maximum of 4 float values
    *        (x, y, z, w) using PropertyNumberEdit for each component
    *
    * @author Daniel Konings
    */
    class PropertyVectorEdit : public QWidget
    {

      Q_OBJECT;

    public:

      /**
      * @brief Construct through a parent and a respective length
      *
      * @param[in] length The length of the vector to edit (max 4)
      * @param[in] parent The parent of this widget, default = nullptr
      */
      PropertyVectorEdit(int length, QWidget* parent = nullptr);

      /**
      * @brief Sets a vector value as this vector edit's values
      *
      * @param[in] vec The vector value to set
      *
      * @remarks This will set values upward to the length of this
      *          vector edit
      */
      void SetValue(const glm::vec4& vec);

      /**
      * @return The current vector value
      *
      * @remarks This will contain values upward to the length of this
      *          vector edit, the rest will be 0.0f
      */
      glm::vec4 Value() const;

    private:

      int length_; //!< The length of this vector edit
      PropertyNumberEdit* edits_[4]; //!< The boxes for editing

      /**
      * @brief The size of a vector component label
      */
      static const int kVectorLabelSize_;

      /**
      * @brief The maximum spacing between vector components
      */
      static const int kVectorSpacing_;

      /**
      * @brief The width of the spinboxes
      */
      static const int kVectorEditWidth_;
    };
  }
}