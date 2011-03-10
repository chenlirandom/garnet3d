#ifndef __GN_BASE_MEMORY_H__
#define __GN_BASE_MEMORY_H__
// *****************************************************************************
/// \file
/// \brief   Basic memory management routines
/// \author  chenlee (2005.7.23)
// *****************************************************************************

#include <new>

/// \name macro to exception throw
//@{
#if GN_GCC
#define GN_THROW_BADALLOC() throw(std::bad_alloc)
#define GN_NOTHROW() throw()
#else
#define GN_THROW_BADALLOC()
#define GN_NOTHROW() throw()
#endif
//@}

// *****************************************************************************
// Basic memory allocation/deallocation routines
// *****************************************************************************

namespace GN
{
    namespace HeapMemory
    {
        ///
        /// Allocate memory from heap. Can cross DLL boundary.
        ///
        GN_PUBLIC void * alloc( size_t sizeInBytes );

        ///
        /// Re-allocate memory from heap. Can cross DLL boundary.
        ///
        GN_PUBLIC void * realloc( void * ptr, size_t sizeInBytes );

        ///
        /// Allocate aligned memory from heap. Can cross DLL boundary
        ///
        GN_PUBLIC void * alignedAlloc( size_t sizeInBytes, size_t alignment );

        ///
        /// Re-allocate aligned memory from heap. Can cross DLL boundary
        ///
        GN_PUBLIC void * alignedRealloc( void * ptr, size_t sizeInBytes, size_t alignment );

        ///
        /// Free heap-allocated memory (aligned or unaligned). Can cross DLL boundary.
        ///
        GN_PUBLIC void dealloc( void * ptr );
    };
}

// *****************************************************************************
// Global new/delete operators
// *****************************************************************************

#if GN_ICL
#pragma warning(push)
#pragma warning(disable:522) // inline function is declared after being used.
#endif

/// \name overloaded global new and delete operators
//@{
// TODO: more standard conforming implementation.
inline void * operator new( size_t s ) GN_THROW_BADALLOC() { return ::GN::HeapMemory::alloc( s ); }
inline void * operator new[]( size_t s ) GN_THROW_BADALLOC() { return ::GN::HeapMemory::alloc( s ); }
inline void operator delete( void* p ) GN_NOTHROW() { ::GN::HeapMemory::dealloc( p ); }
inline void operator delete[]( void* p ) GN_NOTHROW() { ::GN::HeapMemory::dealloc( p ); }
//@}

#if GN_ICL
#pragma warning(pop)
#endif

// *****************************************************************************
// Other common memory management utilities
// *****************************************************************************

namespace GN
{
    ///
    /// enable CRT memory leak checking. Currently only work for MSVC compiler
    ///
    /// \param breakOnAllocID       Set allocation ID for "break-on-memory-allocation". Set to 0 to disable it.
    void enableCRTMemoryCheck( long breakOnAllocID = 0 );

    ///
    /// free heap memory pointer allocated using GN::HeapMemory functions
    // ------------------------------------------------------------------------
    template < typename T >
    GN_FORCE_INLINE void safeHeapDealloc( T * & ptr )
    {
        if( ptr )
        {
            HeapMemory::dealloc( ptr );
            ptr = 0;
        }
    }

    ///
    /// delete one object
    // ------------------------------------------------------------------------
    template < typename T >
    GN_FORCE_INLINE void safeDelete( T * & ptr )
    {
        if( ptr )
        {
            delete ptr;
            ptr = 0;
        }
    }

    ///
    /// delete object array
    // ------------------------------------------------------------------------
    template < typename T >
    GN_FORCE_INLINE void safeDeleteArray( T * & ptr )
    {
        if( ptr )
        {
            delete [] ptr;
            ptr = 0;
        }
    }

    ///
    /// Safe release COM interface
    ///
    template < typename T >
    GN_FORCE_INLINE void safeRelease( T * & ptr )
    {
        if( ptr )
        {
            ptr->Release();
            ptr = 0;
        }
    }

    ///
    /// Safe release RefCounter class
    ///
    template < typename T >
    GN_FORCE_INLINE void safeDecref( T * & ptr )
    {
        if( ptr )
        {
            ptr->decref();
            ptr = 0;
        }
    }

    ///
    /// STL compilant allocator that use garnet heap memory management routines.
    ///
    template<typename T, size_t ALIGNMENT=16>
    class StlAllocator
    {
        /// \cond NEVER
    public:
        typedef T                  value_type;
        typedef size_t             size_type;
        typedef ptrdiff_t          difference_type;
        typedef const value_type * const_pointer;
        typedef const value_type & const_reference;
        typedef value_type *       pointer;
        typedef value_type &       reference;

