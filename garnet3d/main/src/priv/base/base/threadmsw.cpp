#include "pch.h"
#include "garnet/base/thread.h"

float const GN::INFINITE_TIME = 1e38f;

#if GN_MSWIN

#include <process.h>

static GN::Logger * sLogger = GN::getLogger("GN.base.Thread");

using namespace GN;

// *****************************************************************************
// local class and functions
// *****************************************************************************


///
/// convert seconds to milliseconds on MS windows platform
///
UInt32 sec2usec( float time )
{
    if( INFINITE_TIME == time ) return INFINITE;
    else return (UInt32)( time * 1000000.0f );
}

static int sPriorityTable[] =
{
    THREAD_PRIORITY_TIME_CRITICAL,
    THREAD_PRIORITY_ABOVE_NORMAL,
    THREAD_PRIORITY_NORMAL,
    THREAD_PRIORITY_BELOW_NORMAL,
    THREAD_PRIORITY_IDLE,
};
GN_CASSERT( GN_ARRAY_COUNT(sPriorityTable) == NUM_THREAD_PRIORITIES );

///
/// convert thread priority to WIN32 constant
///
static int sPriority2Msw( ThreadPriority p )
{
    GN_ASSERT( p < NUM_THREAD_PRIORITIES );

    return sPriorityTable[p];
}

// *****************************************************************************
// mutex class
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::Mutex::Mutex()
{
    GN_CASSERT( sizeof(CRITICAL_SECTION) <= sizeof(mInternal) );

    // try spin loop 5000 times before sleeping.
    InitializeCriticalSectionAndSpinCount( (CRITICAL_SECTION*)mInternal, 5000 );
}

//
//
// -----------------------------------------------------------------------------
GN::Mutex::~Mutex()
{
    DeleteCriticalSection( (CRITICAL_SECTION*)mInternal );
}

//
//
// -----------------------------------------------------------------------------
bool GN::Mutex::trylock()
{
    return !!TryEnterCriticalSection( (CRITICAL_SECTION*)mInternal );
}

//
//
// -----------------------------------------------------------------------------
void GN::Mutex::lock()
{
    EnterCriticalSection( (CRITICAL_SECTION*)mInternal );
}

//
//
// -----------------------------------------------------------------------------
void GN::Mutex::unlock()
{
    LeaveCriticalSection( (CRITICAL_SECTION*)mInternal );
}

// *****************************************************************************
// syncevent class
// *****************************************************************************

///
/// sync event on MS Windows.
///
class SyncEventMsw : public SyncEvent, public StdClass
{
    GN_DECLARE_STDCLASS( SyncEventMsw, StdClass );

    // ********************************
    // ctor/dtor
    // ********************************

    //@{
public:
    SyncEventMsw()          { clear(); }
    virtual ~SyncEventMsw() { quit(); }
    //@}

    // ********************************
    // from StdClass
    // ********************************

    //@{
public:
    bool init( bool initialSignaled, bool autoreset, const char * name )
    {
        GN_GUARD;

        // standard init procedure
        GN_STDCLASS_INIT( SyncEventMsw, () );

        GN_MSW_CHECK_RV(
            mHandle = CreateEventA( 0, !autoreset, initialSignaled, name ),
            failure() );

        // success
        return success();

        GN_UNGUARD;
    }
    void quit()
    {
        GN_GUARD;

        if( mHandle ) CloseHandle( mHandle );

        // standard quit procedure
        GN_STDCLASS_QUIT();

        GN_UNGUARD;
    }
private:
    void clear() { mHandle = 0; }
    //@}

    // ********************************
    // from SyncEvent
    // ********************************
public:

    virtual void signal()
    {
        GN_ASSERT( mHandle );
        GN_MSW_CHECK( SetEvent( mHandle ) );
    }

    virtual void unsignal()
    {
        GN_ASSERT( mHandle );
        GN_MSW_CHECK( ResetEvent( mHandle ) );
    }

    virtual bool wait( float seconds )
    {
        return WAIT_OBJECT_0 == WaitForSingleObject( mHandle, sec2usec( seconds ) );
    }

    // ********************************
    // private variables
    // ********************************
private:

    HANDLE mHandle;

    // ********************************
    // private functions
    // ********************************
private:
};

// *****************************************************************************
// semaphore class
// *****************************************************************************

///
/// semaphore on MS Windows.
///
class SemaphoreMsw : public Semaphore, public StdClass
{
    GN_DECLARE_STDCLASS( SemaphoreMsw, StdClass );

    // ********************************
    // ctor/dtor
    // ********************************

    //@{
public:
    SemaphoreMsw()          { clear(); }
    virtual ~SemaphoreMsw() { quit(); }
    //@}

    // ********************************
    // from StdClass
    // ********************************

    //@{
public:
    bool init( size_t maxcount, size_t initialcount, const char * name )
    {
        GN_GUARD;

        // standard init procedure
        GN_STDCLASS_INIT( SemaphoreMsw, () );

        GN_MSW_CHECK_RV(
            mHandle = CreateSemaphoreA( 0, (LONG)initialcount, (LONG)maxcount, name ),
            failure() );

        // success
        return success();

        GN_UNGUARD;
    }
    void quit()
    {
        GN_GUARD;

        if( mHandle ) CloseHandle( mHandle );

        // standard quit procedure
        GN_STDCLASS_QUIT();

        GN_UNGUARD;
    }
private:
    void clear() { mHandle = 0; }
    //@}

    // ********************************
    // from Semaphore
    // ********************************
public:

    virtual bool wait( float seconds )
    {
        return WAIT_OBJECT_0 == WaitForSingleObject( mHandle, sec2usec( seconds ) );
    }

