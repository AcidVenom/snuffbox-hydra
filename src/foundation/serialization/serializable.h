#pragma once

namespace snuffbox
{
  namespace foundation
  {
    class SaveArchive;
    class LoadArchive;

    /**
    * @brief Used to make classes serializable by calling either Serialize
    *        or Deserialize during archiving
    *
    * When a class is serializable, the archive will redirect to the
    * serialization functions, so the user can implement their own archiving.
    *
    * This can be used in conjuction with specialization of 
    * SaveArchive::Serialize and LoadArchive::Deserialize to easily serialize
    * user-defined classes.
    *
    * @author Daniel Konings
    */
    class ISerializable
    {
    public:

      /**
      * @brief Called when a save archive receives this serializable object
      *        as a parameter in the operator()
      *
      * @param[in] archive The current archive being serialized
      */
      virtual void Serialize(SaveArchive& archive) const = 0;

      /**
      * @brief Called when a load archive receives this serializable object
      *        as a parameter in the operator()
      *
      * @param[in] archive The current archive being deserialized
      */
      virtual void Deserialize(LoadArchive& archive) = 0;
    };
  }
}