        template<class T2>
        struct rebind
        {
            typedef StlAllocator<T2> other;
        };

        StlAllocator() GN_NOTHROW() {}

        ~StlAllocator() GN_NOTHROW() {}

        StlAllocator( const StlAllocator<T> & ) GN_NOTHROW() {}

        template<class T2>
        StlAllocator( const StlAllocator<T2> & ) GN_NOTHROW() {}

        template<class T2>
        StlAllocator<T> & operator=( const StlAllocator<T2> & )
        {
            return *this;
        }

        pointer address( reference x ) const
        {
            return &x;
        }

        const_pointer address( const_reference x ) const
        {
            return &x;
        }

        pointer allocate( size_type count )
        {
            void * p = HeapMemory::alignedAlloc( count * sizeof(T), ALIGNMENT );

            // Note: here we are different from standard STL allocator. We return
            // NULL pointer instead of throw std::bad_alloc() pointer.
            return static_cast<pointer>(p);
        }

        void deallocate( pointer ptr, size_type )
        {
            HeapMemory::dealloc( ptr );
        }

        void construct( pointer ptr, const_reference x )
        {
            new (ptr) T(x);
        }

        void construct( pointer ptr )
        {
            new (ptr) T;
        }

        void destroy( pointer ptr )
        {
            ptr->T::~T();

            // Note:
            //  This is to ensure that the compiler thinks that variable ptr is used.
            //
            //  When T is a simple type, like char or integer, VC compiler will generate
            //  warning of "unreferenced formal parameter", since there's no destructor for POD types.
            (void)ptr;
        }

        size_type max_size() const GN_NOTHROW()
        {
            size_type count = (size_t)(-1) / sizeof(T);
            return ( 0 < count ? count : 1 );
        }
        /// \endcond
    };

    ///
    /// Fix-sized raw memory pool, no ctor/dtor involved.
    ///
    template<
        size_t ITEM_SIZE,
        size_t ALIGNMENT = 16,
        size_t INITIAL_ITEMS_PER_BLOCK = 32,
        size_t MAX_ITEMS = 0 >
    class FixSizedRawMemoryPool : public NoCopy
    {
        // alignment must be 2^N and can not be zero
        GN_CASSERT( ( 0 == ( (ALIGNMENT-1) & ALIGNMENT ) ) && ( ALIGNMENT > 0 ) );

        template<size_t N,size_t A>
        struct Alignment
        {
            static const size_t VALUE = ( N + (A-1) ) & ~(A-1);
        };

        static const size_t ALIGNED_ITEM_SIZE = Alignment< Alignment<ITEM_SIZE,sizeof(void*)>::VALUE + sizeof(void*)*3, ALIGNMENT>::VALUE;

        union Item
        {
            uint8  raw[ALIGNED_ITEM_SIZE];
            struct
            {
                uint8  data[ITEM_SIZE];
                void * pool; ///< pointer to the pool
                Item * prev; ///< points to previous item
                Item * next; ///< points to next item
            };
        };
        GN_CASSERT( sizeof(Item) == ALIGNED_ITEM_SIZE );

        struct Block
        {
            size_t  count; ///< number of items in item array
            Item  * items; ///< item array
            Block * next;  ///< points to next block
        };

        Block * mBlocks;
        Item  * mItems;
        Item  * mFreeItems;
        size_t  mItemCount;
        size_t  mNewBlockSize;

    public:

        ///
        /// Default ctor.
        ///
        FixSizedRawMemoryPool()
            : mBlocks(0)
            , mItems(0)
            , mFreeItems(0)
            , mItemCount(0)
            , mNewBlockSize(INITIAL_ITEMS_PER_BLOCK)
        {
        }

        ///
        /// Default ctor.
        ///
        ~FixSizedRawMemoryPool()
        {
            freeAll();
        }

        ///
        /// make sure a valid pointer belongs to this pool
        ///
        bool check( const void * p ) const
        {
            if( 0 == p ) return false;

            Item * i = (Item*)p;

            return i->pool == this;
        }

