#include "tools/editor/input/editor_event_filter.h"
#include <engine/input/input_event.h>

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
    engine::Keys EditorEventFilter::ConvertKeyCode(Qt::Key kc)
    {
      switch (kc)
      {
      case Qt::Key::Key_Escape:
        return engine::Keys::kEscape;

      case Qt::Key::Key_Tab:
        return engine::Keys::kTab;

      case Qt::Key::Key_Backspace:
      case Qt::Key::Key_Backtab:
        return engine::Keys::kBackspace;

      case Qt::Key::Key_Return:
      case Qt::Key::Key_Enter:
        return engine::Keys::kEnter;

      case Qt::Key::Key_Insert:
        return engine::Keys::kInsert;

      case Qt::Key::Key_Delete:
        return engine::Keys::kDelete;

      case Qt::Key::Key_Pause:
        return engine::Keys::kPause;

      case Qt::Key::Key_Print:
        return engine::Keys::kPrintScreen;

      case Qt::Key::Key_Home:
        return engine::Keys::kHome;

      case Qt::Key::Key_End:
        return engine::Keys::kEnd;

      case Qt::Key::Key_Left:
        return engine::Keys::kLeft;

      case Qt::Key::Key_Up:
        return engine::Keys::kUp;

      case Qt::Key::Key_Right:
        return engine::Keys::kRight;

      case Qt::Key::Key_Down:
        return engine::Keys::kDown;

      case Qt::Key::Key_PageUp:
        return engine::Keys::kPageUp;

      case Qt::Key::Key_PageDown:
        return engine::Keys::kPageDown;

      case Qt::Key::Key_Shift:
        return engine::Keys::kLeftShift;

      case Qt::Key::Key_Control:
        return engine::Keys::kLeftControl;

      case Qt::Key::Key_Alt:
        return engine::Keys::kLeftAlt;

      case Qt::Key::Key_CapsLock:
        return engine::Keys::kCapsLock;

      case Qt::Key::Key_NumLock:
        return engine::Keys::kNumLock;

      case Qt::Key::Key_ScrollLock:
        return engine::Keys::kScrollLock;

      case Qt::Key::Key_SysReq:
      case Qt::Key::Key_Clear:
      case Qt::Key::Key_Meta:
      case Qt::Key::Key_Super_L:
      case Qt::Key::Key_Super_R:
      case Qt::Key::Key_Hyper_L:
      case Qt::Key::Key_Hyper_R:
      case Qt::Key::Key_Direction_L:
      case Qt::Key::Key_Direction_R:
        return engine::Keys::kUnknown;

      case Qt::Key::Key_Menu:
        return engine::Keys::kMenu;

      default:
        break;
      }

      if (kc >= Qt::Key::Key_F1 && kc <= Qt::Key::Key_F25)
      {
        int offset = static_cast<int>(kc) - static_cast<int>(Qt::Key_F1);
        return static_cast<engine::Keys>(
          static_cast<int>(engine::Keys::kF1) + offset);
      }

      return static_cast<engine::Keys>(kc);
    }

    //--------------------------------------------------------------------------
    void EditorEventFilter::HandleKeyEvent(QKeyEvent* evt)
    {
      if (evt->isAutoRepeat() == true)
      {
        return;
      }

      engine::InputKeyboardKeyEvent e;

      e.evt = evt->type() == QEvent::Type::KeyPress ? 
        engine::KeyButtonEvent::kPressed :
        engine::KeyButtonEvent::kReleased;

      e.key_code = ConvertKeyCode(static_cast<Qt::Key>(evt->key()));

      if (e.key_code == engine::Keys::kUnknown)
      {
        return;
      }
      
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

        e.button = static_cast<engine::MouseButtons>(evt->button() - 1);

        BufferEvent(&e);
      }
      else if (type == QEvent::Type::Scroll)
      {
        engine::InputMouseScrollEvent e;
        QScrollEvent* se = reinterpret_cast<QScrollEvent*>(evt);

        e.delta = static_cast<int>(se->contentPos().y());

        BufferEvent(&e);
      }

      const QPointF& p = evt->windowPos();

      engine::InputMouseMoveEvent e;

      e.x = static_cast<int>(p.x());
      e.y = static_cast<int>(p.y());

      BufferEvent(&e);
    }
  }
}