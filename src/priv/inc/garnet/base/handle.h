#ifndef __GN_BASE_HANDLE_H__
#define __GN_BASE_HANDLE_H__
// *****************************************************************************
/// \file
/// \brief   Handle manager
/// \author  chenlee (2005.7.25)
// *****************************************************************************

namespace GN
{
    ///
    /// Handle Manager
    ///
    template<typename T, typename HANDLE_TYPE = size_t >
    class HandleManager : public NoCopy
    {
        ///
        /// Handle item used internally by manager
        ///
        struct Item
        {
            UInt8 buf[sizeof(T)];
            bool  occupied;

            T & t() const
            {
                GN_ASSERT( occupied );
                return *(T*)buf;
            }

            void ctor()
            {
                GN_ASSERT( !occupied );
                new (buf) T();
                occupied = true;
            }

            void ctor( const T & v )
            {
                GN_ASSERT( !occupied );
                new (buf) T( v );
                occupied = true;
            }

            void dtor()
            {
                GN_ASSERT( occupied );
                t().T::~T();
                occupied = false;
            }
        };

        FixSizedRawMemoryPool<sizeof(Item)> mPool;
        DynaArray<Item*>                  mItems;
        DynaArray<size_t>                 mFreeList;


        static inline size_t h2idx( HANDLE_TYPE h ) { return (UIntPtr)h - 1; }

        static HANDLE_TYPE idx2h( size_t idx ) { return (HANDLE_TYPE)( idx + 1 ); }

    public:

        ///
        /// dtor
        ///
        ~HandleManager() { Clear(); }

        ///
        /// clear all handles
        ///
        void Clear()
        {
            for( size_t i = 0; i < mItems.Size(); ++i )
            {
                GN_ASSERT( mItems[i] );
                if( mItems[i]->occupied ) mItems[i]->dtor();
                mPool.Dealloc( mItems[i] );
            }
            mItems.Clear();
            mFreeList.Clear();
        }

        ///
        /// Get number of handles
        ///
        size_t Size() const
        {
            return mItems.Size() - mFreeList.Size();
        }

        ///
        /// Is the manager empty or not.
        ///
        bool Empty() const
        {
            return mItems.Size() == mFreeList.Size();
        }

        ///
        /// get current capacity
        ///
        size_t capacity() const { return mItems.Capacity(); }

        ///
        /// set capacity
        ///
        void reserve( size_t n ) { mItems.Reserve(n); mFreeList.Reserve(n); }

        ///
        /// return first handle
        ///
        HANDLE_TYPE first() const
        {
            if( Empty() ) return (HANDLE_TYPE)0;
            size_t idx = 0;
            while( !mItems[idx]->occupied )
            {
                GN_ASSERT( idx < mItems.Size() );
                ++idx;
            }
            return idx2h( idx );
        }

        ///
        /// return next handle
        ///
        HANDLE_TYPE next( HANDLE_TYPE h ) const
        {
            if( !validHandle(h) ) return (HANDLE_TYPE)0;
            size_t idx = h2idx( h ) + 1;
            while( idx < mItems.Size() && !mItems[idx]->occupied )
            {
                GN_ASSERT( idx < mItems.Size() );
                ++idx;
            }
            return idx < mItems.Size() ? idx2h( idx ) : (HANDLE_TYPE)0;
        }

        ///
        /// Add new item with user define value
        ///
        HANDLE_TYPE add( const T & val )
        {
            if( mFreeList.Empty() )
            {
                Item * newItem = (Item*)mPool.Alloc();
                if( 0 == newItem )
                {
                    GN_ERROR(GetLogger("GN.base.HandleManager"))( "out of memory" );
                    return 0;
                }
                newItem->occupied = false;
                newItem->ctor( val );
                mItems.Append( newItem );
                return (HANDLE_TYPE)mItems.Size();
            }
            else
            {
                size_t i = mFreeList.Back();
                mFreeList.PopBack();
                GN_ASSERT( mItems[i] );
                mItems[i]->ctor( val );
                return idx2h( i );
            }
        }

