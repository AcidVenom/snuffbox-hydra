#pragma once

#include "scripting/script_state.h"
#include "scripting/duk/duk_allocator.h"

struct duk_hthread;

namespace snuffbox
{
  namespace scripting
  {
    /**
    * @brief Wraps the duktape execution environment using the native scripting
    *        interface
    *
    * @author Daniel Konings
    */
    class DukState : public IScriptState
    {
      
    public:

      /**
      * @brief Default constructor
      */
      DukState();

      /**
      * @see IScriptState::Initialize
      */
      void Initialize() override;

      /**
      * @see IScriptState::CompileFromSource
      */
      bool CompileFromSource(const char* ctx, const char* src, bool print);

      /**
      * @see IScriptState::Shutdown
      */
      void Shutdown() override;

    protected:

      /**
      * @brief Used to redirect fatal errors that duktape throws
      */
      static void FatalErrorHandler(void* udata, const char* msg);

    private:

      duk_hthread* context_; //!< The underlying duktape context
      DukAllocator allocator_; //!< The allocator of this state
    };
  }
}