    virtual void wake( size_t count )
    {
        GN_ASSERT( mHandle );
        GN_MSW_CHECK( ReleaseSemaphore( mHandle, (LONG)count, 0 ) );
    }

    // ********************************
    // private variables
    // ********************************
private:

    HANDLE mHandle;

    // ********************************
    // private functions
    // ********************************
private:
};

// *****************************************************************************
// thread class
// *****************************************************************************

///
/// thread class on MS Windows
///
class ThreadMsw : public Thread, public StdClass
{
    GN_DECLARE_STDCLASS( ThreadMsw, StdClass );

    // ********************************
    // ctor/dtor
    // ********************************

    //@{
public:
    ThreadMsw()          { clear(); }
    virtual ~ThreadMsw() { quit(); }
    //@}

    // ********************************
    // from StdClass
    // ********************************

    //@{
public:
    bool init(
        const ThreadProcedure & proc,
        void * param,
        ThreadPriority priority,
        bool initialSuspended,
        const char * )
    {
        GN_GUARD;

        // standard init procedure
        GN_STDCLASS_INIT( ThreadMsw, () );

        // check parameter
        if( priority < 0 || priority >= NUM_THREAD_PRIORITIES )
        {
            GN_ERROR(sLogger)( "invalid thread priority." );
            return failure();
        }

        mProc = proc;
        mParam.instance = this;
        mParam.userparam = param;
        mPriority = priority;

        mHandle = (HANDLE)_beginthreadex(
            0, // security
            0, // default stack size
            &sProcDispatcher,
            &mParam,
            initialSuspended ? CREATE_SUSPENDED : 0,
            (unsigned int*)&mId );
        GN_MSW_CHECK_RV( mHandle, failure() );

        // success
        return success();

        GN_UNGUARD;
    }

    void quit()
    {
        GN_GUARD;

        // wait for thread termination
        waitForTermination( INFINITE_TIME, 0 );

        // close thread handle
        CloseHandle( mHandle );

        // standard quit procedure
        GN_STDCLASS_QUIT();

        GN_UNGUARD;
    }

private:
    void clear()
    {
        mHandle = 0;
        mId = 0;
    }
    //@}

    // ********************************
    // inherited from Thread
    // ********************************
public:

    virtual ThreadPriority getPriority() const
    {
        return mPriority;
    }

    virtual void setPriority( ThreadPriority p )
    {
        if( p < 0 || p >= NUM_THREAD_PRIORITIES )
        {
            GN_ERROR(sLogger)( "invalid thread priority!" );
            return;
        }

        GN_MSW_CHECK_R( ::SetThreadPriority( mHandle, sPriority2Msw(p) ) );

        mPriority = p;
    }

    bool isCurrentThread() const
    {
        return ::GetCurrentThreadId() == mId;
    }

    virtual void suspend()
    {
        if( (DWORD)-1 == ::SuspendThread( mHandle ) )
        {
            GN_ERROR(sLogger)( getOSErrorInfo() );
        }
    }

    virtual void resume()
    {
        if( (DWORD)-1 == ::ResumeThread( mHandle ) )
        {
            GN_ERROR(sLogger)( getOSErrorInfo() );
        }
    }

    virtual bool waitForTermination( float seconds, UInt32 * threadProcReturnValue )
    {
        // can't wait for self termination
        GN_ASSERT( !isCurrentThread() );

        UInt32 ret = ::WaitForSingleObject( mHandle, sec2usec( seconds ) );

        if( WAIT_TIMEOUT == ret )
        {
            GN_TRACE(sLogger)( "time out!" );
            return false;
        }
        else if( WAIT_OBJECT_0 == ret )
        {
            if( threadProcReturnValue )
            {
                GN_MSW_CHECK( GetExitCodeThread( mHandle, (LPDWORD)threadProcReturnValue ) );
            }
            return true;
        }
        else
        {
            GN_ERROR(sLogger)( getOSErrorInfo() );
            return false;
        }
    }

    // ********************************
    // private variables
    // ********************************
private:

    struct ThreadParam
    {
        ThreadMsw * instance;
        void      * userparam;
    };

    ThreadProcedure mProc;
    ThreadParam     mParam;
    ThreadPriority  mPriority;

    HANDLE          mHandle;
    DWORD           mId;

    // ********************************
    // private functions
    // ********************************
private:

    ///
    /// thread procedure dispather
    ///
    static unsigned int __stdcall sProcDispatcher( void * parameter )
    {
        GN_ASSERT( parameter );

        ThreadParam * p = (ThreadParam*)parameter;

        GN_ASSERT( p->instance );

        return p->instance->mProc( p->userparam );
    };

};

// *****************************************************************************
// public functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::SyncEvent * GN::createSyncEvent(
    bool initialSignaled,
    bool autoreset,
    const char * name )
{
    GN_GUARD;

    AutoObjPtr<SyncEventMsw> s( new SyncEventMsw );

    if( !s->init( initialSignaled, autoreset, name ) ) return 0;

    return s.detach();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
GN::Semaphore * GN::createSemaphore(
    size_t maxcount,
    size_t initialcount,
    const char * name )
{
    GN_GUARD;

    AutoObjPtr<SemaphoreMsw> s( new SemaphoreMsw );

    if( !s->init( maxcount, initialcount, name ) ) return 0;

    return s.detach();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
GN::Thread *
GN::createThread(
    const ThreadProcedure & proc,
    void * param,
    ThreadPriority priority,
    bool initialSuspended,
    const char * name )
{
    GN_GUARD;

    AutoObjPtr<ThreadMsw> s( new ThreadMsw );

    if( !s->init( proc, param, priority, initialSuspended, name ) ) return 0;

    return s.detach();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::sleepCurrentThread( float seconds )
{
   ::Sleep( sec2usec( seconds ) );
}

#endif