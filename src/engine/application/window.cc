#include "engine/application/window.h"

#include "engine/auxiliary/debug.h"

#include <GLFW/glfw3.h>

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    bool Window::glfw_initialized_ = false;

    //--------------------------------------------------------------------------
    Window::Window(const char* title, uint16_t width, uint16_t height) :
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

      return true;
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
    Window::~Window()
    {
      if (window_ != nullptr)
      {
        glfwDestroyWindow(window_);
      }
    }
  }
}