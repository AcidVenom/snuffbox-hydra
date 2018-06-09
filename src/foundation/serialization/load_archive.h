#pragma once

namespace snuffbox
{
  namespace foundation
  {
    /**
    * @brief Used to load data archived by SaveArchive
    *
    * The load archive requires a JSON format, created from the SaveArchive.
    * All values can then be retrieved by their original name within the
    * SaveArchive. This is order-independent and thus compatible over multiple
    * versions of serialized data.
    *
    * @author Daniel Konings
    */
    class LoadArchive
    {

    public:

      /**
      * @brief Load a value from the archive
      *
      * @remarks This is a recursive unroll to retrieve multiple values at once
      *
      * @tparam T The current type
      * @tparam Args... The remaining types
      *
      * @param[out] value The value to store the found data in
      * @param[out] args The other values to retrieve
      */
      template <typename T, typename ... Args>
      void operator()(T* value, Args&&... args);

      /**
      * @see LoadArchive::operator()
      *
      * @remarks The last function of the unroll when there is only 
      *          one item left
      */
      template <typename T>
      void operator()(T* value);
    };
  }
}