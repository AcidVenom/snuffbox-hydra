#pragma once

namespace snuffbox
{
  namespace foundation
  {
    class SaveArchive;
    class LoadArchive;

    class ISerializable
    {
    public:

      virtual void Serialize(SaveArchive& archive) const = 0;

      virtual void Deserialize(LoadArchive& archive) = 0;
    };
  }
}