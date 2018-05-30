#include "engine/assets/script_asset.h"
#include "engine/application/application.h"
#include "engine/services/script_service.h"

#include <tools/builder/compilers/script_compiler.h>

namespace snuffbox
{
  namespace engine
  {
    //--------------------------------------------------------------------------
    ScriptAsset::ScriptAsset(const foundation::Path& path) :
      IAsset(builder::AssetTypes::kScript, path)
    {

    }

    //--------------------------------------------------------------------------
    bool ScriptAsset::LoadImpl(const foundation::Path& path)
    {
      builder::ScriptCompiler c;
      if (c.Decompile(path) == false)
      {
        return false;
      }

      size_t len;
      const uint8_t* buffer = c.Data(&len);

      foundation::String src(reinterpret_cast<const char*>(buffer), len);

      Application* app = Application::Instance();
      
      return app->GetService<ScriptService>()->CompileAndRun(
        path.ToString().c_str(),
        src.c_str(),
        false);
    }
  }
}