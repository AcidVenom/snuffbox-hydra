#pragma once

#include "foundation/memory/allocators/malloc_allocator.h"
#include "foundation/memory/allocators/eastl_allocator.h"

#include <EASTL/memory.h>

#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>

namespace snuffbox
{
  namespace foundation
  {
    template <typename T>
    struct MemoryDeleter
    {
      /**
      * @brief Default constructor
      */
      MemoryDeleter();

      /**
      * @brief Used to check if a class pointer is convertible to another class
      *
      * @tparam T The class to convert to
      * @tparam U The class to convert from
      */
      template <typename U>
      using is_convertible = 
        typename eastl::enable_if<eastl::is_convertible<U*, T*>::value>::type;

      /**
      * @brief Allowing conversion between derived and base classes
      *
      * @param[in] other The deleter of the U class
      * @param[in] test SFINAE for checking if the U class is 
                        actually convertible
      */
      template <typename U>
      MemoryDeleter(
        const MemoryDeleter<U>& other,
        is_convertible<U>* test = nullptr);

      /**
      * @brief Destructs the passed pointer
      *
      * @param[in] ptr The pointer to be destructed
      */
      void operator()(T* ptr);
    };

    /**
    * @brief An EASTL unique_ptr with a custom allocator
    *
    * @tparam T The type stored underneath this pointer
    *
    * @see MemoryDeleter<T>
    */
    template <typename T>
    using UniquePtr = eastl::unique_ptr<T, MemoryDeleter<T>>;

    /**
    * @brief An EASTL shared_ptr with a custom allocator
    *
    * @tparam T The type stored underneath this pointer
    */
    template <typename T>
    using SharedPtr = eastl::shared_ptr<T>;

    /**
    * @brief The memory interface to be used throughout the engine
    * 
    * "new" and "delete" should be used nowhere, this interface is here to
    * guarantee that. The interface uses the custom memory allocators provided
    * by the user and has a default allocator fallback if not specified.
    *
    * @see Allocator
    *
    * @author Daniel Konings
    */
    class Memory
    {
      
    public:

      /**
      * @brief The default allocator type
      */
      using DefaultAllocator = MallocAllocator;

      /**
      * @brief Allocates a block of memory with a specified size and alignment
      *
      * This function also allocates a Memory::AllocationHeader,
      * the header is used to store a reference to the allocator that was used
      * to allocate the memory block. During deallocation using
      * Memory::Deallocate this allocator will be used.
      *
      * @param[in] size The size to allocate
      * @param[in] align The alignment to use
      * @param[in] allocator The allocator to use for the allocation
      *
      * @return The pointer to the address of the allocated memory block
      */
      static void* Allocate(
        size_t size, 
        size_t align, 
        Allocator* allocator = &default_allocator());

      /**
      * @see Memory::Allocate
      *
      * Uses the default alignment specified in Memory::kDefaultAlignment_
      */
      static void* Allocate(
        size_t size,
        Allocator* allocator = &default_allocator());

      /**
      * @brief Deallocates a memory block and its respective header
      *
      * @see Memory::Allocate
      *
      * @param[in] ptr The pointer to the memory block to deallocate
      */
      static void Deallocate(void* ptr);

      /**
      * @brief Constructs a class using and calls its constructor
      *
      * The equivalent of "new" to use instead
      *
      * @tparam T The type to construct
      * @tparam Args... The arguments to pass into the constructor
      *
      * @param[in] allocator The allocator to use
      * @param[in] args The arguments to pass into the constructor
      *
      * @return The constructed object pointer
      */
      template <typename T, typename ... Args>
      static T* Construct(Allocator* allocator, Args&&... args);

      /**
      * @brief Destructs a class and calls its destructor
      *
      * @tparam T The type to destruct
      *
      * @param[in] ptr The pointer to the constructed object
      */
      template <typename T>
      static void Destruct(T* ptr);

      /**
      * @brief Creates a shared pointer for a provided pointer
      *
      * @tparam T The type of the pointer to make a shared pointer of
      * 
      * @param[in] ptr The pointer to create a shared pointer of
      *
      * @return The created shared pointer
      */
      template <typename T>
      static SharedPtr<T> MakeShared(T* ptr);

      /**
      * @see Memory::Construct
      *
      * @brief This constructs and object and makes it a shared pointer
      *
      * @return The constructed shared pointer
      */
      template <typename T, typename ... Args>
      static SharedPtr<T> ConstructShared(Allocator* alloc, Args&&... args);

      /**
      * @see Memory::Construct
      *
      * @brief This constructs an object and makes it a unique pointer
      *
      * @return The constructed unique pointer
      */
      template <typename T, typename ... Args>
      static UniquePtr<T> ConstructUnique(Allocator* alloc, Args&&... args);

      /**
      * @return The default allocator
      */
      static DefaultAllocator& default_allocator();

    protected:

      /**
      * @brief An allocation header to store allocation information
      *
      * @author Daniel Konings
      */
      struct AllocationHeader
      {
        Allocator* allocator; //!< The allocator used for the allocation
        size_t align; //!< The alignment used during the allocation
        size_t size; //!< The size of the allocation
      };

    private:

      static const size_t kDefaultHeapSize_; //!< The default heap size
      static const size_t kDefaultAlignment_; //!< The default alignment

      /**
      * @brief The default allocator, 
      *        it is initialized with Memory::kDefaultHeapSize_
      */
      static DefaultAllocator default_allocator_;
    };

    //--------------------------------------------------------------------------
    template <typename T>
    MemoryDeleter<T>::MemoryDeleter()
    {

    }

    //--------------------------------------------------------------------------
    template <typename T> template <typename U>
    inline MemoryDeleter<T>::MemoryDeleter(
      const MemoryDeleter<U>&,
      is_convertible<U>* = nullptr)
    {

    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void MemoryDeleter<T>::operator()(T* ptr)
    {
      Memory::Destruct(ptr);
    }

    //--------------------------------------------------------------------------
    template <typename T, typename ... Args>
    inline T* Memory::Construct(Allocator* allocator, Args&&... args)
    {
      T* allocated = reinterpret_cast<T*>(Allocate(sizeof(T), allocator));
      new (allocated) T(eastl::forward<Args>(args)...);

      return allocated;
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline void Memory::Destruct(T* ptr)
    {
      ptr->~T();
      Deallocate(ptr);
    }

    //--------------------------------------------------------------------------
    template <typename T>
    inline SharedPtr<T> Memory::MakeShared(T* ptr)
    {
      return SharedPtr<T>(ptr, MemoryDeleter<T>(), EASTLAllocator());
    }

    //--------------------------------------------------------------------------
    template <typename T, typename ... Args>
    inline SharedPtr<T> Memory::ConstructShared(
      Allocator* alloc, 
      Args&&... args)
    {
      return MakeShared<T>(
        Construct<T, Args...>(alloc, eastl::forward<Args>(args)...));
    }

    //--------------------------------------------------------------------------
    template <typename T, typename ... Args>
    inline UniquePtr<T> Memory::ConstructUnique(
      Allocator* alloc, 
      Args&&... args)
    {
      return UniquePtr<T>(
        Construct<T, Args...>(
          alloc, 
          eastl::forward<Args>(args)...), 
        MemoryDeleter<T>());
    }
  }
}