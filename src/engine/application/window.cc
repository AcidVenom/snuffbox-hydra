#include "engine/application/window.h"

#include "engine/auxiliary/debug.h"
#include "engine/input/input_event.h"

#include <GLFW/glfw3.h>

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    bool Window::glfw_initialized_ = false;
    const size_t Window::kMaxInputEvents_ = 128;

    //--------------------------------------------------------------------------
    Window::Window(const char* title, uint16_t width, uint16_t height) :
      IInputFilter(kMaxInputEvents_),
      title_(title),
      width_(width),
      height_(height),
      window_(nullptr)
    {
      
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

      glfwShowWindow(window_);

      return true;
    }

    //--------------------------------------------------------------------------
    bool Window::ProcessEvents()
    {
      glfwPollEvents();

      return glfwWindowShouldClose(window_) == GL_TRUE;
    }

    //--------------------------------------------------------------------------
    bool Window::InitializeGLFW()
    {
      if (glfw_initialized_ == true)
      {
        Debug::Log(foundation::LogSeverity::kError, "GLFW already initialized");
        return false;
      }

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
    Window::~Window()
    {
      if (window_ != nullptr)
      {
        glfwDestroyWindow(window_);
      }
    }
  }
}