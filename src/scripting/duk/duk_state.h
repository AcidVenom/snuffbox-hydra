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

    protected:

      /**
      * @brief Used to redirect fatal errors that duktape throws
      */
      static void FatalErrorHandler(void* udata, const char* msg);

    public:

      /**
      * @see IScriptState::Initialize
      */
      bool Initialize() override;

      /**
      * @see IScriptState::CompileFromSource
      */
      bool CompileFromSource(
        const char* ctx, 
        const char* src, 
        bool print) override;

      /**
      * @brief Logs the last error that is on the stack
      *
      * The format should have atleast 4 arguments included, where:
      * {0} = The file name the error occurred in
      * {1} = The line the error occurred at
      * {2} = The error message, including stack
      *
      * @param[in] format The format to log the error with
      *
      * @remarks This doesn't pop the error off of the stack
      */
      void LogLastError(const char* format);

      /**
      * @see IScriptState::Shutdown
      */
      void Shutdown() override;

      /**
      * @return The duktape context
      */
      duk_hthread* context() const;

    private:

      duk_hthread* context_; //!< The underlying duktape context
      DukAllocator allocator_; //!< The allocator of this state
    };
  }
}