        ///
        /// Add new item, with undefined value
        ///
        HANDLE_TYPE newItem()
        {
            if( mFreeList.Empty() )
            {
                Item * newItem = (Item*)mPool.Alloc();
                if( 0 == newItem )
                {
                    GN_ERROR(GetLogger("GN.base.HandleManager"))( "out of memory" );
                    return 0;
                }
                newItem->occupied = false;
                newItem->ctor();
                mItems.Append( newItem );
                return (HANDLE_TYPE)mItems.Size();
            }
            else
            {
                size_t i = mFreeList.Back();
                mFreeList.PopBack();
                GN_ASSERT( mItems[i] );
                mItems[i]->ctor();
                return idx2h( i );
            }
        }

        ///
        /// Remove item from manager
        ///
        void Remove( HANDLE_TYPE h )
        {
            if( !validHandle(h) )
            {
                GN_ERROR(GetLogger("GN.base.HandleManager"))( "Invalid handle!" );
            }
            else
            {
                size_t idx = h2idx(h);
                GN_ASSERT( mItems[idx] );
                mItems[idx]->dtor();
                mFreeList.Append(idx);
            }
        }

        ///
        /// Find specific item (always return first found)
        ///
        HANDLE_TYPE find( const T & val ) const
        {
            for( size_t i = 0; i < mItems.Size(); ++i )
            {
                if( !mItems[i]->occupied ) continue;
                if( mItems[i]->t() == val ) return idx2h( i ); // found!
            }
            return (HANDLE_TYPE)0; // not found
        }

        ///
        /// Find specific item (always return first found)
        ///
        template<typename FUNC>
        HANDLE_TYPE findIf( const FUNC & fp ) const
        {
            for( size_t i = 0; i < mItems.Size(); ++i )
            {
                if( !mItems[i]->occupied ) continue;
                if( fp( mItems[i]->t() ) ) return idx2h( i ); // found!
            }
            return (HANDLE_TYPE)0; // not found
        }

        ///
        /// Is valid handle or not?
        ///
        bool validHandle( HANDLE_TYPE h ) const
        {
            size_t idx = h2idx(h);
            return idx < mItems.Size() && mItems[idx]->occupied;
        }

        ///
        /// Get item from manager. Handle must be valid.
        ///
        T & get( HANDLE_TYPE h ) const
        {
            GN_ASSERT( validHandle(h) );
            return mItems[h2idx(h)]->t();
        }

        ///
        /// Get item from manager. Handle must be valid.
        ///
        T & operator[]( HANDLE_TYPE h ) const { return get(h); }
    };

    ///
    /// expension to HandleManager class, that object can be referenced by both handle and name.
    ///
    template< typename T, typename H, bool CASE_INSENSITIVE = false>
    class NamedHandleManager
    {
        typedef StringMap<char,H> NameMap;

        struct NamedItem
        {
            NamedHandleManager & mgr;
            const H              handle;
            const StrA           name;
            T                    data;

            NamedItem( NamedHandleManager & m, H h, const StrA & n, const T & d )
                : mgr(m), handle(h), name(n), data(d) {}

            NamedItem( NamedHandleManager & m, H h, const StrA & n )
                : mgr(m), handle(h), name(n) {}
        };

        NameMap                     mNames; // name -> handle
        HandleManager<NamedItem*,H> mItems; // handle -> name/data
        ObjectPool<NamedItem>       mPool;  // named item pool

    public:

        //@{

        typedef H ItemHandle;

        ///
        /// dtor
        ///
        ~NamedHandleManager() { Clear(); }

        ///
        /// clear all handles
        ///
        void Clear()
        {
            for( H i = mItems.first(); i != 0; i = mItems.next( i ) )
            {
                mPool.DeconstructAndFree( mItems[i] );
            }
            mItems.Clear();
            mNames.Clear();
        }

