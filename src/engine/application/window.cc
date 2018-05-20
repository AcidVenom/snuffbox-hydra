#include "engine/application/window.h"

#include "engine/auxiliary/debug.h"
#include "engine/input/input_event.h"

#include <GLFW/glfw3.h>

#ifdef SNUFF_WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined (SNUFF_LINUX)
#define GLFW_EXPOSE_NATIVE_X11
#else
#error "Unsupported window implementation"
#endif

#include <GLFW/glfw3native.h>

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    bool Window::glfw_initialized_ = false;
    const size_t Window::kMaxInputEvents_ = 512;

    //--------------------------------------------------------------------------
    Window::JoystickData::JoystickData()
    {
      for (size_t i = 0; i < static_cast<size_t>(JoystickButtons::kCount); ++i)
      {
        buttons[i] = GLFW_RELEASE;
      }

      for (size_t i = 0; i < static_cast<size_t>(JoystickAxes::kCount); ++i)
      {
        axes[i] = 0.0f;
      }
      
      axes[static_cast<size_t>(JoystickAxes::kLeftTrigger)] = -1.0f;
      axes[static_cast<size_t>(JoystickAxes::kRightTrigger)] = -1.0f;
    }

    //--------------------------------------------------------------------------
    Window::Window(const char* title, uint16_t width, uint16_t height) :
      IInputFilter(kMaxInputEvents_),
      title_(title),
      width_(width),
      height_(height),
      window_(nullptr),
      on_resize_(nullptr)
    {
      for (size_t i = 0; i < kNumSupportedJoysticks; ++i)
      {
        previous_connected_joysticks_[i] = false;
      }
    }

    //--------------------------------------------------------------------------
    bool Window::Initialize()
    {
      if (glfw_initialized_ == false)
      {
        Debug::Log(foundation::LogSeverity::kError,
          "Attempted to create a window without GLFW being initialized");

        return false;
      }

      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
      glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
      glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
      glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);

      window_ = glfwCreateWindow(
        width_, 
        height_, 
        title_.c_str(),
        nullptr, 
        nullptr);

      if (window_ == nullptr)
      {
        Debug::Log(foundation::LogSeverity::kError,
          "Could not create GLFW window");

        return false;
      }

      glfwSetWindowUserPointer(window_, this);

      glfwSetKeyCallback(window_, GLFWKeyCallback);
      glfwSetMouseButtonCallback(window_, GLFWMouseButtonCallback);
      glfwSetCursorPosCallback(window_, GLFWMousePosCallback);
      glfwSetScrollCallback(window_, GLFWMouseScrollCallback);
      glfwSetWindowSizeCallback(window_, GLFWWindowSizeCallback);

      return true;
    }

    //--------------------------------------------------------------------------
    bool Window::ProcessEvents()
    {
      if (glfwWindowShouldClose(window_) == GL_TRUE)
      {
        return true;
      }

      UpdateJoysticks();
      glfwPollEvents();

      return false;
    }

    //--------------------------------------------------------------------------
    void Window::Show()
    {
      glfwShowWindow(window_);
    }

    //--------------------------------------------------------------------------
    void* Window::NativeHandle() const
    {
#ifdef SNUFF_WIN32
      return glfwGetWin32Window(window_);
#elif defined(SNUFF_LINUX)
      return glfwGetX11Window(window_);
#endif
    }

    //--------------------------------------------------------------------------
    void Window::UpdateJoysticks()
    {
      CheckJoystickConnected();

      int count;
      int id;

      const unsigned char* buttons;
      const float* axes;

      unsigned char button_state;
      float axis;

      static const int max_buttons = static_cast<int>(JoystickButtons::kCount);
      static const int max_axes = static_cast<int>(JoystickAxes::kCount);

      for (size_t i = 0; i < kNumSupportedJoysticks; ++i)
      {
        JoystickData& data = joystick_data_[i];

        if (previous_connected_joysticks_[i] == false)
        {
          continue;
        }

        id = static_cast<int>(i);
        buttons = glfwGetJoystickButtons(id, &count);

        count = count > max_buttons ? max_buttons : count;

        for (int b = 0; b < count; ++b)
        {
          button_state = buttons[b];

          if (button_state != data.buttons[b])
          {
            InputJoystickButtonEvent e;
            e.button = static_cast<JoystickButtons>(b);
            e.evt = button_state == GLFW_PRESS ? 
              KeyButtonEvent::kPressed : 
              KeyButtonEvent::kReleased;
            e.id = id;
            
            BufferEvent(&e);

            data.buttons[b] = button_state;
          }
        }

        axes = glfwGetJoystickAxes(id, &count);

        count = count > max_axes ? max_axes : count;

        for (int a = 0; a < count; ++a)
        {
          axis = axes[a];

          if (axis != data.axes[a])
          {
            InputJoystickAxisEvent e;
            e.axis = static_cast<JoystickAxes>(a);
            e.id = id;
            e.value = axis;

            BufferEvent(&e);

            data.axes[a] = axis;
          }
        }
      }
    }

    //--------------------------------------------------------------------------
    void Window::CheckJoystickConnected()
    {
      bool now = false;
      bool prev = false;
      int id;

      for (size_t i = 0; i < kNumSupportedJoysticks; ++i)
      {
        id = static_cast<int>(i);
        prev = previous_connected_joysticks_[i];
        now = glfwJoystickPresent(id);

        if (prev != now)
        {
          InputJoystickConnectEvent e;
          e.connected = now;
          e.device_name = now == true ? glfwGetJoystickName(id) : "";
          e.id = id;

          BufferEvent(&e);

          previous_connected_joysticks_[i] = now;
        }
      }
    }

    //--------------------------------------------------------------------------
    bool Window::InitializeGLFW()
    {
      if (glfw_initialized_ == true)
      {
        Debug::Log(foundation::LogSeverity::kError, "GLFW already initialized");
        return false;
      }

      auto GLFWErrorHandler = [](int err, const char* msg)
      {
        Debug::Log(
          foundation::LogSeverity::kError, 
          "[GLFW] {0}: {1}", 
          err, 
          msg);
      };

      glfwSetErrorCallback(GLFWErrorHandler);

      if (glfwInit() == GLFW_FALSE)
      {
        Debug::Log(foundation::LogSeverity::kError, "Could not initalize GLFW");
        return false;
      }

      glfw_initialized_ = true;
      return true;
    }

    //--------------------------------------------------------------------------
    void Window::ShutdownGLFW()
    {
      if (glfw_initialized_ == false)
      {
        return;
      }

      glfwTerminate();
      glfw_initialized_ = false;
    }

    //--------------------------------------------------------------------------
    void Window::GLFWKeyCallback(
      GLFWwindow* window, 
      int key_code,
      int scan_code,
      int action,
      int mods)
    {
      if (action != GLFW_PRESS && action != GLFW_RELEASE)
      {
        return;
      }

      Window* ud = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

      if (ud == nullptr)
      {
        return;
      }

      InputKeyboardKeyEvent e;
      e.key_code = static_cast<Keys>(key_code);
      
      switch (action)
      {
      case GLFW_PRESS:
        e.evt = KeyButtonEvent::kPressed;
        break;

      case GLFW_RELEASE:
        e.evt = KeyButtonEvent::kReleased;
        break;

      default:
        e.evt = KeyButtonEvent::kPressed;
        break;
      }

      ud->BufferEvent(&e);
    }

    //--------------------------------------------------------------------------
    void Window::GLFWMouseButtonCallback(
      GLFWwindow* window,
      int button,
      int action,
      int mods)
    {
      if (action != GLFW_PRESS && action != GLFW_RELEASE)
      {
        return;
      }

      Window* ud = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

      if (ud == nullptr)
      {
        return;
      }

      InputMouseButtonEvent e;
      e.button = static_cast<MouseButtons>(button);

      switch (action)
      {
      case GLFW_PRESS:
        e.evt = KeyButtonEvent::kPressed;
        break;

      case GLFW_RELEASE:
        e.evt = KeyButtonEvent::kReleased;
        break;

      default:
        e.evt = KeyButtonEvent::kPressed;
        break;
      }

      ud->BufferEvent(&e);
    }

    //--------------------------------------------------------------------------
    void Window::GLFWMousePosCallback(GLFWwindow* window, double x, double y)
    {
      Window* ud = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

      if (ud == nullptr)
      {
        return;
      }

      int px = static_cast<int>(x);
      int py = static_cast<int>(y);

      InputMouseMoveEvent e;
      e.x = px;
      e.y = py;

      ud->BufferEvent(&e);
    }

    //--------------------------------------------------------------------------
    void Window::GLFWMouseScrollCallback(
      GLFWwindow* window, 
      double x, 
      double y)
    {
      Window* ud = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

      if (ud == nullptr)
      {
        return;
      }

      InputMouseScrollEvent e;
      e.delta = static_cast<int>(y);

      ud->BufferEvent(&e);
    }

    //--------------------------------------------------------------------------
    void Window::GLFWWindowSizeCallback(
      GLFWwindow* window,
      int width,
      int height)
    {
      Window* ud = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

      if (ud == nullptr)
      {
        return;
      }

      if (ud->on_resize_ != nullptr)
      {
        ud->on_resize_(width, height);
      }
    }

    //--------------------------------------------------------------------------
    Window::~Window()
    {
      if (window_ != nullptr)
      {
        glfwDestroyWindow(window_);
      }
    }
  }
}