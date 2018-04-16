#pragma once

#include "engine/input/input_filter.h"

#include <foundation/containers/string.h>

#include <cinttypes>

struct GLFWwindow;

namespace snuffbox
{
  namespace engine
  {
    /**
    * @brief The application's main window, when not ran from the editor
    *
    * The window serves as an input filter to send any input events to the
    * input service
    *
    * @see IInputFilter
    *
    * @author Daniel Konings
    */
    class Window : public IInputFilter
    {

      friend class WindowService;

    public:

      /**
      * @brief Construct a window with a provided title, width and height
      *
      * @param[in] title The title of the window
      * @param[in] width The width of the window
      * @param[in] height The height of the window
      */
      Window(const char* title, uint16_t width, uint16_t height);

    protected:

      /**
      * @brief Initializes the window for use
      *
      * @return Was the initialization a success?
      */
      bool Initialize();

      /**
      * @brief Initializes GLFW for use
      *
      * @return Was the initialization a success?
      */
      static bool InitializeGLFW();

      /**
      * @brief Terminates GLFW if it was initialized
      */
      static void ShutdownGLFW();

    public:

      /**
      * @brief Destroys the GLFW window
      */
      ~Window();

    private:

      foundation::String title_; //!< The window title
      uint16_t width_; //!< The window width
      uint16_t height_; //!< The window height

      GLFWwindow* window_; //!< The window handle

      static bool glfw_initialized_; //!< Is GLFW initialized?
    };
  }
}