        ///
        /// Get number of handles
        ///
        size_t Size() const
        {
            GN_ASSERT( mItems.Size() == mNames.Size() );
            return mItems.Size();
        }

        ///
        /// Is the manager empty or not.
        ///
        bool Empty() const
        {
            GN_ASSERT( mItems.Size() == mNames.Size() );
            return mItems.Empty();
        }

        ///
        /// return first handle
        ///
        H first() const { return mItems.first(); }

        ///
        /// return next handle
        ///
        H next( H h ) const { return mItems.next( h ); }

        ///
        /// name must be unique.
        ///
        H add( const StrA & name )
        {
            if( CASE_INSENSITIVE )
            {
                GN_UNIMPL();
            }

            if( NULL != mNames.Find( name ) )
            {
                GN_ERROR(GetLogger("GN.base.NamedHandleManager"))( "name '%s' is not unique.", name.ToRawPtr() );
                return 0;
            }

            // create new item
            NamedItem * p = mPool.AllocUnconstructed();
            if( 0 == p ) return 0;

            H handle = mItems.newItem();
            if( 0 == handle ) return 0;

            new (p) NamedItem(*this,handle,name);

            mItems[handle] = p;

            mNames.Insert( name, handle );

            return handle;
        }

        ///
        /// name must be unique.
        ///
        H add( const StrA & name, const T & data )
        {
            if( CASE_INSENSITIVE )
            {
                GN_UNIMPL();
            }

            if( NULL != mNames.Find( name ) )
            {
                GN_ERROR(GetLogger("GN.base.NamedHandleManager"))( "name '%s' is not unique.", name.ToRawPtr() );
                return 0;
            }

            NamedItem * p = mPool.AllocUnconstructed();
            if( 0 == p ) return 0;

            H handle = mItems.newItem();
            if( 0 == handle ) return 0;

            new (p) NamedItem(*this,handle,name,data);

            mItems[handle] = p;

            mNames.Insert( name, handle );

            return handle;
        }

        void Remove( H h )
        {
            if( !validHandle( h ) )
            {
                GN_ERROR(GetLogger("GN.base.NamedHandleManager"))( "invalid handle : %d.", h );
                return;
            }

            NamedItem * item = mItems[h];
            GN_ASSERT( item && item->handle == h );

            mNames.Remove( item->name );

            mItems.Remove( item->handle );

            mPool.DeconstructAndFree( item );
        }

        void Remove( const StrA & name )
        {
            if( CASE_INSENSITIVE )
            {
                GN_UNIMPL();
            }

            if( !validName( name ) )
            {
                GN_ERROR(GetLogger("GN.base.NamedHandleManager"))( "invalid name: %s.", name.ToRawPtr() );
                return;
            }

            size_t handle = mNames[name];

            NamedItem * item = mItems[handle];

            mNames.Remove( name );

            mItems.Remove( handle );

            mPool.DeconstructAndFree( item );
        }

        bool validHandle( H h ) const
        {
            return mItems.validHandle( h );
        }

        bool validName( const StrA & name ) const
        {
            if( CASE_INSENSITIVE )
            {
                GN_UNIMPL();
            }

            return NULL != mNames.Find( name );
        }

        H name2handle( const StrA & name ) const
        {
            if( CASE_INSENSITIVE )
            {
                GN_UNIMPL();
            }

            const H * h = mNames.Find( name );

            return h ? *h : (H)0;
        }

        const char * handle2name( H h ) const
        {
            if( !mItems.validHandle( h ) )
                return NULL;
            else
                return mItems[h]->name;
        }

        T & get( H h ) const
        {
            return mItems[h]->data;
        }

        T & get( const StrA & name ) const
        {
            GN_ASSERT( validName(name) );
            return mItems[mNames[name]]->data;
        }

        T & operator[]( H h ) const { return get(h); }

        T & operator[]( const StrA & name ) const { return get(name); }

        //@}
    };}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_BASE_HANDLE_H__

