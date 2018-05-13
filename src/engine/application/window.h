#pragma once

#include "engine/input/input_filter.h"
#include "engine/definitions/keycodes.h"

#include <graphics/definitions/graphics_window.h>

#include <foundation/containers/string.h>
#include <foundation/containers/function.h>

#include <cstddef>
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
      * @brief Processes the GLFW window events
      *
      * @return Should the application exit?
      */
      bool ProcessEvents();

      /**
      * @brief Shows the window
      */
      void Show();

      /**
      * @return The native handle of this window
      */
      void* NativeHandle() const;

      /**
      * @brief As we don't have a GLFWwindow user pointer in a joystick callback
      *        we manually process the controllers by retrieving the data
      *        from GLFW directly
      *
      * This is done through a global list of connected controllers, since
      * we simply don't have any state for the joysticks or whatsoever
      */
      void UpdateJoysticks();

      /**
      * @brief Checks if any new joysticks have been connected
      */
      void CheckJoystickConnected();

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

      /**
      * @see http://www.glfw.org/docs/latest/input_guide.html
      */
      static void GLFWKeyCallback(
        GLFWwindow* window, 
        int key_code,
        int scan_code,
        int action,
        int mods);

      /**
      * @see http://www.glfw.org/docs/latest/input_guide.html
      */
      static void GLFWMouseButtonCallback(
        GLFWwindow* window, 
        int button,
        int action,
        int mods);

      /**
      * @see http://www.glfw.org/docs/latest/input_guide.html
      */
      static void GLFWMousePosCallback(GLFWwindow* window, double x, double y);

      /**
      * @see http://www.glfw.org/docs/latest/input_guide.html
      */
      static void GLFWMouseScrollCallback(
        GLFWwindow* window, 
        double x, 
        double y);

      /**
      * @see http://www.glfw.org/docs/latest/window_guide.html#window_size
      */
      static void GLFWWindowSizeCallback(
        GLFWwindow* window, 
        int width, 
        int height);

    protected:

      /**
      * @brief Used to store previous data of joysticks to send events if they
      *        changed
      *
      * @author Daniel Konings
      */
      struct JoystickData
      {
        /**
        * @brief Default constructor, sets all data to their initial value
        */
        JoystickData();

        /**
        * @brief The previous state of every button
        */
        unsigned char buttons[static_cast<int>(JoystickButtons::kCount)];

        /**
        * @brief The previous state of each axis
        */
        float axes[static_cast<int>(JoystickAxes::kCount)];
      };

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

      /**
      * @brief The callback for when the window resizes
      */
      graphics::GraphicsWindow::SizeCallback on_resize_;

      /**
      * @brief The previously connected joysticks
      */
      bool previous_connected_joysticks_[kNumSupportedJoysticks];

      /**
      * @brief The current data of every supported joystick 
      */
      JoystickData joystick_data_[kNumSupportedJoysticks];

      static bool glfw_initialized_; //!< Is GLFW initialized?

      /**
      * @brief The maximum number of input events that can be buffered
      */
      static const size_t kMaxInputEvents_;
    };
  }
}