#pragma once

namespace eastl
{
  class allocator;
}

namespace snuffbox
{
  namespace foundation
  {
    class IAllocator;

    /**
    * @brief The allocator for EASTL to use
    *
    * This allocator just uses the default memory allocator for now
    *
    * @author Daniel Konings
    */
    class EASTLAllocator
    {
    public:

      /**
      * @brief Construct an EASTL allocator with a name
      *
      * @param[in] pName The name for debugging
      */
      EASTLAllocator(const char* pName = "EASTLAllocator");

      /**
      * @brief Copy constructor
      *
      * @param[in] x The allocator to copy from
      * @param[in] pName The name for debugging
      */
      EASTLAllocator(
        const eastl::allocator& x, 
        const char* pName = "EASTLAllocator");

      /**
      * @brief Default destructor
      */
      ~EASTLAllocator();

      /**
      * @brief Copy assignment operator
      *
      * @param[in] x The allocator to copy from
      */
      EASTLAllocator& operator=(const EASTLAllocator& x);

      /**
      * @brief Comparison operator, always returns true
      *
      * @param[in] x The allocator to compare against
      *
      * @return Always true, as there is only 1 EASTLAllocator
      */
      bool operator==(const EASTLAllocator& x);

      /**
      * @brief Comparison operator, always returns false
      *
      * @see EASTLAllocator::operator==
      */
      bool operator!=(const EASTLAllocator& x);

      /**
      * @brief Allocate with a given size
      *
      * @param[in] n The size to allocate
      * @param[in] flags EASTL allocator flags
      *
      * @remarks This uses the underlying Allocator
      *
      * @return The pointer pointing to the allocated memory block
      */
      void* allocate(size_t n, int flags = 0);

      /**
      * @brief Allocate with a given size and alignment
      *
      * @param[in] n The size to allocate
      * @param[in] alignment The alignment to use
      * @param[in] offset The offset to use, unused right now
      * @param[in] flags EASTL allocator flags
      *
      * @remarks This uses the underlying Allocator
      *
      * @return The pointer pointing to the allocated memory block
      */
      void* allocate(size_t n, size_t alignment, size_t offset, int flags = 0);

      /**
      * @brief Deallocates memory at a provided pointer
      *
      * @param[in] p The pointer to deallocate the memory from
      * @param[in] n The size to deallocate
      */
      void deallocate(void* p, size_t n);

      /**
      * @return The debug name of this allocator
      */
      const char* get_name() const;

      /**
      * @brief Sets the debug name of this allocator
      *
      * @param[in] pName The name to set
      */
      void set_name(const char* pName);

    protected:

      const char* mpName; //!< The debug name of this allocator
      IAllocator& allocator_; //!< A reference to the underlying IAllocator
    };
  }
}