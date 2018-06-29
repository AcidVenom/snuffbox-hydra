#pragma once

#include <foundation/containers/string.h>
#include <foundation/serialization/load_archive.h>
#include <foundation/serialization/save_archive.h>

namespace snuffbox
{
  namespace engine
  {
    /**
    * @brief Used to define materials 
    *
    * @author Daniel Konings
    */
    struct MaterialData
    {
      foundation::String vs_path; //!< The vertex shader path
      foundation::String ps_path; //!< The pixel shader path
      bool has_gs; //!< Does the material have a geometry shader?
      foundation::String gs_path; //!< The geometry shader path
    };
  }

  namespace foundation
  {
    //--------------------------------------------------------------------------
    template <>
    inline void SaveArchive::Serialize(
      SaveArchive& archive,
      const engine::MaterialData& value)
    {
      const foundation::String& vs_path = value.vs_path;
      const foundation::String& ps_path = value.vs_path;
      bool has_gs = value.has_gs;
      const foundation::String& gs_path = value.vs_path;

      archive(
        SET_ARCHIVE_PROP(vs_path),
        SET_ARCHIVE_PROP(ps_path),
        SET_ARCHIVE_PROP(has_gs));

      if (has_gs == true)
      {
        archive(SET_ARCHIVE_PROP(gs_path));
      }
    }

    //--------------------------------------------------------------------------
    template <>
    inline void LoadArchive::Deserialize(
      LoadArchive& archive,
      engine::MaterialData* out)
    {
      foundation::String vs_path;
      foundation::String ps_path;
      bool has_gs;
      foundation::String gs_path;

      archive(
        GET_ARCHIVE_PROP(vs_path),
        GET_ARCHIVE_PROP(ps_path),
        GET_ARCHIVE_PROP(has_gs));

      if (has_gs == true)
      {
        archive(GET_ARCHIVE_PROP(gs_path));
      }

      out->vs_path = vs_path;
      out->ps_path = ps_path;
      out->has_gs = has_gs;
      out->gs_path = gs_path;
    }
  }
}
