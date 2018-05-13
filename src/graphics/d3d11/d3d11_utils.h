#pragma once

#include <foundation/containers/string.h>

#include <comdef.h>

namespace snuffbox
{
  namespace graphics
  {
    /**
    * @brief Used to provide utility functions to make writing D3D11/Win32 code
    *        easier
    *
    * @author Daniel Konings
    */
    class D3D11Utils
    {

    public:

      /**
      * @brief Creates an empty D3D11 struct
      *
      * The structure is memset to 0, instead of using the ZeroMemory macro
      * that is regurarly used to clear the memory of the corresponding
      * structures.
      *
      * @tparam T The struct to create
      *
      * @return The zero-memory struct
      */
      template <typename T>
      static T EmptyStruct();

      /**
      * @brief Converts a HRESULT value to a human-readable format
      *
      * @param[in] err The HRESULT value
      *
      * @return The error message
      */
      static foundation::String HRToString(HRESULT err);

      /**
      * @brief Logs the error result of a HRESULT value as a human-readable
      *        error string using 2 as log verbosity, if any errors occurred
      *
      * @remarks This function only logs if the result is not S_OK, thus can
      *          be used around every function call that has a HRESULT value
      *
      * @param[in] err The error to convert
      *
      * @return Was the result S_OK?
      */
      static bool Check(HRESULT err);
    };

    //--------------------------------------------------------------------------
    template <typename T>
    inline T D3D11Utils::EmptyStruct()
    {
      T s;
      memset(&s, 0, sizeof(T));

      return s;
    }
  }
}