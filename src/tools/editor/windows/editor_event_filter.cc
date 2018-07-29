#include "tools/editor/windows/editor_event_filter.h"
#include "engine/input/input_event.h"

#include <qevent.h>

namespace snuffbox
{
  namespace editor
  {
    //--------------------------------------------------------------------------
    EditorEventFilter::EditorEventFilter(size_t max_buffered_events) :
      engine::IInputFilter(max_buffered_events)
    {
      
    }

    //--------------------------------------------------------------------------
    bool EditorEventFilter::eventFilter(QObject* obj, QEvent* evt)
    {
      switch (evt->type())
      {
      case QEvent::Type::KeyPress:
      case QEvent::Type::KeyRelease:
        HandleKeyEvent(static_cast<QKeyEvent*>(evt));
        break;

      case QEvent::Type::MouseButtonPress:
      case QEvent::Type::MouseButtonRelease:
      case QEvent::Type::MouseMove:
      case QEvent::Type::Scroll:
        HandleMouseEvent(static_cast<QMouseEvent*>(evt));
        break;

      default:
        return QObject::eventFilter(obj, evt);
      }

      return false;
    }

    //--------------------------------------------------------------------------
    void EditorEventFilter::HandleKeyEvent(QKeyEvent* evt)
    {
      engine::InputKeyboardKeyEvent e;

      e.evt = evt->type() == QEvent::Type::KeyPress ? 
        engine::KeyButtonEvent::kPressed :
        engine::KeyButtonEvent::kReleased;

      e.key_code = static_cast<engine::Keys>(evt->key());
      
      BufferEvent(&e);
    }

    //--------------------------------------------------------------------------
    void EditorEventFilter::HandleMouseEvent(QMouseEvent* evt)
    {
      QEvent::Type type = evt->type();

      bool pressed = false;

      if (
        (pressed = (type == QEvent::Type::MouseButtonPress)) || 
        type == QEvent::Type::MouseButtonRelease)
      {
        engine::InputMouseButtonEvent e;
        e.evt = pressed == true ? 
          engine::KeyButtonEvent::kPressed : 
          engine::KeyButtonEvent::kReleased;

        e.button = static_cast<engine::MouseButtons>(evt->button());

        BufferEvent(&e);

        return;
      }
      else if (type == QEvent::Type::MouseMove)
      {
        const QPointF& p = evt->windowPos();

        engine::InputMouseMoveEvent e;

        e.x = static_cast<int>(p.x());
        e.y = static_cast<int>(p.y());

        BufferEvent(&e);

        return;
      }
      else if (type == QEvent::Type::Scroll)
      {
        engine::InputMouseScrollEvent e;
        QScrollEvent* se = reinterpret_cast<QScrollEvent*>(evt);

        e.delta = static_cast<int>(se->contentPos().y());

        BufferEvent(&e);
      }
    }
  }
}