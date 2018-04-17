#pragma once

#include <scripting/script_class.h>

namespace snuffbox
{
  namespace engine
  {
    /**
    * @brief Key codes taken from 
    *        http://www.glfw.org/docs/latest/group__keys.html
    *
    * Written out so that 'sparse' can pick it up and create key/value pairs
    * for scripting
    */
    SCRIPT_ENUM() enum class Keys
    {
      kUnknown = -1,
      kSpace = 32,
      kApostrophe = 39,
      kComma = 44,
      kMinus = 45,
      kPeriod = 46,
      kSlash = 47,
      k0 = 48,
      k1 = 49,
      k2 = 50,
      k3 = 51,
      k4 = 52,
      k5 = 53,
      k6 = 54,
      k7 = 55,
      k8 = 56,
      k9 = 57,
      kSemicolon = 59,
      kEqual = 61,
      kA = 65,
      kB = 66,
      kC = 67,
      kD = 68,
      kE = 69,
      kF = 70,
      kG = 71,
      kH = 72,
      kI = 73,
      kJ = 74,
      kK = 75,
      kL = 76,
      kM = 77,
      kN = 78,
      kO = 79,
      kP = 80,
      kQ = 81,
      kR = 82,
      kS = 83,
      kT = 84,
      kU = 85,
      kV = 86,
      kW = 87,
      kX = 88,
      kY = 89,
      kZ = 90,
      kLeftBracket = 91,
      kBackslash = 92,
      kRightBracket = 93,
      kGraveAccent = 96,
      kWorld1 = 161,
      kWorld2 = 162,
      kEscape = 256,
      kEnter = 257,
      kTab = 258,
      kBackspace = 259,
      kInsert = 260,
      kDelete = 261,
      kRight = 262,
      kLeft = 263,
      kDown = 264,
      kUp = 265,
      kPageUp = 266,
      kPageDown = 267,
      kHome = 268,
      kEnd = 269,
      kCapsLock = 280,
      kScrollLock = 281,
      kNumLock = 282,
      kPrintScreen = 283,
      kPause = 284,
      kF1 = 290,
      kF2 = 291,
      kF3 = 292,
      kF4 = 293,
      kF5 = 294,
      kF6 = 295,
      kF7 = 296,
      kF8 = 297,
      kF9 = 298,
      kF10 = 299,
      kF11 = 300,
      kF12 = 301,
      kF13 = 302,
      kF14 = 303,
      kF15 = 304,
      kF16 = 305,
      kF17 = 306,
      kF18 = 307,
      kF19 = 308,
      kF20 = 309,
      kF21 = 310,
      kF22 = 311,
      kF23 = 312,
      kF24 = 313,
      kF25 = 314,
      kKeyPad0 = 320,
      kKeyPad1 = 321,
      kKeyPad2 = 322,
      kKeyPad3 = 323,
      kKeyPad4 = 324,
      kKeyPad5 = 325,
      kKeyPad6 = 326,
      kKeyPad7 = 327,
      kKeyPad8 = 328,
      kKeyPad9 = 329,
      kKeyPadDecimal = 330,
      kKeyPadDivide = 331,
      kKeyPadMultiply = 332,
      kKeyPadSubtract = 333,
      kKeyPadAdd = 334,
      kKeyPadEnter = 335,
      kKeyPadEqual = 336,
      kLeftShift = 340,
      kLeftControl = 341,
      kLeftAlt = 342,
      kLeftSuper = 343,
      kRightShift = 344,
      kRightControl = 345,
      kRightAlt = 346,
      kRightSuper = 347,
      kMenu = 348,
      kCount = 349
    };

    /**
    * @brief Mouse button codes taken from
    *        http://www.glfw.org/docs/latest/group__buttons.html
    *
    * Written out so that 'sparse' can pick it up and create key/value pairs
    * for scripting
    */
    SCRIPT_ENUM() enum class MouseButtons
    {
      kUnknown = -1,
      kMouse1 = 0,
      kMouse2 = 1,
      kMouse3 = 2,
      kMouse4 = 3,
      kMouse5 = 4,
      kMouse6 = 5,
      kMouse7 = 6,
      kMouse8 = 7,
      kCount = 8,
      kLeft = 0,
      kRight = 1,
      kMiddle = 2,
    };

    /**
    * @brief Joystick input buttons, based on the layout of an Xbox controller
    */
    SCRIPT_ENUM() enum class JoystickButtons
    {
      kUnknown = -1,
      kA = 0,
      kB = 1,
      kX = 2,
      kY = 3,
      kLeftBumper = 4,
      kRightBumper = 5,
      kBack = 6,
      kStart = 7,
      kLeftStick = 8,
      kRightStick = 9,
      kUp = 10,
      kRight = 11,
      kDown = 12,
      kLeft = 13,
      kCount = 14
    };

    /**
    * @brief The different joystick axes that exist, to retrieve a value
    *        between -1 and 1 for each available axis
    */
    SCRIPT_ENUM() enum class JoystickAxes
    {
      kUnknown = -1,
      kLeftStickX = 0,
      kLeftStickY = 1,
      kRightStickX = 2,
      kRightStickY = 3,
      kLeftTrigger = 4,
      kRightTrigger = 5,
      kCount = 6
    };
  }
}

SCRIPT_ENUM_DECL(snuffbox::engine::Keys);
SCRIPT_ENUM_DECL(snuffbox::engine::MouseButtons);
SCRIPT_ENUM_DECL(snuffbox::engine::JoystickButtons);
SCRIPT_ENUM_DECL(snuffbox::engine::JoystickAxes);