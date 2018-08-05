#pragma once

#include <engine/input/input_filter.h>
#include <engine/definitions/keycodes.h>

#include <qobject.h>
#include <qnamespace.h>

class QKeyEvent;
class QMouseEvent;

namespace snuffbox
{
  namespace editor
  {
    class MainWindow;

    /**
    * @brief Used to send Qt input events to the editor, so that any
    *        engine logic can be handled by focussing the editor window
    *
    * @author Daniel Konings
    */
    class EditorEventFilter : public QObject, public engine::IInputFilter
    {

      Q_OBJECT

      friend MainWindow;

    public:

      /**
      * @see IInputFilter::IInputFilter
      */
      EditorEventFilter(size_t max_buffered_events = kDefaultMaxEvents_);

    protected:

      /**
      * @brief Called when a key event has been fired
      *
      * @remarks This includes key presses and releases
      *
      * The key should be automatically converted to the respective Snuffbox
      * keycode where necessary
      *
      * @param[in] evt The keyboard event that was fired
      */
      void HandleKeyEvent(QKeyEvent* evt);

      /**
      * @brief Called when a mouse event has been fired
      *
      * @remarks This includes mouse button presses and releases and mouse
      *          move operations, along with scrolling
      *
      * The mouse buttons should be automatically converted to the respective 
      * Snuffbox keycode where necessary. The mouse move events should also
      * respect that client rectangle of the widget that fires it.
      *
      * @param[in] evt The mouse event that was fired
      */
      void HandleMouseEvent(QMouseEvent* evt);

      /**
      * @brief The event filter to filter out keyboard and mouse events
      */
      bool eventFilter(QObject* obj, QEvent* evt);

      /**
      * @brief Converts a Qt keycode to the respective engine keycode
      *
      * @remarks Most keycodes are the same, but modifier keys and special keys
      *          are handled differently. These are converted
      *
      * Modifier keys are returned as their left-sided equivalent, e.g.
      * Qt::Key::Key_Shift becomes engine::Key::kLeftShift
      *
      * @param[in] kc The keycode to convert
      *
      * @return The converted keycode
      */
      static engine::Keys ConvertKeyCode(Qt::Key kc);

    private:

      /**
      * @brief The default maximum events that can be buffered at the same time
      */
      static const size_t kDefaultMaxEvents_ = 128;
    };
  }
}