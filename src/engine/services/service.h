#pragma once

#include <foundation/containers/string.h>
#include <foundation/definitions/error_codes.h>

#include <cinttypes>

namespace snuffbox
{
  namespace engine
  {
    class Application;
    class CVarService;

    template <typename T>
    class ServiceBase;

    /**
    * @brief The base class for every application service to use
    *
    * Type assignment isn't done in this base class, but is done in
    * ServiceBase instead. This makes it so that you can still use and cast
    * service pointers appropriately as they share the same base.
    *
    * @author Daniel Konings
    */
    class IService
    {

      friend Application;

    protected:

      /**
      * @brief Construct by providing the service name
      *
      * @param[in] name The name of the service
      */
      IService(const char* name);

      /**
      * @brief Delete copy constructor 
      */
      IService(const IService& other) = delete;

      /**
      * @brief Delete move constructor
      */
      IService(const IService&& other) = delete;

      /**
      * @brief Delete assignment operator
      */
      const IService& operator=(const IService& other) = delete;

      /**
      * @brief Delete move assignment operator
      */
      const IService& operator=(const IService&& other) = delete;

      /**
      * @brief Called on every service when the Application class is initialized
      *
      * @param[in] app The application the service was created from
      *
      * @return The error code, should be ErrorCodes::kSuccess if initialized
      *         succesfully
      */
      virtual foundation::ErrorCodes OnInitialize(Application& app) = 0;

      /**
      * @brief Called on every service when the Application class is shutdown 
      *
      * @param[in] app The application the service was created from
      */
      virtual void OnShutdown(Application& app) = 0;

      /**
      * @brief To register CVars of a system, this method should be overridden
      *
      * @param[in] cvar The current CVar service from the application
      */
      virtual void RegisterCVars(CVarService* cvar);

      /**
      * @return The name of this service
      */
      const foundation::String& name() const;

      /**
      * @brief Used to assign a unique ID to each created service
      *        at compile-time
      *
      * @return The created unique ID
      */
      static uint32_t get_next_type();

    public:

      /**
      * @brief Virtual destructor
      */
      virtual ~IService();

    private:

      foundation::String name_; //!< The name of this service
    };

    //--------------------------------------------------------------------------
    
    /**
    * @brief Used to provide type IDs to every service in the application
    * 
    * @tparam ID The type ID to assign for this service
    *
    * The type IDs are statically assigned when a new service is created at
    * compile time.
    *
    * @author Daniel Konings
    */
    template <typename T>
    class ServiceBase : public IService
    {

      friend Application;

    protected:

      /**
      * @see IService::Service
      */
      ServiceBase(const char* name);

      /**
      * @brief Assigns a unique ID to this service
      */
      static uint32_t type_id();

    public:

      /**
      * @brief Virtual destructor
      */
      virtual ~ServiceBase();

    private:

      uint32_t id_; //!< The ID of this service
    };

    //--------------------------------------------------------------------------
    template <typename T>
    inline ServiceBase<T>::ServiceBase(const char* name) :
      IService(name),
      id_(type_id())
    {

    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline uint32_t ServiceBase<T>::type_id()
    {
      static uint32_t id = IService::get_next_type();
      return id;
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline ServiceBase<T>::~ServiceBase()
    {

    }
  }
}