        ///
        /// Allocate raw memory for one item
        ///
        void * alloc()
        {
            if( MAX_ITEMS > 0 && mItemCount == MAX_ITEMS )
            {
                GN_ERROR(getLogger("FixSizedRawMemoryPool"))( "out of pool memory!" );
                return 0;
            }

            if( 0 == mFreeItems )
            {
                // no free items. create new block
                Block * b = (Block*)HeapMemory::alloc( sizeof(Block) );
                if( 0 == b )
                {
                    GN_ERROR(getLogger("FixSizedRawMemoryPool"))( "out of heap memory!" );
                    return 0;
                }
                b->items = (Item*)HeapMemory::alignedAlloc( sizeof(Item) * mNewBlockSize, ALIGNMENT );
                if( 0 == b )
                {
                    GN_ERROR(getLogger("FixSizedRawMemoryPool"))( "out of heap memory!" );
                    HeapMemory::dealloc( b );
                    return 0;
                }
                b->count = mNewBlockSize;
                mNewBlockSize *= 2; // size of next block is doubled.

                // build free list
                for( size_t i = 0; i < b->count; ++i )
                {
                    b->items[i].pool = this;
                    b->items[i].next = mFreeItems;
                    mFreeItems = &b->items[i];
                }

                // add to block list
                b->next = mBlocks;
                mBlocks = b;
            }

            // get one from free list.
            Item * p = mFreeItems;
            mFreeItems = mFreeItems->next;

            // insert to item list
            p->prev = 0;
            p->next = mItems;
            if( mItems ) mItems->prev = p;
            mItems = p;

            ++mItemCount;

            return p;
        }

        ///
        /// Deallocate
        ///
        void dealloc( void * p )
        {
            if( 0 == p ) return;

            Item * i = (Item*)p;
            if( !check(p) )
            {
                GN_ERROR(getLogger("FixSizedRawMemoryPool"))( "invalid pointer!" );
                return;
            }

            if( 0 == mItemCount )
            {
                GN_ERROR(getLogger("FixSizedRawMemoryPool"))( "input pointer is not belong to this pool!" );
                return;
            }

            --mItemCount;

            // remove from item list
            if( i->prev ) i->prev->next = i->next;
            if( i->next ) i->next->prev = i->prev;
            if( i == mItems ) mItems = i->next;

            // add to free list
            i->next = mFreeItems;
            mFreeItems = i;
        }

        ///
        /// free all items
        ///
        void freeAll()
        {
            Block * p;
            while( mBlocks )
            {
                p = mBlocks;
                mBlocks = mBlocks->next;
                HeapMemory::dealloc( p->items );
                HeapMemory::dealloc( p );
            }
            mBlocks = 0;
            mItems = 0;
            mFreeItems = 0;
            mItemCount = 0;
        }

        ///
        /// get first item in allocator
        ///
        void * getFirst() const { return mItems; }

        ///
        /// get next item in allocator
        ///
        void * getNext( const void * p ) const { GN_ASSERT(p); return ((Item*)p)->next; }
    };

    ///
    /// object pool
    ///
    template<
        class  T,
        class  RAW_MEMORY_POOL = FixSizedRawMemoryPool<sizeof(T)> >
    class ObjectPool
    {
        RAW_MEMORY_POOL mRawMem;

        T * doAlloc()
        {
            T * p = (T*)mRawMem.alloc();
            if( 0 == p ) return 0;

            // construct the object, using defualt constructor
            new (p) T;

            // success
            return p;
        }

        void doDealloc( T * p )
        {
            if( 0 == p ) return;

            // destruct the object
            p->T::~T();

            // free p
            mRawMem.dealloc( p );
        }

        void doFreeAll()
        {
            // destruct all objects
            for( T * p = (T*)mRawMem.getFirst(); 0 != p; p = (T*)mRawMem.getNext(p) )
            {
                p->T::~T();
            }

            // free memory
            mRawMem.freeAll();
        }

    public:

        //@{

        ObjectPool() {}

        ~ObjectPool() { deconstructAndFreeAll(); }

        //@}

        //@{
        T  * allocConstructed() { return doAlloc(); }
        T  * allocUnconstructed() { return (T*)mRawMem.alloc(); }
        void deconstructAndFree( void * p ) { doDealloc( (T*)p ); }
        void freeWithoutDeconstruct( void * p ) { mRawMem.dealloc( p ); }
        void deconstructAndFreeAll() { doFreeAll(); }
        bool check( const T * p ) const { return mRawMem.check( p ); }
        T  * getFirst() const { return (T*)mRawMem.getFirst(); }
        T  * getNext( const T * p ) const { return (T*)mRawMem.getNext(p); }
        //@}
    };
}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_BASE_MEMORY_H__
